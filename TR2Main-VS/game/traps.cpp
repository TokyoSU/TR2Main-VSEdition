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

void MineControl(short mineID) {
	ITEM_INFO* mine = &Items[mineID];
	if (CHK_ANY(mine->flags, IFL_ONESHOT)) {
		return;
	}

	if (!MinesDetonated) {
		short roomNumber = mine->roomNumber;
		GetFloor(mine->pos.x, mine->pos.y - 0x800, mine->pos.z, &roomNumber);

		ITEM_INFO* item = NULL;
		short itemID = RoomInfo[roomNumber].itemNumber;
		for (; itemID >= 0; itemID = item->nextItem) {
			item = &Items[itemID];
			if (item->objectID == ID_BOAT) {
				int x = item->pos.x - mine->pos.x;
				int y = item->pos.z - mine->pos.z;
				if (SQR(x) + SQR(y) < SQR(0x200)) {
					break;
				}
			}
		}

		if (itemID < 0) {
			return;
		}

		if (Lara.skidoo == itemID) {
			ExplodingDeath(Lara.item_number, ~0, 0);
			LaraItem->hitPoints = 0;
			LaraItem->flags |= IFL_ONESHOT;
		}
		item->objectID = ID_BOAT_BITS;
		ExplodingDeath(itemID, ~0, 0);
		KillItem(itemID);
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

void ControlSpikeWall(short itemID) {
	ITEM_INFO* item;
	int x, z;
	short roomID;

	item = &Items[itemID];
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
				ItemNewRoom(itemID, roomID);
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

void ControlCeilingSpikes(short itemID) {
	ITEM_INFO* item;
	int y;
	short roomID;

	item = &Items[itemID];
	if (TriggerActive(item) && item->status != ITEM_DISABLED) {
		y = item->pos.y + 5;
		roomID = item->roomNumber;
		if (GetHeight(GetFloor(item->pos.x, y, item->pos.z, &roomID), item->pos.x, y, item->pos.z) < y + 1024) {
			item->status = ITEM_DISABLED;
		}
		else {
			item->pos.y = y;
			if (roomID != item->roomNumber)
				ItemNewRoom(itemID, roomID);
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

void HookControl(short itemID) {
	ITEM_INFO* item;
	static BOOL IsHookHit = FALSE;

	item = &Items[itemID];
	if (item->touchBits && !IsHookHit) {
		LaraItem->hitPoints -= 50;
		LaraItem->hitStatus = 1;
		IsHookHit = TRUE;
		DoLotsOfBlood(LaraItem->pos.x, LaraItem->pos.y - 512, LaraItem->pos.z, LaraItem->speed, LaraItem->pos.rotY, LaraItem->roomNumber, 3);
	}
	else {
		IsHookHit = FALSE;
	}
	AnimateItem(item);
}

void SpinningBlade(short itemID) {
	ITEM_INFO* item;
	int x, z;
	short roomID;
	BOOL reverse;

	item = &Items[itemID];
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
			DoLotsOfBlood(LaraItem->pos.x, LaraItem->pos.y - 512, LaraItem->pos.z, 2 * item->speed, LaraItem->pos.rotY, LaraItem->roomNumber, 2);
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
		ItemNewRoom(itemID, roomID);
	if (reverse && item->currentAnimState == 1)
		item->pos.rotY += PHD_180;
}

void IcicleControl(short itemID) {
	ITEM_INFO* item;
	short roomID;
	FLOOR_INFO* floor;

	item = &Items[itemID];
	switch (item->currentAnimState) {
	case 1:
		item->goalAnimState = 2;
		break;
	case 2:
		if (!item->gravity) {
			item->fallSpeed = 50;
			item->gravity = 1;
		}
		if (item->touchBits) {
			LaraItem->hitPoints -= 200;
			LaraItem->hitStatus = 1;
		}
		break;
	case 3:
		item->gravity = 0;
		break;
	}
	AnimateItem(item);
	if (item->status == ITEM_DISABLED) {
		RemoveActiveItem(itemID);
	}
	else {
		roomID = item->roomNumber;
		floor = GetFloor(item->pos.x, item->pos.y, item->pos.z, &roomID);
		if (item->roomNumber != roomID)
			ItemNewRoom(itemID, roomID);
		item->floor = GetHeight(floor, item->pos.x, item->pos.y, item->pos.z);
		if (item->currentAnimState == 2 && item->pos.y >= item->floor) {
			item->gravity = 0;
			item->goalAnimState = 3;
			item->pos.y = item->floor;
			item->fallSpeed = 0;
			item->meshBits = 0x2B;
		}
	}
}

void InitialiseBlade(short itemID) {
	ITEM_INFO* item;

	item = &Items[itemID];
	item->animNumber = Objects[ID_BLADE].animIndex + 2;
	item->currentAnimState = 1;
	item->frameNumber = Anims[item->animNumber].frameBase;
}

void BladeControl(short itemID) {
	ITEM_INFO* item;

	item = &Items[itemID];
	if (TriggerActive(item) && item->currentAnimState == 1) {
		item->goalAnimState = 2;
	}
	else {
		item->goalAnimState = 1;
	}
	if (CHK_ANY(item->touchBits, 2) && item->currentAnimState == 2) {
		LaraItem->hitStatus = 1;
		LaraItem->hitPoints -= 100;
		DoLotsOfBlood(LaraItem->pos.x, item->pos.y - 256, LaraItem->pos.z, LaraItem->speed, LaraItem->pos.rotY, LaraItem->roomNumber, 2);
	}
	AnimateItem(item);
}

void InitialiseKillerStatue(short itemID) {
	ITEM_INFO* item;

	item = &Items[itemID];
	item->animNumber = Objects[item->objectID].animIndex + 3;
	item->currentAnimState = 1;
	item->frameNumber = Anims[item->animNumber].frameBase;
}

void KillerStatueControl(short itemID) {
	ITEM_INFO* item;

	item = &Items[itemID];
	if (TriggerActive(item) && item->currentAnimState == 1) {
		item->goalAnimState = 2;
	}
	else {
		item->goalAnimState = 1;
	}
	if (CHK_ANY(item->touchBits, 0x80) && item->currentAnimState == 2) {
		LaraItem->hitStatus = 1;
		LaraItem->hitPoints -= 20;
		DoBloodSplat(LaraItem->pos.x + (GetRandomControl() - 16384) / 256,
			LaraItem->pos.y - GetRandomControl() / 44,
			LaraItem->pos.z + (GetRandomControl() - 16384) / 256,
			LaraItem->speed,
			LaraItem->pos.rotY + (GetRandomControl() - 16384) / 8,
			LaraItem->roomNumber);
	}
	AnimateItem(item);
}

void SpringBoardControl(short itemID) {
	ITEM_INFO* item = &Items[itemID];
	if (item->currentAnimState == 0 && item->pos.y == LaraItem->pos.y &&
		(LaraItem->pos.x >> WALL_SHIFT) == (item->pos.x >> WALL_SHIFT) &&
		(LaraItem->pos.z >> WALL_SHIFT) == (item->pos.z >> WALL_SHIFT))
	{
		if (LaraItem->hitPoints > 0) {
			if (LaraItem->currentAnimState == AS_BACK || LaraItem->currentAnimState == AS_FASTBACK)
				LaraItem->speed = -LaraItem->speed;
			LaraItem->fallSpeed = -240;
			LaraItem->gravity = 1;
			LaraItem->animNumber = 34;
			LaraItem->frameNumber = Anims[LaraItem->animNumber].frameBase;
			LaraItem->currentAnimState = AS_FORWARDJUMP;
			LaraItem->goalAnimState = AS_FORWARDJUMP;
			item->goalAnimState = 1;
			AnimateItem(item);
		}
	}
	else {
		AnimateItem(item);
	}
}

void InitialiseRollingBall(short itemID) {
	ITEM_INFO* item;
	GAME_VECTOR* pos;

	item = &Items[itemID];
	item->data = game_malloc(sizeof(GAME_VECTOR), GBUF_RollingBallStuff);
	pos = (GAME_VECTOR*)item->data;
	pos->x = item->pos.x;
	pos->y = item->pos.y;
	pos->z = item->pos.z;
	pos->roomNumber = item->roomNumber;
}

void RollingBallControl(short itemID) {
	ITEM_INFO* item;
	int oldX, oldZ, distance, x, z;
	short roomID;
	FLOOR_INFO* floor;
	GAME_VECTOR* pos;

	item = &Items[itemID];
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
				ItemNewRoom(itemID, roomID);
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
	else {
		if (item->status == ITEM_DISABLED && !TriggerActive(item)) {
			pos = (GAME_VECTOR*)item->data;
			item->status = ITEM_INACTIVE;
			item->pos.x = pos->x;
			item->pos.y = pos->y;
			item->pos.z = pos->z;
			if (item->roomNumber != pos->roomNumber) {
				RemoveDrawnItem(itemID);
				item->nextItem = RoomInfo[pos->roomNumber].itemNumber;
				RoomInfo[pos->roomNumber].itemNumber = itemID;
				item->roomNumber = pos->roomNumber;
			}
			item->animNumber = Objects[item->objectID].animIndex;
			item->frameNumber = Anims[item->animNumber].frameBase;
			item->requiredAnimState = 0;
			item->goalAnimState = Anims[item->animNumber].currentAnimState;
			item->currentAnimState = item->goalAnimState;
			RemoveActiveItem(itemID);
		}
	}
}

void RollingBallCollision(short itemID, ITEM_INFO* laraItem, COLL_INFO* coll) {
	ITEM_INFO* item;
	int dx, dy, dz, distance, i;

	item = &Items[itemID];
	if (item->status == ITEM_ACTIVE) {
		if (TestBoundsCollide(item, laraItem, coll->radius) && TestCollision(item, laraItem)) {
			if (laraItem->gravity) {
				if (coll->enableBaddiePush)
					ItemPushLara(item, laraItem, coll, coll->enableSpaz, TRUE);
				laraItem->hitPoints -= 100;
				dx = laraItem->pos.x - item->pos.x;
				dy = laraItem->pos.y - item->pos.y + 162;
				dz = laraItem->pos.z - item->pos.z;
				distance = phd_sqrt(SQR(dx) + SQR(dy) + SQR(dz));
				if (distance < 512)
					distance = 512;
				DoBloodSplat(item->pos.x + (dx << WALL_SHIFT) / 2 / distance,
					item->pos.y + (dy << WALL_SHIFT) / 2 / distance - 512,
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
			ObjectCollision(itemID, laraItem, coll);
	}
}

void Pendulum(short itemID) {
	ITEM_INFO* item = &Items[itemID];
	if (item->touchBits) {
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

void TeethTrap(short itemID) {
	ITEM_INFO* item;
	static BITE_INFO Teeth[3][2] = {
		{{-23, 0, -1718, 0}, {71, 0, -1718, 1}},
		{{-23, 10, -1718, 0}, {71, 10, -1718, 1}},
		{{-23, -10, -1718, 0}, {71, -10, -1718, 1}}
	};

	item = &Items[itemID];
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

void FallingCeiling(short itemID) {
	ITEM_INFO* item;
	short roomID;

	item = &Items[itemID];
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
		RemoveActiveItem(itemID);
	}
	else {
		roomID = item->roomNumber;
		item->floor = GetHeight(GetFloor(item->pos.x, item->pos.y, item->pos.z, &roomID), item->pos.x, item->pos.y, item->pos.z);
		if (roomID != item->roomNumber)
			ItemNewRoom(itemID, roomID);
		if (item->currentAnimState == 1 && item->pos.y >= item->floor) {
			item->gravity = 0;
			item->goalAnimState = 2;
			item->pos.y = item->floor;
			item->fallSpeed = 0;
		}
	}
}

void DartEmitterControl(short itemID) {
	ITEM_INFO* item, * dynamic;
	short dynamicID;
	int dx, dz;

	item = &Items[itemID];
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

void DartsControl(short itemID) {
	ITEM_INFO* item;
	short roomID, fxID;
	FLOOR_INFO* floor;
	FX_INFO* fx;

	item = &Items[itemID];
	if (item->touchBits) {
		LaraItem->hitPoints -= 50;
		LaraItem->hitStatus = 1;
		DoBloodSplat(item->pos.x, item->pos.y, item->pos.z, LaraItem->speed, LaraItem->pos.rotY, LaraItem->roomNumber);
	}
	AnimateItem(item);
	roomID = item->roomNumber;
	floor = GetFloor(item->pos.x, item->pos.y, item->pos.z, &roomID);
	if (item->roomNumber != roomID)
		ItemNewRoom(itemID, roomID);
	item->floor = GetHeight(floor, item->pos.x, item->pos.y, item->pos.z);
	item->pos.rotX += PHD_45 / 2;
	if (item->pos.y >= item->floor) {
		KillItem(itemID);
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

void LavaSpray(short itemNum)
{
	ITEM_INFO* item = &Items[itemNum];
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

/*
 * Inject function
 */
void Inject_Traps() {
	INJECT(0x00440FC0, MineControl);
	INJECT(0x004411C0, ControlSpikeWall);
	INJECT(0x00441300, ControlCeilingSpikes);
	INJECT(0x00441420, HookControl);
	//INJECT(0x004414B0, PropellerControl);
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
	//INJECT(0x004421C0, SpikeCollision);
	//INJECT(0x00442320, TrapDoorControl);
	//INJECT(0x00442370, TrapDoorFloor);
	//INJECT(0x004423B0, TrapDoorCeiling);
	//INJECT(0x004423F0, OnTrapDoor);
	INJECT(0x004424A0, Pendulum);
	//INJECT(0x004425B0, FallingBlock);
	//INJECT(0x004426C0, FallingBlockFloor);
	//INJECT(0x00442700, FallingBlockCeiling);
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