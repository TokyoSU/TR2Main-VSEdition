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
#include "game/draw.h"
#include "game/diver.h"
#include "game/items.h"
#include "game/larafire.h"
#include "game/effects.h"
#include "game/lot.h"
#include "game/missile.h"
#include "specific/game.h"
#include "global/vars.h"

#if defined(FEATURE_MOD_CONFIG)
#include "modding/mod_utils.h"
#endif

void InitialiseCreature(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];
	item->collidable = TRUE;
	item->data = NULL;
	item->pos.rotY += (GetRandomControl() - PHD_90) >> 1;
}

int CreatureActive(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];
	if (item->status == ITEM_INVISIBLE)
	{
		if (!EnableBaddieAI(itemNumber, FALSE))
			return FALSE;
		item->status = ITEM_ACTIVE;
	}
	return TRUE;
}

void CreatureAIInfo(ITEM_INFO* item, AI_INFO* AI)
{
	CREATURE_INFO* creature = GetCreatureInfo(item);
	if (creature == NULL) return;

#if defined(FEATURE_MOD_CONFIG)
	if ((item->objectID == ID_BANDIT1 || item->objectID == ID_BANDIT2 || item->objectID == ID_BANDIT2B) && !Mod.makeMercenaryAttackLaraFirst)
		GetBaddieTarget(creature->itemNumber, FALSE);
	else if ((item->objectID == ID_MONK1 || item->objectID == ID_MONK2) && !Mod.makeMonkAttackLaraFirst)
		GetBaddieTarget(creature->itemNumber, TRUE);
#else
	if (item->objectID == ID_BANDIT1 || item->objectID == ID_BANDIT2 || item->objectID == ID_BANDIT2B)
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

	short* zone = creature->LOT.fly != 0 ? FlyZones[FlipStatus] : GroundZones[2 * (creature->LOT.step >> 8) + FlipStatus];
	room = &RoomInfo[item->roomNumber];
	item->boxNumber = GetSectorBoxXZ(item, room);
	AI->zoneNumber = zone[item->boxNumber];
	room = &RoomInfo[enemy->roomNumber];
	enemy->boxNumber = GetSectorBoxXZ(enemy, room);
	AI->enemyZone = zone[enemy->boxNumber];

	if ((Boxes[enemy->boxNumber].overlapIndex & creature->LOT.blockMask) || creature->LOT.node[item->boxNumber].searchNumber == (creature->LOT.searchNumber | 0x8000))
		AI->enemyZone |= 0x4000;

	obj = &Objects[item->objectID];
	x = enemy->pos.x - (obj->pivotLength * phd_sin(item->pos.rotY) >> W2V_SHIFT) - item->pos.x;
	z = enemy->pos.z - (obj->pivotLength * phd_cos(item->pos.rotY) >> W2V_SHIFT) - item->pos.z;
	angle = phd_atan(z, x);
	if (creature->enemy)
		AI->distance = SQR(x) + SQR(z);
	else
		AI->distance = INT_MAX;
	AI->angle = angle - item->pos.rotY;
	AI->enemyFacing = (angle - enemy->pos.rotY) + PHD_180;
	AI->ahead = AI->angle > -PHD_90 && AI->angle < PHD_90;
	AI->bite = AI->ahead && enemy->hitPoints > 0 && ABS(item->pos.y - enemy->pos.y) <= 384;
}

void TargetBox(LOT_INFO* LOT, short boxNumber)
{
	BOX_INFO* box = &Boxes[boxNumber];
	LOT->target.x = GetRandomControl() * (((int)box->bottom - (int)box->top - 1) >> 5) + 512 + ((int)box->top * 1024);
	LOT->target.y = LOT->fly != 0 ? box->height - 384 : box->height;
	LOT->target.z = GetRandomControl() * (((int)box->right - (int)box->left - 1) >> 5) + 512 + ((int)box->left * 1024);
	LOT->requiredBox = boxNumber;
}

int ValidBox(ITEM_INFO* item, short zoneNumber, short boxNumber)
{
	CREATURE_INFO* creature = GetCreatureInfo(item);
	short* zone = creature->LOT.fly != 0 ? FlyZones[FlipStatus] : GroundZones[2 * (creature->LOT.step >> 8) + FlipStatus];
	if (zone[boxNumber] != zoneNumber)
		return FALSE;
	BOX_INFO* box = &Boxes[boxNumber];
	if (creature->LOT.blockMask & box->overlapIndex)
		return FALSE;
	if (item->pos.z > ((int)box->left << WALL_SHIFT) && item->pos.z < ((int)box->right << WALL_SHIFT)
    &&  item->pos.x > ((int)box->top << WALL_SHIFT)  && item->pos.x < ((int)box->bottom << WALL_SHIFT))
		return FALSE;
	return TRUE;
}

