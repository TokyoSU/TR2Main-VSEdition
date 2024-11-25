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
#include "game/lot.h"
#include "specific/init.h"
#include "global/vars.h"

#ifdef FEATURE_GOLD
extern bool IsGold();
#endif

void InitialiseSlot(short itemNumber, int baddieSlotID)
{
    CREATURE_INFO* creature = &BaddiesSlots[baddieSlotID];
    ITEM_INFO* item = &Items[itemNumber];
    if (itemNumber == Lara.item_number)
        Lara.creature = &BaddiesSlots[baddieSlotID];
    else
        item->data = creature;
    creature->itemNumber = itemNumber;
    creature->mood = MOOD_BORED;
    creature->neckRotation = 0;
    creature->headRotation = 0;
    creature->maximumTurn = ANGLE(1);
    creature->flags = 0;
    creature->enemy = NULL;
    creature->LOT.step = CLICK_SIZE;
    creature->LOT.drop = -(CLICK_SIZE * 2);
    creature->LOT.blockMask = 0x4000;
    creature->LOT.fly = 0;
    switch (item->objectID)
    {
    case ID_LARA:
        creature->LOT.step = WALL_SIZE * 20;
        creature->LOT.drop = -(WALL_SIZE * 20);
        creature->LOT.fly = 256;
        break;
    case ID_SHARK:
    case ID_BARRACUDA:
    case ID_DIVER:
    case ID_JELLY:
    case ID_CROW:
    case ID_EAGLE:
        creature->LOT.step = WALL_SIZE * 20;
        creature->LOT.drop = -(WALL_SIZE * 20);
        creature->LOT.fly = 16;
        if (item->objectID == ID_SHARK)
            creature->LOT.blockMask = 0x8000;
        break;
    case ID_WORKER3:
    case ID_WORKER4:
    case ID_YETI:
        creature->LOT.step = WALL_SIZE * 4;
        creature->LOT.drop = -(WALL_SIZE * 4);
        break;
    case ID_SPIDER_or_WOLF:
#ifdef FEATURE_GOLD
        if (IsGold()) // NOTE: Wolf don't climb or drop 4 clicks !
            break;
#endif
        creature->LOT.step = CLICK_SIZE / 2;
        creature->LOT.drop = -(WALL_SIZE * 4);
        break;
    case ID_SKIDOO_ARMED:
        creature->LOT.step = CLICK_SIZE / 2;
        creature->LOT.drop = -(WALL_SIZE * 4);
        break;
    case ID_DINO:
        creature->LOT.blockMask = 0x8000;
        break;
    default:
        break;
    }
    ClearLOT(&creature->LOT);
    if (itemNumber != Lara.item_number)
        CreateZone(item);
    ++BaddiesSlotsCount;
}

 /*
  * Inject function
  */
void Inject_Lot() {
	//INJECT(0x00432B10, InitialiseLOTarray);
	//INJECT(0x00432B70, DisableBaddieAI);
	//INJECT(0x00432BC0, EnableBaddieAI);
	INJECT(0x00432D70, InitialiseSlot);
	//INJECT(0x00432F80, CreateZone);
	//INJECT(0x00433040, ClearLOT);
}
