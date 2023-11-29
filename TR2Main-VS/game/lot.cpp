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

CREATURE_INFO* BaddiesSlots = NULL;
int BaddiesSlotUsed = 0;

void InitialiseLOTarray()
{
	CREATURE_INFO* creature = NULL;
	BaddiesSlots = (CREATURE_INFO*)game_malloc(sizeof(CREATURE_INFO) * MAX_CREATURES, GBUF_CreatureData);
	for (int i = 0; i < MAX_CREATURES; i++)
	{
		creature = &BaddiesSlots[i];
		creature->item_num = -1;
		creature->LOT.node = (BOX_NODE*)game_malloc(sizeof(BOX_NODE) * BoxesCount, GBUF_CreatureLOT);
	}
	BaddiesSlotUsed = 0;
}

void DisableBaddieAI(short itemID)
{
	CREATURE_INFO* creature = NULL;
	ITEM_INFO* item = NULL;

	if (Lara.item_number == itemID)
	{
		creature = Lara.creature;
		Lara.creature = NULL;
	}
	else
	{
		item = &Items[itemID];
		creature = (CREATURE_INFO*)item->data;
		item->data = NULL;
	}

	if (creature != NULL)
	{
		BaddiesSlotUsed--;
		LogDebug("Creature item_number: %d with objectID: %d was removed because it's AI was disabled, Active creature remaining: %d", creature->item_num, Items[creature->item_num].objectID, BaddiesSlotUsed);
		creature->item_num = -1;
	}
}

BOOL EnableBaddieAI(short itemID, BOOL isAlways)
{
	CREATURE_INFO* creature = NULL, *new_creature = NULL;
	ITEM_INFO* item = NULL, *new_item = NULL;
	PHD_VECTOR pos = {};
	int farestSlot = -1;
	int farestDistance = 0, distance = 0;

	if (Lara.item_number == itemID && Lara.creature != NULL)
	{
		if (Lara.creature != NULL)
			LogDebug("Lara.creature is already initialized !");
		return TRUE;
	}

	item = &Items[itemID];
	if (Lara.item_number != itemID && item->data != NULL)
		return TRUE;

	// Max creatures reached, search for the farest one and replace it by the new one !
	if (BaddiesSlotUsed >= MAX_CREATURES)
	{
		if (!isAlways)
		{
			pos.x = (item->pos.x - Camera.pos.x) >> 8;
			pos.y = (item->pos.y - Camera.pos.y) >> 8;
			pos.z = (item->pos.z - Camera.pos.z) >> 8;
			farestDistance = SQR(pos.x) + SQR(pos.y) + SQR(pos.z);
		}
		else
		{
			farestDistance = 0;
		}

		farestSlot = -1;
		for (int i = 0; i < MAX_CREATURES; i++)
		{
			creature = &BaddiesSlots[i];
			item = &Items[creature->item_num];
			pos.x = (item->pos.x - Camera.pos.x) >> 8;
			pos.y = (item->pos.y - Camera.pos.y) >> 8;
			pos.z = (item->pos.z - Camera.pos.z) >> 8;
			distance = SQR(pos.x) + SQR(pos.y) + SQR(pos.z);
			if (distance > farestDistance)
			{
				farestDistance = distance;
				farestSlot = i;
			}
		}

		if (farestSlot >= 0)
		{
			creature = &BaddiesSlots[farestSlot];
			item = &Items[creature->item_num];
			item->status = ITEM_INVISIBLE;
			DisableBaddieAI(creature->item_num);
			InitialiseSlot(itemID, farestSlot);

			new_item = &Items[itemID];
			if (new_item != NULL)
			{
				new_creature = GetCreatureInfo(new_item);
				if (new_creature != NULL)
					LogDebug("Creature item_number: %d will takeover the farest creature item_number: %d slot since the baddies array is satured !", new_creature->item_num, creature->item_num);
			}
			return TRUE;
		}
		return FALSE;
	}
	else
	{
		for (int i = 0; i < MAX_CREATURES; i++)
		{
			creature = &BaddiesSlots[i];
			if (creature->item_num == -1)
			{
				InitialiseSlot(itemID, i);
				return TRUE;
			}
		}
	}

	return FALSE;
}

