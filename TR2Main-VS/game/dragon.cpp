/*
 * Copyright (c) 2017-2020 Michael Chaban. All rights reserved.
 * Original game is created by Core Design Ltd. in 1997.
 * Lara Croft and Tomb Raider are trademarks of Embracer Group AB.
 *
 * This file is part of TR2Main.
 *
 * TR2Main is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TR2Main is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TR2Main.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "precompiled.h"
#include "game/dragon.h"
#include "3dsystem/phd_math.h"
#include "game/box.h"
#include "game/control.h"
#include "game/draw.h"
#include "game/items.h"
#include "game/missile.h"
#include "specific/game.h"
#include "global/vars.h"

#define DINO_TOUCH (0x3000)
#define DINO_KILL_ANIM 11
#define DINO_ATTACK_FRAME 21 // NOTE: This will smooth the special lara death animation and avoid other enemies to be kill in one animation...
#define DINO_TOUCH_DAMAGE 1
#define DINO_TRAMPLE_DAMAGE 10
#define DINO_BITE_DAMAGE 10000
#define DINO_BITE_DAMAGE_TO_OTHER 15
#define DINO_ROAR_CHANCE 0x200
#define DINO_RUN_TURN ANGLE(4)
#define DINO_WALK_TURN ANGLE(2)
#define DINO_RUN_RANGE SQR(BLOCK(5))
#define DINO_ATTACK_RANGE SQR(BLOCK(4))
#define DINO_BITE_RANGE SQR(1500)

static const BITE_INFO DinoBite = { 0, 0, 0, 0 };

typedef enum
{
	DINO_LINK,
	DINO_STOP,
	DINO_WALK,
	DINO_RUN,
	DINO_ATTACK1,
	DINO_DEATH,
	DINO_ROAR,
	DINO_ATTACK2,
	DINO_KILL
} DINO_STATE;

void ControlTwinkle(short fxNum)
{
	FX_INFO* fx = &Effects[fxNum];
	fx->frameNumber--;

	if (fx->frameNumber <= Objects[fx->objectID].nMeshes)
		fx->frameNumber = 0;

	if (fx->counter >= 0)
	{
		ITEM_INFO* item = &Items[fx->counter];
		int x = item->pos.x;
		int y = item->pos.y;
		int z = item->pos.z;
		if (item->objectID == ID_DRAGON_FRONT)
		{
			int s = phd_sin(item->pos.rotY);
			int c = phd_cos(item->pos.rotY);
			x += (490 * s + 1100 * c) >> W2V_SHIFT;
			y -= 540;
			z += (490 * c - 1100 * s) >> W2V_SHIFT;
		}
		fx->pos.x += (x - fx->pos.x) >> 4;
		fx->pos.y += (y - fx->pos.y) >> 4;
		fx->pos.z += (z - fx->pos.z) >> 4;
		if (ABS(x - fx->pos.x) < 256 &&
			ABS(y - fx->pos.y) < 256 &&
			ABS(z - fx->pos.z) < 256)
			KillEffect(fxNum);
	}
	else
	{
		fx->counter++;
		if (fx->counter == 0)
			KillEffect(fxNum);
	}
}

void CreateBartoliLight(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];
	short fxNum = CreateEffect(item->roomNumber);
	if (fxNum != -1)
	{
		FX_INFO* fx = &Effects[fxNum];
		fx->pos.rotY = GetRandomDraw() * 2;
		fx->pos.x = item->pos.x + (ANGLE(5) * phd_sin(fx->pos.rotY) >> W2V_SHIFT);
		fx->pos.y = (GetRandomDraw() >> 2) + item->pos.y - 1024;
		fx->pos.z = item->pos.z + (ANGLE(5) * phd_cos(fx->pos.rotY) >> W2V_SHIFT);
		fx->roomNumber = item->roomNumber;
		fx->counter = itemNumber;
		fx->frameNumber = 0;
		fx->objectID = ID_TWINKLE;
	}
	AddDynamicLight(item->pos.x, item->pos.y, item->pos.z, (4 * GetRandomDraw() >> 15) + 12, (4 * GetRandomDraw() >> 15) + 10);
}

short DragonFire(int x, int y, int z, short speed, short rotY, short roomNum)
{
	short fxnum = CreateEffect(roomNum);
	if (fxnum != -1)
	{
		FX_INFO* fx = &Effects[fxnum];
		fx->pos.x = x;
		fx->pos.y = y;
		fx->pos.z = z;
		fx->roomNumber = roomNum;
		fx->pos.rotZ = 0;
		fx->pos.rotX = 0;
		fx->pos.rotY = rotY;
		fx->speed = 200;
		fx->objectID = ID_MISSILE_FLAME;
		fx->frameNumber = (Objects[fx->objectID].nMeshes + 1) * GetRandomDraw() >> 15;
		fx->shade = 0xE00;
		ShootAtLara(fx);
		if (Objects[ID_DRAGON_FRONT].loaded)
		{
			fx->counter = 0x4000;
			return fxnum;
		}
		fx->counter = 20;
	}
	return fxnum;
}

void DinoControl(short itemNumber)
{
	if (!CreatureActive(itemNumber))
		return;

	ITEM_INFO* item = &Items[itemNumber];
	CREATURE_INFO* dino = GetCreatureInfo(item);
	AI_INFO info;
	short head = 0, angle = 0;
	bool isLaraAlive = LaraItem->hitPoints > 0;

	if (item->hitPoints <= 0)
	{
		if (item->currentAnimState == DINO_STOP)
			item->goalAnimState = DINO_DEATH;
		else
			item->goalAnimState = DINO_STOP;
	}
	else
	{
		CreatureAIInfo(item, &info);
		if (info.ahead)
			head = info.angle;
		CreatureMood(item, &info, TRUE);
		angle = CreatureTurn(item, dino->maximumTurn);

		if (item->touchBits && dino->enemy != NULL && dino->enemy->objectID == ID_LARA)
		{
			dino->enemy->hitPoints -= (item->currentAnimState == DINO_RUN) ? DINO_TRAMPLE_DAMAGE : DINO_TOUCH_DAMAGE;
			dino->enemy->hitStatus = TRUE;
		}

		dino->flags = !info.ahead && info.enemyFacing > -ANGLE(45) && info.enemyFacing < ANGLE(45);
		if (!dino->flags && info.distance > DINO_BITE_RANGE && info.distance < DINO_ATTACK_RANGE && info.bite)
			dino->flags |= 1;

		switch (item->currentAnimState)
		{
		case DINO_STOP:
			if (item->requiredAnimState)
				item->goalAnimState = item->requiredAnimState;
			else if (info.distance < DINO_BITE_RANGE && info.bite)
				item->goalAnimState = DINO_ATTACK2;
			else if (dino->mood == MOOD_BORED || dino->flags != 0)
				item->goalAnimState = DINO_WALK;
			else
				item->goalAnimState = DINO_RUN;
			break;

		case DINO_WALK:
			dino->maximumTurn = DINO_WALK_TURN;

			if (dino->mood != MOOD_BORED || !dino->flags)
				item->goalAnimState = DINO_STOP;
			else if (info.ahead && GetRandomControl() < DINO_ROAR_CHANCE)
			{
				item->requiredAnimState = DINO_ROAR;
				item->goalAnimState = DINO_STOP;
			}
			break;

		case DINO_RUN:
			dino->maximumTurn = DINO_RUN_TURN;

			if (info.distance < DINO_RUN_RANGE && info.bite)
				item->goalAnimState = DINO_STOP;
			else if (dino->flags)
				item->goalAnimState = DINO_STOP;
			else if (dino->mood != MOOD_ESCAPE && info.ahead && GetRandomControl() < DINO_ROAR_CHANCE)
			{
				item->requiredAnimState = DINO_ROAR;
				item->goalAnimState = DINO_STOP;
			}
			else if (dino->mood == MOOD_BORED)
				item->goalAnimState = DINO_STOP;
			break;

		case DINO_ATTACK2:
			if (item->frameNumber == (Anims[item->animNumber].frameBase + DINO_ATTACK_FRAME))
			{
				if (DamageLaraOrEnemy(item, dino->enemy, &DinoBite, DINO_BITE_DAMAGE, DINO_BITE_DAMAGE, item->touchBits & DINO_TOUCH, BLOCK(4), DamageTargetFlags::DisableBlood))
					if (dino->enemy != NULL && dino->enemy->objectID == ID_LARA) break;
			}
			item->requiredAnimState = DINO_WALK;
			break;
		}
	}

	if (dino->enemy == LaraItem && isLaraAlive && LaraItem->hitPoints <= 0)
	{
		// NOTE: Reset any rotation to fix dino looking elsewhere when killing lara !
		head = 0;
		dino->maximumTurn = 0;
		dino->headRotation = 0;
		dino->neckRotation = 0;
		CreatureKill(item, DINO_KILL_ANIM, DINO_KILL, EXTRA_DINOKILL);
		return;
	}

	if (item->currentAnimState != DINO_KILL)
	{
		CreatureHead(item, (short)(head >> 1));
		CreatureNeck(item, (short)(head >> 1));
		CreatureAnimation(itemNumber, angle, 0);
	}
	else
	{
		AnimateItem(item);
	}
}

 /*
  * Inject function
  */
void Inject_Dragon() {
	INJECT(0x00417780, ControlTwinkle);
	INJECT(0x00417900, CreateBartoliLight);
	INJECT(0x004179E0, DragonFire);
	//INJECT(0x00417A90, DragonCollision);
	//INJECT(0x00417D80, DragonBones);
	//INJECT(0x00417E60, DragonControl);
	//INJECT(0x004183B0, InitialiseBartoli);
	//INJECT(0x004184D0, BartoliControl);
	INJECT(0x00418670, DinoControl);
}