void CreatureMood(ITEM_INFO* item, AI_INFO* ai, BOOL isViolent)
{
	CREATURE_INFO* creature = GetCreatureInfo(item);
	if (creature == NULL) return;

	LOT_INFO* LOT = &creature->LOT;
	ITEM_INFO* enemy = creature->enemy;

	if (LOT->node[item->boxNumber].searchNumber == (creature->LOT.searchNumber | 0x8000))
		creature->LOT.requiredBox = -1;

	if (creature->mood != MOOD_ATTACK && creature->LOT.requiredBox != -1 && !ValidBox(item, ai->zoneNumber, creature->LOT.targetBox))
	{
		if (ai->zoneNumber == ai->enemyZone)
			creature->mood = MOOD_BORED;
		creature->LOT.requiredBox = -1;
	}

	MOOD_TYPE oldmood = creature->mood;
	if (enemy == NULL)
	{
		creature->mood = MOOD_BORED;
		enemy = LaraItem;
	}
	else if (enemy->hitPoints < 1)
	{
		creature->mood = MOOD_BORED;
	}
	else if (isViolent)
	{
		switch (creature->mood)
		{
		case MOOD_ATTACK:
			if (ai->zoneNumber != ai->enemyZone)
				creature->mood = MOOD_BORED;
			break;
		case MOOD_BORED:
		case MOOD_STALK:
			if (ai->zoneNumber == ai->enemyZone)
				creature->mood = MOOD_ATTACK;
			else if (item->hitStatus)
				creature->mood = MOOD_ESCAPE;
			break;
		case MOOD_ESCAPE:
			if (ai->zoneNumber == ai->enemyZone)
				creature->mood = MOOD_ATTACK;
			break;
		}
	}
	else
	{
		switch (creature->mood)
		{
		case MOOD_BORED:
		case MOOD_STALK:
			if (item->hitStatus && (GetRandomControl() < 2048 || ai->zoneNumber != ai->enemyZone))
			{
				creature->mood = MOOD_ESCAPE;
				
			}
			else if (ai->zoneNumber == ai->enemyZone)
			{
				if (ai->distance < 0x900000 || (creature->mood == MOOD_STALK && creature->LOT.requiredBox == -1))
					creature->mood = MOOD_ATTACK;
				else
					creature->mood = MOOD_STALK;
			}
			break;
		case MOOD_ATTACK:
			if (item->hitStatus && (GetRandomControl() < 2048 || ai->zoneNumber != ai->enemyZone))
				creature->mood = MOOD_ESCAPE;
			else if (ai->zoneNumber != ai->enemyZone)
				creature->mood = MOOD_BORED;
			break;
		case MOOD_ESCAPE:
			if (ai->zoneNumber == ai->enemyZone && GetRandomControl() < 256)
				creature->mood = MOOD_STALK;
			break;
		}
	}

	if (oldmood != creature->mood)
	{
		if (oldmood == MOOD_ATTACK)
			TargetBox(LOT, creature->LOT.targetBox);
		creature->LOT.requiredBox = -1;
	}

	short boxNumber = 0;
	switch (creature->mood)
	{
	case MOOD_ATTACK:
		creature->LOT.target.x = enemy->pos.x;
		creature->LOT.target.y = enemy->pos.y;
		creature->LOT.target.z = enemy->pos.z;
		creature->LOT.requiredBox = enemy->boxNumber;
		if (creature->LOT.fly != 0 && Lara.water_status == LWS_AboveWater)
			creature->LOT.target.y += GetBestFrame(enemy)[2]; // ymin (above)
		break;
	case MOOD_BORED:
		boxNumber = LOT->node[creature->LOT.zoneCount * GetRandomControl() >> 15].boxNumber;
		if (ValidBox(item, ai->zoneNumber, boxNumber))
		{
			if (StalkBox(item, enemy, boxNumber) && enemy->hitPoints > 0 && creature->enemy != NULL)
			{
				TargetBox(LOT, boxNumber);
				creature->mood = MOOD_STALK;
			}
			else if (creature->LOT.requiredBox == -1)
				TargetBox(LOT, boxNumber);
		}
		break;
	case MOOD_STALK:
		boxNumber = creature->LOT.requiredBox;
		if (boxNumber == -1 || !StalkBox(item, enemy, boxNumber))
		{
			boxNumber = LOT->node[creature->LOT.zoneCount * GetRandomControl() >> 15].boxNumber;
			if (ValidBox(item, ai->zoneNumber, boxNumber))
			{
				if (StalkBox(item, enemy, boxNumber))
				{
					TargetBox(LOT, boxNumber);
				}
				else if (creature->LOT.requiredBox == -1)
				{
					TargetBox(LOT, boxNumber);
					if (ai->zoneNumber != ai->enemyZone)
						creature->mood = MOOD_BORED;
				}
			}
		}
		break;
	case MOOD_ESCAPE:
		boxNumber = LOT->node[creature->LOT.zoneCount * GetRandomControl() >> 15].boxNumber;
		if (ValidBox(item, ai->zoneNumber, boxNumber) && creature->LOT.requiredBox == -1)
		{
			if (EscapeBox(item, enemy, boxNumber))
			{
				TargetBox(LOT, boxNumber);
			}
			else if (ai->zoneNumber == ai->enemyZone && StalkBox(item, enemy, boxNumber))
			{
				TargetBox(LOT, boxNumber);
				creature->mood = MOOD_STALK;
			}
		}
		break;
	}

	if (creature->LOT.targetBox == -1)
		TargetBox(LOT, item->boxNumber);
	CalculateTarget(&creature->target, item, LOT);
}

