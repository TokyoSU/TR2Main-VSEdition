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
#include "3dsystem/3d_gen.h"
#include "game/box.h"
#include "game/items.h"
#include "game/larafire.h"
#include "game/draw.h"
#include "game/effects.h"
#include "game/missile.h"
#include "game/people.h"
#include "game/lot.h"
#include "game/sound.h"
#include "specific/game.h"
#include "specific/output.h"
#include "global/vars.h"

// Guy with a stick (Climb)
enum Worker3State {
	WORK3_EMPTY,
	WORK3_STOP,
	WORK3_WALK,
	WORK3_PUNCH2,
	WORK3_AIM2,
	WORK3_WAIT,
	WORK3_AIM1,
	WORK3_AIM0,
	WORK3_PUNCH1,
	WORK3_PUNCH0,
	WORK3_RUN,
	WORK3_DEATH,
	WORK3_CLIMB3,
	WORK3_CLIMB1,
	WORK3_CLIMB2,
	WORK3_FALL3
};

// Xian Warrior (Sword)
enum WarriorState {
	WARRIOR_EMPTY,
	WARRIOR_STOP,
	WARRIOR_WALK,
	WARRIOR_AIM1,
	WARRIOR_SLASH1,
	WARRIOR_AIM2,
	WARRIOR_SLASH2,
	WARRIOR_WAIT,
	WARRIOR_FLY,
	WARRIOR_START,
	WARRIOR_AIM3,
	WARRIOR_SLASH3,
	WARRIOR_DEATH
};

// Xian Lord (2 Spear)
enum XianState {
	XIAN_EMPTY,
	XIAN_STOP,
	XIAN_STOP2,
	XIAN_WALK,
	XIAN_RUN,
	XIAN_AIM1,
	XIAN_HIT1,
	XIAN_AIM2,
	XIAN_HIT2,
	XIAN_AIM3,
	XIAN_HIT3,
	XIAN_AIM4,
	XIAN_HIT4,
	XIAN_AIM5,
	XIAN_HIT5,
	XIAN_AIM6,
	XIAN_HIT6,
	XIAN_DEATH,
	XIAN_START,
	XIAN_KILL
};

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

// Knifethrower
enum Cult2State
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

#define WORK3_HIT_DAMAGE 80
#define WORK3_SWIPE_DAMAGE 100
#define WORK3_HIT_DAMAGE_TO_OTHER 3
#define WORK3_SWIPE_DAMAGE_TO_OTHER 6
#define WORK3_WALK_TURN ANGLE(5)
#define WORK3_RUN_TURN ANGLE(6)
#define WORK3_ATTACK0_RANGE SQR(BLOCK(1)/2)
#define WORK3_ATTACK1_RANGE SQR(BLOCK(1))
#define WORK3_ATTACK2_RANGE SQR(BLOCK(3)/2)
#define WORK3_WALK_RANGE SQR(BLOCK(2))
#define WORK3_WALK_CHANCE 0x100
#define WORK3_WAIT_CHANCE 0x100
#define WORK3_DIE_ANIM 26
#define WORK3_CLIMB1_ANIM 28
#define WORK3_CLIMB2_ANIM 29
#define WORK3_CLIMB3_ANIM 27
#define WORK3_FALL3_ANIM  30
#define WORK3_TOUCH 0x600
#define WORK3_VAULT_SHIFT 260

#define XIAN_HIT1_DAMAGE 75
#define XIAN_HIT2_DAMAGE 75
#define XIAN_HIT5_DAMAGE 75
#define XIAN_HIT6_DAMAGE 120
#define XIAN_HIT1_DAMAGE_TO_OTHER 3
#define XIAN_HIT2_DAMAGE_TO_OTHER 3
#define XIAN_HIT5_DAMAGE_TO_OTHER 3
#define XIAN_HIT6_DAMAGE_TO_OTHER 5
#define XIAN_WALK_TURN ANGLE(3)
#define XIAN_RUN_TURN ANGLE(5)
#define XIAN_STOP_CHANCE 0x200
#define XIAN_WALK_CHANCE (XIAN_STOP_CHANCE + 0x200)
#define XIAN_ATTACK1_RANGE SQR(BLOCK(1))
#define XIAN_ATTACK2_RANGE SQR(BLOCK(3)/2)
#define XIAN_ATTACK3_RANGE SQR(BLOCK(2))
#define XIAN_ATTACK4_RANGE SQR(BLOCK(2))
#define XIAN_ATTACK5_RANGE SQR(BLOCK(1))
#define XIAN_ATTACK6_RANGE SQR(BLOCK(2))
#define XIAN_RUN_RANGE SQR(BLOCK(3))
#define XIAN_DIE_ANIM 0
#define XIAN_START_ANIM 48
#define XIAN_KILL_ANIM 49
#define XIAN_TOUCHL MESH_BITS(11)
#define XIAN_TOUCHR MESH_BITS(18)

