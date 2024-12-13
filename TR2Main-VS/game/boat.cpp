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
#include "game/boat.h"
#include "3dsystem/phd_math.h"
#include "game/control.h"
#include "game/collide.h"
#include "game/box.h"
#include "game/items.h"
#include "game/lara.h"
#include "game/effects.h"
#include "game/missile.h"
#include "game/sound.h"
#include "game/sphere.h"
#include "game/skidoo.h"
#include "specific/game.h"
#include "specific/init.h"
#include "specific/output.h"
#include "global/vars.h"

typedef enum {
	BOAT_GETON,
	BOAT_STILL,
	BOAT_MOVING,
	BOAT_JUMPR,
	BOAT_JUMPL,
	BOAT_HIT,
	BOAT_FALL,
	BOAT_TURNR,
	BOAT_DEATH,
	BOAT_TURNL
} BOAT_STATE;

static BOAT_INFO* GetBoatData(ITEM_INFO* item)
{
	return static_cast<BOAT_INFO*>(item->data);
}

void InitialiseBoat(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];
	item->data = (BOAT_INFO*)game_malloc(sizeof(BOAT_INFO), GBUF_TempAlloc);
	BOAT_INFO* boat = GetBoatData(item);
	boat->turn = 0;
	boat->rightFallspeed = 0;
	boat->leftFallspeed = 0;
	boat->tiltAngle = 0;
	boat->extraRotation = 0;
	boat->water = 0;
	boat->pitch = 0;
}

BOAT_GETON_TYPE BoatCheckGeton(short itemNumber, COLL_INFO* coll)
{
	if (Lara.gun_status != LGS_Armless)
		return BGF_NOTON;

	ITEM_INFO* item = &Items[itemNumber];
	int distance = ((LaraItem->pos.z - item->pos.z) * phd_cos(-item->pos.rotY) - (LaraItem->pos.x - item->pos.x) * phd_sin(-item->pos.rotY)) >> W2V_SHIFT;
	if (distance > 200)
		return BGF_NOTON;

	short rotation = item->pos.rotY - LaraItem->pos.rotY;
	BOAT_GETON_TYPE geton = BGF_NOTON;

	if (Lara.water_status == LWS_Surface || Lara.water_status == LWS_Wade)
	{
		if (!CHK_ANY(InputStatus, IN_ACTION) || LaraItem->gravity || item->speed)
			return BGF_NOTON;

		if (rotation > 0x2000 && rotation < 0x6000)
			geton = BGF_GETRIGHT; // Right
		else if (rotation > -0x6000 && rotation < -0x2000)
			geton = BGF_GETLEFT; // Left
	}
	else if (Lara.water_status == LWS_AboveWater)
	{
		short fallspeed = LaraItem->fallSpeed;
		if (fallspeed > 0)
		{
			if (rotation > -0x6000 && rotation < 0x6000 && LaraItem->pos.y > item->pos.y)
				geton = BGF_JUMPON; // Jumped on
		}
		else if (fallspeed == 0 && rotation > -0x6000 && rotation < 0x6000)
		{
			if (LaraItem->pos.x == item->pos.x && LaraItem->pos.y == item->pos.y && LaraItem->pos.z == item->pos.z)
				geton = BGF_SAMESPOT; // Same spot as boat
			else
				geton = BGF_JUMPON; // Jumped
		}
	}

	if (!geton)
		return BGF_NOTON;

	if (!TestBoundsCollide(item, LaraItem, coll->radius))
		return BGF_NOTON;

	if (!TestCollision(item, LaraItem))
		return BGF_NOTON;

	return geton;
}

