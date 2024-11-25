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
#include "game/bird.h"
#include "game/box.h"
#include "global/vars.h"

enum BIRD_STATE
{
	// No state 0.
	BIRD_STATE_FLY = 1,
	BIRD_STATE_IDLE = 2, // For crow
	BIRD_STATE_PLANE = 3,
	BIRD_STATE_DEATH_START = 4,
	BIRD_STATE_DEATH_END = 5,
	BIRD_STATE_ATTACK = 6,
	BIRD_STATE_EAT = 7
};

#define EAGLE_IDLE_ANIM (5)
#define EAGLE_DIE_ANIM (8)
#define CROW_IDLE_ANIM (14)
#define CROW_DIE_ANIM (1)

#define EAGLE_DAMAGE 20
#define CROW_DAMAGE 20
#define EAGLE_DAMAGE_TO_OTHER 3
#define CROW_DAMAGE_TO_OTHER 3

const BITE_INFO EagleBite = { 15, 46, 21, 6 };
const BITE_INFO CrowBite = { 2, 10, 60, 14 };

static void InitialiseEagle(ITEM_INFO* item)
{
	item->animNumber = Objects[item->objectID].animIndex + EAGLE_IDLE_ANIM;
	item->frameNumber = Anims[item->animNumber].frameBase;
	item->currentAnimState = BIRD_STATE_EAT;
	item->goalAnimState = BIRD_STATE_EAT;
}

static void InitialiseCrow(ITEM_INFO* item)
{
	item->animNumber = Objects[item->objectID].animIndex + CROW_IDLE_ANIM;
	item->frameNumber = Anims[item->animNumber].frameBase;
	item->currentAnimState = BIRD_STATE_IDLE;
	item->goalAnimState = BIRD_STATE_IDLE;
}

void InitialiseBird(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];
	InitialiseCreature(itemNumber);
	if (item->objectID == ID_CROW)
		InitialiseCrow(item);
	else
		InitialiseEagle(item);
}

void BirdControl(short itemNumber)
{
	if (!CreatureActive(itemNumber))
		return;
	ITEM_INFO* item = &Items[itemNumber];
	CREATURE_INFO* bird = GetCreatureInfo(item);
	if (bird == NULL) return; // NOTE: Not exist in the original game.
	AI_INFO ai = {};
	short angle = 0;

	if (item->hitPoints <= 0)
	{
		item->pos.rotX = 0; // Reset rotation to avoid weird behaviour.
		switch (item->currentAnimState)
		{
		case BIRD_STATE_DEATH_START: // Fall.
			if (item->pos.y >= item->floor)
			{
				item->pos.y = item->floor;
				item->gravity = FALSE;
				item->fallSpeed = 0;
				item->goalAnimState = BIRD_STATE_DEATH_END;
			}
			break;
		case BIRD_STATE_DEATH_END: // Fall end.
			item->pos.y = item->floor;
			break;
		default:
			if (item->currentAnimState != BIRD_STATE_DEATH_START && item->currentAnimState != BIRD_STATE_DEATH_END)
			{
				SetAnimation(item, item->objectID == ID_CROW ? CROW_DIE_ANIM : EAGLE_DIE_ANIM, BIRD_STATE_DEATH_START);
				item->gravity = TRUE;
				item->speed = 0;
			}
			break;
		}
	}
	else
	{
		CreatureAIInfo(item, &ai);
		CreatureMood(item, &ai, FALSE);
		angle = CreatureTurn(item, ANGLE(3));

		switch (item->currentAnimState)
		{
		case BIRD_STATE_EAT:
		case BIRD_STATE_IDLE:
			item->pos.y = item->floor;
			if (bird->mood != MOOD_BORED)
				item->goalAnimState = BIRD_STATE_FLY;
			break;
		case BIRD_STATE_FLY:
			bird->flags = 0;

			if (item->requiredAnimState)
			{
				item->goalAnimState = item->requiredAnimState;
			}
			else if (bird->mood != MOOD_BORED)
			{
				if (ai.ahead && ai.distance < 0x40000)
					item->goalAnimState = BIRD_STATE_ATTACK;
				else
					item->goalAnimState = BIRD_STATE_PLANE;
			}
			else
			{
				item->goalAnimState = BIRD_STATE_IDLE;
			}
			break;
		case BIRD_STATE_PLANE:
			if (bird->mood != MOOD_BORED)
			{
				if (ai.ahead && ai.distance < 0x40000)
					item->goalAnimState = BIRD_STATE_ATTACK;
			}
			else
			{
				item->requiredAnimState = BIRD_STATE_IDLE;
				item->goalAnimState = BIRD_STATE_FLY;
			}
			break;
		case BIRD_STATE_ATTACK:
			if (bird->flags == 0)
			{
				if (item->objectID == ID_CROW && DamageLaraOrEnemy(item, bird->enemy, &CrowBite, CROW_DAMAGE, CROW_DAMAGE_TO_OTHER, item->touchBits != 0))
					bird->flags = 1;
				else if (DamageLaraOrEnemy(item, bird->enemy, &EagleBite, EAGLE_DAMAGE, EAGLE_DAMAGE_TO_OTHER, item->touchBits != 0))
					bird->flags = 1;
			}
			break;
		}
	}

	CreatureAnimation(itemNumber, angle, 0);
}

 /*
  * Inject function
  */
void Inject_Bird() {
	INJECT(0x0040C860, InitialiseBird);
	INJECT(0x0040C8F0, BirdControl);
}
