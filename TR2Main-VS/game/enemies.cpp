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
#include "game/enemies.h"
#include "game/box.h"
#include "game/items.h"
#include "game/larafire.h"
#include "game/effects.h"
#include "game/missile.h"
#include "game/people.h"
#include "game/sound.h"
#include "specific/game.h"
#include "global/vars.h"

#define MONK_DAMAGE_TO_OTHER_ENEMIES 5
#define MONK_DAMAGE 150
#define MONK_DEATH_ANIM 20
#define MONK_TOUCHBITS MESH_BITS(14)
#define MONK_CLOSE_RANGE SQR(CLICK(2))
#define MONK_LONG_RANGE SQR(BLOCK(1))
#define MONK_ATTACK5_RANGE SQR(BLOCK(3))
#define MONK_WALK_RANGE SQR(BLOCK(2))
#define MONK_HIT_RANGE CLICK(2)

#define CULT2_DEATH_ANIM 23
#define CULT2_WALK_TURN ANGLE(3)
#define CULT2_RUN_TURN ANGLE(6)
#define CULT2_WALK_RANGE SQR(BLOCK(4))
#define CULT2_KNIFE_RANGE SQR(BLOCK(6))
#define CULT2_STOP_RANGE SQR(BLOCK(5) / 2)

enum MonkState
{
	MONK_STOP1 = 1,
	MONK_WALK,
	MONK_RUN,
	MONK_ATTACK1,
	MONK_ATTACK2,
	MONK_ATTACK3,
	MONK_ATTACK4,
	MONK_AIM,
	MONK_DEATH,
	MONK_ATTACK5,
	MONK_STOP2
};

enum Cult2State // Knifethrower StateIDs
{
	CULT2_STOP = 1,
	CULT2_WALK,
	CULT2_RUN,
	CULT2_AIM1L,
	CULT2_SHOOT1L,
	CULT2_AIM1R,
	CULT2_SHOOT1R,
	CULT2_AIM2,
	CULT2_SHOOT2,
	CULT2_DEATH
};

static const BITE_INFO MonkBite = { -23, 16, 265, 14 };
static const BITE_INFO Cult2Left = { 0, 0, 0, 5 };
static const BITE_INFO Cult2Right = { 0, 0, 0, 8 };

short Knife(int x, int y, int z, short speed, short rotY, short roomNum)
{
	short fxNum = CreateEffect(roomNum);
	if (fxNum != -1)
	{
		FX_INFO* fx = &Effects[fxNum];
		fx->pos.x = x;
		fx->pos.y = y;
		fx->pos.z = z;
		fx->roomNumber = roomNum;
		fx->pos.rotZ = 0;
		fx->pos.rotX = 0;
		fx->pos.rotY = rotY;
		fx->speed = 150;
		fx->frameNumber = 0;
		fx->objectID = ID_MISSILE_KNIFE;
		fx->shade = 0xE00;
		ShootAtLara(fx);
	}
	return fxNum;
}

