/*
 * Copyright (c) 2017-2020 Michael Chaban. All rights reserved.
 * Copyright (c) 2020 ChocolateFan <asasas9500@gmail.com>
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
#include "game/traps.h"
#include "3dsystem/phd_math.h"
#include "game/collide.h"
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

#ifdef FEATURE_INPUT_IMPROVED
#include "modding/joy_output.h"
#endif // FEATURE_INPUT_IMPROVED

constexpr auto SPIKE_DAMAGE = 15;

void MineControl(short mineID) {
	ITEM_INFO* mine = &Items[mineID];
	if (CHK_ANY(mine->flags, IFL_ONESHOT)) {
		return;
	}

	if (!MinesDetonated) {
		short roomNumber = mine->roomNumber;
		GetFloor(mine->pos.x, mine->pos.y - 0x800, mine->pos.z, &roomNumber);

		ITEM_INFO* item = NULL;
		short itemNumber = Rooms[roomNumber].itemNumber;
		for (; itemNumber >= 0; itemNumber = item->nextItem) {
			item = &Items[itemNumber];
			if (item->objectID == ID_BOAT) {
				int x = item->pos.x - mine->pos.x;
				int y = item->pos.z - mine->pos.z;
				if (SQR(x) + SQR(y) < SQR(0x200)) {
					break;
				}
			}
		}

		if (itemNumber < 0) {
			return;
		}

		if (Lara.skidoo == itemNumber) {
			ExplodingDeath(Lara.item_number, ~0, 0);
			LaraItem->hitPoints = 0;
			LaraItem->flags |= IFL_ONESHOT;
		}
		item->objectID = ID_BOAT_BITS;
		ExplodingDeath(itemNumber, ~0, 0);
		KillItem(itemNumber);
		item->objectID = ID_BOAT;

		FLOOR_INFO* floor = GetFloor(mine->pos.x, mine->pos.y, mine->pos.z, &roomNumber);
		GetHeight(floor, mine->pos.x, mine->pos.y, mine->pos.z);
		TestTriggers(TriggerPtr, 1);
		MinesDetonated = 1;
	}
	else if (GetRandomControl() < 0x7800) {
		return;
	}

	short fxID = CreateEffect(mine->roomNumber);
	if (fxID != -1)
	{
		FX_INFO* fx = &Effects[fxID];
		fx->pos.x = mine->pos.x;
		fx->pos.y = mine->pos.y - 0x400;
		fx->pos.z = mine->pos.z;
		fx->speed = 0;
		fx->frameNumber = 0;
		fx->counter = 0;
		fx->objectID = ID_EXPLOSION;
	}

	Splash(mine);
	PlaySoundEffect(105, &mine->pos, 0);
	mine->flags |= IFL_ONESHOT;
	mine->collidable = 0;
	mine->meshBits = 1;
#ifdef FEATURE_INPUT_IMPROVED
	JoyRumbleExplode(mine->pos.x, mine->pos.y, mine->pos.z, 0x2800, false);
#endif // FEATURE_INPUT_IMPROVED
}

void ControlSpikeWall(short itemNumber) {
	ITEM_INFO* item;
	int x, z;
	short roomID;

	item = &Items[itemNumber];
	if (TriggerActive(item) && item->status != ITEM_DISABLED) {
		z = item->pos.z + (16 * phd_cos(item->pos.rotY) >> W2V_SHIFT);
		x = item->pos.x + (16 * phd_sin(item->pos.rotY) >> W2V_SHIFT);
		roomID = item->roomNumber;
		if (GetHeight(GetFloor(x, item->pos.y, z, &roomID), x, item->pos.y, z) != item->pos.y) {
			item->status = ITEM_DISABLED;
		}
		else {
			item->pos.z = z;
			item->pos.x = x;
			if (roomID != item->roomNumber)
				ItemNewRoom(itemNumber, roomID);
		}
		PlaySoundEffect(204, &item->pos, 0);
	}
	if (item->touchBits) {
		LaraItem->hitPoints -= 20;
		LaraItem->hitStatus = 1;
		DoLotsOfBlood(LaraItem->pos.x, LaraItem->pos.y - 512, LaraItem->pos.z, 1, item->pos.rotY, LaraItem->roomNumber, 3);
		item->touchBits = 0;
		PlaySoundEffect(205, &item->pos, 0);
	}
}

void ControlCeilingSpikes(short itemNumber) {
	ITEM_INFO* item;
	int y;
	short roomID;

	item = &Items[itemNumber];
	if (TriggerActive(item) && item->status != ITEM_DISABLED) {
		y = item->pos.y + 5;
		roomID = item->roomNumber;
		if (GetHeight(GetFloor(item->pos.x, y, item->pos.z, &roomID), item->pos.x, y, item->pos.z) < y + 1024) {
			item->status = ITEM_DISABLED;
		}
		else {
			item->pos.y = y;
			if (roomID != item->roomNumber)
				ItemNewRoom(itemNumber, roomID);
		}
		PlaySoundEffect(204, &item->pos, 0);
	}
	if (item->touchBits) {
		LaraItem->hitPoints -= 20;
		LaraItem->hitStatus = 1;
		DoLotsOfBlood(LaraItem->pos.x, item->pos.y + 768, LaraItem->pos.z, 1, item->pos.rotY, LaraItem->roomNumber, 3);
		item->touchBits = 0;
		PlaySoundEffect(205, &item->pos, 0);
	}
}

void HookControl(short itemNumber) {
	static BOOL IsHookHit = FALSE;

	ITEM_INFO* item = &Items[itemNumber];
	if (item->touchBits != 0 && !IsHookHit) {
		LaraItem->hitPoints -= 50;
		LaraItem->hitStatus = 1;
		IsHookHit = TRUE;
		DoLotsOfBlood(LaraItem->pos.x, LaraItem->pos.y - CLICK(2), LaraItem->pos.z, LaraItem->speed, LaraItem->pos.rotY, LaraItem->roomNumber, 3);
	}
	else {
		IsHookHit = FALSE;
	}

	AnimateItem(item);
}

void PropellerControl(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];

	if (TriggerActive(item) && !(item->flags & IFL_ONESHOT))
	{
		item->goalAnimState = 0;
		if (item->touchBits & 6)
		{
			LaraItem->hitPoints -= 200;
			LaraItem->hitStatus = TRUE;
			DoLotsOfBlood(LaraItem->pos.x, LaraItem->pos.y - CLICK(2), LaraItem->pos.z, GetRandomDraw() >> 10, item->pos.rotY + CLICK(180), LaraItem->roomNumber, 3);
			if (item->objectID == ID_PROPELLER1)
				PlaySoundEffect(207, &item->pos, NULL);
		}
		else
		{
			switch (item->objectID)
			{
			case ID_PROPELLER2:
				PlaySoundEffect(206, &item->pos, NULL);
				break;
			case ID_PROPELLER1:
				PlaySoundEffect(213, &item->pos, NULL);
				break;
			case ID_PROPELLER3:
				PlaySoundEffect(215, &item->pos, SFX_UNDERWATER);
				break;
			default:
				PlaySoundEffect(217, &item->pos, NULL);
				break;
			}
		}
	}
	else if (item->goalAnimState != 1)
	{
		if (item->objectID == ID_PROPELLER1)
			PlaySoundEffect(213, &item->pos, NULL);
		else if (item->objectID == ID_PROPELLER3)
			PlaySoundEffect(216, &item->pos, SFX_UNDERWATER);
		item->goalAnimState = 1;
	}

	AnimateItem(item);
	if (item->status == ITEM_DISABLED)
	{
		RemoveActiveItem(itemNumber);
		if (item->objectID != ID_PROPELLER2)
			item->collidable = FALSE;
	}
}

void SpinningBlade(short itemNumber) {
	ITEM_INFO* item;
	int x, z;
	short roomID;
	BOOL reverse;

	item = &Items[itemNumber];
	if (item->currentAnimState == 2) {
		if (item->goalAnimState != 1) {
			z = item->pos.z + (1536 * phd_cos(item->pos.rotY) >> W2V_SHIFT);
			x = item->pos.x + (1536 * phd_sin(item->pos.rotY) >> W2V_SHIFT);
			roomID = item->roomNumber;
			if (GetHeight(GetFloor(x, item->pos.y, z, &roomID), x, item->pos.y, z) == NO_HEIGHT)
				item->goalAnimState = 1;
		}
		reverse = TRUE;
		if (item->touchBits) {
			LaraItem->hitStatus = 1;
			LaraItem->hitPoints -= 100;
			DoLotsOfBlood(LaraItem->pos.x, LaraItem->pos.y - CLICK(2), LaraItem->pos.z, 2 * item->speed, LaraItem->pos.rotY, LaraItem->roomNumber, 2);
		}
		PlaySoundEffect(231, &item->pos, 0);
	}
	else {
		if (TriggerActive(item))
			item->goalAnimState = 2;
		reverse = FALSE;
	}
	AnimateItem(item);
	roomID = item->roomNumber;
	item->pos.y = GetHeight(GetFloor(item->pos.x, item->pos.y, item->pos.z, &roomID), item->pos.x, item->pos.y, item->pos.z);
	item->floor = item->pos.y;
	if (roomID != item->roomNumber)
		ItemNewRoom(itemNumber, roomID);
	if (reverse && item->currentAnimState == 1)
		item->pos.rotY += PHD_180;
}

#define ICICLE_FALLSPEED 50
#define ICICLE_DAMAGE 200
#define ICICLE_FULLMESH (MESHBITS_GET(1) | MESHBITS_GET(2) | MESHBITS_GET(3) | MESHBITS_GET(4) | MESHBITS_GET(5) |  MESHBITS_GET(6))
#define ICICLE_NOTIPMESH (MESHBITS_GET(1) | MESHBITS_GET(3) | MESHBITS_GET(5))

void InitialiseIcicle(short itemNumber) {
	ITEM_INFO* item = &Items[itemNumber];
	item->data = game_malloc(sizeof(GAME_VECTOR), GBUF_TempAlloc);
	GAME_VECTOR* oldPos = (GAME_VECTOR*)item->data;
	oldPos->x = item->pos.x;
	oldPos->y = item->pos.y;
	oldPos->z = item->pos.z;
	oldPos->roomNumber = item->roomNumber;
}

void IcicleControl(short itemNumber) {
	ITEM_INFO* item = &Items[itemNumber];
	GAME_VECTOR* oldPos = (GAME_VECTOR*)item->data;

	if (TriggerActive(item))
	{
		switch (item->currentAnimState) {
		case 1:
			item->goalAnimState = 2;
			break;
		case 2:
			if (!item->gravity) {
				item->fallSpeed = ICICLE_FALLSPEED;
				item->gravity = TRUE;
			}
			if (item->touchBits != 0) {
				LaraItem->hitPoints -= ICICLE_DAMAGE;
				LaraItem->hitStatus = TRUE;
			}
			break;
		case 3:
			item->gravity = FALSE;
			break;
		}

		AnimateItem(item);

		short roomID = item->roomNumber;
		FLOOR_INFO* floor = GetFloor(item->pos.x, item->pos.y, item->pos.z, &roomID);
		if (item->roomNumber != roomID)
			ItemNewRoom(itemNumber, roomID);
		item->floor = GetHeight(floor, item->pos.x, item->pos.y, item->pos.z);
		if (item->currentAnimState == 2 && item->pos.y >= item->floor) {
			item->gravity = 0;
			item->goalAnimState = 3;
			item->pos.y = item->floor;
			item->fallSpeed = 0;
			item->meshBits = ICICLE_NOTIPMESH;
		}
	}
	else // Reset position back to original
	{
		item->gravity = FALSE;
		item->animNumber = Objects[item->objectID].animIndex;
		item->frameNumber = Anims[item->animNumber].frameBase;
		item->currentAnimState = 1;
		item->pos.x = oldPos->x;
		item->pos.y = oldPos->y;
		item->pos.z = oldPos->z;
		if (oldPos->roomNumber != item->roomNumber)
			ItemNewRoom(itemNumber, oldPos->roomNumber);
		item->meshBits = ICICLE_FULLMESH;
	}
}

void InitialiseBlade(short itemNumber) {
	ITEM_INFO* item = &Items[itemNumber];
	item->animNumber = Objects[ID_BLADE].animIndex + 2;
	item->currentAnimState = 1;
	item->frameNumber = Anims[item->animNumber].frameBase;
}

void BladeControl(short itemNumber) {
	ITEM_INFO* item = &Items[itemNumber];
	if (TriggerActive(item) && item->currentAnimState == 1) {
		item->goalAnimState = 2;
	}
	else {
		item->goalAnimState = 1;
	}
	if (CHK_ANY(item->touchBits, 2) && item->currentAnimState == 2) {
		LaraItem->hitStatus = 1;
		LaraItem->hitPoints -= 100;
		DoLotsOfBlood(LaraItem->pos.x, item->pos.y - CLICK(1), LaraItem->pos.z, LaraItem->speed, LaraItem->pos.rotY, LaraItem->roomNumber, 2);
	}
	AnimateItem(item);
}

void InitialiseKillerStatue(short itemNumber) {
	ITEM_INFO* item = &Items[itemNumber];
	item->animNumber = Objects[item->objectID].animIndex + 3;
	item->currentAnimState = 1;
	item->frameNumber = Anims[item->animNumber].frameBase;
}

void KillerStatueControl(short itemNumber) {
	ITEM_INFO* item = &Items[itemNumber];
	if (TriggerActive(item) && item->currentAnimState == 1) {
		item->goalAnimState = 2;
	}
	else {
		item->goalAnimState = 1;
	}
	if (CHK_ANY(item->touchBits, 0x80) && item->currentAnimState == 2) {
		LaraItem->hitPoints -= 20;
		LaraItem->hitStatus = 1;
		DoBloodSplat(LaraItem->pos.x + (GetRandomControl() - 16384) / 256,
			LaraItem->pos.y - GetRandomControl() / 44,
			LaraItem->pos.z + (GetRandomControl() - 16384) / 256,
			LaraItem->speed,
			LaraItem->pos.rotY + (GetRandomControl() - 16384) / 8,
			LaraItem->roomNumber);
	}
	AnimateItem(item);
}

void SpringBoardControl(short itemNumber) {
	ITEM_INFO* item = &Items[itemNumber];
	if (item->currentAnimState == 0 && item->pos.y == LaraItem->pos.y &&
		(LaraItem->pos.x >> WALL_SHIFT) == (item->pos.x >> WALL_SHIFT) &&
		(LaraItem->pos.z >> WALL_SHIFT) == (item->pos.z >> WALL_SHIFT) &&
		LaraItem->hitPoints > 0)
	{
		if (LaraItem->currentAnimState == AS_BACK || LaraItem->currentAnimState == AS_FASTBACK)
			LaraItem->speed = -LaraItem->speed;
		LaraItem->fallSpeed = -240;
		LaraItem->gravity = 1;
		LaraItem->animNumber = 34;
		LaraItem->frameNumber = Anims[LaraItem->animNumber].frameBase;
		LaraItem->currentAnimState = AS_FORWARDJUMP;
		LaraItem->goalAnimState = AS_FORWARDJUMP;
		item->goalAnimState = 1;
	}
	AnimateItem(item);
}

void InitialiseRollingBall(short itemNumber) {
	ITEM_INFO* item = &Items[itemNumber];
	item->data = game_malloc(sizeof(GAME_VECTOR), GBUF_RollingBallStuff);
	GAME_VECTOR* oldPos = (GAME_VECTOR*)item->data;
	oldPos->x = item->pos.x;
	oldPos->y = item->pos.y;
	oldPos->z = item->pos.z;
	oldPos->roomNumber = item->roomNumber;
}

void RollingBallControl(short itemNumber) {
	int oldX, oldZ, distance, x, z;
	short roomID;
	FLOOR_INFO* floor;

	ITEM_INFO* item = &Items[itemNumber];
	if (item->status == ITEM_ACTIVE) {
		if (item->goalAnimState == 2) {
			AnimateItem(item);
		}
		else {
			if (item->pos.y < item->floor) {
				if (!item->gravity) {
					item->fallSpeed = -10;
					item->gravity = 1;
				}
			}
			else {
				if (!item->currentAnimState)
					item->goalAnimState = 1;
			}
			oldX = item->pos.x;
			oldZ = item->pos.z;
			AnimateItem(item);
			roomID = item->roomNumber;
			floor = GetFloor(item->pos.x, item->pos.y, item->pos.z, &roomID);
			if (item->roomNumber != roomID)
				ItemNewRoom(itemNumber, roomID);
			item->floor = GetHeight(floor, item->pos.x, item->pos.y, item->pos.z);
			TestTriggers(TriggerPtr, TRUE);
			if (item->pos.y >= item->floor - 256) {
				item->gravity = 0;
				item->pos.y = item->floor;
				item->fallSpeed = 0;
				if (item->objectID == ID_ROLLING_BALL2) {
					PlaySoundEffect(222, &item->pos, 0);
				}
				else {
					if (item->objectID == ID_ROLLING_BALL3) {
						PlaySoundEffect(227, &item->pos, 0);
					}
					else {
						PlaySoundEffect(147, &item->pos, 0);
					}
				}
				distance = phd_sqrt(SQR(Camera.micPos.x - item->pos.x) + SQR(Camera.micPos.z - item->pos.z));
				if (distance < 10240)
					Camera.bounce = 40 * (distance - 10240) / 10240;
			}
			distance = item->objectID == ID_ROLLING_BALL1 ? 384 : 1024;
			x = item->pos.x + (distance * phd_sin(item->pos.rotY) >> W2V_SHIFT);
			z = item->pos.z + (distance * phd_cos(item->pos.rotY) >> W2V_SHIFT);
			if (GetHeight(GetFloor(x, item->pos.y, z, &roomID), x, item->pos.y, z) < item->pos.y) {
				if (item->objectID == ID_ROLLING_BALL2) {
					PlaySoundEffect(223, &item->pos, 0);
					item->goalAnimState = 2;
				}
				else {
					if (item->objectID == ID_ROLLING_BALL3) {
						PlaySoundEffect(228, &item->pos, 0);
						item->goalAnimState = 2;
					}
					else {
						item->status = ITEM_DISABLED;
					}
				}
				item->pos.y = item->floor;
				item->pos.x = oldX;
				item->pos.z = oldZ;
				item->fallSpeed = 0;
				item->speed = 0;
				item->touchBits = 0;
			}
		}
	}
	else if (item->status == ITEM_DISABLED && !TriggerActive(item)) {
		GAME_VECTOR* oldPos = (GAME_VECTOR*)item->data;
		item->status = ITEM_INACTIVE;
		item->pos.x = oldPos->x;
		item->pos.y = oldPos->y;
		item->pos.z = oldPos->z;
		if (item->roomNumber != oldPos->roomNumber) {
			RemoveDrawnItem(itemNumber);
			item->nextItem = Rooms[oldPos->roomNumber].itemNumber;
			Rooms[oldPos->roomNumber].itemNumber = itemNumber;
			item->roomNumber = oldPos->roomNumber;
		}
		item->animNumber = Objects[item->objectID].animIndex;
		item->frameNumber = Anims[item->animNumber].frameBase;
		item->requiredAnimState = 0;
		item->goalAnimState = Anims[item->animNumber].currentAnimState;
		item->currentAnimState = item->goalAnimState;
		RemoveActiveItem(itemNumber);
	}
}

void RollingBallCollision(short itemNumber, ITEM_INFO* laraItem, COLL_INFO* coll) {
	ITEM_INFO* item;
	int dx, dy, dz, distance, i;

	item = &Items[itemNumber];
	if (item->status == ITEM_ACTIVE) {
		if (TestBoundsCollide(item, laraItem, coll->radius) && TestCollision(item, laraItem)) {
			if (laraItem->gravity) {
				if (CHK_ANY(coll->flags, CF_ENABLE_BADDIE_PUSH))
					ItemPushLara(item, laraItem, coll, CHK_ANY(coll->flags, CF_ENABLE_SPAZ), TRUE);
				laraItem->hitPoints -= 100;
				dx = laraItem->pos.x - item->pos.x;
				dy = laraItem->pos.y - item->pos.y + 162;
				dz = laraItem->pos.z - item->pos.z;
				distance = phd_sqrt(SQR(dx) + SQR(dy) + SQR(dz));
				if (distance < CLICK(2))
					distance = CLICK(2);
				DoBloodSplat(item->pos.x + (dx << WALL_SHIFT) / 2 / distance,
					item->pos.y + (dy << WALL_SHIFT) / 2 / distance - CLICK(2),
					item->pos.z + (dz << WALL_SHIFT) / 2 / distance,
					item->speed,
					item->pos.rotY,
					item->roomNumber);
			}
			else {
				laraItem->hitStatus = 1;
				if (laraItem->hitPoints > 0) {
					laraItem->hitPoints = -1;
					laraItem->pos.rotY = item->pos.rotY;
					laraItem->pos.rotZ = 0;
					laraItem->pos.rotX = 0;
					laraItem->animNumber = 139;
					laraItem->frameNumber = Anims[laraItem->animNumber].frameBase;
					laraItem->currentAnimState = AS_SPECIAL;
					laraItem->goalAnimState = AS_SPECIAL;
					Camera.flags = CFL_FollowCenter;
					Camera.targetAngle = ANGLE(170);
					Camera.targetElevation = -ANGLE(25);
					for (i = 0; i < 15; ++i)
						DoBloodSplat(laraItem->pos.x + (GetRandomControl() - 16384) / 256,
							laraItem->pos.y - GetRandomControl() / 64,
							laraItem->pos.z + (GetRandomControl() - 16384) / 256,
							2 * item->speed,
							item->pos.rotY + (GetRandomControl() - 16384) / 8,
							item->roomNumber);
				}
			}
		}
	}
	else {
		if (item->status != ITEM_INVISIBLE)
			ObjectCollision(itemNumber, laraItem, coll);
	}
}

void SpikeCollision(short itemNumber, ITEM_INFO* laraItem, COLL_INFO* coll)
{
	ITEM_INFO* item = &Items[itemNumber];
	if (laraItem->hitPoints < 0)
		return;

	if (!TestBoundsCollide(item, laraItem, coll->radius))
		return;
	if (!TestCollision(item, laraItem))
		return;

	int num = GetRandomControl() / 24576;
	if (laraItem->gravity)
	{
		if (laraItem->fallSpeed > 6)
		{
			laraItem->hitPoints = -1;
			num = 20;
		}
	}
	else if (laraItem->speed < 30)
	{
		return;
	}

	laraItem->hitPoints -= SPIKE_DAMAGE;
	for (; num > 0; num--)
	{
		int x = laraItem->pos.x + (GetRandomControl() - 16384) / 256;
		int y = laraItem->pos.y - GetRandomControl() / 64;
		int z = laraItem->pos.z + (GetRandomControl() - 16384) / 256;
		DoBloodSplat(x, y, z, 20, GetRandomControl() & (ANGLE(360) - 1), item->roomNumber);
	}

	if (laraItem->hitPoints <= 0)
	{
		laraItem->animNumber = 149; // Spike death animation.
		laraItem->frameNumber = Anims[laraItem->animNumber].frameBase;
		laraItem->currentAnimState = AS_DEATH;
		laraItem->goalAnimState = AS_DEATH;
		laraItem->pos.y = item->pos.y;
		laraItem->gravity = FALSE;
	}
}

void TrapDoorControl(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];
	if (TriggerActive(item))
	{
		if (item->currentAnimState == DOOR_CLOSED)
			item->goalAnimState = DOOR_OPEN;
	}
	else
	{
		if (item->currentAnimState == DOOR_OPEN)
			item->goalAnimState = DOOR_CLOSED;
	}
	AnimateItem(item);
}

void TrapDoorFloor(ITEM_INFO* item, int x, int y, int z, int* height)
{
	if (!OnTrapDoor(item, x, z))
		return;
	if (y <= item->pos.y)
	{
		if (item->currentAnimState == DOOR_CLOSED && item->pos.y < *height)
			*height = item->pos.y;
	}
}

void TrapDoorCeiling(ITEM_INFO* item, int x, int y, int z, int* height)
{
	if (!OnTrapDoor(item, x, z))
		return;
	if (y > item->pos.y)
	{
		if (item->currentAnimState == DOOR_CLOSED && item->pos.y > *height)
			*height = item->pos.y + CLICK(1);
	}
}

BOOL OnTrapDoor(ITEM_INFO* item, int x, int z)
{
	x >>= WALL_SHIFT;
	z >>= WALL_SHIFT;
	int ix = item->pos.x >> WALL_SHIFT;
	int iz = item->pos.z >> WALL_SHIFT;
	// Same block as the item position.
	// No need to calculate nearest block because trap is only 1 block.
	return x == ix && z == iz;
}

void Pendulum(short itemNumber) {
	ITEM_INFO* item = &Items[itemNumber];
	if (TriggerActive(item))
	{
		if (item->touchBits != 0) {
			LaraItem->hitPoints -= 50;
			LaraItem->hitStatus = 1;
			DoBloodSplat(LaraItem->pos.x + (GetRandomControl() - 16384) / 256,
				LaraItem->pos.y - GetRandomControl() / 44,
				LaraItem->pos.z + (GetRandomControl() - 16384) / 256,
				LaraItem->speed,
				LaraItem->pos.rotY + (GetRandomControl() - 16384) / 8,
				LaraItem->roomNumber);
		}
		item->floor = GetHeight(GetFloor(item->pos.x, item->pos.y, item->pos.z, &item->roomNumber), item->pos.x, item->pos.y, item->pos.z);
		AnimateItem(item);
	}
	else if (item->frameNumber != 0)
	{
		AnimateItem(item);
	}
}

void FallingBlock(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];
	if (item->currentAnimState == 0)
	{
		int origin = item->objectID == ID_FALLING_BLOCK3 ? BLOCK(1) : CLICK(2);
		int dx = LaraItem->pos.x - item->pos.x;
		int dz = LaraItem->pos.z - item->pos.z;
		// NOTE: There was no check for X and Z position, so if lara was at the same height
		// as the falling block, he just activate like if lara was above it.
		// This check avoid that !
		if (ABS(dx) <= CLICK(2) && ABS(dz) <= CLICK(2) && LaraItem->pos.y == item->pos.y - origin)
			item->goalAnimState = 1;
	}
	else if (item->currentAnimState == 1)
		item->goalAnimState = 2;
	else if (item->currentAnimState == 2 && item->goalAnimState != 3)
		item->gravity = TRUE;

	AnimateItem(item);
	if (item->status == ITEM_DISABLED)
	{
		RemoveActiveItem(itemNumber);
		return;
	}

	short room_number = item->roomNumber;
	FLOOR_INFO* floor = GetFloor(item->pos.x, item->pos.y, item->pos.z, &room_number);
	if (item->roomNumber != room_number)
		ItemNewRoom(itemNumber, room_number);

	item->floor = GetHeight(floor, item->pos.x, item->pos.y, item->pos.z);
	if (item->currentAnimState == 2 && item->pos.y >= item->floor)
	{
		item->goalAnimState = 3;
		item->pos.y = item->floor;
		item->fallSpeed = 0;
		item->gravity = FALSE;
	}
}

void FallingBlockFloor(ITEM_INFO* item, int x, int y, int z, int* height)
{
	int origin = item->objectID == ID_FALLING_BLOCK3 ? BLOCK(1) : CLICK(2);
	if (y <= item->pos.y - origin && item->currentAnimState <= 1)
		*height = item->pos.y - origin;
}

void FallingBlockCeiling(ITEM_INFO* item, int x, int y, int z, int* height)
{
	int origin = item->objectID == ID_FALLING_BLOCK3 ? BLOCK(1) : CLICK(2);
	if (y > item->pos.y - origin && item->currentAnimState <= 1)
		*height = item->pos.y + CLICK(1) - origin;
}

void TeethTrap(short itemNumber) {
	ITEM_INFO* item;
	static BITE_INFO Teeth[3][2] = {
		{{-23, 0, -1718, 0}, {71, 0, -1718, 1}},
		{{-23, 10, -1718, 0}, {71, 10, -1718, 1}},
		{{-23, -10, -1718, 0}, {71, -10, -1718, 1}}
	};

	item = &Items[itemNumber];
	if (TriggerActive(item)) {
		item->goalAnimState = 1;
		if (item->touchBits && item->currentAnimState == 1) {
			LaraItem->hitPoints -= 400;
			LaraItem->hitStatus = 1;
			BaddieBiteEffect(item, &Teeth[0][0]);
			BaddieBiteEffect(item, &Teeth[0][1]);
			BaddieBiteEffect(item, &Teeth[1][0]);
			BaddieBiteEffect(item, &Teeth[1][1]);
			BaddieBiteEffect(item, &Teeth[2][0]);
			BaddieBiteEffect(item, &Teeth[2][1]);
		}
	}
	else {
		item->goalAnimState = 0;
	}
	AnimateItem(item);
}

void FallingCeiling(short itemNumber) {
	ITEM_INFO* item;
	short roomID;

	item = &Items[itemNumber];
	if (!item->currentAnimState) {
		item->gravity = 1;
		item->goalAnimState = 1;
	}
	else {
		if (item->currentAnimState == 1 && item->touchBits) {
			LaraItem->hitPoints -= 300;
			LaraItem->hitStatus = 1;
		}
	}
	AnimateItem(item);
	if (item->status == ITEM_DISABLED) {
		RemoveActiveItem(itemNumber);
	}
	else {
		roomID = item->roomNumber;
		item->floor = GetHeight(GetFloor(item->pos.x, item->pos.y, item->pos.z, &roomID), item->pos.x, item->pos.y, item->pos.z);
		if (roomID != item->roomNumber)
			ItemNewRoom(itemNumber, roomID);
		if (item->currentAnimState == 1 && item->pos.y >= item->floor) {
			item->gravity = 0;
			item->goalAnimState = 2;
			item->pos.y = item->floor;
			item->fallSpeed = 0;
		}
	}
}

void DartEmitterControl(short itemNumber) {
	ITEM_INFO* item, * dynamic;
	short dynamicID;
	int dx, dz;

	item = &Items[itemNumber];
	if (TriggerActive(item)) {
		if (!item->currentAnimState)
			item->goalAnimState = 1;
	}
	else {
		if (item->currentAnimState == 1)
			item->goalAnimState = 0;
	}
	if (item->currentAnimState == 1 && item->frameNumber == Anims[item->animNumber].frameBase) {
		dynamicID = CreateItem();
		if (dynamicID != -1) {
			dynamic = &Items[dynamicID];
			dynamic->objectID = ID_DARTS;
			dynamic->roomNumber = item->roomNumber;
			dynamic->shade1 = -1;
			dynamic->pos.rotY = item->pos.rotY;
			dynamic->pos.y = item->pos.y - 512;
			dz = 0;
			dx = 0;
			if (dynamic->pos.rotY <= -PHD_90) {
				if (dynamic->pos.rotY == -PHD_90) {
					dx = 412;
				}
				else {
					if (dynamic->pos.rotY == -PHD_180)
						dz = 412;
				}
			}
			else {
				if (!dynamic->pos.rotY) {
					dz = -412;
				}
				else {
					if (dynamic->pos.rotY == PHD_90)
						dx = -412;
				}
			}
			dynamic->pos.x = item->pos.x + dx;
			dynamic->pos.z = item->pos.z + dz;
			InitialiseItem(dynamicID);
			AddActiveItem(dynamicID);
			dynamic->status = ITEM_ACTIVE;
			PlaySoundEffect(254, &dynamic->pos, 0);
		}
	}
	AnimateItem(item);
}

void DartsControl(short itemNumber) {
	ITEM_INFO* item;
	short roomID, fxID;
	FLOOR_INFO* floor;
	FX_INFO* fx;

	item = &Items[itemNumber];
	if (item->touchBits) {
		LaraItem->hitPoints -= 50;
		LaraItem->hitStatus = 1;
		DoBloodSplat(item->pos.x, item->pos.y, item->pos.z, LaraItem->speed, LaraItem->pos.rotY, LaraItem->roomNumber);
	}
	AnimateItem(item);
	roomID = item->roomNumber;
	floor = GetFloor(item->pos.x, item->pos.y, item->pos.z, &roomID);
	if (item->roomNumber != roomID)
		ItemNewRoom(itemNumber, roomID);
	item->floor = GetHeight(floor, item->pos.x, item->pos.y, item->pos.z);
	item->pos.rotX += PHD_45 / 2;
	if (item->pos.y >= item->floor) {
		KillItem(itemNumber);
		fxID = CreateEffect(item->roomNumber);
		if (fxID != -1) {
			fx = &Effects[fxID];
			fx->pos = item->pos;
			fx->speed = 0;
			fx->counter = 6;
			fx->objectID = ID_RICOCHET;
			fx->frameNumber = -3 * GetRandomControl() / 32768;
		}
		PlaySoundEffect(258, &item->pos, 0);
	}
}

void DartEffectControl(short fxID) {
	FX_INFO* fx;

	fx = &Effects[fxID];
	++fx->counter;
	if (fx->counter >= 3) {
		--fx->frameNumber;
		fx->counter = 0;
		if (fx->frameNumber <= Objects[fx->objectID].nMeshes)
			KillEffect(fxID);
	}
}

void FlameEmitterControl(short item_id) {
	ITEM_INFO* item;
	short fxID;
	FX_INFO* fx;

	item = &Items[item_id];
	if (TriggerActive(item)) {
		if (!item->data) {
			fxID = CreateEffect(item->roomNumber);
			if (fxID != -1) {
				fx = &Effects[fxID];
				fx->pos.x = item->pos.x;
				fx->pos.y = item->pos.y;
				fx->pos.z = item->pos.z;
				fx->frameNumber = 0;
				fx->objectID = ID_FLAME;
				fx->counter = 0;
			}
			item->data = (LPVOID)(fxID + 1);
		}
	}
	else {
		if (item->data) {
			KillEffect((int)item->data - 1);
			item->data = (LPVOID)0;
		}
	}
}

void FlameControl(short fx_id) {
	FX_INFO* fx = &Effects[fx_id];
	if (--fx->frameNumber <= Objects[ID_FLAME].nMeshes) {
		fx->frameNumber = 0;
	}
	if (fx->counter < 0) {
#ifdef FEATURE_CHEAT
		if (Lara.water_status == LWS_Cheat) {
			fx->counter = 0;
			KillEffect(fx_id);
			Lara.burn = 0;
			return;
		}
#endif // FEATURE_CHEAT
		fx->pos.x = 0;
		fx->pos.y = 0;
		fx->pos.z = (fx->counter == -1) ? -100 : 0;
		GetJointAbsPosition(LaraItem, (PHD_VECTOR*)&fx->pos, -1 - fx->counter);
		if (LaraItem->roomNumber != fx->roomNumber) {
			EffectNewRoom(fx_id, LaraItem->roomNumber);
		}
		int height = GetWaterHeight(fx->pos.x, fx->pos.y, fx->pos.z, fx->roomNumber);
		if (height != NO_HEIGHT && fx->pos.y > height) {
			fx->counter = 0;
			KillEffect(fx_id);
			Lara.burn = 0;
		}
		else {
			PlaySoundEffect(150, &fx->pos, 0);
			LaraItem->hitPoints -= 7;
			LaraItem->hitStatus = 1;
		}
	}
	else {
		PlaySoundEffect(150, &fx->pos, 0);
		if (fx->counter) {
			--fx->counter;
		}
		else if (ItemNearLara(&fx->pos, 600)) {
			LaraItem->hitPoints -= 5;
			LaraItem->hitStatus = 1;
			int dx = LaraItem->pos.x - fx->pos.x;
			int dz = LaraItem->pos.z - fx->pos.z;
			if (SQR(dx) + SQR(dz) < SQR(450)) {
				fx->counter = 100;
				LaraBurn();
			}
		}
	}
}

void LaraBurn() {
#ifdef FEATURE_CHEAT
	if (Lara.water_status == LWS_Cheat) {
		return;
	}
#endif // FEATURE_CHEAT
	if (Lara.burn) {
		return;
	}

	short fx_id = CreateEffect(LaraItem->roomNumber);
	if (fx_id < 0) {
		return;
	}
	FX_INFO* fx = &Effects[fx_id];
	fx->objectID = ID_FLAME;
	fx->frameNumber = 0;
	fx->counter = -1;
	Lara.burn = 1;
}

void LavaBurn(ITEM_INFO* item) {
#ifdef FEATURE_CHEAT
	if (Lara.water_status == LWS_Cheat) {
		return;
	}
#endif // FEATURE_CHEAT
	if (item->hitPoints < 0) {
		return;
	}

	short room_number = item->roomNumber;
	FLOOR_INFO* floor = GetFloor(item->pos.x, 32000, item->pos.z, &room_number);
	if (item->floor != GetHeight(floor, item->pos.x, 32000, item->pos.z)) {
		return;
	}

	item->hitStatus = 1;
	item->hitPoints = -1;
	for (int i = 0; i < 10; ++i) {
		short fx_id = CreateEffect(item->roomNumber);
		if (fx_id < 0) continue;
		FX_INFO* fx = &Effects[fx_id];
		fx->objectID = ID_FLAME;
		fx->frameNumber = Objects[ID_FLAME].nMeshes * GetRandomControl() / 0x7FFF;
		fx->counter = -1 - 24 * GetRandomControl() / 0x7FFF;
	}
}

void LavaSpray(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];
	short fxNum = CreateEffect(item->roomNumber);
	if (fxNum != -1)
	{
		FX_INFO* fx = &Effects[fxNum];
		fx->pos.x = item->pos.x;
		fx->pos.y = item->pos.y;
		fx->pos.z = item->pos.z;
		fx->pos.rotY = (GetRandomControl() - 0x4000) * 2;
		fx->speed = GetRandomControl() >> 10;
		fx->fallspeed = -GetRandomControl() / 200;
		fx->frameNumber = -GetRandomControl() * 4 / 0x7FFF;
		fx->objectID = ID_LAVA;
		PlaySoundEffect(149, &item->pos, 0);
	}
}

void ControlLavaBlob(short fxNum)
{
	FX_INFO* fx = &Effects[fxNum];

	fx->fallspeed += 6;
	fx->pos.x += fx->speed * phd_sin(fx->pos.rotY) >> W2V_SHIFT;
	fx->pos.y += fx->fallspeed;
	fx->pos.z += fx->speed * phd_cos(fx->pos.rotY) >> W2V_SHIFT;

	short newRoom = fx->roomNumber;
	FLOOR_INFO* floor = GetFloor(fx->pos.x, fx->pos.y, fx->pos.z, &newRoom);
	int height = GetHeight(floor, fx->pos.x, fx->pos.y, fx->pos.z);
	int ceiling = GetCeiling(floor, fx->pos.x, fx->pos.y, fx->pos.z);
	// NOTE: Bounce if it touch the ceiling, new feature !
	if (ceiling != NO_HEIGHT)
		ceiling += 128;
	if (fx->pos.y <= ceiling)
	{
		fx->speed /= 2;
		fx->fallspeed /= 2;
		fx->fallspeed = -fx->fallspeed;
	}

	if (fx->pos.y >= height)
	{
		KillEffect(fxNum);
		return;
	}

	if (ItemNearLara(&fx->pos, 200))
	{
		LaraItem->hitPoints -= 10;
		LaraItem->hitStatus = TRUE;
		KillEffect(fxNum);
		return;
	}

	if (newRoom != fx->roomNumber)
		EffectNewRoom(fxNum, newRoom);

}

static CANDLE_EMITTER_DATA* GetCandleFlameData(ITEM_INFO* item)
{
	return (CANDLE_EMITTER_DATA*)item->data;
}

void InitializeCandleFlameEmitter(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];
	item->data = (CANDLE_EMITTER_DATA*)game_malloc(sizeof(CANDLE_EMITTER_DATA), GBUF_TempAlloc);
}

static void SpawnCandleFlame(ITEM_INFO* item, int radius, short angle, int height, short* candleID)
{
	*candleID = CreateEffect(item->roomNumber);
	if (*candleID != -1) {
		FX_INFO* fx = &Effects[*candleID];
		fx->pos.x = item->pos.x + (radius * phd_sin(item->pos.rotY + angle) >> W2V_SHIFT);
		fx->pos.y = item->pos.y + height;
		fx->pos.z = item->pos.z + (radius * phd_cos(item->pos.rotY + angle) >> W2V_SHIFT);
		fx->objectID = ID_CANDLE_FLAME_SPRITE;
		fx->frameNumber = Objects[fx->objectID].nMeshes * GetRandomControl() / 0x7FFF;
	}
}

void CandleFlameEmitterControl(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];
	CANDLE_EMITTER_DATA* candle = GetCandleFlameData(item);
	if (candle->on)
	{
		AddDynamicLight(item->pos.x, item->pos.y, item->pos.z, 12, 11); // power, range
	}
	if (candle->on && !TriggerActive(item))
	{
		KillEffect(candle->leftID);
		KillEffect(candle->middleID);
		KillEffect(candle->rightID);
		candle->on = false;
	}
	else if (!candle->on && TriggerActive(item))
	{
		SpawnCandleFlame(item, 182, -ANGLE(90), 32, &candle->leftID);
		SpawnCandleFlame(item, 0, ANGLE(0), 0, &candle->middleID);
		SpawnCandleFlame(item, 182, ANGLE(90), 0, &candle->rightID);
		candle->on = true;
	}
}

#define CANDLE_TIMER (FRAMES_PER_SECOND / 4)
void CandleEmitterSpriteControl(short fxNumber)
{
	FX_INFO* fx = &Effects[fxNumber];
	if (fx->counter <= 0)
	{
		if (--fx->frameNumber <= Objects[ID_CANDLE_FLAME_SPRITE].nMeshes) {
			fx->frameNumber = 0;
		}
		fx->counter = CANDLE_TIMER;
	}
	else
	{
		fx->counter--;
	}
	PlaySoundEffect(150, &fx->pos, NULL);
}

/*
 * Inject function
 */