void BoatCollision(short itemNumber, ITEM_INFO* laraitem, COLL_INFO* coll)
{
	if (laraitem->hitPoints <= 0 || Lara.skidoo != -1)
		return;

	int onflag = BoatCheckGeton(itemNumber, coll);
	if (onflag == BGF_NOTON)
	{
		coll->flags |= CF_ENABLE_BADDIE_PUSH;
		ObjectCollision(itemNumber, laraitem, coll);
		return;
	}

	int animIndex = Objects[ID_LARA_BOAT].animIndex;
	switch (onflag)
	{
	case BGF_GETRIGHT:
		laraitem->animNumber = animIndex + 8;
		break;
	case BGF_GETLEFT:
		laraitem->animNumber = animIndex + 0;
		break;
	case BGF_JUMPON:
		laraitem->animNumber = animIndex + 6;
		break;
	case BGF_SAMESPOT:
		laraitem->animNumber = animIndex + 1;
		break;
	}

	ITEM_INFO* item = &Items[itemNumber];
	laraitem->frameNumber = Anims[laraitem->animNumber].frameBase;
	laraitem->pos.x = item->pos.x;
	laraitem->pos.y = item->pos.y - 5;
	laraitem->pos.z = item->pos.z;
	laraitem->pos.rotX = 0;
	laraitem->pos.rotY = item->pos.rotY;
	laraitem->pos.rotZ = 0;
	laraitem->speed = 0;
	laraitem->fallSpeed = 0;
	laraitem->goalAnimState = BOAT_GETON;
	laraitem->currentAnimState = BOAT_GETON;
	laraitem->gravity = FALSE;

	AnimateItem(laraitem);

	short roomNum = item->roomNumber;
	if (laraitem->roomNumber != roomNum)
		ItemNewRoom(Lara.item_number, roomNum);

	if (item->status != ITEM_ACTIVE)
	{
		AddActiveItem(itemNumber);
		item->status = ITEM_ACTIVE;
	}

	Lara.water_status = LWS_AboveWater;
	Lara.skidoo = itemNumber;
}

int TestWaterHeight(ITEM_INFO* item, int zoff, int xoff, PHD_VECTOR* pos)
{
	short rotYCos = phd_cos(item->pos.rotY);
	short rotYSin = phd_sin(item->pos.rotY);
	pos->y = item->pos.y + (xoff * phd_sin(item->pos.rotZ) >> W2V_SHIFT) - (zoff * phd_sin(item->pos.rotX) >> W2V_SHIFT);
	pos->z = item->pos.z + ((zoff * rotYCos - xoff * rotYSin) >> W2V_SHIFT);
	pos->x = item->pos.x + ((xoff * rotYCos + zoff * rotYSin) >> W2V_SHIFT);
	short roomNumber = item->roomNumber;
	FLOOR_INFO* floor = GetFloor(pos->x, pos->y, pos->z, &roomNumber);
	int height = GetWaterHeight(pos->x, pos->y, pos->z, roomNumber);
	if (height == NO_HEIGHT)
	{
		floor = GetFloor(pos->x, pos->y, pos->z, &roomNumber);
		height = GetHeight(floor, pos->x, pos->y, pos->z);
		if (height == NO_HEIGHT)
			return height;
	}
	return height - 5;
}

void DoBoatShift(int itemNumber) {
	ITEM_INFO* item, *link = NULL;
	short linkID;
	int x, z, dx, dz;

	item = &Items[itemNumber];
	for (linkID = Rooms[item->roomNumber].itemNumber; linkID != -1; linkID = link->nextItem) {
		link = &Items[linkID];
		if (link->objectID == ID_BOAT && linkID != itemNumber && Lara.skidoo != linkID) {
			dz = link->pos.z - item->pos.z;
			dx = link->pos.x - item->pos.x;
			if (SQR(dx) + SQR(dz) < SQR(1000)) {
				item->pos.x = link->pos.x - SQR(1000) * dx / (SQR(dx) + SQR(dz));
				item->pos.z = link->pos.z - SQR(1000) * dz / (SQR(dx) + SQR(dz));
			}
		}
		else {
			if (link->objectID == ID_GONDOLA && link->currentAnimState == 1) {
				x = link->pos.x - (512 * phd_sin(link->pos.rotY) >> W2V_SHIFT);
				z = link->pos.z - (512 * phd_cos(link->pos.rotY) >> W2V_SHIFT);
				dx = x - item->pos.x;
				dz = z - item->pos.z;
				if (SQR(dx) + SQR(dz) < SQR(1000)) {
					if (item->speed < 80) {
						item->pos.x = x - SQR(1000) * dx / (SQR(dx) + SQR(dz));
						item->pos.z = z - SQR(1000) * dz / (SQR(dx) + SQR(dz));
					}
					else {
						if (link->pos.y - item->pos.y < 2048) {
							PlaySoundEffect(337, &link->pos, 0);
							link->goalAnimState = 2;
						}
					}
				}
			}
		}
	}
}