int BadFloor(int x, int y, int z, int boxHeight, int nextHeight, short roomNumber, LOT_INFO* LOT)
{
	FLOOR_INFO* floor = GetFloor(x, y, z, &roomNumber);
	if (floor->box == -1)
		return TRUE;
	BOX_INFO* box = &Boxes[floor->box];
	int height = box->height;
	if (box->overlapIndex & LOT->blockMask)
		return TRUE;
	if (boxHeight - height > LOT->step || boxHeight - height < LOT->drop)
		return TRUE;
	if (boxHeight - height < -LOT->step && height > nextHeight)
		return TRUE;
	if (LOT->fly != 0 && y > height + LOT->fly)
		return TRUE;
	return FALSE;
}

void CreatureDie(short itemNumber, BOOL explode) {
	ITEM_INFO* item = &Items[itemNumber];
	item->collidable = FALSE;
	item->hitPoints = HP_DONT_TARGET;

	if (explode) {
		ExplodingDeath(itemNumber, ~0, 0);
		KillItem(itemNumber);
		item->status = ITEM_DISABLED;
	}
	else {
		RemoveActiveItem(itemNumber);
	}

	DisableBaddieAI(itemNumber);
	item->flags |= IFL_ONESHOT;
	if (item->clear_body) {
		item->nextActive = PrevItemActive;
		PrevItemActive = itemNumber;
	}

	CreatureDropItem(item);
}

short CreatureTurn(ITEM_INFO* item, short maximumTurn)
{
	CREATURE_INFO* creature = GetCreatureInfo(item);
	if (creature == NULL)
		return 0;
	if (item->speed == 0 || maximumTurn == 0)
		return 0;

	int x = creature->target.x - item->pos.x;
	int z = creature->target.z - item->pos.z;
	int range = (item->speed << W2V_SHIFT) / maximumTurn;
	short angle = phd_atan(z, x) - item->pos.rotY;
	bool in_range = SQR(x) + SQR(z) < SQR(range);
	if ((angle > ANGLE(90) || angle < ANGLE(90)) && in_range)
		maximumTurn >>= 1;

	if (angle > maximumTurn)
		angle = maximumTurn;
	else if (angle < -maximumTurn)
		angle = -maximumTurn;

	item->pos.rotY += angle;
	return angle;
}

int CreatureVault(short itemNumber, short angle, int vault, int shift)
{
	ITEM_INFO* item = &Items[itemNumber];
	int x = item->pos.x >> WALL_SHIFT;
	int y = item->pos.y;
	int z = item->pos.z >> WALL_SHIFT;
	short roomNumber = item->roomNumber;

	CreatureAnimation(itemNumber, angle, 0);

	if (item->floor > y + (CLICK_SIZE * 7 / 2))
		vault = -4;
	else if (item->pos.y > y - (CLICK_SIZE * 3 / 2))
		return 0;
	else if (item->pos.y > y - (CLICK_SIZE * 5 / 2))
		vault = 2;
	else if (item->pos.y > y - (CLICK_SIZE * 7 / 2))
		vault = 3;
	else
		vault = 4;

	int xFloor = item->pos.x >> WALL_SHIFT;
	int zFloor = item->pos.z >> WALL_SHIFT;
	if (z == zFloor)
	{
		if (x == xFloor)
			return 0;

		if (x < xFloor)
		{
			item->pos.x = (xFloor << WALL_SHIFT) - shift;
			item->pos.rotY = ANGLE(90);
		}
		else
		{
			item->pos.x = (xFloor << WALL_SHIFT) - shift;
			item->pos.rotY = -ANGLE(90);
		}
	}
	else if (x == xFloor)
	{
		if (z < zFloor)
		{
			item->pos.z = (zFloor << WALL_SHIFT) - shift;
			item->pos.rotY = ANGLE(0);
		}
		else
		{
			item->pos.z = (z << WALL_SHIFT) - shift;
			item->pos.rotY = -ANGLE(180);
		}
	}

	item->pos.y = y;
	item->floor = y;
	if (roomNumber != item->roomNumber)
		ItemNewRoom(itemNumber, roomNumber);

	return vault;
}

