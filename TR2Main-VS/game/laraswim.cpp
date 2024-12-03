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
#include "game/laraswim.h"
#include "3dsystem/phd_math.h"
#include "game/box.h"
#include "game/control.h"
#include "game/collide.h"
#include "game/objects.h"
#include "game/lara.h"
#include "game/larafire.h"
#include "game/laramisc.h"
#include "global/vars.h"

static int OpenDoorsCheatCooldown = 0;

void LaraUnderWater(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->badPos = 32512;
	coll->badNeg = -400;
	coll->badCeiling = 400;
	coll->old.x = item->pos.x;
	coll->old.y = item->pos.y;
	coll->old.z = item->pos.z;
	coll->radius = 300;
	coll->trigger = NULL;
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_LAVA_IS_PIT | CF_SLOPE_ARE_PITS | CF_SLOPE_ARE_WALLS);
	coll->flags |= CF_ENABLE_BADDIE_PUSH;

	if (CHK_ANY(InputStatus, IN_LOOK) && Lara.extra_anim == 0 && Lara.look)
		LookLeftRight();
	else
		ResetLook();
	Lara.look = TRUE;

	if (Lara.extra_anim != 0)
		ExtraFunctions[item->currentAnimState](item, coll);
	else
		LaraControlFunctions[item->currentAnimState](item, coll);

	if (item->pos.rotZ >= -ANGLE(2) && item->pos.rotZ <= ANGLE(2))
		item->pos.rotZ = 0;
	else if (item->pos.rotZ < 0)
		item->pos.rotZ += ANGLE(2);
	else
		item->pos.rotZ -= ANGLE(2);

	CLAMP(item->pos.rotX, -ANGLE(85), ANGLE(85));
	CLAMP(item->pos.rotZ, -ANGLE(22), ANGLE(22));

	if (Lara.turn_rate >= -ANGLE(2) && Lara.turn_rate <= ANGLE(2))
		Lara.turn_rate = 0;
	else if (Lara.turn_rate < -ANGLE(2))
		Lara.turn_rate += ANGLE(2);
	else
		Lara.turn_rate -= ANGLE(2);
	item->pos.rotY += Lara.turn_rate;

	if (Lara.current_active != 0 && Lara.water_status != LWS_Cheat)
		LaraWaterCurrent(coll);

	AnimateLara(item);
	item->pos.y -= (phd_sin(item->pos.rotX) * item->fallSpeed) >> 16;
	item->pos.x += (((phd_sin(item->pos.rotY) * item->fallSpeed) >> 16) * phd_cos(item->pos.rotX)) >> W2V_SHIFT;
	item->pos.z += (((phd_cos(item->pos.rotY) * item->fallSpeed) >> 16) * phd_cos(item->pos.rotX)) >> W2V_SHIFT;

	if (Lara.extra_anim == 0)
	{
		if (Lara.water_status != LWS_Cheat)
			LaraBaddieCollision(item, coll);
		if (Lara.skidoo == -1)
			LaraCollisionFunctions[item->currentAnimState](item, coll);
	}

	if (Lara.water_status == LWS_Cheat) {
		if (OpenDoorsCheatCooldown) {
			OpenDoorsCheatCooldown--;
		}
		else if (InputStatus & IN_DRAW) {
			OpenDoorsCheatCooldown = TICKS_PER_SECOND;
			OpenNearestDoor();
		}
	}

	UpdateLaraRoom(item, 0);
	LaraGun();
	TestTriggers(coll->trigger, FALSE);
}

void SwimTurn(ITEM_INFO* item) {
	if (CHK_ANY(InputStatus, IN_FORWARD)) {
		item->pos.rotX -= ANGLE(2);
	}
	else if (CHK_ANY(InputStatus, IN_BACK)) {
		item->pos.rotX += ANGLE(2);
	}

	if (CHK_ANY(InputStatus, IN_LEFT)) {
		Lara.turn_rate -= ANGLE(9) / 4;
		CLAMPL(Lara.turn_rate, -ANGLE(6));
		item->pos.rotZ -= ANGLE(3);
	}
	else if (CHK_ANY(InputStatus, IN_RIGHT)) {
		Lara.turn_rate += ANGLE(9) / 4;
		CLAMPG(Lara.turn_rate, ANGLE(6));
		item->pos.rotZ += ANGLE(3);
	}
}