void DoWakeEffect(ITEM_INFO* item) {
	short frame_number, fxID;
	int i;
	FX_INFO* fx;

	PhdMatrixPtr->_23 = 0;
	S_CalculateLight(item->pos.x, item->pos.y, item->pos.z, item->roomNumber);
	frame_number = Objects[ID_WATER_SPRITE].nMeshes * GetRandomDraw() >> 15;
	for (i = 0; i < 3; ++i) {
		fxID = CreateEffect(item->roomNumber);
		if (fxID != -1) {
			fx = &Effects[fxID];
			fx->pos.x = item->pos.x + ((-700 * phd_sin(item->pos.rotY) + 300 * (i - 1) * phd_cos(item->pos.rotY)) >> W2V_SHIFT);
			fx->pos.y = item->pos.y;
			fx->pos.z = item->pos.z + ((-700 * phd_cos(item->pos.rotY) - 300 * (i - 1) * phd_sin(item->pos.rotY)) >> W2V_SHIFT);
			fx->pos.rotY = PHD_90 * (i - 1) + item->pos.rotY;
			fx->roomNumber = item->roomNumber;
			fx->frameNumber = frame_number;
			fx->counter = 20;
			fx->objectID = ID_WATER_SPRITE;
			fx->speed = item->speed >> 2;
			if (item->speed < 64) {
				fx->fallspeed = (ABS(item->speed) - 64) * GetRandomDraw() >> 15;
			}
			else {
				fx->fallspeed = 0;
			}
			fx->shade = LsAdder - 768;
			CLAMPL(fx->shade, 0);
		}
	}
}

int DoBoatDynamics(int height, int fallspeed, int* y)
{
	int result;

	if (height <= *y)
	{
		result = ((height - fallspeed - *y) >> 3) + fallspeed;
		if (result < -20)
			result = -20;
		if (*y > height)
			*y = height;
	}
	else
	{
		*y += fallspeed;
		if (*y <= height)
		{
			result = fallspeed + 6;
		}
		else
		{
			*y = height;
			result = 0;
		}
	}

	return result;
}

