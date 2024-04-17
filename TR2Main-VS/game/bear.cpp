/*
 * Copyright (c) 2017-2019 Michael Chaban. All rights reserved.
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
#include "game/bear.h"
#include "game/box.h"
#include "game/effects.h"
#include "game/lot.h"
#include "specific/game.h"
#include "global/vars.h"

#define BEAR_DAMAGE0 (200)
#define BEAR_DAMAGE1 (400)
#define BEAR_DAMAGE2 (200)
#define BEAR_DAMAGE_TOUCH (3)
#define BEAR_TOUCH (0x2406C)

typedef enum {
	BEAR_STROLL,
	BEAR_STOP,
	BEAR_WALK,
	BEAR_RUN,
	BEAR_REAR,
	BEAR_ROAR,
	BEAR_ATTACK1,
	BEAR_ATTACK2,
	BEAR_EAT,
	BEAR_DEATH
} BEAR_ANIMS;

static const BITE_INFO BearBite = { 0, 96, 335, 14 };

void BearControl(short itemID) {
	if (!CreatureActive(itemID))
		return;

	ITEM_INFO* item = &Items[itemID];
	CREATURE_INFO* bear = GetCreatureInfo(item);
	if (bear == NULL) return; // NOTE: additional check not presented in the original game
	AI_INFO info{};
	BOOL isLaraDead = FALSE;
	short angle = 0, head = 0;

	if (item->hitPoints <= 0) {
		angle = CreatureTurn(item, 182);
		switch (item->currentAnimState) {
		case BEAR_STROLL:
		case BEAR_RUN:
			item->goalAnimState = BEAR_STOP;
			break;
		case BEAR_REAR:
			bear->flags = 1;
			item->goalAnimState = BEAR_DEATH;
			break;
		case BEAR_STOP:
			bear->flags = 0;
			item->goalAnimState = BEAR_DEATH;
			break;
		case BEAR_DEATH:
			if (bear->flags && CHK_ANY(item->touchBits, BEAR_TOUCH)) {
				bear->enemy->hitPoints -= BEAR_DAMAGE2;
				bear->enemy->hitStatus = 1;
				bear->flags = 0;
			}
			break;
		case BEAR_WALK:
			item->goalAnimState = BEAR_REAR;
			break;
		}
	}
	else {

		CreatureAIInfo(item, &info);
		if (info.ahead) {
			head = info.angle;
		}

		CreatureMood(item, &info, TRUE);
		angle = CreatureTurn(item, bear->maximumTurn);
		isLaraDead = (LaraItem->hitPoints <= 0);

		if (item->hitStatus) {
			bear->flags = 1;
		}

		switch (item->currentAnimState) {
		case BEAR_STOP:
			if (isLaraDead) {
				if (info.bite && info.distance < 0x90000) {
					item->goalAnimState = BEAR_EAT;
				}
				else {
					item->goalAnimState = BEAR_STROLL;
				}
			}
			else if (item->requiredAnimState != BEAR_STROLL) {
				item->goalAnimState = item->requiredAnimState;
			}
			else if (bear->mood != MOOD_BORED) {
				item->goalAnimState = BEAR_RUN;
			}
			else {
				item->goalAnimState = BEAR_STROLL;
			}
			break;

		case BEAR_STROLL:
			bear->maximumTurn = 364;
			if (isLaraDead && CHK_ANY(item->touchBits, BEAR_TOUCH) && info.ahead) {
				item->goalAnimState = BEAR_STOP;
			}
			else if (bear->mood != MOOD_BORED) {
				item->goalAnimState = BEAR_STOP;
				if (bear->mood == MOOD_ESCAPE) {
					item->requiredAnimState = BEAR_STROLL;
				}
			}
			else if (GetRandomControl() < 0x50) {
				item->requiredAnimState = BEAR_ROAR;
				item->goalAnimState = BEAR_STOP;
			}
			break;

		case BEAR_RUN:
			bear->maximumTurn = 910;
			if (CHK_ANY(item->touchBits, BEAR_TOUCH)) {
				bear->enemy->hitPoints -= BEAR_DAMAGE_TOUCH;
				bear->enemy->hitStatus = 1;
			}

			if (bear->mood == MOOD_BORED || isLaraDead) {
				item->goalAnimState = BEAR_STOP;
			}
			else if (info.ahead && item->requiredAnimState == BEAR_STROLL) {
				if (!bear->flags && info.distance < 0x400000 && GetRandomControl() < 0x300) {
					item->requiredAnimState = BEAR_REAR;
					item->goalAnimState = BEAR_STOP;
				}
				else if (info.distance < 0x100000) {
					item->goalAnimState = BEAR_ATTACK1;
				}
			}
			break;

		case BEAR_REAR:
			if (bear->flags) {
				item->requiredAnimState = BEAR_STROLL;
				item->goalAnimState = BEAR_STOP;
			}
			else if (item->requiredAnimState != BEAR_STROLL) {
				item->goalAnimState = item->requiredAnimState;
			}
			else if (bear->mood == MOOD_BORED || bear->mood == MOOD_ESCAPE) {
				item->goalAnimState = BEAR_STOP;
			}
			else if (info.bite && info.distance < 360000) {
				item->goalAnimState = BEAR_ATTACK2;
			}
			else {
				item->goalAnimState = BEAR_WALK;
			}
			break;

		case BEAR_WALK:
			if (bear->flags) {
				item->requiredAnimState = BEAR_STROLL;
				item->goalAnimState = BEAR_REAR;
			}
			else if (info.ahead && CHK_ANY(item->touchBits, BEAR_TOUCH)) {
				item->goalAnimState = BEAR_REAR;
			}
			else if (bear->mood == MOOD_ESCAPE) {
				item->goalAnimState = BEAR_REAR;
				item->requiredAnimState = BEAR_STROLL;
			}
			else if (bear->mood == MOOD_BORED || GetRandomControl() < 0x50) {
				item->requiredAnimState = BEAR_ROAR;
				item->goalAnimState = BEAR_REAR;
			}
			else if (info.distance > 0x400000 || GetRandomControl() < 0x600) {
				item->requiredAnimState = BEAR_STOP;
				item->goalAnimState = BEAR_REAR;
			}
			break;

		case BEAR_ATTACK2:
			if (item->requiredAnimState == BEAR_STROLL && CHK_ANY(item->touchBits, BEAR_TOUCH)) {
				bear->enemy->hitPoints -= BEAR_DAMAGE1;
				bear->enemy->hitStatus = 1;
				item->requiredAnimState = BEAR_REAR;
			}
			break;

		case BEAR_ATTACK1:
			if (item->requiredAnimState == BEAR_STROLL && CHK_ANY(item->touchBits, BEAR_TOUCH))
			{
				CreatureEffect(item, &BearBite, DoBloodSplat);
				bear->enemy->hitPoints -= BEAR_DAMAGE0;
				bear->enemy->hitStatus = 1;
				item->requiredAnimState = BEAR_STOP;
			}
			break;

		default:
			break;
		}
	}

	CreatureHead(item, head);
	CreatureAnimation(itemID, angle, 0);
}

/*
 * NOTE: there is no inject function for tomb2gold code
 */