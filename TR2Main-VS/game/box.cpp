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
#include "game/box.h"
#include "3dsystem/phd_math.h"
#include "game/control.h"
#include "game/items.h"
#include "game/larafire.h"
#include "game/lot.h"
#include "game/missile.h"
#include "global/vars.h"

#if defined(FEATURE_MOD_CONFIG)
#include "modding/mod_utils.h"
#endif

void CreatureAIInfo(ITEM_INFO* item, AI_INFO* AI)
{
	CREATURE_INFO* creature = (CREATURE_INFO*)item->data;
	if (creature == NULL) return;
#if defined(FEATURE_MOD_CONFIG)
	if ((item->objectID == ID_BANDIT1 || item->objectID == ID_BANDIT2 || item->objectID == ID_BANDIT2B) && !GetModMakeMercenaryAttackLaraDirectly())
		GetBaddieTarget(creature->item_num, FALSE);
	else if (item->objectID == ID_MONK1 || item->objectID == ID_MONK2 && !GetModMakeMonkAttackLaraDirectly())
		GetBaddieTarget(creature->item_num, TRUE);
#else
	if (item->objectID == ID_BANDIT1 || item->objectID == ID_BANDIT2)
		GetBaddieTarget(creature->item_num, FALSE);
	else if (item->objectID == ID_MONK1 || item->objectID == ID_MONK2)
		GetBaddieTarget(creature->item_num, TRUE);
#endif
	else
		creature->enemy = LaraItem;

	ITEM_INFO* enemy = creature->enemy;
	ROOM_INFO* room = NULL;
	OBJECT_INFO* obj = NULL;
	int x = 0, z = 0;
	short angle = 0;
	if (enemy == NULL)
		enemy = LaraItem;

	short* zone = creature->LOT.fly != 0 ? FlyZones[FlipStatus] : GroundZones[creature->LOT.step >> 8][FlipStatus];
	
	room = &RoomInfo[item->roomNumber];
	item->boxNumber = room->floor[((item->pos.z - room->z) >> WALL_SHIFT) + room->xSize * ((item->pos.x - room->x) >> WALL_SHIFT)].box;
	AI->zone_number = zone[item->boxNumber];
	
	room = &RoomInfo[enemy->roomNumber];
	enemy->boxNumber = room->floor[((enemy->pos.z - room->z) >> WALL_SHIFT) + room->xSize * ((enemy->pos.x - room->x) >> WALL_SHIFT)].box;
	AI->enemy_zone = zone[enemy->boxNumber];

	if ((Boxes[enemy->boxNumber].overlapIndex & creature->LOT.block_mask) || (creature->LOT.node[item->boxNumber].search_number == (creature->LOT.search_number | 0x8000)))
		zone[enemy->boxNumber] |= 0x8000;

	obj = &Objects[item->objectID];
	x = enemy->pos.x - (item->pos.x + (obj->pivotLength * phd_sin(item->pos.rotY) >> W2V_SHIFT));
	z = enemy->pos.z - (item->pos.z + (obj->pivotLength * phd_cos(item->pos.rotY) >> W2V_SHIFT));
	angle = phd_atan(z, x);
	if (creature->enemy)
		AI->distance = SQR(x) + SQR(z);
	else
		AI->distance = 0x7FFFFFFF;
	AI->angle = angle - item->pos.rotY;
	AI->enemy_facing = angle - enemy->pos.rotY + 0x8000;
	AI->ahead = AI->angle > -0x4000 && AI->angle < 0x4000;
	AI->bite = AI->ahead && enemy->hitPoints > 0 && ABS(item->pos.y - enemy->pos.y) <= 384;
}

void CreatureDie(short itemID, BOOL explode) {
	ITEM_INFO* item = &Items[itemID];
	item->collidable = FALSE;
	item->hitPoints = HP_DONT_TARGET;

	if (explode) {
		ExplodingDeath(itemID, ~0, 0);
		KillItem(itemID);
	}
	else {
		RemoveActiveItem(itemID);
	}

	DisableBaddieAI(itemID);
	item->flags |= IFL_INVISIBLE;
	if (item->clear_body) {
		item->nextActive = PrevItemActive;
		PrevItemActive = itemID;
	}

	ITEM_INFO* pickup = NULL;
	for (int i = item->carriedItem; i != -1; i = pickup->carriedItem) {
		pickup = &Items[i];
		pickup->pos.x = item->pos.x;
		pickup->pos.y = item->pos.y;
		pickup->pos.z = item->pos.z;
		ItemNewRoom(i, item->roomNumber);
	}
}