void lara_as_swim(ITEM_INFO* item, COLL_INFO* coll) {
	if (item->hitPoints <= 0) {
		item->goalAnimState = AS_UWDEATH;
		return;
	}

	if (CHK_ANY(InputStatus, IN_ROLL)) {
		item->currentAnimState = AS_WATERROLL;
		item->animNumber = 203;
		item->frameNumber = Anims[item->animNumber].frameBase;
	}
	else {
		SwimTurn(item);
		item->fallSpeed += 8;
#ifdef FEATURE_CHEAT
		if (Lara.water_status == LWS_Cheat) {
			CLAMPG(item->fallSpeed, 400);
		}
		else {
			CLAMPG(item->fallSpeed, 200);
		}
#else // FEATURE_CHEAT
		CLAMPG(item->fallSpeed, 200);
#endif // FEATURE_CHEAT
		if (!CHK_ANY(InputStatus, IN_JUMP)) {
			item->goalAnimState = AS_GLIDE;
		}
	}
}

int GetWaterDepth(int x, int y, int z, short roomNumber)
{
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	int wh, x_floor, y_floor;
	short data;

	r = &Rooms[roomNumber];
	do
	{
		x_floor = (z - r->z) >> WALL_SHIFT;
		y_floor = (x - r->x) >> WALL_SHIFT;
		if (x_floor <= 0)
		{
			x_floor = 0;
			if (y_floor < 1)
				y_floor = 1;
			else if (y_floor > r->ySize - 2)
				y_floor = r->ySize - 2;
		}
		else if (x_floor >= r->xSize - 1)
		{
			x_floor = r->xSize - 1;
			if (y_floor < 1)
				y_floor = 1;
			else if (y_floor > r->ySize - 2)
				y_floor = r->ySize - 2;
		}
		else if (y_floor < 0)
			y_floor = 0;
		else if (y_floor >= r->ySize)
			y_floor = r->ySize - 1;

		floor = &r->floor[x_floor + y_floor * r->xSize];
		data = GetDoor(floor);
		if (data != NO_ROOM)
		{
			roomNumber = data;
			r = &Rooms[data];
		}
	} while (data != NO_ROOM);

	if (CHK_ANY(r->flags, ROOM_UNDERWATER | ROOM_QUICKSAND))
	{
		while (floor->skyRoom != NO_ROOM)
		{
			r = &Rooms[floor->skyRoom];
			if (!CHK_ANY(r->flags, ROOM_UNDERWATER | ROOM_QUICKSAND))
			{
				wh = ((int)floor->ceiling << 8);
				floor = GetFloor(x, y, z, &roomNumber);
				return GetHeight(floor, x, y, z) - wh;
			}
			floor = GetFloorSector(x, z, r);
		}
		return 0x7FFF;
	}
	else
	{
		while (floor->pitRoom != NO_ROOM)
		{
			r = &Rooms[floor->pitRoom];
			if (CHK_ANY(r->flags, ROOM_UNDERWATER | ROOM_QUICKSAND))
			{
				wh = ((int)floor->floor << 8);
				floor = GetFloor(x, y, z, &roomNumber);
				return GetHeight(floor, x, y, z) - wh;
			}
			floor = GetFloorSector(x, z, r);
		}
		return NO_HEIGHT;
	}
}