int BoatDynamics(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];
	BOAT_INFO* boat = GetBoatData(item);
	item->pos.rotZ -= boat->tiltAngle;

	PHD_VECTOR fl_old;
	PHD_VECTOR bl_old;
	PHD_VECTOR fr_old;
	PHD_VECTOR br_old;
	PHD_VECTOR f_old;
	int hfl_old = TestWaterHeight(item, 750, -300, &fl_old);
	int hfr_old = TestWaterHeight(item, 750, 300, &fr_old);
	int hbl_old = TestWaterHeight(item, -750, -300, &bl_old);
	int hbr_old = TestWaterHeight(item, -750, 300, &br_old);
	int hf_old = TestWaterHeight(item, 1000, 0, &f_old);

	PHD_VECTOR old{};
	old.x = item->pos.x;
	old.y = item->pos.y;
	old.z = item->pos.z;
	if (bl_old.y > hbl_old)
		bl_old.y = hbl_old;
	if (br_old.y > hbr_old)
		br_old.y = hbr_old;
	if (fl_old.y > hfl_old)
		fl_old.y = hfl_old;
	if (fr_old.y > hfr_old)
		fr_old.y = hfr_old;
	if (f_old.y > hf_old)
		f_old.y = hf_old;

	item->pos.rotY += boat->extraRotation + boat->turn;
	boat->tiltAngle = 6 * boat->turn;
	item->pos.z += item->speed * phd_cos(item->pos.rotY) >> W2V_SHIFT;
	item->pos.x += item->speed * phd_sin(item->pos.rotY) >> W2V_SHIFT;

	int slip = 30 * phd_sin(item->pos.rotZ) >> W2V_SHIFT;
	if (!slip && item->pos.rotZ)
		slip = item->pos.rotZ <= 0 ? -1 : 1;
	item->pos.z -= slip * phd_sin(item->pos.rotY) >> W2V_SHIFT;
	item->pos.x += slip * phd_cos(item->pos.rotY) >> W2V_SHIFT;
	slip = 10 * phd_sin(item->pos.rotX) >> W2V_SHIFT;
	if (!slip && item->pos.rotX)
		slip = item->pos.rotX <= 0 ? -1 : 1;
	item->pos.z -= (slip * phd_cos(item->pos.rotY) >> W2V_SHIFT);
	item->pos.x -= (slip * phd_sin(item->pos.rotY) >> W2V_SHIFT);

	PHD_VECTOR moved{};
	moved.x = item->pos.x;
	moved.y = item->pos.y;
	moved.z = item->pos.z;
	DoBoatShift(itemNumber);

	PHD_VECTOR fl;
	PHD_VECTOR bl;
	PHD_VECTOR fr;
	PHD_VECTOR br;
	PHD_VECTOR f;
	int rot = 0;
	int hbl = TestWaterHeight(item, -750, -300, &bl);
	if (hbl < bl_old.y - 128)
		rot = DoShift(item, &bl, &bl_old);
	int hbr = TestWaterHeight(item, -750, 300, &br);
	if (hbr < br_old.y - 128)
		rot += DoShift(item, &br, &br_old);
	int hfl = TestWaterHeight(item, 750, -300, &fl);
	if (hfl < fl_old.y - 128)
		rot += DoShift(item, &fl, &fl_old);
	int hfr = TestWaterHeight(item, 750, 300, &fr);
	if (hfr < fr_old.y - 128)
		rot += DoShift(item, &fr, &fr_old);
	if (!slip)
	{
		int hf = TestWaterHeight(item, 1000, 0, &f);
		if (hf < f_old.y - 128)
			DoShift(item, &f, &f_old);
	}

	short roomNum = item->roomNumber;
	FLOOR_INFO* floor = GetFloor(item->pos.x, item->pos.y, item->pos.z, &roomNum);
	int height = GetWaterHeight(item->pos.x, item->pos.y, item->pos.z, roomNum);
	if (height == NO_HEIGHT)
		height = GetHeight(floor, item->pos.x , item->pos.y, item->pos.z);
	if (height < item->pos.y - 128)
		DoShift(item, (PHD_VECTOR*)&item->pos, &old);
	boat->extraRotation = rot;
	int collide = GetCollisionAnim(item, &moved);
	if (slip || collide)
	{
		int newspeed = (((item->pos.z - old.z) * phd_cos(item->pos.rotY)) + ((item->pos.x - old.x) * phd_sin(item->pos.rotY))) >> W2V_SHIFT;
		if (Lara.skidoo == itemNumber && item->speed > 95 && newspeed < item->speed - 10)
		{
			LaraItem->hitPoints -= (item->speed - newspeed) >> 1;
			LaraItem->hitStatus = TRUE;
			PlaySoundEffect(31, &LaraItem->pos, 0);
			item->speed >>= 1;
			newspeed >>= 1;
		}

		if (slip && item->speed <= 100)
			item->speed = newspeed;
		else if (item->speed > 0 && newspeed < item->speed || item->speed < 0 && newspeed > item->speed)
			item->speed = newspeed;

		if (item->speed < -20)
			item->speed = -20;
	}

	return collide;
}