void Cult2Control(short itemNumber)
{
	if (!CreatureActive(itemNumber))
		return;

	ITEM_INFO* item = &Items[itemNumber];
	CREATURE_INFO* cult2 = GetCreatureInfo(item);
	if (cult2 == NULL) return; // NOTE: Not exist in the original game.
	short tilt = 0, neck = 0, head = 0, angle = 0;

	if (item->hitPoints <= 0)
	{
		if (item->currentAnimState != CULT2_DEATH)
			SetAnimation(item, CULT2_DEATH_ANIM, CULT2_DEATH);
	}
	else
	{
		AI_INFO ai = {};
		bool isLeft = false;

		CreatureAIInfo(item, &ai);
		CreatureMood(item, &ai, FALSE);
		angle = CreatureTurn(item, cult2->maximumTurn);

		switch (item->currentAnimState)
		{
		case CULT2_STOP:
			cult2->maximumTurn = 0;
			if (ai.ahead)
				neck = ai.angle;

			if (cult2->mood == MOOD_ESCAPE)
				item->goalAnimState = CULT2_RUN;
			else if (Targetable(item, &ai))
				item->goalAnimState = CULT2_AIM2;
			else if (cult2->mood == MOOD_BORED)
			{
				if (!ai.ahead || ai.distance > CULT2_KNIFE_RANGE)
					item->goalAnimState = CULT2_WALK;
			}
			else if (ai.ahead && ai.distance < CULT2_WALK_RANGE)
				item->goalAnimState = CULT2_WALK;
			else
				item->goalAnimState = CULT2_RUN;

			break;
		case CULT2_WALK:
			cult2->maximumTurn = CULT2_WALK_TURN;
			if (ai.ahead)
				neck = ai.angle;

			if (cult2->mood == MOOD_ESCAPE)
				item->goalAnimState = CULT2_RUN;
			else if (Targetable(item, &ai))
			{
				if (ai.distance < CULT2_STOP_RANGE || ai.zoneNumber != ai.enemyZone)
					item->goalAnimState = CULT2_STOP;
				else if (GetRandomControl() & 0x4000)
					item->goalAnimState = CULT2_AIM1L;
				else
					item->goalAnimState = CULT2_AIM1R;
			}
			else if (cult2->mood == MOOD_BORED)
			{
				if (ai.ahead && ai.distance < CULT2_KNIFE_RANGE)
					item->goalAnimState = CULT2_STOP;
			}
			else if (!ai.ahead || ai.distance > CULT2_WALK_RANGE)
				item->goalAnimState = CULT2_RUN;

			break;
		case CULT2_RUN:
			cult2->maximumTurn = CULT2_RUN_TURN;
			tilt = angle >> 2;
			if (ai.ahead)
				neck = ai.angle;

			if (cult2->mood == MOOD_ESCAPE)
				break;
			else if (Targetable(item, &ai))
				item->goalAnimState = CULT2_WALK;
			else if (cult2->mood == MOOD_BORED)
			{
				if (ai.ahead && ai.distance < CULT2_KNIFE_RANGE)
					item->goalAnimState = CULT2_STOP;
				else
					item->goalAnimState = CULT2_WALK;
			}
			else if (ai.ahead && ai.distance < CULT2_WALK_RANGE)
				item->goalAnimState = CULT2_WALK;

			break;
		case CULT2_AIM1L:
		case CULT2_AIM1R:
			cult2->flags = 0;
			if (ai.ahead)
				head = ai.angle;

			isLeft = (item->currentAnimState == CULT2_AIM1L);
			if (Targetable(item, &ai))
				item->goalAnimState = isLeft ? CULT2_SHOOT1L : CULT2_SHOOT1R;
			else
				item->goalAnimState = CULT2_WALK;

			break;
		case CULT2_AIM2:
			cult2->flags = 0;
			if (ai.ahead)
				head = ai.angle;
			
			if (Targetable(item, &ai))
				item->goalAnimState = CULT2_SHOOT2;
			else
				item->goalAnimState = CULT2_WALK;

			break;
		case CULT2_SHOOT1L:
		case CULT2_SHOOT1R:
			if (ai.ahead)
				head = ai.angle;

			isLeft = (item->currentAnimState == CULT2_SHOOT1L);
			if (!cult2->flags)
			{
				CreatureEffect(item, isLeft ? &Cult2Left : &Cult2Right, Knife);
				cult2->flags = 1;
			}

			break;
		case CULT2_SHOOT2:
			if (ai.ahead)
				head = ai.angle;

			if (!cult2->flags)
			{
				CreatureEffect(item, &Cult2Left, Knife);
				CreatureEffect(item, &Cult2Right, Knife);
				cult2->flags = 1;
			}

			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureNeck(item, neck);
	CreatureHead(item, head);
	CreatureAnimation(itemNumber, angle, 0);
}

void MonkControl(short itemNumber)
{
	if (!CreatureActive(itemNumber))
		return;

	ITEM_INFO* item = &Items[itemNumber];
	CREATURE_INFO* monk = GetCreatureInfo(item);
	if (monk == NULL) return; // NOTE: Not exist in the original game.
	AI_INFO ai{};
	int rand = 0;
	short tilt = 0, head = 0, angle = 0;

	if (item->hitPoints <= 0)
	{
		if (item->currentAnimState != MONK_DEATH)
		{
			item->animNumber = (Objects[item->objectID].animIndex + GetRandomControl() / 0x4000) + MONK_DEATH_ANIM;
			item->frameNumber = Anims[item->animNumber].frameBase;
			item->currentAnimState = MONK_DEATH;
		}
	}
	else
	{
		CreatureAIInfo(item, &ai);
		CreatureMood(item, &ai, TRUE);
		angle = CreatureTurn(item, monk->maximumTurn);

		if (ai.ahead)
			head = ai.angle;

		switch (item->currentAnimState)
		{
		case MONK_STOP1:
			monk->flags &= ~0x1000;
			monk->flags &= 0xFFF;

			if (IsMonkAngry || !ai.ahead || Lara.target != item)
			{
				if (monk->mood != MOOD_BORED)
				{
					if (monk->mood == MOOD_ESCAPE)
					{
						item->goalAnimState = MONK_RUN;
					}
					else if (ai.ahead && ai.distance < MONK_CLOSE_RANGE)
					{
						if (GetRandomControl() >= 0x7000)
							item->goalAnimState = MONK_STOP2;
						else
							item->goalAnimState = MONK_ATTACK1;
					}
					else if (ai.ahead && ai.distance < MONK_LONG_RANGE)
						item->goalAnimState = MONK_ATTACK4;
					else if (ai.ahead && ai.distance < MONK_WALK_RANGE)
						item->goalAnimState = MONK_WALK;
					else
						item->goalAnimState = MONK_RUN;
				}
				else
				{
					item->goalAnimState = MONK_WALK;
				}
			}
			break;
		case MONK_STOP2:
			monk->flags &= ~0x1000;
			monk->flags &= 0xFFF;

			if (IsMonkAngry || !ai.ahead || Lara.target != item)
			{
				if (monk->mood != MOOD_BORED)
				{
					if (monk->mood == MOOD_ESCAPE)
						item->goalAnimState = MONK_RUN;
					else if (ai.ahead && ai.distance < MONK_CLOSE_RANGE)
					{
						rand = GetRandomControl();
						if (rand < 0x3000)
							item->goalAnimState = MONK_ATTACK2;
						else if (rand < 0x6000)
							item->goalAnimState = MONK_AIM;
						else
							item->goalAnimState = MONK_STOP1;
					}
					else if (ai.ahead && ai.distance < MONK_WALK_RANGE)
						item->goalAnimState = MONK_WALK;
					else
						item->goalAnimState = MONK_RUN;
				}
				else
					item->goalAnimState = MONK_WALK;
			}
			break;
		case MONK_WALK:
			monk->maximumTurn = 546;

			if (monk->mood != MOOD_BORED)
			{
				if (monk->mood == MOOD_ESCAPE)
				{
					item->goalAnimState = MONK_RUN;
				}
				else if (ai.ahead && ai.distance < 0x40000)
				{
					if (GetRandomControl() >= 0x4000)
						item->goalAnimState = MONK_STOP2;
					else
						item->goalAnimState = MONK_STOP1;
				}
				else if (!ai.ahead || ai.distance > 0x400000)
				{
					item->goalAnimState = MONK_RUN;
				}
			}
			else if (!IsMonkAngry && ai.ahead && Lara.target == item)
			{
				if (GetRandomControl() >= 0x4000)
					item->goalAnimState = MONK_STOP2;
				else
					item->goalAnimState = MONK_STOP1;
			}
			break;
		case MONK_RUN:
			monk->flags &= ~0x1000;
			monk->flags &= 0xFFF;
			if (IsMonkAngry)
				monk->maximumTurn = 910;
			else
				monk->maximumTurn = 728;
			tilt = angle >> 2;

			if (monk->mood != MOOD_BORED && monk->mood != MOOD_ESCAPE)
			{
				if (ai.ahead && ai.distance < 0x40000)
				{
					if (GetRandomControl() >= 0x4000)
						item->goalAnimState = MONK_STOP2;
					else
						item->goalAnimState = MONK_STOP1;
				}
				else if (ai.ahead)
				{
					if (ai.distance >= 0x900000)
					{
						if (ai.ahead && ai.distance < 0x400000)
						{
							if (GetRandomControl() >= 0x4000)
								item->goalAnimState = MONK_STOP2;
							else
								item->goalAnimState = MONK_STOP1;
						}
					}
					else
					{
						item->goalAnimState = MONK_ATTACK5;
					}
				}
			}
			else
			{
				item->goalAnimState = MONK_STOP1;
			}
			break;
		case MONK_AIM:
			if (ai.ahead && ai.distance <= 0x40000)
				item->goalAnimState = MONK_ATTACK3;
			else
				item->goalAnimState = MONK_STOP2;
			break;
		case MONK_ATTACK1:
		case MONK_ATTACK2:
		case MONK_ATTACK3:
		case MONK_ATTACK4:
		case MONK_ATTACK5:
			if (!(monk->flags & 0x1000) && monk->enemy != NULL)
			{
				if (monk->enemy == LaraItem)
				{
					if (CHK_ANY(item->touchBits, MONK_TOUCHBITS))
					{
						monk->enemy->hitPoints -= MONK_DAMAGE;
						monk->enemy->hitStatus = 1;
						monk->flags |= 0x1000;
						PlaySoundEffect(245, &item->pos, 0);
						CreatureEffect(item, &MonkBite, DoBloodSplat);
					}
				}
				else if (IsCreatureNearTarget(item, monk->enemy))
				{
					DamageTarget(item, monk->enemy, &MonkBite, MONK_DAMAGE_TO_OTHER_ENEMIES);
					PlaySoundEffect(245, &item->pos, 0);
					monk->flags |= 0x1000;
				}
			}
			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureHead(item, head);
	CreatureAnimation(itemNumber, angle, 0);
}

void WarriorSparkleTrail(ITEM_INFO* item)
{
	short fxNum = CreateEffect(item->roomNumber);
	if (fxNum != -1)
	{
		FX_INFO* fx = &Effects[fxNum];
		fx->pos.x = item->pos.x + (GetRandomDraw() * 256 >> 15) - 128;
		fx->pos.y = item->pos.y + (GetRandomDraw() * 256 >> 15) - 256;
		fx->pos.z = item->pos.z + (GetRandomDraw() * 256 >> 15) - 128;
		fx->roomNumber = item->roomNumber;
		fx->counter = -30;
		fx->frameNumber = 0;
		fx->objectID = ID_TWINKLE;
	}
	PlaySoundEffect(312, &item->pos, 0);
}

 /*
  * Inject function
  */
void Inject_Enemies() {
	INJECT(0x0041DB30, Knife);
	INJECT(0x0041DBB0, Cult2Control);
	INJECT(0x0041DFE0, MonkControl);
	//INJECT(0x0041E4B0, Worker3Control);
	//INJECT(0x0041EAC0, DrawXianLord);
	//INJECT(0x0041EEC0, XianDamage);
	//INJECT(0x0041EF70, InitialiseXianLord);
	//INJECT(0x0041EFD0, XianLordControl);
	INJECT(0x0041F5B0, WarriorSparkleTrail);
	//INJECT(0x0041F650, WarriorControl);
}
