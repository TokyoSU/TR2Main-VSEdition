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
#include "game/larafire.h"
#include "game/effects.h"
#include "game/sound.h"
#include "specific/game.h"
#include "global/vars.h"

#define MONK_DAMAGE_TO_OTHER_ENEMIES 5
#define MONK_DAMAGE 150
#define MONK_DEATH_ANIM 20
#define MONK_TOUCHBITS MESH_BITS(14)
#define MONK_CLOSE_RANGE SQR(WALL_SIZE/2)
#define MONK_LONG_RANGE SQR(WALL_SIZE)
#define MONK_ATTACK5_RANGE SQR(WALL_SIZE*3)
#define MONK_WALK_RANGE SQR(WALL_SIZE*2)
#define MONK_HIT_RANGE (CLICK_SIZE*2)

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

static const BITE_INFO MonkBite = { -23, 16, 265, 14 };

void MonkControl(short itemID)
{
	if (!CreatureActive(itemID))
		return;

	ITEM_INFO* item = &Items[itemID];
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
		angle = CreatureTurn(item, monk->maximum_turn);

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
			monk->maximum_turn = 546;

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
				monk->maximum_turn = 910;
			else
				monk->maximum_turn = 728;
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
	CreatureAnimation(itemID, angle, 0);
}

 /*
  * Inject function
  */
void Inject_Enemies() {
	//INJECT(0x0041DB30, Knife);
	//INJECT(0x0041DBB0, Cult2Control);
	INJECT(0x0041DFE0, MonkControl);
	//INJECT(0x0041E4B0, Worker3Control);
	//INJECT(0x0041EAC0, DrawXianLord);
	//INJECT(0x0041EEC0, XianDamage);
	//INJECT(0x0041EF70, InitialiseXianLord);
	//INJECT(0x0041EFD0, XianLordControl);
	//INJECT(0x0041F5B0, WarriorSparkleTrail);
	//INJECT(0x0041F650, WarriorControl);
}