int BoatUserControl(ITEM_INFO* item)
{
	BOAT_INFO* boat = GetBoatData(item);
	int no_turn = 1;
	if (item->pos.y < boat->water - 128 || boat->water == NO_HEIGHT)
		return no_turn;

	if (CHK_ANY(InputStatus, IN_LOOK) && !item->speed)
	{
		LookUpDown();
	}
	else
	{
		if ((CHK_ANY(InputStatus, IN_LEFT) && !CHK_ANY(InputStatus, IN_BACK)) || (CHK_ANY(InputStatus, IN_RIGHT) && CHK_ANY(InputStatus, IN_BACK)))
		{
			if (boat->turn > 0)
			{
				boat->turn -= 45;
			}
			else
			{
				boat->turn -= 22;
				if (boat->turn < -728)
					boat->turn = -728;
			}
			no_turn = 0;
		}
		else if ((CHK_ANY(InputStatus, IN_RIGHT) && !CHK_ANY(InputStatus, IN_BACK)) || (CHK_ANY(InputStatus, IN_LEFT)) && CHK_ANY(InputStatus, IN_BACK))
		{
			if (boat->turn < 0)
			{
				boat->turn += 45;
			}
			else
			{
				boat->turn += 22;
				if (boat->turn > 728)
					boat->turn = 728;
			}
			no_turn = 0;
		}

		if (CHK_ANY(InputStatus, IN_BACK))
		{
			if (item->speed > 0)
				item->speed -= 5;
			else if (item->speed > -20)
				item->speed -= 2;
		}
		else if (CHK_ANY(InputStatus, IN_FORWARD))
		{
			int max_speed;
			if (CHK_ANY(InputStatus, IN_SLOW))
				max_speed = 90;
			else if (CHK_ANY(InputStatus, IN_ACTION))
				max_speed = 140;
			else
				max_speed = 110;
			if (item->speed < max_speed)
				item->speed = short(5 * item->speed / (2 * max_speed) + item->speed + 2);
			else if (item->speed > max_speed + 1)
				item->speed--;
		}
		else if (item->speed >= 0 && item->speed < 20 && CHK_ANY(InputStatus, (IN_RIGHT | IN_LEFT)))
			item->speed = 20;
		else if (item->speed > 1)
			item->speed--;
		else
			item->speed = 0;
	}

	return no_turn;
}

void BoatAnimation(ITEM_INFO* item, int collide)
{
	BOAT_INFO* boat = GetBoatData(item);

	if (LaraItem->hitPoints <= 0)
	{
		if (LaraItem->currentAnimState != BOAT_DEATH)
			SetAnimationWithObject(LaraItem, ID_LARA_BOAT, 18, BOAT_DEATH);
	}
	else if (item->pos.y < (boat->water - 128) && item->fallSpeed > 0)
	{
		if (LaraItem->currentAnimState != BOAT_FALL)
			SetAnimationWithObject(LaraItem, ID_LARA_BOAT, 15, BOAT_FALL);
	}
	else if (collide != 0)
	{
		if (LaraItem->currentAnimState != BOAT_HIT)
			SetAnimationWithObject(LaraItem, ID_LARA_BOAT, collide, BOAT_HIT);
	}
	else
	{
		switch (LaraItem->currentAnimState)
		{
		case BOAT_STILL:
			if (CHK_ANY(InputStatus, IN_JUMP))
			{
				if (item->speed <= 0)
				{
					if (CHK_ANY(InputStatus, IN_LEFT))
						LaraItem->goalAnimState = BOAT_JUMPL;
					else if (CHK_ANY(InputStatus, IN_RIGHT))
						LaraItem->goalAnimState = BOAT_JUMPR;
				}
			}

			if (item->speed > 0)
				LaraItem->goalAnimState = BOAT_MOVING;
			break;
		case BOAT_MOVING:
			if (item->speed <= 0)
				LaraItem->goalAnimState = BOAT_STILL;
			else if (CHK_ANY(InputStatus, IN_JUMP))
			{
				if (CHK_ANY(InputStatus, IN_LEFT))
					LaraItem->goalAnimState = BOAT_JUMPL;
				else if (CHK_ANY(InputStatus, IN_RIGHT))
					LaraItem->goalAnimState = BOAT_JUMPR;
			}
			else if (CHK_ANY(InputStatus, IN_LEFT))
				LaraItem->goalAnimState = BOAT_TURNL;
			else if (CHK_ANY(InputStatus, IN_RIGHT))
				LaraItem->goalAnimState = BOAT_TURNR;
			break;
		case BOAT_FALL:
			LaraItem->goalAnimState = BOAT_MOVING;
			break;
		case BOAT_TURNR:
			if (item->speed <= 0)
				LaraItem->goalAnimState = BOAT_STILL;
			else if (!CHK_ANY(InputStatus, IN_RIGHT))
				LaraItem->goalAnimState = BOAT_MOVING;
			break;
		case BOAT_TURNL:
			if (item->speed <= 0)
				LaraItem->goalAnimState = BOAT_STILL;
			else if (!CHK_ANY(InputStatus, IN_LEFT))
				LaraItem->goalAnimState = BOAT_MOVING;
			break;
		}
	}
}