void LaraWaterCurrent(COLL_INFO* coll)
{
	auto* room = &Rooms[LaraItem->roomNumber];
	LaraItem->boxNumber = room->floor[((LaraItem->pos.z - room->z) >> WALL_SHIFT) + ((LaraItem->pos.x - room->x) >> WALL_SHIFT) * room->xSize].box;

	// Exit if creature is not set !
	if (Lara.creature == NULL)
	{
#if defined(_DEBUG)
		LogDebug("Lara.creature is null !");
#endif
		Lara.current_active = 0;
		return;
	}

	PHD_VECTOR target = {};
	if (CalculateTarget(&target, LaraItem, &Lara.creature->LOT) == NO_TARGET)
		return;

	// Move lara to target.
	target.x -= LaraItem->pos.x;
	if (target.x > Lara.current_active)
		LaraItem->pos.x += Lara.current_active;
	else if (target.x < -Lara.current_active)
		LaraItem->pos.x -= Lara.current_active;
	else
		LaraItem->pos.x += target.x;

	target.z -= LaraItem->pos.z;
	if (target.z > Lara.current_active)
		LaraItem->pos.z += Lara.current_active;
	else if (target.z < -Lara.current_active)
		LaraItem->pos.z -= Lara.current_active;
	else
		LaraItem->pos.z += target.z;

	target.y -= LaraItem->pos.y;
	if (target.y > Lara.current_active)
		LaraItem->pos.y += Lara.current_active;
	else if (target.y < -Lara.current_active)
		LaraItem->pos.y -= Lara.current_active;
	else
		LaraItem->pos.y += target.y;

	// Reset, will be set again if lara is still on trigger !
	Lara.current_active = 0;

	// Do collision...
	coll->facing = phd_atan((LaraItem->pos.z - coll->old.z), (LaraItem->pos.x - coll->old.x));
	GetCollisionInfo(coll, LaraItem->pos.x, LaraItem->pos.y + 200, LaraItem->pos.z, LaraItem->roomNumber, 400);
	
	if (coll->collType == COLL_FRONT)
	{
		if (LaraItem->pos.rotX > ANGLE(35))
			LaraItem->pos.rotX += ANGLE(2);
		else if (LaraItem->pos.rotX < -ANGLE(35))
			LaraItem->pos.rotX -= ANGLE(2);
		else
			LaraItem->fallSpeed = 0;
	}
	else if (coll->collType == COLL_TOP)
		LaraItem->pos.rotX -= ANGLE(2);
	else if (coll->collType == COLL_TOPFRONT)
		LaraItem->fallSpeed = 0;
	else if (coll->collType == COLL_LEFT)
		LaraItem->pos.rotY += ANGLE(5);
	else if (coll->collType == COLL_RIGHT)
		LaraItem->pos.rotY -= ANGLE(5);

	if (coll->sideMid.floor < 0)
	{
		LaraItem->pos.y += coll->sideMid.floor;
		LaraItem->pos.rotX += ANGLE(2);
	}

	ShiftItem(LaraItem, coll);
	coll->old.x = LaraItem->pos.x;
	coll->old.y = LaraItem->pos.y;
	coll->old.z = LaraItem->pos.z;
}

/*
 * Inject function
 */
void Inject_LaraSwim() {
	INJECT(0x00432000, LaraUnderWater);
	INJECT(0x00432230, SwimTurn);
	INJECT(0x004322C0, lara_as_swim);
	//INJECT(0x00432330, lara_as_glide);
	//INJECT(0x004323B0, lara_as_tread);
	//INJECT(0x00432440, lara_as_dive);
	//INJECT(0x00432460, lara_as_uwdeath);
	//INJECT(0x004324C0, lara_as_waterroll);
	//INJECT(0x004324D0, lara_col_swim);
	//INJECT(----------, lara_col_glide);
	//INJECT(----------, lara_col_tread);
	//INJECT(----------, lara_col_dive);
	//INJECT(0x004324F0, lara_col_uwdeath);
	//INJECT(----------, lara_col_waterroll);
	INJECT(0x00432550, GetWaterDepth);
	//INJECT(0x004326F0, LaraTestWaterDepth);
	//INJECT(0x004327C0, LaraSwimCollision);
	INJECT(0x00432920, LaraWaterCurrent);
}