void CreatureKill(ITEM_INFO* item, int killAnim, int killState, int laraKillState) {
#ifdef FEATURE_CHEAT
	// Return Lara to normal state if Dozy cheat enabled
	if (Lara.water_status == LWS_Cheat) {
		Lara.water_status = LWS_AboveWater;
		Lara.mesh_effects = 0;
	}
#endif // FEATURE_CHEAT
	item->animNumber = killAnim + Objects[item->objectID].animIndex;
	item->frameNumber = Anims[item->animNumber].frameBase;
	item->currentAnimState = killState;
	LaraItem->animNumber = Objects[ID_LARA_EXTRA].animIndex;
	LaraItem->frameNumber = Anims[LaraItem->animNumber].frameBase;
	LaraItem->currentAnimState = 0;
	LaraItem->goalAnimState = laraKillState;
	LaraItem->pos.x = item->pos.x;
	LaraItem->pos.y = item->pos.y;
	LaraItem->pos.z = item->pos.z;
	LaraItem->pos.rotY = item->pos.rotY;
	LaraItem->pos.rotX = item->pos.rotX;
	LaraItem->pos.rotZ = item->pos.rotZ;
	LaraItem->fallSpeed = 0;
	LaraItem->gravity = 0;
	LaraItem->speed = 0;
	if (LaraItem->roomNumber != item->roomNumber) {
		ItemNewRoom(Lara.item_number, item->roomNumber);
	}
	AnimateItem(LaraItem);
	LaraItem->goalAnimState = laraKillState;
	LaraItem->currentAnimState = laraKillState;
	Lara.extra_anim = 1;
	Lara.hit_direction = -1;
	Lara.air = -1;
	Lara.gun_status = LGS_HandBusy;
	Lara.gun_type = LGT_Unarmed;
	Camera.pos.roomNumber = LaraItem->roomNumber;
}

void GetBaddieTarget(short creatureIdx, BOOL isMonk)
{
	ITEM_INFO* item = NULL, *targetItem = NULL, *bestTarget = NULL;
	CREATURE_INFO* creature = NULL, *baddy = NULL;
	int bestdistance = 0, x = 0, y = 0, z = 0, distance = 0;

	item = &Items[creatureIdx];
	creature = (CREATURE_INFO*)item->data;
	if (creature == NULL) 
		return;

	bestdistance = 0x7FFFFFFF;
	for (int i = 0; i < MAX_CREATURES; i++)
	{
		baddy = &BaddiesSlots[i];
		if (baddy->item_num == -1 || creatureIdx == baddy->item_num)
			continue;
		targetItem = &Items[baddy->item_num];
		if (isMonk)
		{
			if (targetItem->objectID != ID_BANDIT1 && targetItem->objectID != ID_BANDIT2 && targetItem->objectID != ID_BANDIT2B)
				continue;
		}
		else if (targetItem->objectID != ID_MONK1 && targetItem->objectID != ID_MONK2)
			continue;
		x = (targetItem->pos.x - item->pos.x) >> 6;
		y = (targetItem->pos.y - item->pos.y) >> 6;
		z = (targetItem->pos.z - item->pos.z) >> 6;
		distance = SQR(z) + SQR(y) + SQR(x);
		if (distance < bestdistance)
		{
			bestdistance = distance;
			bestTarget = targetItem;
		}
	}

	if (bestTarget != NULL)
	{
		if (!isMonk || IsMonkAngry)
		{
			x = (LaraItem->pos.x - item->pos.x) >> 6;
			y = (LaraItem->pos.y - item->pos.y) >> 6;
			z = (LaraItem->pos.z - item->pos.z) >> 6;
			distance = SQR(z) + SQR(y) + SQR(x);
			if (distance < bestdistance)
			{
				bestdistance = distance;
				bestTarget = LaraItem;
			}
		}

		if (creature->enemy != NULL && creature->enemy->status == ITEM_ACTIVE)
		{
			x = (creature->enemy->pos.x - item->pos.x) >> 6;
			y = (creature->enemy->pos.y - item->pos.y) >> 6;
			z = (creature->enemy->pos.z - item->pos.z) >> 6;
			distance = SQR(z) + SQR(y) + SQR(x);
			if (distance < (bestdistance + 0x400000))
				creature->enemy = bestTarget;
		}
		else
		{
			creature->enemy = bestTarget;
		}
	}
	else if (!isMonk || IsMonkAngry)
	{
		creature->enemy = LaraItem;
	}
	else
	{
		creature->enemy = NULL;
	}
}

/*
 * Inject function
 */
void Inject_Box() {
	//INJECT(0x0040E190, InitialiseCreature);
	//INJECT(0x0040E1C0, CreatureActive);
	INJECT(0x0040E210, CreatureAIInfo);
	//INJECT(0x0040E470, SearchLOT);
	//INJECT(0x0040E670, UpdateLOT);
	//INJECT(0x0040E6E0, TargetBox);
	//INJECT(0x0040E780, StalkBox);
	//INJECT(0x0040E880, EscapeBox);
	//INJECT(0x0040E930, ValidBox);
	//INJECT(0x0040E9E0, CreatureMood);
	//INJECT(0x0040EE50, CalculateTarget);
	//INJECT(0x0040F2B0, CreatureCreature);
	//INJECT(0x0040F3B0, BadFloor);
	INJECT(0x0040F440, CreatureDie);
	//INJECT(0x0040F500, CreatureAnimation);
	//INJECT(0x0040FDD0, CreatureTurn);
	//INJECT(0x0040FEB0, CreatureTilt);
	//INJECT(0x0040FEF0, CreatureHead);
	//INJECT(0x0040FF40, CreatureNeck);
	//INJECT(0x0040FF90, CreatureFloat);
	//INJECT(0x00410040, CreatureUnderwater);
	//INJECT(0x00410090, CreatureEffect);
	//INJECT(0x004100F0, CreatureVault);
	INJECT(0x00410230, CreatureKill);
	INJECT(0x004103A0, GetBaddieTarget);
}