void BoatControl(short itemNumber)
{
	ITEM_INFO* item;
	BOAT_INFO* boat;
	FLOOR_INFO* floor;
	PHD_VECTOR flPos, frPos;
	long hitWall, driving, no_turn, front_left, front_right, h, wh, x, y, z;
	short room_number, oldFallSpeed, x_rot, z_rot;

	item = &Items[itemNumber];
	boat = GetBoatData(item);
	no_turn = 1;
	driving = 0;
	hitWall = BoatDynamics(itemNumber);
	front_left = TestWaterHeight(item, 750, -300, &flPos);
	front_right = TestWaterHeight(item, 750, 300, &frPos);

	room_number = item->roomNumber;
	floor = GetFloor(item->pos.x, item->pos.y, item->pos.z, &room_number);
	h = GetHeight(floor, item->pos.x, item->pos.y, item->pos.z);
	GetCeiling(floor, item->pos.x, item->pos.y, item->pos.z);

	if (Lara.skidoo == itemNumber)
	{
		TestTriggers(TriggerPtr, FALSE);
		TestTriggers(TriggerPtr, TRUE);
	}

	boat->water = GetWaterHeight(item->pos.x, item->pos.y, item->pos.z, room_number);
	wh = boat->water;

	if (Lara.skidoo == itemNumber && LaraItem->hitPoints > 0)
	{
		if (LaraItem->currentAnimState && (LaraItem->currentAnimState <= BOAT_MOVING || LaraItem->currentAnimState > BOAT_JUMPL))
		{
			driving = 1;
			no_turn = BoatUserControl(item);
		}
	}
	else if (item->speed > 1)
		item->speed--;
	else
		item->speed = 0;

	if (no_turn)
	{
		if (boat->turn < -45)
			boat->turn += 45;
		else if (boat->turn > 45)
			boat->turn -= 45;
		else
			boat->turn = 0;
	}

	item->floor = h - 5;
	if (boat->water == NO_HEIGHT)
		boat->water = h;
	else
		boat->water -= 5;

	boat->leftFallspeed = DoBoatDynamics(front_left, boat->leftFallspeed, &flPos.y);
	boat->rightFallspeed = DoBoatDynamics(front_right, boat->rightFallspeed, &frPos.y);

	oldFallSpeed = item->fallSpeed;
	item->fallSpeed = (short)DoBoatDynamics(boat->water, item->fallSpeed, &item->pos.y);
	if ((oldFallSpeed - item->fallSpeed) > 32 && !item->fallSpeed && wh != NO_HEIGHT)
		Splash2(item, wh);

	h = frPos.y + flPos.y;
	if (h >= 0)
		h >>= 1;
	else
		h = -abs(h) >> 1;

	x_rot = (short)phd_atan(750, item->pos.y - h);
	z_rot = (short)phd_atan(300, h - flPos.y);
	item->pos.rotX += (x_rot - item->pos.rotX) >> 1;
	item->pos.rotZ += (z_rot - item->pos.rotZ) >> 1;

	if (!x_rot && abs(item->pos.rotX) < 4)
		item->pos.rotX = 0;
	if (!z_rot && abs(item->pos.rotZ) < 4)
		item->pos.rotZ = 0;

	if (Lara.skidoo == itemNumber)
	{
		BoatAnimation(item, hitWall);

		// NOTE: Changed how lara change room there, to fix a bug where lara don't update room correctly !
		UpdateLaraRoom(LaraItem, -762); // 762 is the height of lara from 0,0,0 to the eyes.
		if (room_number != item->roomNumber)
			ItemNewRoom(itemNumber, room_number);

		item->pos.rotZ += boat->tiltAngle;
		LaraItem->pos.x = item->pos.x;
		LaraItem->pos.y = item->pos.y;
		LaraItem->pos.z = item->pos.z;
		LaraItem->pos.rotX = item->pos.rotX;
		LaraItem->pos.rotY = item->pos.rotY;
		LaraItem->pos.rotZ = item->pos.rotZ;
		AnimateItem(LaraItem);

		if (LaraItem->hitPoints > 0)
		{
			item->animNumber = Objects[ID_BOAT].animIndex + LaraItem->animNumber - Objects[ID_LARA_BOAT].animIndex;
			item->frameNumber = LaraItem->frameNumber + Anims[item->animNumber].frameBase - Anims[LaraItem->animNumber].frameBase;
		}

		Camera.targetElevation = -3640;
		Camera.targetDistance = 2048;
	}
	else
	{
		if (room_number != item->roomNumber)
			ItemNewRoom(itemNumber, room_number);
		item->pos.rotZ += boat->tiltAngle;
	}

	boat->pitch += (item->speed - boat->pitch) >> 2;
	if (item->speed && (wh - 5) != item->pos.y)
		PlaySoundEffect(336, &item->pos, 0);
	else if (item->speed > 8)
		PlaySoundEffect(197, &item->pos, 25600 * boat->pitch + 0xD50804);
	else if (driving)
		PlaySoundEffect(195, &item->pos, 25600 * boat->pitch + 0xD50804);

	if (item->speed != 0 && (wh - 5) == item->pos.y)
		DoWakeEffect(item);

	if (Lara.skidoo == itemNumber && (LaraItem->currentAnimState == BOAT_JUMPR || LaraItem->currentAnimState == BOAT_JUMPL) && LaraItem->frameNumber == Anims[LaraItem->animNumber].frameEnd)
	{
		if (LaraItem->currentAnimState == BOAT_JUMPL)
			LaraItem->pos.rotY -= PHD_90;
		else
			LaraItem->pos.rotY += PHD_90;

		LaraItem->animNumber = 77;
		LaraItem->frameNumber = Anims[LaraItem->animNumber].frameBase;
		LaraItem->currentAnimState = AS_FORWARDJUMP;
		LaraItem->goalAnimState = AS_FORWARDJUMP;
		LaraItem->gravity = TRUE;
		LaraItem->fallSpeed = -40;
		LaraItem->speed = 20;
		LaraItem->pos.rotX = 0;
		LaraItem->pos.rotZ = 0;
		Lara.skidoo = -1;

		room_number = LaraItem->roomNumber;
		x = LaraItem->pos.x + ((360 * phd_sin(LaraItem->pos.rotY)) >> W2V_SHIFT);
		y = LaraItem->pos.y - 90;
		z = LaraItem->pos.z + ((360 * phd_cos(LaraItem->pos.rotY)) >> W2V_SHIFT);
		floor = GetFloor(x, y, z, &room_number);

		if (GetHeight(floor, x, y, z) >= y - 256)
		{
			LaraItem->pos.x = x;
			LaraItem->pos.z = z;
			if (room_number != LaraItem->roomNumber)
				ItemNewRoom(Lara.item_number, room_number);
		}

		LaraItem->pos.y = y;
		item->animNumber = Objects[ID_BOAT].animIndex;
		item->frameNumber = Anims[item->animNumber].frameBase;
	}
}

