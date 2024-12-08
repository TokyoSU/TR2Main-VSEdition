/*
 * Copyright (c) 2017-2018 Michael Chaban. All rights reserved.
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
#include "game/dog.h"
#include "game/box.h"
#include "game/effects.h"
#include "specific/game.h"
#include "global/vars.h"

constexpr auto DOG_BITE_DAMAGE = 100;
constexpr auto DOG_LEAP_DAMAGE = 200;
constexpr auto DOG_LUNGE_DAMAGE = 100;
constexpr auto DOG_BITE_DAMAGE_TO_OTHER = 2;
constexpr auto DOG_LEAP_DAMAGE_TO_OTHER = 3;
constexpr auto DOG_LUNGE_DAMAGE_TO_OTHER = 2;
// Includes all front legs up to the shoulder, neck, head and jaw
constexpr auto DOG_TOUCH = MESH_BITS(11) | MESH_BITS(12) | MESH_BITS(13) | MESH_BITS(14) | MESH_BITS(16) | MESH_BITS(17) | MESH_BITS(18) | MESH_BITS(19) | MESH_BITS(20) | MESH_BITS(21) | MESH_BITS(24);
constexpr auto DOG_DIE_ANIM = 13;
constexpr auto DOG_WALK_TURN = ANGLE(3);
constexpr auto DOG_RUN_TURN = ANGLE(6);
constexpr auto DOG_ATTACK1_RANGE = SQR(BLOCK(1) / 3);
constexpr auto DOG_ATTACK2_RANGE = SQR(BLOCK(3) / 4);
constexpr auto DOG_ATTACK3_RANGE = SQR(BLOCK(2) / 3);
constexpr auto DOG_BARK_CHANCE = 0x300;
constexpr auto DOG_CROUCH_CHANCE = (DOG_BARK_CHANCE + 0x300);
constexpr auto DOG_STAND_CHANCE = (DOG_CROUCH_CHANCE + 0x500);
constexpr auto DOG_WALK_CHANCE = (DOG_CROUCH_CHANCE + 0x2000);
constexpr auto DOG_UNCROUCH_CHANCE = 0x100;
constexpr auto DOG_UNSTAND_CHANCE = 0x200;
constexpr auto DOG_UNBARK_CHANCE = 0x500;

constexpr auto TIGER_BITE_DAMAGE = 90;
constexpr auto TIGER_BITE_DAMAGE_TO_OTHER = 3;
// Includes the head, jaw and left hand.
constexpr auto TIGER_TOUCH = MESH_BITS(16) | MESH_BITS(22) | MESH_BITS(26);
constexpr auto TIGER_DIE_ANIM = 11;
constexpr auto TIGER_WALK_TURN = ANGLE(3);
constexpr auto TIGER_RUN_TURN = ANGLE(6);
constexpr auto TIGER_ATTACK1_RANGE = SQR(BLOCK(1) / 3);
constexpr auto TIGER_ATTACK2_RANGE = SQR(BLOCK(3) / 2);
constexpr auto TIGER_ATTACK3_RANGE = SQR(BLOCK(1));
constexpr auto TIGER_ROAR_CHANCE = 0x60;
constexpr auto TIGER_WALK_CHANCE = 0x460;

enum TigerState
{
	// No state 0
	TIGER_STOP = 1,
	TIGER_WALK,
	TIGER_RUN,
	TIGER_WAIT,
	TIGER_ROAR,
	TIGER_ATTACK1,
	TIGER_ATTACK2,
	TIGER_ATTACK3,
	TIGER_DEATH
};

enum DogState {
	DOG_EMPTY,
	DOG_WALK,
	DOG_RUN,
	DOG_STOP,
	DOG_BARK,
	DOG_CROUCH,
	DOG_STAND,
	DOG_ATTACK1,
	DOG_ATTACK2,
	DOG_ATTACK3,
	DOG_DEATH
};

static BITE_INFO DogBite = { 0, 30, 141, 20 };
static BITE_INFO TigerBite = { 19, -13, 3, 26 };

void DogControl(short itemNumber)
{
	if (!CreatureActive(itemNumber))
		return;

	ITEM_INFO* item = &Items[itemNumber];
	CREATURE_INFO* dog = GetCreatureInfo(item);
	short head = 0, angle = 0, tilt = 0;

	if (item->hitPoints <= 0)
	{
		if (item->currentAnimState != DOG_DEATH)
			SetAnimation(item, DOG_DIE_ANIM, DOG_DEATH);
	}
	else
	{
		AI_INFO info;
		CreatureAIInfo(item, &info);
		if (info.ahead)
			head = info.angle;
		CreatureMood(item, &info, FALSE);
		angle = CreatureTurn(item, dog->maximumTurn);

		switch (item->currentAnimState)
		{
		case DOG_STOP:
			dog->maximumTurn = 0;
			dog->flags = 0;

			if (dog->mood == MOOD_BORED)
			{
				if (item->requiredAnimState)
					item->goalAnimState = item->requiredAnimState;
				else
				{
					int random = GetRandomControl();
					if (random < DOG_BARK_CHANCE)
						item->goalAnimState = DOG_BARK;
					else if (random < DOG_CROUCH_CHANCE)
						item->goalAnimState = DOG_CROUCH;
					else if (random < DOG_WALK_CHANCE)
						item->goalAnimState = DOG_WALK;
				}
			}
			else if (dog->mood == MOOD_ESCAPE)
				item->goalAnimState = DOG_RUN;
			else if (info.distance < DOG_ATTACK1_RANGE && info.ahead)
				item->goalAnimState = DOG_ATTACK1;
			else
				item->goalAnimState = DOG_RUN;
			break;

		case DOG_WALK:
			dog->maximumTurn = DOG_WALK_TURN;

			if (dog->mood == MOOD_BORED)
			{
				int random = GetRandomControl();
				if (random < DOG_BARK_CHANCE)
				{
					item->requiredAnimState = DOG_BARK;
					item->goalAnimState = DOG_STOP;
				}
				else if (random < DOG_CROUCH_CHANCE)
				{
					item->requiredAnimState = DOG_CROUCH;
					item->goalAnimState = DOG_STOP;
				}
				else if (random < DOG_STAND_CHANCE)
					item->goalAnimState = DOG_STOP;
			}
			else
				item->goalAnimState = DOG_RUN;
			break;

		case DOG_RUN:
			tilt = angle;
			dog->maximumTurn = DOG_RUN_TURN;

			if (dog->mood == MOOD_BORED)
				item->goalAnimState = DOG_STOP;
			else if (info.distance < DOG_ATTACK2_RANGE)
				item->goalAnimState = DOG_ATTACK2;
			break;

		case DOG_ATTACK2:
			if (dog->flags != 2)
			{
				if (DamageLaraOrEnemy(item, dog->enemy, &DogBite, DOG_LEAP_DAMAGE, DOG_LEAP_DAMAGE_TO_OTHER, item->touchBits & DOG_TOUCH))
					dog->flags = 2;
			}

			if (info.distance < DOG_ATTACK1_RANGE)
				item->goalAnimState = DOG_ATTACK1;
			else if (info.distance < DOG_ATTACK3_RANGE)
				item->goalAnimState = DOG_ATTACK3;
			break;

		case DOG_ATTACK1:
			dog->maximumTurn = 0;
			if (dog->flags != 1 && info.ahead)
			{
				if (DamageLaraOrEnemy(item, dog->enemy, &DogBite, DOG_BITE_DAMAGE, DOG_BITE_DAMAGE_TO_OTHER, item->touchBits & DOG_TOUCH))
					dog->flags = 1;
			}

			if (info.distance > DOG_ATTACK1_RANGE && info.distance < DOG_ATTACK3_RANGE)
				item->goalAnimState = DOG_ATTACK3;
			else
				item->goalAnimState = DOG_STOP;
			break;

		case DOG_ATTACK3:
			dog->maximumTurn = DOG_RUN_TURN;
			if (dog->flags != 3)
			{
				if (DamageLaraOrEnemy(item, dog->enemy, &DogBite, DOG_LUNGE_DAMAGE, DOG_LUNGE_DAMAGE_TO_OTHER, item->touchBits & DOG_TOUCH))
					dog->flags = 3;
			}

			if (info.distance < DOG_ATTACK1_RANGE)
				item->goalAnimState = DOG_ATTACK1;
			break;

		case DOG_BARK:
			if (dog->mood != MOOD_BORED || GetRandomControl() < DOG_UNBARK_CHANCE)
				item->goalAnimState = DOG_STOP;
			break;

		case DOG_CROUCH:
			if (dog->mood != MOOD_BORED || GetRandomControl() < DOG_UNCROUCH_CHANCE)
				item->goalAnimState = DOG_STOP;
			break;

		case DOG_STAND:
			if (dog->mood != MOOD_BORED || GetRandomControl() < DOG_UNSTAND_CHANCE)
				item->goalAnimState = DOG_STOP;
			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureHead(item, head);
	CreatureAnimation(itemNumber, angle, tilt);
}

void TigerControl(short itemNumber)
{
	if (!CreatureActive(itemNumber))
		return;

	ITEM_INFO* item = &Items[itemNumber];
	CREATURE_INFO* creature = GetCreatureInfo(item);
	short head = 0, angle = 0, tilt = 0;

	if (item->hitPoints <= 0)
	{
		if (item->currentAnimState != TIGER_DEATH)
			SetAnimation(item, TIGER_DIE_ANIM, TIGER_DEATH);
	}
	else
	{
		AI_INFO info;
		CreatureAIInfo(item, &info);
		if (info.ahead)
			head = info.angle;
		CreatureMood(item, &info, TRUE);
		angle = CreatureTurn(item, creature->maximumTurn);
		switch (item->currentAnimState)
		{
		case TIGER_STOP:
			creature->maximumTurn = 0;
			creature->flags = 0;

			if (creature->mood == MOOD_ESCAPE)
				item->goalAnimState = TIGER_RUN;
			else if (item->requiredAnimState)
				item->goalAnimState = item->requiredAnimState;
			else if (creature->mood != MOOD_BORED)
			{
				if (info.ahead && info.distance < TIGER_ATTACK1_RANGE)
					item->goalAnimState = TIGER_ATTACK1;
				else if (info.ahead && info.distance < TIGER_ATTACK3_RANGE)
				{
					creature->maximumTurn = TIGER_WALK_TURN;
					item->goalAnimState = TIGER_ATTACK3;
				}
				else if (creature->mood == MOOD_ATTACK || (GetRandomControl() >= TIGER_ROAR_CHANCE))
					item->goalAnimState = TIGER_RUN;
				else
					item->goalAnimState = TIGER_ROAR;
			}
			else if (GetRandomControl() < TIGER_ROAR_CHANCE)
				item->goalAnimState = TIGER_ROAR;
			else
				item->goalAnimState = TIGER_WALK;
			break;

		case TIGER_WALK:
			creature->maximumTurn = TIGER_WALK_TURN;

			if (GetRandomControl() < TIGER_ROAR_CHANCE)
			{
				item->goalAnimState = TIGER_STOP;
				item->requiredAnimState = TIGER_ROAR;
			}
			break;

		case TIGER_RUN:
			creature->maximumTurn = TIGER_RUN_TURN;

			if (creature->mood == MOOD_BORED)
				item->goalAnimState = TIGER_STOP;
			else if (creature->flags && info.ahead)
				item->goalAnimState = TIGER_STOP;
			else if (info.ahead && info.distance < TIGER_ATTACK2_RANGE)
			{
				if (LaraItem->speed == 0)
					item->goalAnimState = TIGER_STOP;
				else
					item->goalAnimState = TIGER_ATTACK2;
			}
			else if (creature->mood != MOOD_ATTACK && GetRandomControl() < TIGER_ROAR_CHANCE)
			{
				item->requiredAnimState = TIGER_ROAR;
				item->goalAnimState = TIGER_STOP;
			}
			else if (creature->mood == MOOD_ESCAPE && info.ahead)
				item->goalAnimState = TIGER_STOP;

			creature->flags = 0;
			break;

		case TIGER_ATTACK1:
		case TIGER_ATTACK2:
		case TIGER_ATTACK3:
			if (!creature->flags)
			{
				if (DamageLaraOrEnemy(item, creature->enemy, &TigerBite, TIGER_BITE_DAMAGE, TIGER_BITE_DAMAGE_TO_OTHER, item->touchBits & TIGER_TOUCH))
					creature->flags = 1;
			}
			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureHead(item, head);
	CreatureAnimation(itemNumber, angle, tilt);
}

/*
 * Inject function
 */
void Inject_Dog() {
	INJECT(0x00417130, DogControl);
	INJECT(0x004174E0, TigerControl);
}
