/*
 * Copyright (c) 2017-2024 Michael Chaban. All rights reserved.
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
#include "game/collide.h"
#include "3dsystem/3d_gen.h"
#include "3dsystem/phd_math.h"
#include "game/draw.h"
#include "game/items.h"
#include "game/control.h"
#include "game/sphere.h"
#include "game/sound.h"
#include "global/vars.h"

void GetCollisionInfo(COLL_INFO* coll, int x, int y, int z, short roomID, int height) {
	int head, top, h, c, frontX, frontZ, leftX, leftZ, rightX, rightZ;
	FLOOR_INFO* floor;
	short tilt;

	coll->collType = 0;
	coll->shift.z = 0;
	coll->shift.y = 0;
	coll->shift.x = 0;
	head = y - height;
	top = head - 160;
	coll->quadrant = (short)GetOrientAxis(coll->facing);
	floor = GetFloor(x, top, z, &roomID);
	h = GetHeight(floor, x, top, z);
	if (h != NO_HEIGHT)
		h -= y;
	c = GetCeiling(floor, x, top, z);
	if (c != NO_HEIGHT)
		c -= head;
	coll->sideMid.floor = h;
	coll->sideMid.ceiling = c;
	coll->sideMid.type = HeightType;
	coll->trigger = TriggerPtr;
	// NOTE: They added lara pos Y there instead of the argument y because it seem like lara could slide on bridge ?
	tilt = GetTiltType(floor, x, LaraItem->pos.y, z);
	coll->xTilt = (char)(tilt);
	coll->zTilt = (char)(tilt >> 8);
	switch (coll->quadrant) {
	case NORTH:
		frontX = coll->radius * phd_sin(coll->facing) >> W2V_SHIFT;
		frontZ = coll->radius;
		leftX = -coll->radius;
		leftZ = coll->radius;
		rightX = coll->radius;
		rightZ = coll->radius;
		break;
	case EAST:
		frontX = coll->radius;
		frontZ = coll->radius * phd_cos(coll->facing) >> W2V_SHIFT;
		leftX = coll->radius;
		leftZ = coll->radius;
		rightX = coll->radius;
		rightZ = -coll->radius;
		break;
	case SOUTH:
		frontX = coll->radius * phd_sin(coll->facing) >> W2V_SHIFT;
		frontZ = -coll->radius;
		leftX = coll->radius;
		leftZ = -coll->radius;
		rightX = -coll->radius;
		rightZ = -coll->radius;
		break;
	case WEST:
		frontX = -coll->radius;
		frontZ = coll->radius * phd_cos(coll->facing) >> W2V_SHIFT;
		rightZ = coll->radius;
		leftX = -coll->radius;
		leftZ = -coll->radius;
		rightX = -coll->radius;
		break;
	default:
		frontZ = 0;
		frontX = 0;
		leftZ = 0;
		leftX = 0;
		rightZ = 0;
		rightX = 0;
		break;
	}

	floor = GetFloor(x + frontX, top, z + frontZ, &roomID);
	h = GetHeight(floor, x + frontX, top, z + frontZ);
	if (h != NO_HEIGHT)
		h -= y;
	c = GetCeiling(floor, x + frontX, top, z + frontZ);
	if (c != NO_HEIGHT)
		c -= head;

	coll->sideFront.floor = h;
	coll->sideFront.ceiling = c;
	coll->sideFront.type = HeightType;
	if (CHK_ANY(coll->flags, CF_SLOPE_ARE_WALLS) && coll->sideFront.type == HT_BIG_SLOPE && coll->sideFront.floor < 0) {
		coll->sideFront.floor = -32767;
	}
	else {
		if ((CHK_ANY(coll->flags, CF_SLOPE_ARE_PITS) &&
			coll->sideFront.type == HT_BIG_SLOPE &&
			coll->sideFront.floor > 0) || (CHK_ANY(coll->flags, CF_LAVA_IS_PIT) && coll->sideFront.floor > 0 && TriggerPtr != NULL && (TriggerPtr[0] & DATA_TYPE) == FT_LAVA))
			coll->sideFront.floor = 512;
	}
	floor = GetFloor(x + leftX, top, z + leftZ, &roomID);
	h = GetHeight(floor, x + leftX, top, z + leftZ);
	if (h != NO_HEIGHT)
		h -= y;
	c = GetCeiling(floor, x + leftX, top, z + leftZ);
	if (c != NO_HEIGHT)
		c -= head;
	coll->sideLeft.ceiling = c;
	coll->sideLeft.floor = h;
	coll->sideLeft.type = HeightType;
	if (CHK_ANY(coll->flags, CF_SLOPE_ARE_WALLS) && coll->sideLeft.type == HT_BIG_SLOPE && coll->sideLeft.floor < 0) {
		coll->sideLeft.floor = -32767;
	}
	else {
		if ((CHK_ANY(coll->flags, CF_SLOPE_ARE_PITS) &&
			coll->sideLeft.type == HT_BIG_SLOPE &&
			coll->sideLeft.floor > 0) ||
			(CHK_ANY(coll->flags, CF_LAVA_IS_PIT) &&
				coll->sideLeft.floor > 0 &&
				TriggerPtr &&
				(*TriggerPtr & DATA_TYPE) == FT_LAVA))
			coll->sideLeft.floor = 512;
	}
	floor = GetFloor(x + rightX, top, z + rightZ, &roomID);
	h = GetHeight(floor, x + rightX, top, z + rightZ);
	if (h != NO_HEIGHT)
		h -= y;
	c = GetCeiling(floor, x + rightX, top, z + rightZ);
	if (c != NO_HEIGHT)
		c -= head;
	coll->sideRight.ceiling = c;
	coll->sideRight.floor = h;
	coll->sideRight.type = HeightType;
	if (CHK_ANY(coll->flags, CF_SLOPE_ARE_WALLS) && coll->sideRight.type == HT_BIG_SLOPE && coll->sideRight.floor < 0) {
		coll->sideRight.floor = -32767;
	}
	else {
		if ((CHK_ANY(coll->flags, CF_SLOPE_ARE_PITS) &&
			coll->sideRight.type == HT_BIG_SLOPE &&
			coll->sideRight.floor > 0) ||
			(CHK_ANY(coll->flags, CF_LAVA_IS_PIT) &&
				coll->sideRight.floor > 0 &&
				TriggerPtr &&
				(*TriggerPtr & DATA_TYPE) == FT_LAVA))
			coll->sideRight.floor = 512;
	}
	if (CollideStaticObjects(coll, x, y, z, roomID, height)) {
		floor = GetFloor(x + coll->shift.x, y, z + coll->shift.z, &roomID);
		if (GetHeight(floor, x + coll->shift.x, y, z + coll->shift.z) < y - 512 ||
			GetCeiling(floor, x + coll->shift.x, y, z + coll->shift.z) > head)
		{
			coll->shift.x = -coll->shift.x;
			coll->shift.z = -coll->shift.z;
		}
	}
	if (coll->sideMid.floor == NO_HEIGHT) {
		coll->shift.y = coll->old.y - y;
		coll->shift.x = coll->old.x - x;
		coll->shift.z = coll->old.z - z;
		coll->collType = COLL_FRONT;
	}
	else {
		if (coll->sideMid.floor - coll->sideMid.ceiling <= 0) {
			coll->shift.y = coll->old.y - y;
			coll->shift.x = coll->old.x - x;
			coll->shift.z = coll->old.z - z;
			coll->collType = COLL_CLAMP;
		}
		else {
			if (coll->sideMid.ceiling >= 0) {
				coll->shift.y = coll->sideMid.ceiling;
				coll->collType = COLL_TOP;
			}
			if (coll->sideFront.floor <= coll->badPos &&
				coll->sideFront.floor >= coll->badNeg &&
				coll->sideFront.ceiling <= coll->badCeiling)
			{
				if (coll->sideFront.ceiling >= coll->badCeiling)
				{
					coll->shift.y = coll->old.y - y;
					coll->shift.x = coll->old.x - x;
					coll->shift.z = coll->old.z - z;
					coll->collType = COLL_TOPFRONT;
				}
				else {
					if (coll->sideLeft.floor <= coll->badPos && coll->sideLeft.floor >= coll->badNeg) {
						if (coll->sideRight.floor > coll->badPos || coll->sideRight.floor < coll->badNeg) {
							switch (coll->quadrant) {
							case 0:
							case 2:
								coll->shift.x = FindGridShift(x + rightX, x + frontX);
								break;
							case 1:
							case 3:
								coll->shift.z = FindGridShift(z + rightZ, z + frontZ);
								break;
							}
							coll->collType = COLL_RIGHT;
						}
					}
					else {
						switch (coll->quadrant) {
						case 0:
						case 2:
							coll->shift.x = FindGridShift(x + leftX, x + frontX);
							break;
						case 1:
						case 3:
							coll->shift.z = FindGridShift(z + leftZ, z + frontZ);
							break;
						}
						coll->collType = COLL_LEFT;
					}
				}
			}
			else {
				switch (coll->quadrant) {
				case 0:
				case 2:
					coll->shift.x = coll->old.x - x;
					coll->shift.z = FindGridShift(z + frontZ, z);
					break;
				case 1:
				case 3:
					coll->shift.x = FindGridShift(x + frontX, x);
					coll->shift.z = coll->old.z - z;
					break;
				}
				coll->collType = COLL_FRONT;
			}
		}
	}
}

int FindGridShift(int src, int dest)
{
	int srcw = src >> WALL_SHIFT;
	int destw = dest >> WALL_SHIFT;
	if (srcw == destw)
		return 0;
	src &= (WALL_SIZE - 1);
	if (destw <= srcw)
		return -1 - src;
	return (WALL_SIZE + 1) - src;
}

int CollideStaticObjects(COLL_INFO* coll, int x, int y, int z, short roomID, int hite) {
	int rxMin = x - coll->radius;
	int rxMax = x + coll->radius;
	int ryMin = y - hite;
	int ryMax = y;
	int rzMin = z - coll->radius;
	int rzMax = z + coll->radius;

	coll->hitStatic = FALSE;
	GetNearByRooms(x, y, z, coll->radius + 50, hite + 50, roomID);

	// outer loop
	for (int i = 0; i < DrawRoomsCount; ++i) {
		ROOM_INFO* room = &Rooms[DrawRoomsArray[i]];
		for (int j = 0; j < room->numMeshes; ++j) {
			MESH_INFO* mesh = &room->mesh[j];
			if (CHK_ANY(StaticObjects[mesh->staticNumber].flags, 1)) {
				continue;
			}

			STATIC_BOUNDS* bounds = &StaticObjects[mesh->staticNumber].collisionBounds;
			// If bounds is empty then skip it !
			if (bounds->xMin == 0 && bounds->xMax == 0 &&
				bounds->yMin == 0 && bounds->yMax == 0 &&
				bounds->zMin == 0 && bounds->zMax == 0)
				continue;
			int yMin = mesh->y + bounds->yMin;
			int yMax = mesh->y + bounds->yMax;
			int xMin = mesh->x;
			int xMax = mesh->x;
			int zMin = mesh->z;
			int zMax = mesh->z;

			switch (mesh->yRot) {
			case -PHD_90: // west
				xMin -= bounds->zMax;
				xMax -= bounds->zMin;
				zMin += bounds->xMin;
				zMax += bounds->xMax;
				break;
			case -PHD_180: // south
				xMin -= bounds->xMax;
				xMax -= bounds->xMin;
				zMin -= bounds->zMax;
				zMax -= bounds->zMin;
				break;
			case PHD_90: // east
				xMin += bounds->zMin;
				xMax += bounds->zMax;
				zMin -= bounds->xMax;
				zMax -= bounds->xMin;
				break;
			default: // north
				xMin += bounds->xMin;
				xMax += bounds->xMax;
				zMin += bounds->zMin;
				zMax += bounds->zMax;
				break;
			}

			if (rxMax <= xMin || rxMin >= xMax ||
				ryMax <= yMin || ryMin >= yMax ||
				rzMax <= zMin || rzMin >= zMax)
			{
				continue;
			}
			int shift[2]{};

			shift[0] = rxMax - xMin;
			shift[1] = xMax - rxMin;
			int xShift = (shift[0] < shift[1]) ? -shift[0] : shift[1];

			shift[0] = rzMax - zMin;
			shift[1] = zMax - rzMin;
			int zShift = (shift[0] < shift[1]) ? -shift[0] : shift[1];

			switch (coll->quadrant) {
			case 0: // north
				if (xShift > coll->radius || xShift < -coll->radius) {
					coll->shift.x = coll->old.x - x;
					coll->shift.z = zShift;
					coll->collType = COLL_FRONT;
				}
				else if (xShift > 0) {
					coll->shift.x = xShift;
					coll->shift.z = 0;
					coll->collType = COLL_LEFT;
				}
				else if (xShift < 0) {
					coll->shift.x = xShift;
					coll->shift.z = 0;
					coll->collType = COLL_RIGHT;
				}
				break;

			case 1: // east
				if (zShift > coll->radius || zShift < -coll->radius) {
					coll->shift.x = xShift;
					coll->shift.z = coll->old.z - z;
					coll->collType = COLL_FRONT;
				}
				else if (zShift > 0) {
					coll->shift.x = 0;
					coll->shift.z = zShift;
					coll->collType = COLL_RIGHT;
				}
				else if (zShift < 0) {
					coll->shift.x = 0;
					coll->shift.z = zShift;
					coll->collType = COLL_LEFT;
				}
				break;

			case 2: // south
				if (xShift > coll->radius || xShift < -coll->radius) {
					coll->shift.x = coll->old.x - x;
					coll->shift.z = zShift;
					coll->collType = COLL_FRONT;
				}
				else if (xShift > 0) {
					coll->shift.x = xShift;
					coll->shift.z = 0;
					coll->collType = COLL_RIGHT;
				}
				else if (xShift < 0) {
					coll->shift.x = xShift;
					coll->shift.z = 0;
					coll->collType = COLL_LEFT;
				}
				break;

			case 3: // west
				if (zShift > coll->radius || zShift < -coll->radius) {
					coll->shift.x = xShift;
					coll->shift.z = coll->old.z - z;
					coll->collType = COLL_FRONT;
				}
				else if (zShift > 0) {
					coll->shift.x = 0;
					coll->shift.z = zShift;
					coll->collType = COLL_LEFT;
				}
				else if (zShift < 0) {
					coll->shift.x = 0;
					coll->shift.z = zShift;
					coll->collType = COLL_RIGHT;
				}
				break;
			}

			coll->hitStatic = 1;
			return 1;
		}
	}
	return 0;
}

void GetNearByRooms(int x, int y, int z, int r, int h, short roomID) {
	DrawRoomsArray[0] = roomID;
	DrawRoomsCount = 1;
	GetNewRoom(x + r, y, z + r, roomID);
	GetNewRoom(x - r, y, z + r, roomID);
	GetNewRoom(x + r, y, z - r, roomID);
	GetNewRoom(x - r, y, z - r, roomID);
	GetNewRoom(x + r, y - h, z + r, roomID);
	GetNewRoom(x - r, y - h, z + r, roomID);
	GetNewRoom(x + r, y - h, z - r, roomID);
	GetNewRoom(x - r, y - h, z - r, roomID);
}

void GetNewRoom(int x, int y, int z, short roomID) {
	GetFloor(x, y, z, &roomID);
	for (int i = 0; i < DrawRoomsCount; ++i) {
		if (DrawRoomsArray[i] == roomID) {
			return;
		}
	}
	DrawRoomsArray[DrawRoomsCount++] = roomID;
}

void ShiftItem(ITEM_INFO* item, COLL_INFO* coll)
{
	item->pos.x += coll->shift.x;
	item->pos.y += coll->shift.y;
	item->pos.z += coll->shift.z;
	coll->shift.x = 0;
	coll->shift.y = 0;
	coll->shift.z = 0;
}

void UpdateLaraRoom(ITEM_INFO* item, int height)
{
	int x = item->pos.x;
	int y = item->pos.y + height;
	int z = item->pos.z;
	short roomNumber = item->roomNumber;
	item->floor = GetHeight(GetFloor(x, y, z, &roomNumber), x, y, z);
	if (item->roomNumber != roomNumber)
		ItemNewRoom(Lara.item_number, roomNumber);
}

short GetTiltType(FLOOR_INFO* floor, int x, int y, int z)
{
	while (floor->pitRoom != NO_ROOM)
		floor = GetFloorSector(x, z, &Rooms[floor->pitRoom]);

	if (y + CLICK(1) < (floor->floor << 8))
		return 0;

	if (floor->index)
	{
		short* data = &FloorData[floor->index];
		if ((data[0] & 0xFF) == FT_TILT)
			return data[1];
	}

	return 0;
}

void LaraBaddieCollision(ITEM_INFO* laraitem, COLL_INFO* coll)
{
	ITEM_INFO* targetItem;
	OBJECT_INFO* obj;
	int	i, x, y, z;
	short roomList[20], roomCount;
	short targetItemNumber;

	Lara.hit_direction = -1;
	laraitem->hitStatus = FALSE;
	if (laraitem->hitPoints <= 0)
		return;

	memset(roomList, 0, sizeof(roomList));
	roomList[0] = laraitem->roomNumber;
	roomCount = 1;

	ROOM_INFO* room = &Rooms[laraitem->roomNumber];
	if (room->doors != NULL && room->doors->wCount > 0)
	{
		for (int i = 0; i < room->doors->wCount; ++i) {
			DOOR_INFO* door = &room->doors->door[i];
			if (door->room != NO_ROOM)
				roomList[roomCount++] = door->room;
		}
	}

	for (i = 0; i < roomCount; i++)
	{
		targetItemNumber = Rooms[roomList[i]].itemNumber;
		while (targetItemNumber != -1)
		{
			targetItem = &Items[targetItemNumber];
			if (targetItem->collidable && targetItem->status != ITEM_INVISIBLE)
			{
				obj = &Objects[targetItem->objectID];
				if (obj->collision)
				{
					x = laraitem->pos.x - targetItem->pos.x;
					y = laraitem->pos.y - targetItem->pos.y;
					z = laraitem->pos.z - targetItem->pos.z;
					if (x > -BLOCK(4) && x < BLOCK(4) &&
						z > -BLOCK(4) && z < BLOCK(4) &&
						y > -BLOCK(4) && y < BLOCK(4))
						obj->collision(targetItemNumber, laraitem, coll);
				}
			}
			targetItemNumber = targetItem->nextItem;
		}
	}

	if (Lara.spaz_effect_count != 0)
		EffectSpaz(laraitem, coll);
	if (Lara.hit_direction == -1)
		Lara.hit_frame = 0;
	InventoryChosen = -1;
}

void EffectSpaz(ITEM_INFO* item, COLL_INFO* coll)
{
	int x = Lara.spaz_effect->pos.x - LaraItem->pos.x;
	int z = Lara.spaz_effect->pos.z - LaraItem->pos.z;
	Lara.hit_direction = (short)GetOrientAxis(LaraItem->pos.rotY - (ANGLE(180) + phd_atan(z, x)));
	if (!Lara.hit_frame)
		PlaySoundEffect(31, &LaraItem->pos, NULL);
	Lara.hit_frame++;
	if (Lara.hit_frame > 34)
		Lara.hit_frame = 34;
	Lara.spaz_effect_count--;
}

void CreatureCollision(short itemNumber, ITEM_INFO* laraItem, COLL_INFO* coll)
{
	ITEM_INFO* item = &Items[itemNumber];
	if (!TestBoundsCollide(item, laraItem, coll->radius))
		return;
	if (!TestCollision(item, laraItem))
		return;
	if (CHK_ANY(coll->flags, CF_ENABLE_BADDIE_PUSH) && Lara.water_status != LWS_Underwater && Lara.water_status != LWS_Surface)
		ItemPushLara(item, laraItem, coll, CHK_ANY(coll->flags, CF_ENABLE_SPAZ), FALSE);
}

void ObjectCollision(short itemNumber, ITEM_INFO* laraItem, COLL_INFO* coll)
{
	ITEM_INFO* item = &Items[itemNumber];
	if (!TestBoundsCollide(item, laraItem, coll->radius))
		return;
	if (!TestCollision(item, laraItem))
		return;
	if (CHK_ANY(coll->flags, CF_ENABLE_BADDIE_PUSH))
		ItemPushLara(item, laraItem, coll, FALSE, TRUE);
}

void DoorCollision(short itemNumber, ITEM_INFO* laraItem, COLL_INFO* coll)
{
	ITEM_INFO* item = &Items[itemNumber];
	if (!TestBoundsCollide(item, laraItem, coll->radius))
		return;
	if (!TestCollision(item, laraItem))
		return;
	if (CHK_ANY(coll->flags, CF_ENABLE_BADDIE_PUSH))
	{
		if (item->currentAnimState != item->goalAnimState)
			ItemPushLara(item, laraItem, coll, CHK_ANY(coll->flags, CF_ENABLE_SPAZ), TRUE);
		else
			ItemPushLara(item, laraItem, coll, FALSE, TRUE);
	}
}

void TrapCollision(short itemNumber, ITEM_INFO* laraItem, COLL_INFO* coll)
{
	ITEM_INFO* item = &Items[itemNumber];
	if (item->status == ITEM_ACTIVE)
	{
		if (!TestBoundsCollide(item, laraItem, coll->radius))
			return;
		TestCollision(item, laraItem);
	}
	else if (item->status != ITEM_INVISIBLE)
	{
		ObjectCollision(itemNumber, laraItem, coll);
	}
}

void ItemPushLara(ITEM_INFO* item, ITEM_INFO* laraItem, COLL_INFO* coll, BOOL isSpazEnabled, BOOL isPushEnabled)
{
	OBJECT_INFO* obj = &Objects[item->objectID];
	int x = laraItem->pos.x - item->pos.x;
	int z = laraItem->pos.z - item->pos.z;
	int c = phd_cos(item->pos.rotY);
	int s = phd_sin(item->pos.rotY);
	int rx = (c * x - s * z) >> W2V_SHIFT;
	int rz = (c * z + s * x) >> W2V_SHIFT;

	short* bounds = GetBestFrame(item);
	short minx = bounds[0];
	short maxx = bounds[1];
	short minz = bounds[4];
	short maxz = bounds[5];

	if (isPushEnabled)
	{
		minx -= coll->radius;
		maxx += coll->radius;
		minz -= coll->radius;
		maxz += coll->radius;
	}

	if (rx >= minx && rx <= maxx && rz >= minz && rz <= maxz)
	{
		int l = rx - minx;
		int r = maxx - rx;
		int t = maxz - rz;
		int b = rz - minz;

		if (l <= r && l <= t && l <= b)
			rx -= l;
		else if (r <= l && r <= t && r <= b)
			rx += r;
		else if (t <= l && t <= r && t <= b)
			rz += t;
		else
			rz -= b;

		int ax = (c * rx + s * rz) >> W2V_SHIFT;
		int az = (c * rz - s * rx) >> W2V_SHIFT;
		laraItem->pos.x = item->pos.x + ax;
		laraItem->pos.z = item->pos.z + az;

		rx = (*(bounds + 0) + *(bounds + 1)) / 2;
		rz = (*(bounds + 4) + *(bounds + 5)) / 2;
		x -= (c * rx + s * rz) >> W2V_SHIFT;
		z -= (c * rz - s * rx) >> W2V_SHIFT;
		if (isSpazEnabled && (bounds[3] - bounds[2]) > CLICK(1))
		{
			Lara.hit_direction = (short)GetOrientAxis(laraItem->pos.rotY - (ANGLE(180) + phd_atan(z, x)));
			if (!Lara.hit_frame)
				PlaySoundEffect(31, &laraItem->pos, 0);
			Lara.hit_frame++;
			if (Lara.hit_frame > 34)
				Lara.hit_frame = 34;
		}

		coll->badPos = -NO_HEIGHT;
		coll->badNeg = -384;
		coll->badCeiling = 0;

		short oldfacing = coll->facing;
		coll->facing = phd_atan((laraItem->pos.z - coll->old.z), (laraItem->pos.x - coll->old.x));
		GetCollisionInfo(coll, laraItem->pos.x, laraItem->pos.y, laraItem->pos.z, laraItem->roomNumber, CLICK(3));
		coll->facing = oldfacing;

		if (coll->collType != 0)
		{
			laraItem->pos.x = coll->old.x;
			laraItem->pos.z = coll->old.z;
		}
		else
		{
			coll->old.x = laraItem->pos.x;
			coll->old.y = laraItem->pos.y;
			coll->old.z = laraItem->pos.z;
			UpdateLaraRoom(laraItem, -10);
		}
	}
}

BOOL TestBoundsCollide(ITEM_INFO* item, ITEM_INFO* laraItem, int radius)
{
	OBJECT_INFO* obj = &Objects[item->objectID];
	OBJECT_INFO* objLara = &Objects[laraItem->objectID];
	short* bounds = GetBestFrame(item);
	short* larabounds = GetBestFrame(laraItem);
	if (item->pos.y + bounds[3] <= laraItem->pos.y + larabounds[2] || item->pos.y + bounds[2] >= laraItem->pos.y + larabounds[3])
		return FALSE;
	int c = phd_cos(item->pos.rotY);
	int s = phd_sin(item->pos.rotY);
	int x = laraItem->pos.x - item->pos.x;
	int z = laraItem->pos.z - item->pos.z;
	int rx = (c * x - s * z) >> W2V_SHIFT;
	int rz = (c * z + s * x) >> W2V_SHIFT;
	int minx = bounds[0] - radius;
	int maxx = bounds[1] + radius;
	int minz = bounds[4] - radius;
	int maxz = bounds[5] + radius;
	return rx >= minx && rx <= maxx && rz >= minz && rz <= maxz;
}

BOOL TestLaraPosition(short* bounds, ITEM_INFO* item, ITEM_INFO* laraitem)
{
	short xrotrel = laraitem->pos.rotX - item->pos.rotX;
	short yrotrel = laraitem->pos.rotY - item->pos.rotY;
	short zrotrel = laraitem->pos.rotZ - item->pos.rotZ;

	if (xrotrel < bounds[6] || xrotrel > bounds[7] ||
		yrotrel < bounds[8] || yrotrel > bounds[9] ||
		zrotrel < bounds[10] || zrotrel > bounds[11])
		return FALSE;

	int x = laraitem->pos.x - item->pos.x;
	int y = laraitem->pos.y - item->pos.y;
	int z = laraitem->pos.z - item->pos.z;

	phd_PushUnitMatrix();
	phd_RotYXZ(item->pos.rotY, item->pos.rotX, item->pos.rotZ);
	PHD_MATRIX* mptr = PhdMatrixPtr;
	int rx = ((mptr->_00 * x + mptr->_01 * y + mptr->_02 * z) >> W2V_SHIFT);
	int ry = ((mptr->_10 * x + mptr->_11 * y + mptr->_12 * z) >> W2V_SHIFT);
	int rz = ((mptr->_20 * x + mptr->_21 * y + mptr->_22 * z) >> W2V_SHIFT);
	phd_PopMatrix();

	if (rx < (int)bounds[0] || rx > (int)bounds[1] ||
		ry < (int)bounds[2] || ry > (int)bounds[3] ||
		rz < (int)bounds[4] || rz > (int)bounds[5])
		return FALSE;

	return TRUE;
}

void AlignLaraPosition(PHD_VECTOR* vec, ITEM_INFO* item, ITEM_INFO* laraitem)
{
	laraitem->pos.rotX = item->pos.rotX;
	laraitem->pos.rotY = item->pos.rotY;
	laraitem->pos.rotZ = item->pos.rotZ;

	phd_PushUnitMatrix();
	phd_RotYXZ(item->pos.rotY, item->pos.rotX, item->pos.rotZ);
	PHD_MATRIX* mptr = PhdMatrixPtr;
	int x = item->pos.x + ((mptr->_00 * vec->x + mptr->_01 * vec->y + mptr->_02 * vec->z) >> W2V_SHIFT);
	int y = item->pos.y + ((mptr->_10 * vec->x + mptr->_11 * vec->y + mptr->_12 * vec->z) >> W2V_SHIFT);
	int z = item->pos.z + ((mptr->_20 * vec->x + mptr->_21 * vec->y + mptr->_22 * vec->z) >> W2V_SHIFT);
	phd_PopMatrix();

	short roomNumber = laraitem->roomNumber;
	FLOOR_INFO* floor = GetFloor(x, y, z, &roomNumber);
	int height = GetHeight(floor, x, y, z);
	int ceiling = GetCeiling(floor, x, y, z);
	if (ABS(height - laraitem->pos.y) > CLICK(1) ||
		ABS(ceiling - laraitem->pos.y) < CLICK(3))
		return;

	laraitem->pos.x = x;
	laraitem->pos.y = y;
	laraitem->pos.z = z;
}

BOOL MoveLaraPosition(PHD_VECTOR* vec, ITEM_INFO* item, ITEM_INFO* laraItem)
{
	PHD_3DPOS dest = {};
	dest.rotX = item->pos.rotX;
	dest.rotY = item->pos.rotY;
	dest.rotZ = item->pos.rotZ;
	phd_PushUnitMatrix();
	phd_RotYXZ(item->pos.rotY, item->pos.rotX, item->pos.rotZ);
	PHD_MATRIX* mptr = PhdMatrixPtr;
	dest.x = item->pos.x + ((mptr->_00 * vec->x + mptr->_01 * vec->y + mptr->_02 * vec->z) >> W2V_SHIFT);
	dest.y = item->pos.y + ((mptr->_10 * vec->x + mptr->_11 * vec->y + mptr->_12 * vec->z) >> W2V_SHIFT);
	dest.z = item->pos.z + ((mptr->_20 * vec->x + mptr->_21 * vec->y + mptr->_22 * vec->z) >> W2V_SHIFT);
	phd_PopMatrix();

	if (item->objectID == ID_FLARE_ITEM)
	{
		short roomNumber = laraItem->roomNumber;
		int height = GetHeight(GetFloor(dest.x, dest.y, dest.z, &roomNumber), dest.x, dest.y, dest.z);
		if (ABS(height - laraItem->pos.y) > CLICK(2))
		{
			return 0;
		}
		else
		{
			int x = dest.x - laraItem->pos.x;
			int y = dest.y - laraItem->pos.y;
			int z = dest.z - laraItem->pos.z;
			int dist = phd_sqrt(SQR(x) + SQR(y) + SQR(z));
			if (dist < HALF_CLICK(1))
				return 1;
		}
	}

	return Move3DPosTo3DPos(&laraItem->pos, &dest, 16, ANGLE(2));
}

BOOL Move3DPosTo3DPos(PHD_3DPOS* srcpos, PHD_3DPOS* destpos, int velocity, short angadd)
{
	int x = destpos->x - srcpos->x;
	int y = destpos->y - srcpos->y;
	int z = destpos->z - srcpos->z;
	int dist = phd_sqrt(SQR(x) + SQR(y) + SQR(z));

	if (velocity >= dist)
	{
		srcpos->x = destpos->x;
		srcpos->y = destpos->y;
		srcpos->z = destpos->z;
	}
	else
	{
		srcpos->x += (x * velocity) / dist;
		srcpos->y += (y * velocity) / dist;
		srcpos->z += (z * velocity) / dist;
	}

	short angdif = destpos->rotX - srcpos->rotX;
	if (angdif > angadd)
		srcpos->rotX += angadd;
	else if (angdif < -angadd)
		srcpos->rotX -= angadd;
	else
		srcpos->rotX = destpos->rotX;

	angdif = destpos->rotY - srcpos->rotY;
	if (angdif > angadd)
		srcpos->rotY += angadd;
	else if (angdif < -angadd)
		srcpos->rotY -= angadd;
	else
		srcpos->rotY = destpos->rotY;

	angdif = destpos->rotZ - srcpos->rotZ;
	if (angdif > angadd)
		srcpos->rotZ += angadd;
	else if (angdif < -angadd)
		srcpos->rotZ -= angadd;
	else
		srcpos->rotZ = destpos->rotZ;

	return srcpos->x == destpos->x &&
		   srcpos->y == destpos->y &&
		   srcpos->z == destpos->z &&
		   srcpos->rotX == destpos->rotX &&
		   srcpos->rotY == destpos->rotY &&
		   srcpos->rotZ == destpos->rotZ;
}

// NOTE: This fix the elevator not working in all direction.
bool IsCollidingOnFloorLift(int x, int z, int ix, int iz, short angle)
{
	x >>= WALL_SHIFT;
	z >>= WALL_SHIFT;
	ix >>= WALL_SHIFT;
	iz >>= WALL_SHIFT;
	// Same block as elevator position.
	if (x == ix && z == iz)
		return true;
	switch (FROM_ANGLE(ABS(angle)))
	{
	case 0: // NORTH
		// Now check for east (+X)
		if (x == ix + 1 && z == iz)
			return true;
		// Now check for south (-Z)
		if (x == ix && z == iz - 1)
			return true;
		// Now check for east+south (+X and -Z)
		if (x == ix + 1 && z == iz - 1)
			return true;
		break;
	case 90: // EAST
		// Now check for south (-Z)
		if (x == ix && z == iz - 1)
			return true;
		// Now check for west (-X)
		if (x == ix - 1 && z == iz)
			return true;
		// Now check for west and south (-Z and -X)
		if (x == ix - 1 && z == iz - 1)
			return true;
		break;
	case 180: // SOUTH
		// Now check for west (-X)
		if (x == ix - 1 && z == iz)
			return true;
		// Now check for north (+Z)
		if (x == ix && z == iz + 1)
			return true;
		// Now check for north and west (+Z and -X)
		if (x == ix - 1 && z == iz + 1)
			return true;
		break;
	case 270: // WEST
		// Now check for north (+Z)
		if (x == ix && z == iz + 1)
			return true;
		// Now check for east (+X)
		if (x == ix + 1 && z == iz)
			return true;
		// Now check for north and east (+Z and +X)
		if (x == ix + 1 && z == iz + 1)
			return true;
		break;
	}
	return false;
}

/*
 * Inject function
 */
void Inject_Collide() {
	INJECT(0x004128D0, GetCollisionInfo);
	INJECT(0x00412F90, FindGridShift);
	INJECT(0x00412FC0, CollideStaticObjects);
	INJECT(0x004133B0, GetNearByRooms);
	INJECT(0x00413480, GetNewRoom);
	INJECT(0x004134E0, ShiftItem);
	INJECT(0x00413520, UpdateLaraRoom);
	INJECT(0x00413580, GetTiltType);
	INJECT(0x00413620, LaraBaddieCollision);
	INJECT(0x004137C0, EffectSpaz);
	INJECT(0x00413840, CreatureCollision);
	INJECT(0x004138C0, ObjectCollision);
	INJECT(0x00413920, DoorCollision);
	INJECT(0x004139A0, TrapCollision);
	INJECT(0x00413A10, ItemPushLara);
	INJECT(0x00413D20, TestBoundsCollide);
	INJECT(0x00413DF0, TestLaraPosition);
	INJECT(0x00413F30, AlignLaraPosition);
	INJECT(0x00414070, MoveLaraPosition);
	INJECT(0x00414200, Move3DPosTo3DPos);
}