void GondolaControl(short itemNumber) {
	ITEM_INFO* item;
	short roomID;

	item = &Items[itemNumber];
	switch (item->currentAnimState) {
	case 1:
		if (item->goalAnimState == 2) {
			item->meshBits = 0xFF;
			ExplodingDeath(itemNumber, 0xF0, 0);
		}
		break;
	case 3:
		item->pos.y += 50;
		roomID = item->roomNumber;
		item->floor = GetHeight(GetFloor(item->pos.x, item->pos.y, item->pos.z, &roomID), item->pos.x, item->pos.y, item->pos.z);
		if (item->pos.y >= item->floor) {
			item->goalAnimState = 4;
			item->pos.y = item->floor;
		}
		break;
	}
	AnimateItem(item);
	if (item->status == ITEM_DISABLED)
		RemoveActiveItem(itemNumber);
}

/*
 * Inject function
 */
void Inject_Boat() {
	INJECT(0x0040CB10, InitialiseBoat);
	INJECT(0x0040CB50, BoatCheckGeton);
	INJECT(0x0040CCC0, BoatCollision);
	INJECT(0x0040CE20, TestWaterHeight);
	INJECT(0x0040CF20, DoBoatShift);
	INJECT(0x0040D0F0, DoWakeEffect);
	INJECT(0x0040D270, DoBoatDynamics);
	INJECT(0x0040D2C0, BoatDynamics);
	INJECT(0x0040D7A0, BoatUserControl);
	INJECT(0x0040D930, BoatAnimation);
	INJECT(0x0040DAA0, BoatControl);
	INJECT(0x0040E0D0, GondolaControl);
}