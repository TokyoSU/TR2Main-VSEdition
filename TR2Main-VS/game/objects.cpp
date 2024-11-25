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
#include "game/objects.h"
#include "3dsystem/phd_math.h"
#include "game/control.h"
#include "game/draw.h"
#include "game/effects.h"
#include "game/items.h"
#include "game/missile.h"
#include "game/sound.h"
#include "game/sphere.h"
#include "specific/game.h"
#include "specific/init.h"
#include "global/vars.h"

void BigBowlControl(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];
	if (item->currentAnimState == 1)
	{
		short fxNum = CreateEffect(item->roomNumber);
		if (fxNum != -1)
		{
			FX_INFO* fx = &Effects[fxNum];
			fx->objectID = ID_HOT_LIQUID;
			fx->pos.x = item->pos.x + CLICK(2);
			fx->pos.y = item->pos.y + 612;
			fx->pos.z = item->pos.z + CLICK(2);
			fx->roomNumber = item->roomNumber;
			fx->frameNumber = Objects[fx->objectID].nMeshes * GetRandomDraw() >> 15;
			fx->fallspeed = 0;
			fx->shade = 0x800;
		}
		item->timer++;
		if (item->timer == 150)
		{
			FlipMaps[4] = IFL_CODEBITS|IFL_ONESHOT;                     // CODEBITS|ONESHOT
			if (!FlipStatus)
				FlipMap();
		}
	}
	AnimateItem(item);
	if (item->status == ITEM_DISABLED && item->timer >= (FRAMES_PER_SECOND * 7)) // 7 seconds
		RemoveActiveItem(itemNumber);
}

void BellControl(short itemNumber) {
	ITEM_INFO* item = &Items[itemNumber];
	item->goalAnimState = 1;
	item->floor = GetHeight(GetFloor(item->pos.x, item->pos.y, item->pos.z, &item->roomNumber), item->pos.x, item->pos.y, item->pos.z);
	TestTriggers(TriggerPtr, TRUE);
	AnimateItem(item);
	if (!item->currentAnimState) {
		item->status = ITEM_INACTIVE;
		RemoveActiveItem(itemNumber);
	}
}

void InitialiseWindow(short itemNumber) {
	ITEM_INFO* item;
	ROOM_INFO* room;
	BOX_INFO* box;

	item = &Items[itemNumber];
	item->flags = 0;
	item->meshBits = 1;
	room = &RoomInfo[item->roomNumber];
	box = &Boxes[room->floor[((item->pos.x - room->x) >> WALL_SHIFT) * room->xSize + ((item->pos.z - room->z) >> WALL_SHIFT)].box];
	if (CHK_ANY(box->overlapIndex, 0x8000))
		box->overlapIndex |= 0x4000;
}

void SmashWindow(short itemNumber) {
	ITEM_INFO* item;
	ROOM_INFO* room;
	BOX_INFO* box;

	item = &Items[itemNumber];
	room = &RoomInfo[item->roomNumber];
	box = &Boxes[room->floor[((item->pos.x - room->x) >> WALL_SHIFT) * room->xSize + ((item->pos.z - room->z) >> WALL_SHIFT)].box];
	if (CHK_ANY(box->overlapIndex, 0x8000))
		box->overlapIndex &= ~0x4000;
	PlaySoundEffect(58, &item->pos, 0);
	item->collidable = 0;
	item->meshBits = 0xFFFE;
	ExplodingDeath(itemNumber, 0xFEFE, 0);
	item->flags |= IFL_ONESHOT;
	if (item->status == ITEM_ACTIVE)
		RemoveActiveItem(itemNumber);
	item->status = ITEM_DISABLED;
}

void WindowControl(short itemNumber) {
	ITEM_INFO* item;
	int val;

	item = &Items[itemNumber];
	if (!CHK_ANY(item->flags, IFL_ONESHOT)) {
		if (Lara.skidoo == -1) {
			if (item->touchBits) {
				item->touchBits = 0;
				val = phd_cos(LaraItem->pos.rotY - item->pos.rotY) * LaraItem->speed >> W2V_SHIFT;
				if (ABS(val) >= 50)
					SmashWindow(itemNumber);
			}
		}
		else {
			if (ItemNearLara(&item->pos, 512))
				SmashWindow(itemNumber);
		}
	}
}

