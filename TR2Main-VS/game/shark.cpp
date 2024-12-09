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
#include "game/shark.h"
#include "game/control.h"
#include "game/box.h"
#include "game/items.h"
#include "game/missile.h"
#include "game/lot.h"
#include "specific/game.h"
#include "global/vars.h"

enum JellyState {
	JELLY_EMPTY,
	JELLY_MOVE,
	JELLY_STOP
};

enum BaracuddaState {
	BARA_EMPTY,
	BARA_STOP,
	BARA_SWIM1,
	BARA_SWIM2,
	BARA_ATTACK1,
	BARA_ATTACK2,
	BARA_DEATH
};

enum SharkState {
	SHARK_STOP,
	SHARK_SWIM1,
	SHARK_SWIM2,
	SHARK_ATTACK1,
	SHARK_ATTACK2, 
	SHARK_DEATH,
	SHARK_KILL
};

#define JELLY_STING 5
#define JELLY_STING_TO_OTHER 1
#define JELLY_TURN ANGLE(2)

#define BARA_BITE 100
#define BARA_BITE_TO_OTHER 2
#define BARA_DIE_ANIM 6
#define BARA_SWIM1_TURN ANGLE(2)
#define BARA_SWIM2_TURN ANGLE(4)
#define BARA_ATTACK1_RANGE SQR(BLOCK(2)/3)
#define BARA_ATTACK2_RANGE SQR(BLOCK(1)/3)
#define BARA_TOUCH 0xe0

#define SHARK_BITE 400
#define SHARK_BITE_TO_OTHER 6
#define SHARK_SWIM2_RANGE SQR(BLOCK(3))
#define SHARK_ATTACK1_RANGE SQR(BLOCK(3)/4)
#define SHARK_ATTACK2_RANGE SQR(BLOCK(4)/3)
#define SHARK_SWIM1_TURN (ANGLE(1)/2)
#define SHARK_SWIM2_TURN ANGLE(2)
#define SHARK_ATTACK1_CHANCE 0x800
#define SHARK_DIE_ANIM 4
#define SHARK_KILL_ANIM 19
#define SHARK_TOUCH 0x3400

static BITE_INFO JellyBite = { 0, 0, 0, 0 };
static BITE_INFO BaraBite = { 2, -60, 121, 7 };
static BITE_INFO SharkBite = { 17, -22, 344, 12 };

void JellyControl(short item_number)
{
	if (!CreatureActive(item_number))
		return;

	ITEM_INFO* item = &Items[item_number];
	CREATURE_INFO* creature = GetCreatureInfo(item);
	short angle = 0;

	if (item->hitPoints <= 0)
	{
		if (ExplodingDeath(item_number, 0xFFFFFFFF, 0))
		{
			DisableBaddieAI(item_number);
			KillItem(item_number);
			item->status = ITEM_DISABLED;
			return;
		}
		return;
	}
	else
	{
		AI_INFO ai;
		CreatureAIInfo(item, &ai);
		CreatureMood(item, &ai, FALSE);
		angle = CreatureTurn(item, JELLY_TURN);

		switch (item->currentAnimState)
		{
		case JELLY_STOP:
			if (creature->mood != MOOD_BORED)
				item->goalAnimState = JELLY_MOVE;
			break;

		case JELLY_MOVE:
			if (creature->mood == MOOD_BORED || item->touchBits != 0)
				item->goalAnimState = JELLY_STOP;
			break;
		}

		DamageLaraOrEnemy(item, creature->enemy, &JellyBite, JELLY_STING, JELLY_STING_TO_OTHER, item->touchBits != 0);
	}

	CreatureAnimation(item_number, angle, 0);
	CreatureUnderwater(item, 0);
}