#define WARRIOR_DAMAGE 300
#define WARRIOR_DAMAGE_TO_OTHER 7
#define WARRIOR_TOUCH 0x0C000
#define WARRIOR_WALK_TURN ANGLE(5)
#define WARRIOR_FLY_TURN ANGLE(4)
#define WARRIOR_ATTACK1_RANGE SQR(BLOCK(1))
#define WARRIOR_ATTACK3_RANGE SQR(BLOCK(2))

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

static const BITE_INFO Worker3Bite = { 247, 10, 11, 10 };
static const BITE_INFO MonkBite = { -23, 16, 265, 14 };
static const BITE_INFO Cult2LeftBite = { 0, 0, 0, 5 };
static const BITE_INFO Cult2RightBite = { 0, 0, 0, 8 };
static const BITE_INFO WarriorSwordBite = { 0, 37, 550, 15 };
static const BITE_INFO XianLeftBite = { 0, 0, 920, 11 };
static const BITE_INFO XianRightBite = { 0, 0, 920, 18 };

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
	CREATURE_INFO* creature = GetCreatureInfo(item);
	if (creature == NULL) return; // NOTE: Not exist in the original game.
	short tilt = 0, neck = 0, head = 0, angle = 0;
	bool isLeft = false;

	if (item->hitPoints <= 0)
	{
		if (item->currentAnimState != CULT2_DEATH)
			SetAnimation(item, CULT2_DEATH_ANIM, CULT2_DEATH);
	}
	else
	{
		AI_INFO ai;
		CreatureAIInfo(item, &ai);
		CreatureMood(item, &ai, FALSE);
		angle = CreatureTurn(item, creature->maximumTurn);

		switch (item->currentAnimState)
		{
		case CULT2_STOP:
			creature->maximumTurn = 0;
			if (ai.ahead)
				neck = ai.angle;

			if (creature->mood == MOOD_ESCAPE)
				item->goalAnimState = CULT2_RUN;
			else if (Targetable(item, &ai))
				item->goalAnimState = CULT2_AIM2;
			else if (creature->mood == MOOD_BORED)
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
			creature->maximumTurn = CULT2_WALK_TURN;
			if (ai.ahead)
				neck = ai.angle;

			if (creature->mood == MOOD_ESCAPE)
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
			else if (creature->mood == MOOD_BORED)
			{
				if (ai.ahead && ai.distance < CULT2_KNIFE_RANGE)
					item->goalAnimState = CULT2_STOP;
			}
			else if (!ai.ahead || ai.distance > CULT2_WALK_RANGE)
				item->goalAnimState = CULT2_RUN;

			break;
		case CULT2_RUN:
			creature->maximumTurn = CULT2_RUN_TURN;
			tilt = angle >> 2;
			if (ai.ahead)
				neck = ai.angle;

			if (creature->mood == MOOD_ESCAPE)
				break;
			else if (Targetable(item, &ai))
				item->goalAnimState = CULT2_WALK;
			else if (creature->mood == MOOD_BORED)
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
			creature->flags = 0;
			if (ai.ahead)
				head = ai.angle;

			isLeft = (item->currentAnimState == CULT2_AIM1L);
			if (Targetable(item, &ai))
				item->goalAnimState = isLeft ? CULT2_SHOOT1L : CULT2_SHOOT1R;
			else
				item->goalAnimState = CULT2_WALK;

			break;
		case CULT2_AIM2:
			creature->flags = 0;
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
			if (!creature->flags)
			{
				CreatureEffect(item, isLeft ? &Cult2LeftBite : &Cult2RightBite, Knife);
				creature->flags = 1;
			}

			break;
		case CULT2_SHOOT2:
			if (ai.ahead)
				head = ai.angle;

			if (!creature->flags)
			{
				CreatureEffect(item, &Cult2LeftBite, Knife);
				CreatureEffect(item, &Cult2RightBite, Knife);
				creature->flags = 1;
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
	CREATURE_INFO* creature = GetCreatureInfo(item);
	if (creature == NULL) return; // NOTE: Not exist in the original game.
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
		AI_INFO ai;
		CreatureAIInfo(item, &ai);
		CreatureMood(item, &ai, TRUE);
		angle = CreatureTurn(item, creature->maximumTurn);

		if (ai.ahead)
			head = ai.angle;

		switch (item->currentAnimState)
		{
		case MONK_STOP1:
			creature->flags &= ~0x1000;
			creature->flags &= 0xFFF;

			if (IsMonkAngry || !ai.ahead || Lara.target != item)
			{
				if (creature->mood != MOOD_BORED)
				{
					if (creature->mood == MOOD_ESCAPE)
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
			creature->flags &= ~0x1000;
			creature->flags &= 0xFFF;

			if (IsMonkAngry || !ai.ahead || Lara.target != item)
			{
				if (creature->mood != MOOD_BORED)
				{
					if (creature->mood == MOOD_ESCAPE)
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
			creature->maximumTurn = 546;

			if (creature->mood != MOOD_BORED)
			{
				if (creature->mood == MOOD_ESCAPE)
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
			creature->flags &= ~0x1000;
			creature->flags &= 0xFFF;
			if (IsMonkAngry)
				creature->maximumTurn = 910;
			else
				creature->maximumTurn = 728;
			tilt = angle >> 2;

			if (creature->mood != MOOD_BORED && creature->mood != MOOD_ESCAPE)
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
			if (!(creature->flags & 0x1000) && creature->enemy != NULL)
			{
				if (creature->enemy == LaraItem)
				{
					if (CHK_ANY(item->touchBits, MONK_TOUCHBITS))
					{
						creature->enemy->hitPoints -= MONK_DAMAGE;
						creature->enemy->hitStatus = 1;
						creature->flags |= 0x1000;
						PlaySoundEffect(245, &item->pos, 0);
						CreatureEffect(item, &MonkBite, DoBloodSplat);
					}
				}
				else if (IsCreatureNearTarget(item, creature->enemy))
				{
					DamageTarget(item, creature->enemy, &MonkBite, MONK_DAMAGE_TO_OTHER_ENEMIES);
					PlaySoundEffect(245, &item->pos, 0);
					creature->flags |= 0x1000;
				}
			}
			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureHead(item, head);
	CreatureAnimation(itemNumber, angle, 0);
}

void Worker3Control(short itemNumber)
{
	if (!CreatureActive(itemNumber))
		return;

	ITEM_INFO* item = &Items[itemNumber];
	CREATURE_INFO* creature = GetCreatureInfo(item);
	if (creature == NULL) return; // NOTE: Not exist in the original game.
	short head = 0, neck = 0, angle = 0, tilt = 0;

	if (item->hitPoints <= 0)
	{
		if (item->currentAnimState != WORK3_DEATH)
			SetAnimation(item, WORK3_DIE_ANIM, WORK3_DEATH);
	}
	else
	{
		AI_INFO ai;
		CreatureAIInfo(item, &ai);
		CreatureMood(item, &ai, TRUE);
		angle = CreatureTurn(item, creature->maximumTurn);

		switch (item->currentAnimState)
		{
		case WORK3_STOP:
			creature->maximumTurn = 0;
			creature->flags = 0;
			if (ai.ahead)
				neck = ai.angle;

			if (creature->mood == MOOD_ESCAPE)
				item->goalAnimState = WORK3_RUN;
			else if (creature->mood == MOOD_BORED)
			{
				if (item->requiredAnimState)
					item->goalAnimState = item->requiredAnimState;
				else if (GetRandomControl() < 0x4000)
					item->goalAnimState = WORK3_WALK;
				else
					item->goalAnimState = WORK3_WAIT;
			}
			else if (ai.bite && ai.distance < WORK3_ATTACK0_RANGE)
				item->goalAnimState = WORK3_AIM0;
			else if (ai.bite && ai.distance < WORK3_ATTACK1_RANGE)
				item->goalAnimState = WORK3_AIM1;
			else if (ai.bite && ai.distance < WORK3_WALK_RANGE)
				item->goalAnimState = WORK3_WALK;
			else
				item->goalAnimState = WORK3_RUN;
			break;

		case WORK3_WAIT:
			if (ai.ahead)
				neck = ai.angle;

			if (creature->mood != MOOD_BORED)
				item->goalAnimState = WORK3_STOP;
			else if (GetRandomControl() < WORK3_WALK_CHANCE)
			{
				item->requiredAnimState = WORK3_WALK;
				item->goalAnimState = WORK3_STOP;
			}
			break;

		case WORK3_WALK:
			if (ai.ahead)
				neck = ai.angle;

			creature->maximumTurn = WORK3_WALK_TURN;

			if (creature->mood == MOOD_ESCAPE)
				item->goalAnimState = WORK3_RUN;
			else if (creature->mood == MOOD_BORED)
			{
				if (GetRandomControl() < WORK3_WAIT_CHANCE)
				{
					item->requiredAnimState = WORK3_WAIT;
					item->goalAnimState = WORK3_STOP;
				}
			}
			else if (ai.bite && ai.distance < WORK3_ATTACK0_RANGE)
				item->goalAnimState = WORK3_STOP;
			else if (ai.bite && ai.distance < WORK3_ATTACK2_RANGE)
				item->goalAnimState = WORK3_AIM2;
			else
				item->goalAnimState = WORK3_RUN;
			break;

		case WORK3_RUN:
			if (ai.ahead)
				neck = ai.angle;

			creature->maximumTurn = WORK3_RUN_TURN;
			tilt = angle / 2;

			if (creature->mood == MOOD_ESCAPE)
				break;
			else if (creature->mood == MOOD_BORED)
				item->goalAnimState = WORK3_WALK;
			else if (ai.ahead && ai.distance < WORK3_WALK_RANGE)
				item->goalAnimState = WORK3_WALK;
			break;

		case WORK3_AIM0:
			if (ai.ahead)
				head = ai.angle;

			creature->flags = 0;
			if (ai.bite && ai.distance < WORK3_ATTACK0_RANGE)
				item->goalAnimState = WORK3_PUNCH0;
			else
				item->goalAnimState = WORK3_STOP;
			break;

		case WORK3_AIM1:
			if (ai.ahead)
				head = ai.angle;

			creature->flags = 0;
			if (ai.ahead && ai.distance < WORK3_ATTACK1_RANGE)
				item->goalAnimState = WORK3_PUNCH1;
			else
				item->goalAnimState = WORK3_STOP;
			break;

		case WORK3_AIM2:
			if (ai.ahead)
				head = ai.angle;

			creature->flags = 0;
			if (ai.bite && ai.distance < WORK3_ATTACK2_RANGE)
				item->goalAnimState = WORK3_PUNCH2;
			else
				item->goalAnimState = WORK3_WALK;
			break;

		case WORK3_PUNCH0:
			if (ai.ahead)
				head = ai.angle;

			if (!(creature->flags & 1))
			{
				if (DamageLaraOrEnemy(item, creature->enemy, &Worker3Bite, WORK3_HIT_DAMAGE, WORK3_HIT_DAMAGE_TO_OTHER, item->touchBits & WORK3_TOUCH))
				{
					PlaySoundEffect(72, &item->pos, NULL);
					creature->flags |= 1;
				}
			}
			break;

		case WORK3_PUNCH1:
			if (ai.ahead)
				head = ai.angle;

			if (!(creature->flags & 2))
			{
				if (DamageLaraOrEnemy(item, creature->enemy, &Worker3Bite, WORK3_HIT_DAMAGE, WORK3_HIT_DAMAGE_TO_OTHER, item->touchBits & WORK3_TOUCH))
				{
					PlaySoundEffect(71, &item->pos, NULL);
					creature->flags |= 2;
				}
			}

			if (ai.ahead && ai.distance > WORK3_ATTACK1_RANGE && ai.distance < WORK3_ATTACK2_RANGE)
				item->goalAnimState = WORK3_PUNCH2;
			break;

		case WORK3_PUNCH2:
			if (ai.ahead)
				head = ai.angle;

			if (!(creature->flags & 4))
			{
				if (DamageLaraOrEnemy(item, creature->enemy, &Worker3Bite, WORK3_SWIPE_DAMAGE, WORK3_SWIPE_DAMAGE_TO_OTHER, item->touchBits & WORK3_TOUCH))
				{
					PlaySoundEffect(71, &item->pos, NULL);
					creature->flags |= 4;
				}
			}
			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureHead(item, head);
	CreatureNeck(item, neck);

	if (item->currentAnimState < WORK3_CLIMB3)
	{
		switch (CreatureVault(itemNumber, angle, 2, WORK3_VAULT_SHIFT))
		{
		case 2:
			SetAnimation(item, WORK3_CLIMB1_ANIM, WORK3_CLIMB1);
			break;

		case 3:
			SetAnimation(item, WORK3_CLIMB2_ANIM, WORK3_CLIMB2);
			break;

		case 4:
			SetAnimation(item, WORK3_CLIMB3_ANIM, WORK3_CLIMB3);
			break;
		case -4:
			SetAnimation(item, WORK3_FALL3_ANIM, WORK3_FALL3);
			break;
		}
	}
	else
	{
		CreatureAnimation(itemNumber, angle, 0);
	}
}

void DrawXianLord(ITEM_INFO* item)
{
	short* frames[2];
	int rate;
	int frac = GetFrames(item, frames, &rate);

	OBJECT_INFO* obj = &Objects[item->objectID];
	if (obj->shadowSize)
		S_PrintShadow(obj->shadowSize, frames[0], item);

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x, item->pos.y, item->pos.z);
	phd_RotYXZ(item->pos.rotY, item->pos.rotX, item->pos.rotZ);

	int clip = S_GetObjectBounds(frames[0]);
	if (clip)
	{
		CalculateObjectLighting(item, frames[0]);
		short* extra_rotation = (short*)item->data;
		short** meshpp, **jadepp;

		if (item->objectID == ID_XIAN_LORD)
		{
			meshpp = &MeshPtr[Objects[ID_XIAN_LORD].meshIndex];
			jadepp = &MeshPtr[Objects[ID_CHINESE2].meshIndex];
		}
		else
		{
			meshpp = &MeshPtr[Objects[ID_WARRIOR].meshIndex];
			jadepp = &MeshPtr[Objects[ID_CHINESE4].meshIndex];
		}

		int* bone = &AnimBones[obj->boneIndex];
		int bit = 1;

		if (!frac)
		{
			phd_TranslateRel((int)*(frames[0] + 6), (int)*(frames[0] + 7), (int)*(frames[0] + 8));
			UINT16* rotation1 = (UINT16*)(frames[0] + 9);
			phd_RotYXZsuperpack(&rotation1, 0);

			if (bit & item->meshBits)
				phd_PutPolygons(*meshpp, clip);
			else
				phd_PutPolygons(*jadepp, clip);
			meshpp++;
			jadepp++;

			for (int i = obj->nMeshes - 1; i > 0; i--, bone += 4, meshpp++, jadepp++)
			{
				int poppush = *(bone);
				if (poppush & 1)
					phd_PopMatrix();
				if (poppush & 2)
					phd_PushMatrix();

				phd_TranslateRel(*(bone + 1), *(bone + 2), *(bone + 3));
				phd_RotYXZsuperpack(&rotation1, 0);

				if (extra_rotation && (poppush & (8 | 4 | 16)))
				{
					if (poppush & 8)
						phd_RotY(*(extra_rotation++));
					if (poppush & 4)
						phd_RotX(*(extra_rotation++));
					if (poppush & 16)
						phd_RotZ(*(extra_rotation++));
				}

				bit <<= 1;
				if (bit & item->meshBits)
					phd_PutPolygons(*meshpp, clip);
				else
					phd_PutPolygons(*jadepp, clip);
			}
		}
		else
		{
			InitInterpolate(frac, rate);
			phd_TranslateRel_ID((int)*(frames[0] + 6), (int)*(frames[0] + 7), (int)*(frames[0] + 8),
				                (int)*(frames[1] + 6), (int)*(frames[1] + 7), (int)*(frames[1] + 8));
			UINT16* rotation1 = (UINT16*)(frames[0] + 9);
			UINT16* rotation2 = (UINT16*)(frames[1] + 9);
			phd_RotYXZsuperpack_I(&rotation1, &rotation2, 0);

			if (bit & item->meshBits)
				phd_PutPolygons_I(*meshpp, clip);
			else
				phd_PutPolygons_I(*jadepp, clip);
			meshpp++;
			jadepp++;

			for (int i = obj->nMeshes - 1; i > 0; i--, bone += 4, meshpp++, jadepp++)
			{
				int poppush = *(bone);
				if (poppush & 1)
					phd_PopMatrix_I();
				if (poppush & 2)
					phd_PushMatrix_I();

				phd_TranslateRel_I(*(bone + 1), *(bone + 2), *(bone + 3));
				phd_RotYXZsuperpack_I(&rotation1, &rotation2, 0);

				if (extra_rotation && (poppush & (8 | 4 | 16)))
				{
					if (poppush & 8)
						phd_RotY_I(*(extra_rotation++));
					if (poppush & 4)
						phd_RotX_I(*(extra_rotation++));
					if (poppush & 16)
						phd_RotZ_I(*(extra_rotation++));
				}

				bit <<= 1;
				if (bit & item->meshBits)
					phd_PutPolygons_I(*meshpp, clip);
				else
					phd_PutPolygons_I(*jadepp, clip);
			}

		}
	}
	phd_PopMatrix();
}

void XianDamage(ITEM_INFO* item, CREATURE_INFO* creature, int damage, int damageOther)
{
	if (!(creature->flags & 0x1))
	{
		if (DamageLaraOrEnemy(item, creature->enemy, &XianRightBite, damage, damageOther, item->touchBits & XIAN_TOUCHR))
		{
			PlaySoundEffect(318, &item->pos, NULL);
			creature->flags |= 0x1;
		}
	}
	if (!(creature->flags & 0x2))
	{
		if (DamageLaraOrEnemy(item, creature->enemy, &XianLeftBite, damage, damageOther, item->touchBits & XIAN_TOUCHL))
		{
			PlaySoundEffect(318, &item->pos, NULL);
			creature->flags |= 0x2;
		}
	}
}

void InitialiseXianLord(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];
	if (item->objectID == ID_XIAN_LORD)
		SetAnimation(item, XIAN_START_ANIM, XIAN_START);
	item->status = ITEM_INACTIVE;
	item->meshBits = 0;
}

void XianLordControl(short itemNumber)
{
	if (!CreatureActive(itemNumber))
		return;

	ITEM_INFO* item = &Items[itemNumber];
	CREATURE_INFO* creature = GetCreatureInfo(item);
	if (creature == NULL) return; // NOTE: Not exist in the original game.
	int random = 0;
	short head = 0, neck = 0, angle = 0, tilt = 0;
	bool lara_alive = (LaraItem->hitPoints > 0);

	if (item->hitPoints <= 0)
	{
		item->currentAnimState = XIAN_DEATH;
		item->meshBits >>= 1;
		if (item->meshBits == 0)
		{
			PlaySoundEffect(105, NULL, 0);
			item->meshBits = 0xFFFFFFFF;
			item->objectID = ID_CHINESE2;
			ExplodingDeath(itemNumber, 0xFFFFFFFF, 0);
			item->objectID = ID_XIAN_LORD;
			DisableBaddieAI(itemNumber);
			KillItem(itemNumber);
			item->status = ITEM_DISABLED;
			item->flags |= IFL_ONESHOT;
		}
		return;
	}
	else
	{
		AI_INFO ai;
		CreatureAIInfo(item, &ai);
		CreatureMood(item, &ai, TRUE);
		angle = CreatureTurn(item, creature->maximumTurn);
		if (item->currentAnimState != XIAN_START)
			item->meshBits = 0xFFFFFFFF;

		switch (item->currentAnimState)
		{
		case XIAN_START:
			if (!creature->flags)
			{
				item->meshBits = (item->meshBits << 1) + 1;
				creature->flags = 3;
			}
			else
			{
				creature->flags--;
			}
			break;

		case XIAN_STOP:
			creature->maximumTurn = 0;
			if (ai.ahead)
				neck = ai.angle;

			if (creature->mood == MOOD_BORED)
			{
				random = GetRandomControl();
				if (random < XIAN_STOP_CHANCE)
					item->goalAnimState = XIAN_STOP2;
				else if (random < XIAN_WALK_CHANCE)
					item->goalAnimState = XIAN_WALK;
			}
			else if (ai.ahead && ai.distance < XIAN_ATTACK1_RANGE)
				item->goalAnimState = XIAN_AIM1;
			else
				item->goalAnimState = XIAN_WALK;
			break;

		case XIAN_STOP2:
			creature->maximumTurn = 0;
			if (ai.ahead)
				neck = ai.angle;

			if (creature->mood == MOOD_ESCAPE)
				item->goalAnimState = XIAN_WALK;
			else if (creature->mood == MOOD_BORED)
			{
				random = GetRandomControl();
				if (random < XIAN_STOP_CHANCE)
					item->goalAnimState = XIAN_STOP;
				else if (random < XIAN_WALK_CHANCE)
					item->goalAnimState = XIAN_WALK;
			}
			else if (ai.ahead && ai.distance < XIAN_ATTACK5_RANGE)
				item->goalAnimState = XIAN_AIM5;
			else
				item->goalAnimState = XIAN_WALK;
			break;

		case XIAN_WALK:
			creature->maximumTurn = XIAN_WALK_TURN;
			if (ai.ahead)
				neck = ai.angle;

			if (creature->mood == MOOD_ESCAPE)
				item->goalAnimState = XIAN_RUN;
			else if (creature->mood == MOOD_BORED)
			{
				random = GetRandomControl();
				if (random < XIAN_STOP_CHANCE)
					item->goalAnimState = XIAN_STOP;
				else if (random < XIAN_WALK_CHANCE)
					item->goalAnimState = XIAN_STOP2;
			}
			else if (ai.ahead && ai.distance < XIAN_ATTACK4_RANGE)
			{
				if (ai.distance < XIAN_ATTACK2_RANGE)
					item->goalAnimState = XIAN_AIM2;
				else if (GetRandomControl() < 0x4000)
					item->goalAnimState = XIAN_AIM3;
				else
					item->goalAnimState = XIAN_AIM4;
			}
			else if (!ai.ahead || ai.distance > XIAN_RUN_RANGE)
				item->goalAnimState = XIAN_RUN;
			break;

		case XIAN_RUN:
			creature->maximumTurn = XIAN_RUN_TURN;
			if (ai.ahead)
				neck = ai.angle;

			if (creature->mood == MOOD_ESCAPE)
				break;
			else if (creature->mood == MOOD_BORED)
			{
				if (GetRandomControl() < 0x4000)
					item->goalAnimState = XIAN_STOP;
				else
					item->goalAnimState = XIAN_STOP2;
			}
			else if (ai.ahead && ai.distance < XIAN_ATTACK6_RANGE)
				item->goalAnimState = XIAN_AIM6;
			break;

		case XIAN_AIM1:
			if (ai.ahead)
				head = ai.angle;

			creature->flags = 0;
			if (!ai.ahead || ai.distance > XIAN_ATTACK1_RANGE)
				item->goalAnimState = XIAN_STOP;
			else
				item->goalAnimState = XIAN_HIT1;
			break;

		case XIAN_AIM2:
			if (ai.ahead)
				head = ai.angle;

			creature->flags = 0;
			if (!ai.ahead || ai.distance > XIAN_ATTACK2_RANGE)
				item->goalAnimState = XIAN_WALK;
			else
				item->goalAnimState = XIAN_HIT2;
			break;

		case XIAN_AIM3:
			if (ai.ahead)
				head = ai.angle;

			creature->flags = 0;
			if (!ai.ahead || ai.distance > XIAN_ATTACK3_RANGE)
				item->goalAnimState = XIAN_WALK;
			else
				item->goalAnimState = XIAN_HIT2;
			break;

		case XIAN_AIM4:
			if (ai.ahead)
				head = ai.angle;

			creature->flags = 0;
			if (!ai.ahead || ai.distance > XIAN_ATTACK4_RANGE)
				item->goalAnimState = XIAN_WALK;
			else
				item->goalAnimState = XIAN_HIT2;
			break;

		case XIAN_AIM5:
			if (ai.ahead)
				head = ai.angle;

			creature->flags = 0;
			if (!ai.ahead || ai.distance > XIAN_ATTACK5_RANGE)
				item->goalAnimState = XIAN_STOP2;
			else
				item->goalAnimState = XIAN_HIT5;
			break;

		case XIAN_AIM6:
			if (ai.ahead)
				head = ai.angle;

			creature->flags = 0;
			if (!ai.ahead || ai.distance > XIAN_ATTACK6_RANGE)
				item->goalAnimState = XIAN_RUN;
			else
				item->goalAnimState = XIAN_HIT6;
			break;

		case XIAN_HIT1:
			XianDamage(item, creature, XIAN_HIT1_DAMAGE, XIAN_HIT1_DAMAGE_TO_OTHER);
			break;

		case XIAN_HIT2:
		case XIAN_HIT3:
		case XIAN_HIT4:
			if (ai.ahead)
				head = ai.angle;

			XianDamage(item, creature, XIAN_HIT2_DAMAGE, XIAN_HIT2_DAMAGE_TO_OTHER);
			if (ai.ahead && ai.distance < XIAN_ATTACK1_RANGE)
			{
				if (GetRandomControl() < 0x4000)
					item->goalAnimState = XIAN_STOP;
				else
					item->goalAnimState = XIAN_STOP2;
			}
			else
				item->goalAnimState = XIAN_WALK;
			break;

		case XIAN_HIT5:
			if (ai.ahead)
				head = ai.angle;

			XianDamage(item, creature, XIAN_HIT5_DAMAGE, XIAN_HIT5_DAMAGE_TO_OTHER);
			if (ai.ahead && ai.distance < XIAN_ATTACK1_RANGE)
				item->goalAnimState = XIAN_STOP;
			else
				item->goalAnimState = XIAN_STOP2;
			break;

		case XIAN_HIT6:
			if (ai.ahead)
				head = ai.angle;

			XianDamage(item, creature, XIAN_HIT6_DAMAGE, XIAN_HIT6_DAMAGE_TO_OTHER);
			if (ai.ahead && ai.distance < XIAN_ATTACK1_RANGE)
			{
				if (GetRandomControl() < 0x4000)
					item->goalAnimState = XIAN_STOP;
				else
					item->goalAnimState = XIAN_STOP2;
			}
			else if (ai.ahead && ai.distance < XIAN_ATTACK4_RANGE)
				item->goalAnimState = XIAN_WALK;
			else
				item->goalAnimState = XIAN_RUN;
			break;
		}
	}

	if (lara_alive && LaraItem->hitPoints <= 0)
	{
		CreatureKill(item, XIAN_KILL_ANIM, XIAN_KILL, EXTRA_YETIKILL);
		return;
	}

	CreatureTilt(item, tilt);
	CreatureHead(item, head);
	CreatureNeck(item, neck);
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

void WarriorControl(short itemNumber)
{
	if (!CreatureActive(itemNumber))
		return;

	ITEM_INFO* item = &Items[itemNumber];
	CREATURE_INFO* creature = GetCreatureInfo(item);
	if (creature == NULL) return; // NOTE: Not exist in the original game.
	short head = 0, neck = 0, angle = 0, tilt = 0;

	if (item->hitPoints <= 0)
	{
		item->currentAnimState = WARRIOR_DEATH;
		item->meshBits >>= 1;
		if (item->meshBits == 0)
		{
			PlaySoundEffect(105, NULL, 0);
			item->meshBits = 0xFFFFFFFF;
			item->objectID = ID_CHINESE4;
			ExplodingDeath(itemNumber, 0xFFFFFFFF, 0);
			item->objectID = ID_WARRIOR;
			DisableBaddieAI(itemNumber);
			KillItem(itemNumber);
			item->status = ITEM_DISABLED;
			item->flags |= IFL_ONESHOT;
		}
		return;
	}
	else
	{
		AI_INFO ai;
		creature->LOT.step = CLICK(1);
		creature->LOT.drop = -CLICK(1);
		creature->LOT.fly = 0;
		CreatureAIInfo(item, &ai);

		if (item->currentAnimState == WARRIOR_FLY)
		{
			if (ai.zoneNumber != ai.enemyZone)
			{
				creature->LOT.step = BLOCK(20);
				creature->LOT.drop = -BLOCK(20);
				creature->LOT.fly = CLICK(1) / 4;
				CreatureAIInfo(item, &ai);
			}
		}

		CreatureMood(item, &ai, TRUE);
		angle = CreatureTurn(item, creature->maximumTurn);
		if (item->currentAnimState != WARRIOR_START)
			item->meshBits = 0xFFFFFFFF;

		switch (item->currentAnimState)
		{
		case WARRIOR_START:
			if (!creature->flags)
			{
				item->meshBits = (item->meshBits << 1) + 1;
				creature->flags = 3;
			}
			else
				creature->flags--;
			break;

		case WARRIOR_STOP:
			creature->maximumTurn = 0;
			if (ai.ahead)
				neck = ai.angle;

			if (LaraItem->hitPoints <= 0)
				item->goalAnimState = WARRIOR_WAIT;
			else if (ai.bite && ai.distance < WARRIOR_ATTACK1_RANGE)
			{
				if (GetRandomControl() < 0x4000)
					item->goalAnimState = WARRIOR_AIM1;
				else
					item->goalAnimState = WARRIOR_AIM2;
			}
			else if (ai.zoneNumber != ai.enemyZone)
				item->goalAnimState = WARRIOR_FLY;
			else
				item->goalAnimState = WARRIOR_WALK;
			break;

		case WARRIOR_WALK:
			creature->maximumTurn = WARRIOR_WALK_TURN;
			if (ai.ahead)
				neck = ai.angle;

			if (LaraItem->hitPoints <= 0)
				item->goalAnimState = WARRIOR_STOP;
			else if (ai.bite && ai.distance < WARRIOR_ATTACK3_RANGE)
				item->goalAnimState = WARRIOR_AIM3;
			else if (ai.zoneNumber != ai.enemyZone)
				item->goalAnimState = WARRIOR_STOP;
			break;

		case WARRIOR_FLY:
			creature->maximumTurn = WARRIOR_FLY_TURN;
			if (ai.ahead)
				neck = ai.angle;
			WarriorSparkleTrail(item);
			if (creature->LOT.fly == 0)
				item->goalAnimState = WARRIOR_STOP;
			break;

		case WARRIOR_AIM1:
			creature->flags = 0;
			if (ai.ahead)
				head = ai.angle;

			if (ai.bite && ai.distance < WARRIOR_ATTACK1_RANGE)
				item->goalAnimState = WARRIOR_SLASH1;
			else
				item->goalAnimState = WARRIOR_STOP;
			break;

		case WARRIOR_AIM2:
			creature->flags = 0;
			if (ai.ahead)
				head = ai.angle;

			if (ai.bite && ai.distance < WARRIOR_ATTACK1_RANGE)
				item->goalAnimState = WARRIOR_SLASH2;
			else
				item->goalAnimState = WARRIOR_STOP;
			break;

		case WARRIOR_AIM3:
			creature->flags = 0;
			if (ai.ahead)
				head = ai.angle;

			if (ai.bite && ai.distance < WARRIOR_ATTACK3_RANGE)
				item->goalAnimState = WARRIOR_SLASH3;
			else
				item->goalAnimState = WARRIOR_WALK;
			break;

		case WARRIOR_SLASH1:
		case WARRIOR_SLASH2:
		case WARRIOR_SLASH3:
			if (ai.ahead)
				head = ai.angle;

			if (!creature->flags)
			{
				if (DamageLaraOrEnemy(item, creature->enemy, &WarriorSwordBite, WARRIOR_DAMAGE, WARRIOR_DAMAGE_TO_OTHER, item->touchBits & WARRIOR_TOUCH))
					creature->flags = 1;
			}
			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureHead(item, head);
	CreatureNeck(item, neck);
	CreatureAnimation(itemNumber, angle, 0);
}

 /*
  * Inject function
  */
void Inject_Enemies() {
	INJECT(0x0041DB30, Knife);
	INJECT(0x0041DBB0, Cult2Control);
	INJECT(0x0041DFE0, MonkControl);
	INJECT(0x0041E4B0, Worker3Control);
	INJECT(0x0041EAC0, DrawXianLord);
	INJECT(0x0041EEC0, XianDamage);
	INJECT(0x0041EF70, InitialiseXianLord);
	INJECT(0x0041EFD0, XianLordControl);
	INJECT(0x0041F5B0, WarriorSparkleTrail);
	INJECT(0x0041F650, WarriorControl);
}
