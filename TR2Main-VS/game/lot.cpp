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

void InitialiseLOTarray()
{
    BaddiesSlots = (CREATURE_INFO*)game_malloc(sizeof(CREATURE_INFO) * MAX_CREATURES, GBUF_CreatureData);
    for (int i = 0; i < MAX_CREATURES; i++)
    {
        CREATURE_INFO* creature = &BaddiesSlots[i];
        creature->itemNumber = -1;
        creature->LOT.node = (BOX_NODE*)game_malloc(sizeof(BOX_NODE) * BoxesCount, GBUF_CreatureLOT);
    }
    BaddiesSlotsUsedCount = 0;
}

void DisableBaddieAI(short itemNumber)
{
    CREATURE_INFO* creature = NULL;

    if (itemNumber == Lara.item_number)
    {
        creature = Lara.creature;
        Lara.creature = NULL;
    }
    else
    {
        ITEM_INFO* item = &Items[itemNumber];
        CREATURE_INFO* creature = GetCreatureInfo(item);
        item->data = NULL;
    }

    if (creature != NULL)
    {
        creature->itemNumber = -1;
        BaddiesSlotsUsedCount--;
    }
}

BOOL EnableBaddieAI(short itemNumber, BOOL always)
{
    CREATURE_INFO* creature = NULL;

    if (Lara.item_number == itemNumber)
    {
        if (Lara.creature != NULL)
            return TRUE;
    }
    else if (Items[itemNumber].data)
    {
        return TRUE;
    }

    if (BaddiesSlotsUsedCount < MAX_CREATURES)
    {
        creature = BaddiesSlots;
        for (int slotIndex = 0; slotIndex < MAX_CREATURES; slotIndex++, creature++)
        {
            if (creature->itemNumber == -1)
            {
                InitialiseSlot(itemNumber, slotIndex);
                return TRUE;
            }
        }
    }

    int worstdist = 0;
    if (!always)
    {
        ITEM_INFO* item = &Items[itemNumber];
        int x = (item->pos.x - Camera.pos.x) >> 8;
        int y = (item->pos.y - Camera.pos.y) >> 8;
        int z = (item->pos.z - Camera.pos.z) >> 8;
        worstdist = SQR(x) + SQR(y) + SQR(z);
    }
    else
    {
        worstdist = 0;
    }

    int worstslot = -1;
    creature = BaddiesSlots;
    for (int slot = 0; slot < MAX_CREATURES; slot++, creature++)
    {
        ITEM_INFO* item = &Items[creature->itemNumber];
        int x = (item->pos.x - Camera.pos.x) >> 8;
        int y = (item->pos.y - Camera.pos.y) >> 8;
        int z = (item->pos.z - Camera.pos.z) >> 8;
        int dist = SQR(x) + SQR(y) + SQR(z);
        if (dist > worstdist)
        {
            worstdist = dist;
            worstslot = slot;
        }
    }

    if (worstslot >= 0)
    {
        creature = &BaddiesSlots[worstslot];
        Items[creature->itemNumber].status = ITEM_INVISIBLE;
        DisableBaddieAI(creature->itemNumber);
        InitialiseSlot(itemNumber, worstslot);
        return TRUE;
    }

    return FALSE;
}

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
    creature->LOT.step = CLICK(1);
    creature->LOT.drop = -CLICK(2);
    creature->LOT.blockMask = 0x4000;
    creature->LOT.fly = 0;
    switch (item->objectID)
    {
    case ID_LARA:
        creature->LOT.step = BLOCK(20);
        creature->LOT.drop = -BLOCK(20);
        creature->LOT.fly = 256;
        break;
    case ID_SHARK:
    case ID_BARRACUDA:
    case ID_DIVER:
    case ID_JELLY:
    case ID_CROW:
    case ID_EAGLE:
        creature->LOT.step = BLOCK(20);
        creature->LOT.drop = -BLOCK(20);
        creature->LOT.fly = 16;
        if (item->objectID == ID_SHARK)
            creature->LOT.blockMask = 0x8000;
        break;
    case ID_WORKER3:
    case ID_WORKER4:
    case ID_YETI:
        creature->LOT.step = BLOCK(1);
        creature->LOT.drop = -BLOCK(1);
        break;
    case ID_SPIDER_or_WOLF:
#ifdef FEATURE_GOLD
        if (IsGold()) // NOTE: Wolf don't climb or drop 4 clicks !
            break;
#endif
        creature->LOT.step = CLICK(1) / 2;
        creature->LOT.drop = -BLOCK(1);
        break;
    case ID_SKIDOO_ARMED:
        creature->LOT.step = CLICK(1) / 2;
        creature->LOT.drop = -BLOCK(1);
        break;
    case ID_TREX:
        creature->LOT.blockMask = 0x8000;
        break;
    }
    ClearLOT(&creature->LOT);
    if (itemNumber != Lara.item_number)
        CreateZone(item);
    ++BaddiesSlotsUsedCount;
}

void CreateZone(ITEM_INFO* item)
{
    CREATURE_INFO* creature;
    BOX_NODE* node;
    DWORD i;
    short* zone, zone_number;
    short* flip, flip_number;

    creature = GetCreatureInfo(item);
    if (creature->LOT.fly != 0)
    {
        zone = FlyZones[FALSE];
        flip = FlyZones[TRUE];
    }
    else
    {
        zone = GroundZones[2 * (creature->LOT.step >> 8) + FALSE];
        flip = GroundZones[2 * (creature->LOT.step >> 8) + TRUE];
    }

    ROOM_INFO* r = &Rooms[item->roomNumber];
    item->boxNumber = GetSectorBoxXZ(item, r);
    zone_number = zone[item->boxNumber];
    flip_number = flip[item->boxNumber];
    for (i = 0, creature->LOT.zoneCount = 0, node = creature->LOT.node; i < BoxesCount; i++, zone++, flip++)
    {
        if (*zone == zone_number || *flip == flip_number)
        {
            node->boxNumber = (short)i;
            node++;
            creature->LOT.zoneCount++;
        }
    }
}

void ClearLOT(LOT_INFO* LOT)
{
    LOT->head = -1;
    LOT->tail = -1;
    LOT->searchNumber = 0;
    LOT->targetBox = -1;
    LOT->requiredBox = -1;
    for (DWORD i = 0; i < BoxesCount; i++)
    {
        BOX_NODE* node = &LOT->node[i];
        node->exitBox = -1;
        node->nextExpansion = -1;
        node->searchNumber = 0;
    }
}

 /*
  * Inject function
  */
void Inject_Lot() {
	INJECT(0x00432B10, InitialiseLOTarray);
	INJECT(0x00432B70, DisableBaddieAI);
	INJECT(0x00432BC0, EnableBaddieAI);
	INJECT(0x00432D70, InitialiseSlot);
	INJECT(0x00432F80, CreateZone);
	INJECT(0x00433040, ClearLOT);
}