void BaracuddaControl(short itemNumber)
{
	if (!CreatureActive(itemNumber))
		return;

	ITEM_INFO* item = &Items[itemNumber];
	CREATURE_INFO* creature = GetCreatureInfo(item);
	short angle = 0, head = 0;

	if (item->hitPoints <= 0)
	{
		if (item->currentAnimState != BARA_DEATH)
			SetAnimation(item, BARA_DIE_ANIM, BARA_DEATH);
		CreatureFloat(itemNumber);
		return;
	}
	else
	{
		AI_INFO ai;
		CreatureAIInfo(item, &ai);
		CreatureMood(item, &ai, FALSE);
		angle = CreatureTurn(item, creature->maximumTurn);

		switch (item->currentAnimState)
		{
		case BARA_STOP:
			creature->flags = 0;

			if (creature->mood == MOOD_BORED)
				item->goalAnimState = BARA_SWIM1;
			else if (ai.ahead && ai.distance < BARA_ATTACK1_RANGE)
				item->goalAnimState = BARA_ATTACK1;
			else if (creature->mood == MOOD_STALK)
				item->goalAnimState = BARA_SWIM1;
			else
				item->goalAnimState = BARA_SWIM2;
			break;

		case BARA_SWIM1:
			creature->maximumTurn = BARA_SWIM1_TURN;

			if (creature->mood == MOOD_BORED)
				break;
			else if (ai.ahead && (item->touchBits & BARA_TOUCH))
				item->goalAnimState = BARA_STOP;
			else if (creature->mood != MOOD_STALK)
				item->goalAnimState = BARA_SWIM2;
			break;

		case BARA_SWIM2:
			creature->maximumTurn = BARA_SWIM2_TURN;
			creature->flags = 0;

			if (creature->mood == MOOD_BORED)
				item->goalAnimState = BARA_SWIM1;
			else if (ai.ahead && ai.distance < BARA_ATTACK2_RANGE)
				item->goalAnimState = BARA_ATTACK2;
			else if (ai.ahead && ai.distance < BARA_ATTACK1_RANGE)
				item->goalAnimState = BARA_STOP;
			else if (creature->mood == MOOD_STALK)
				item->goalAnimState = BARA_SWIM1;
			break;

		case BARA_ATTACK1:
		case BARA_ATTACK2:
			if (ai.ahead)
				head = ai.angle;

			if (!creature->flags)
			{
				if (DamageLaraOrEnemy(item, creature->enemy, &BaraBite, BARA_BITE, BARA_BITE_TO_OTHER, item->touchBits & BARA_TOUCH))
					creature->flags = 1;
			}
			break;
		}
	}

	CreatureHead(item, head);
	CreatureAnimation(itemNumber, angle, 0);
	CreatureUnderwater(item, CLICK(1));
}

void SharkControl(short itemNumber)
{
	if (!CreatureActive(itemNumber))
		return;

	ITEM_INFO* item = &Items[itemNumber];
	CREATURE_INFO* creature = GetCreatureInfo(item);
	short angle = 0, head = 0;
	bool lara_alive = (LaraItem->hitPoints > 0);

	if (item->hitPoints <= 0)
	{
		if (item->currentAnimState != SHARK_DEATH)
			SetAnimation(item, SHARK_DIE_ANIM, SHARK_DEATH);
		CreatureFloat(itemNumber);
		return;
	}
	else
	{
		AI_INFO ai;
		CreatureAIInfo(item, &ai);
		CreatureMood(item, &ai, TRUE);
		angle = CreatureTurn(item, creature->maximumTurn);

		switch (item->currentAnimState)
		{
		case SHARK_STOP:
			creature->flags = 0;
			creature->maximumTurn = 0;
			if (ai.ahead && ai.distance < SHARK_ATTACK1_RANGE && ai.zoneNumber == ai.enemyZone)
				item->goalAnimState = SHARK_ATTACK1;
			else
				item->goalAnimState = SHARK_SWIM1;
			break;

		case SHARK_SWIM1:
			creature->maximumTurn = SHARK_SWIM1_TURN;

			if (creature->mood == MOOD_BORED)
				break;
			else if (ai.ahead && ai.distance < SHARK_ATTACK1_RANGE)
				item->goalAnimState = SHARK_STOP;
			else if (creature->mood == MOOD_ESCAPE || ai.distance > SHARK_SWIM2_RANGE || !ai.ahead)
				item->goalAnimState = SHARK_SWIM2;
			break;

		case SHARK_SWIM2:
			creature->flags = 0;
			creature->maximumTurn = SHARK_SWIM2_TURN;

			if (creature->mood == MOOD_BORED)
				item->goalAnimState = SHARK_SWIM1;
			else if (creature->mood == MOOD_ESCAPE)
				break;
			else if (ai.ahead && ai.distance < SHARK_ATTACK2_RANGE && ai.zoneNumber == ai.enemyZone)
			{
				if (GetRandomControl() < SHARK_ATTACK1_CHANCE)
					item->goalAnimState = SHARK_STOP;
				else if (ai.distance < SHARK_ATTACK1_RANGE)
					item->goalAnimState = SHARK_ATTACK2;
			}
			break;

		case SHARK_ATTACK1:
		case SHARK_ATTACK2:
			if (ai.ahead)
				head = ai.angle;

			if (!creature->flags)
			{
				if (DamageLaraOrEnemy(item, creature->enemy, &SharkBite, SHARK_BITE, SHARK_BITE_TO_OTHER, item->touchBits & SHARK_TOUCH))
					creature->flags = 1;
			}
			break;
		}
	}

	if (lara_alive && LaraItem->hitPoints <= 0)
	{
		CreatureKill(item, SHARK_KILL_ANIM, SHARK_KILL, EXTRA_SHARKKILL);
		return;
	}

	if (item->currentAnimState != SHARK_KILL)
	{
		CreatureHead(item, head);
		CreatureAnimation(itemNumber, angle, 0);
		CreatureUnderwater(item, BLOCK(1) / 3);
	}
	else
	{
		AnimateItem(item);
	}
}

 /*
  * Inject function
  */
void Inject_Shark() {
	INJECT(0x0043C900, JellyControl);
	INJECT(0x0043CA20, BaracuddaControl);
	INJECT(0x0043CC50, SharkControl);
}