void OpenNearestDoor()
{
	for (short itemNumber = 0; itemNumber < LevelItemCount; itemNumber++) {
		ITEM_INFO* item = &Items[itemNumber];
		int dx = (item->pos.x - LaraItem->pos.x);
		int dy = (item->pos.y - LaraItem->pos.y);
		int dz = (item->pos.z - LaraItem->pos.z);
		int dist = SQR(dx) + SQR(dy) + SQR(dz);
		if (dist > SQR(WALL_SIZE * 2))
			continue;
		if (item->objectID < ID_DOOR_TYPE1 || item->objectID > ID_TRAPDOOR_TYPE3)
			continue;

		if (!item->active) {
			AddActiveItem(itemNumber);
			item->flags |= IFL_CODEBITS;
		}
		else if (item->flags & IFL_CODEBITS) {
			item->flags &= ~IFL_CODEBITS;
		}
		else {
			item->flags |= IFL_CODEBITS;
		}
		item->timer = 0;
		item->touchBits = 0;
	}
}

constexpr auto LIFT_FLOOR = 16;
constexpr auto LIFT_FLOOR_NEXT = (BLOCK(5) + CLICK(2)) - LIFT_FLOOR;
constexpr auto LIFT_SPEED = 16;
constexpr auto LIFT_TIMER = 90; // Before the door close.
constexpr auto LIFT_CLOSE = 0; // State
constexpr auto LIFT_OPEN = 1;

static LIFT_DATA* GetLiftData(ITEM_INFO* item)
{
	return (LIFT_DATA*)item->data;
}

void InitialiseLift(short itemNumber) {
	ITEM_INFO* item = &Items[itemNumber];
	item->data = (LIFT_DATA*)game_malloc(sizeof(LIFT_DATA), GBUF_TempAlloc);
	LIFT_DATA* lift = GetLiftData(item);
	lift->oldY = item->pos.y;
	lift->timer = 0;
}

void LiftControl(short itemNumber) {
	ITEM_INFO* item = &Items[itemNumber];
	LIFT_DATA* lift = GetLiftData(item);

	if (TriggerActive(item)) {
		if (item->pos.y < (lift->oldY + LIFT_FLOOR_NEXT)) {
			if (lift->timer < LIFT_TIMER) {
				item->goalAnimState = LIFT_OPEN;
				++lift->timer;
			}
			else {
				item->goalAnimState = LIFT_CLOSE;
				item->pos.y += LIFT_SPEED;
			}
		}
		else {
			item->goalAnimState = LIFT_OPEN;
			lift->timer = 0;
		}
	}
	else {
		if (item->pos.y > (lift->oldY + LIFT_FLOOR)) {
			if (lift->timer < LIFT_TIMER) {
				item->goalAnimState = LIFT_OPEN;
				lift->timer++;
			}
			else {
				item->goalAnimState = LIFT_CLOSE;
				item->pos.y -= LIFT_SPEED;
			}
		}
		else {
			item->goalAnimState = LIFT_OPEN;
			lift->timer = 0;
		}
	}

	AnimateItem(item);
	short roomID = item->roomNumber;
	GetFloor(item->pos.x, item->pos.y, item->pos.z, &roomID);
	if (item->roomNumber != roomID)
		ItemNewRoom(itemNumber, roomID);
}

void LiftFloorCeiling(ITEM_INFO* item, int x, int y, int z, int* floor, int* ceiling) {
	int liftX, liftZ, laraX, laraZ;
	BOOL inside;

	liftX = item->pos.x >> WALL_SHIFT;
	liftZ = item->pos.z >> WALL_SHIFT;
	laraX = LaraItem->pos.x >> WALL_SHIFT;
	laraZ = LaraItem->pos.z >> WALL_SHIFT;
	inside = (x >> WALL_SHIFT == liftX || (x >> WALL_SHIFT) + 1 == liftX) && (z >> WALL_SHIFT == liftZ || (z >> WALL_SHIFT) - 1 == liftZ);
	*floor = NO_HEIGHT;
	*ceiling = NO_HEIGHT;
	if ((laraX != liftX && laraX + 1 != liftX) || (laraZ != liftZ && laraZ - 1 != liftZ)) {
		if (inside) {
			if (y <= item->pos.y - 1280) {
				*floor = item->pos.y - 1280;
			}
			else {
				if (y < item->pos.y + 256) {
					if (!item->currentAnimState) {
						*floor = NO_HEIGHT;
						*ceiling = NO_HEIGHT;
					}
					else {
						*floor = item->pos.y;
						*ceiling = item->pos.y - 1024;
					}
				}
				else {
					*ceiling = item->pos.y + 256;
				}
			}
		}
	}
	else {
		if (!item->currentAnimState && LaraItem->pos.y < item->pos.y + 256 && LaraItem->pos.y > item->pos.y - 1024) {
			if (inside) {
				*floor = item->pos.y;
				*ceiling = item->pos.y - 1024;
			}
			else {
				*floor = NO_HEIGHT;
				*ceiling = NO_HEIGHT;
			}
		}
		else {
			if (inside) {
				if (LaraItem->pos.y < item->pos.y - 1024) {
					*floor = item->pos.y - 1280;
				}
				else {
					if (LaraItem->pos.y < item->pos.y + 256) {
						*floor = item->pos.y;
						*ceiling = item->pos.y - 1024;
					}
					else {
						*ceiling = item->pos.y + 256;
					}
				}
			}
		}
	}
}