void Inject_Traps() {
	INJECT(0x00440FC0, MineControl);
	INJECT(0x004411C0, ControlSpikeWall);
	INJECT(0x00441300, ControlCeilingSpikes);
	INJECT(0x00441420, HookControl);
	INJECT(0x004414B0, PropellerControl);
	INJECT(0x00441640, SpinningBlade);
	INJECT(0x004417C0, IcicleControl);
	INJECT(0x004418C0, InitialiseBlade);
	INJECT(0x00441900, BladeControl);
	INJECT(0x004419A0, InitialiseKillerStatue);
	INJECT(0x004419F0, KillerStatueControl);
	INJECT(0x00441B00, SpringBoardControl);
	INJECT(0x00441BE0, InitialiseRollingBall);
	INJECT(0x00441C20, RollingBallControl);
	INJECT(0x00441F70, RollingBallCollision);
	INJECT(0x004421C0, SpikeCollision);
	INJECT(0x00442320, TrapDoorControl);
	INJECT(0x00442370, TrapDoorFloor);
	INJECT(0x004423B0, TrapDoorCeiling);
	INJECT(0x004423F0, OnTrapDoor);
	INJECT(0x004424A0, Pendulum);
	INJECT(0x004425B0, FallingBlock);
	INJECT(0x004426C0, FallingBlockFloor);
	INJECT(0x00442700, FallingBlockCeiling);
	INJECT(0x00442750, TeethTrap);
	INJECT(0x00442810, FallingCeiling);
	INJECT(0x004428F0, DartEmitterControl);
	INJECT(0x00442A30, DartsControl);
	INJECT(0x00442B90, DartEffectControl);
	INJECT(0x00442BE0, FlameEmitterControl);
	INJECT(0x00442C70, FlameControl);
	INJECT(0x00442DE0, LaraBurn);
	INJECT(0x00442E30, LavaBurn);
	INJECT(0x00442F20, LavaSpray);
	INJECT(0x00442FF0, ControlLavaBlob);
}