void CreatureKill(ITEM_INFO* item, int killAnim, int killState, int laraKillState) {
#ifdef FEATURE_CHEAT
	// Return Lara to normal state if Dozy cheat enabled
	if (Lara.water_status == LWS_Cheat) {
		Lara.water_status = LWS_AboveWater;
		Lara.mesh_effects = 0;
	}
#endif // FEATURE_CHEAT
	item->animNumber = Objects[item->objectID].animIndex + killAnim;
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
	creature = GetCreatureInfo(item);
	if (creature == NULL) 
		return;

	bestdistance = 0x7FFFFFFF;
	for (int i = 0; i < MAX_CREATURES; i++)
	{
		baddy = &BaddiesSlots[i];
		if (baddy->itemNumber == -1 || creatureIdx == baddy->itemNumber)
			continue;
		targetItem = &Items[baddy->itemNumber];
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

void CreatureDropItem(ITEM_INFO* item)
{
	ITEM_INFO* pickup = NULL;
	for (int i = item->carriedItem; i != -1; i = pickup->carriedItem) {
		pickup = &Items[i];
		pickup->pos.x = item->pos.x;
		pickup->pos.y = item->pos.y;
		pickup->pos.z = item->pos.z;
		ItemNewRoom(i, item->roomNumber);
	}
}

bool IsCreatureNearTarget(ITEM_INFO* item, ITEM_INFO* enemy, int distance)
{
	return ABS(item->pos.x - enemy->pos.x) < distance &&
		   ABS(item->pos.y - enemy->pos.y) < distance &&
		   ABS(item->pos.z - enemy->pos.z) < distance;
}

bool DamageTarget(ITEM_INFO* item, ITEM_INFO* enemy, const BITE_INFO* bite, int damage)
{
	if (enemy)
	{
		enemy->hitPoints -= damage;
		enemy->hitStatus = TRUE;
		CreatureEffect(item, bite, DoBloodSplat);
		return true;
	}
	return false;
}

bool DamageLaraOrEnemy(ITEM_INFO* item, ITEM_INFO* enemy, const BITE_INFO* bite, int damageLara, int damageEnemy, bool touchBitsLara)
{
	if (enemy != NULL)
	{
		if (enemy->objectID == ID_LARA && touchBitsLara)
		{
			DamageTarget(item, enemy, bite, damageLara);
			return true;
		}
		else if (IsCreatureNearTarget(item, enemy))
		{
			DamageTarget(item, enemy, bite, damageEnemy);
			return true;
		}
	}
	return false;
}

void SetAnimation(ITEM_INFO* item, int animID, int stateID, int frameID)
{
	item->animNumber = Objects[item->objectID].animIndex + animID;
	item->frameNumber = Anims[item->animNumber].frameBase + frameID;
	item->goalAnimState = stateID;
	item->currentAnimState = stateID;
}

void SetAnimationWithObject(ITEM_INFO* item, GAME_OBJECT_ID fromObjectIndex, int animID, int stateID, int frameID)
{
	item->animNumber = Objects[fromObjectIndex].animIndex + animID;
	item->frameNumber = Anims[item->animNumber].frameBase + frameID;
	item->goalAnimState = stateID;
	item->currentAnimState = stateID;
}

/*
 * Inject function
 */
void Inject_Box() {
	INJECT(0x0040E190, InitialiseCreature);
	INJECT(0x0040E1C0, CreatureActive);
	INJECT(0x0040E210, CreatureAIInfo);
	//INJECT(0x0040E470, SearchLOT);
	//INJECT(0x0040E670, UpdateLOT);
	INJECT(0x0040E6E0, TargetBox);
	//INJECT(0x0040E780, StalkBox);
	//INJECT(0x0040E880, EscapeBox);
	INJECT(0x0040E930, ValidBox);
	INJECT(0x0040E9E0, CreatureMood);
	//INJECT(0x0040EE50, CalculateTarget);
	//INJECT(0x0040F2B0, CreatureCreature);
	INJECT(0x0040F3B0, BadFloor);
	INJECT(0x0040F440, CreatureDie);
	//INJECT(0x0040F500, CreatureAnimation);
	INJECT(0x0040FDD0, CreatureTurn);
	//INJECT(0x0040FEB0, CreatureTilt);
	//INJECT(0x0040FEF0, CreatureHead);
	//INJECT(0x0040FF40, CreatureNeck);
	//INJECT(0x0040FF90, CreatureFloat);
	//INJECT(0x00410040, CreatureUnderwater);
	//INJECT(0x00410090, CreatureEffect);
	INJECT(0x004100F0, CreatureVault);
	INJECT(0x00410230, CreatureKill);
	INJECT(0x004103A0, GetBaddieTarget);
}