void LiftFloor(ITEM_INFO* item, int x, int y, int z, int* height) {
	int floor, ceiling;
	LiftFloorCeiling(item, x, y, z, &floor, &ceiling);
	if (ceiling == NO_HEIGHT) return;
	if (floor < *height)
		*height = floor;
}

void LiftCeiling(ITEM_INFO* item, int x, int y, int z, int* height) {
	int floor, ceiling;
	LiftFloorCeiling(item, x, y, z, &floor, &ceiling);
	if (ceiling == NO_HEIGHT) return;
	if (ceiling > *height)
		*height = ceiling;
}

void GeneralControl(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];
	if (TriggerActive(item))
		item->goalAnimState = DOOR_OPEN;
	else
		item->goalAnimState = DOOR_CLOSED;
	AnimateItem(item);

	short roomNumber = item->roomNumber;
	GetFloor(item->pos.x, item->pos.y, item->pos.z, &roomNumber);
	if (roomNumber != item->roomNumber)
		ItemNewRoom(itemNumber, roomNumber);

	// NOTE: This code is actually for the MiniSub
	// This caused the DRAW_BRIDGE to also have a light which dont make sense !
	if (item->objectID == ID_GENERAL)
	{
		PHD_VECTOR pos = {};
		pos.x = 3000;
		pos.y = 720;
		pos.z = 0;
		GetJointAbsPosition(item, &pos, 0);
		AddDynamicLight(pos.x, pos.y, pos.z, 14, 12);
		if (item->status == ITEM_DISABLED)
		{
			RemoveActiveItem(itemNumber);
			item->flags |= IFL_ONESHOT;
		}
	}
}

 /*
  * Inject function
  */
void Inject_Objects() {
	//INJECT(0x004342C0, EarthQuake);
	//INJECT(0x004343A0, ControlCutShotgun);
	//INJECT(0x004343E0, InitialiseFinalLevel);
	//INJECT(0x004344B0, FinalLevelCounter); // TODO: Decompile it for savegame !
	//INJECT(0x004346C0, MiniCopterControl);
	//INJECT(0x004347A0, InitialiseDyingMonk);
	//INJECT(0x00434820, DyingMonk);
	//INJECT(0x004348B0, ControlGongBonger);
	//INJECT(0x00434970, DeathSlideCollision);
	//INJECT(0x00434A30, ControlDeathSlide);
	INJECT(0x00434CC0, BigBowlControl);
	INJECT(0x00434DB0, BellControl);
	INJECT(0x00434E30, InitialiseWindow);
	INJECT(0x00434EB0, SmashWindow);
	INJECT(0x00434F80, WindowControl);
	//INJECT(0x00435020, SmashIceControl);
	//INJECT(0x00435100, ShutThatDoor);
	//INJECT(0x00435150, OpenThatDoor);
	//INJECT(0x00435190, InitialiseDoor);
	//INJECT(0x00435570, DoorControl);
	//INJECT(0x00435640, OnDrawBridge);
	//INJECT(0x00435700, DrawBridgeFloor);
	//INJECT(0x00435740, DrawBridgeCeiling);
	//INJECT(0x00435780, DrawBridgeCollision);
	INJECT(0x004357B0, InitialiseLift);
	INJECT(0x004357F0, LiftControl);
	INJECT(0x004358D0, LiftFloorCeiling);
	INJECT(0x00435A50, LiftFloor);
	INJECT(0x00435A90, LiftCeiling);
	//INJECT(0x00435AD0, BridgeFlatFloor);
	//INJECT(0x00435AF0, BridgeFlatCeiling);
	//INJECT(0x00435B10, GetOffset);
	//INJECT(0x00435B50, BridgeTilt1Floor);
	//INJECT(0x00435B80, BridgeTilt1Ceiling);
	//INJECT(0x00435BC0, BridgeTilt2Floor);
	//INJECT(0x00435BF0, BridgeTilt2Ceiling);
	//INJECT(0x00435C30, CopterControl);
	INJECT(0x00435D40, GeneralControl);
	//INJECT(0x00435E20, DetonatorControl);
}