void InitialiseSlot(short itemID, int creatureIdx)
{
	CREATURE_INFO* creature = NULL;
	ITEM_INFO* item = NULL;

	creature = &BaddiesSlots[creatureIdx];
	item = &Items[itemID];

	if (itemID == Lara.item_number)
		Lara.creature = creature;
	else
		item->data = creature;

	creature->item_num = itemID;
	creature->mood = MOOD_BORED;
	creature->neck_rotation = 0;
	creature->head_rotation = 0;
	creature->maximum_turn = ANGLE(1);
	creature->flags = 0;
	creature->enemy = 0;
	creature->LOT.step = 256;
	creature->LOT.drop = -512;
	creature->LOT.block_mask = 0x4000;
	creature->LOT.fly = 0;

	switch (item->objectID)
	{
	case ID_LARA:
		creature->LOT.step = 20480;
		creature->LOT.drop = -20480;
		creature->LOT.fly = 256;
		break;
	case ID_SHARK:
	case ID_BARRACUDA:
	case ID_DIVER:
	case ID_JELLY:
	case ID_CROW:
	case ID_EAGLE:
		creature->LOT.step = 20480;
		creature->LOT.drop = -20480;
		creature->LOT.fly = 16;
		if (item->objectID == ID_SHARK)
			creature->LOT.block_mask = 0x8000;
		break;
	case ID_DINO:
		creature->LOT.block_mask = 0x8000;
		break;
	case ID_SPIDER_or_WOLF:
	case ID_SKIDOO_ARMED:
		creature->LOT.step = 512;
		creature->LOT.drop = -1024;
		break;
	case ID_WORKER3:
	case ID_WORKER4:
	case ID_YETI:
		creature->LOT.step = 1024;
		creature->LOT.drop = -1024;
		break;
	}

	ClearLOT(&creature->LOT);
	if (itemID != Lara.item_number)
		CreateZone(item);

	BaddiesSlotUsed++;
	if (item->objectID != ID_LARA) // Ignore lara since it's not a creature.
		LogDebug("Creature item_number: %d with objectID: %d was added because it's AI was enabled, Active creature now: %d", itemID, item->objectID, BaddiesSlotUsed);
}

void CreateZone(ITEM_INFO* item)
{
	CREATURE_INFO* creature = NULL;
	FLOOR_INFO* floor = NULL;
	ROOM_INFO* room = NULL;
	BOX_NODE* node = NULL;
	short* zone = NULL, *flip = NULL;
	short zoneNumber = 0, flipNumber = 0;

	creature = (CREATURE_INFO*)item->data;
	zone = creature->LOT.fly != 0 ? FlyZones[0] : GroundZones[creature->LOT.step >> 8][0];
	flip = creature->LOT.fly != 0 ? FlyZones[1] : GroundZones[creature->LOT.step >> 8][1];

	room = &RoomInfo[item->roomNumber];
	item->boxNumber = room->floor[(((item->pos.z - room->z) >> WALL_SHIFT) + room->xSize * ((item->pos.x - room->x) >> WALL_SHIFT))].box;
	zoneNumber = zone[item->boxNumber];
	flipNumber = flip[item->boxNumber];
	node = creature->LOT.node;
	creature->LOT.zone_count = 0;

	for (DWORD i = 0; i < BoxesCount; i++)
	{
		if (*zone == zoneNumber || *flip == flipNumber)
		{
			node->box_number = i;
			++node;
			++creature->LOT.zone_count;
		}
		zone++;
		flip++;
	}
}

void ClearLOT(LOT_INFO* LOT)
{
	BOX_NODE* node = NULL;
	LOT->tail = -1;
	LOT->head = -1;
	LOT->search_number = 0;
	LOT->target_box = -1;
	LOT->required_box = -1;
	for (DWORD i = 0; i < BoxesCount; i++)
	{
		node = &LOT->node[i];
		node->next_expansion = -1;
		node->exit_box = -1;
		node->search_number = 0;
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