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
#include "game/larasurf.h"
#include "3dsystem/phd_math.h"
#include "game/collide.h"
#include "game/control.h"
#include "game/lara.h"
#include "game/laraswim.h"
#include "game/larafire.h"
#include "game/laramisc.h"
#include "global/vars.h"

void LaraSurface(ITEM_INFO* item, COLL_INFO* coll)
{
	Camera.targetElevation = -ANGLE(22);
	coll->badPos = 32512;
	coll->badNeg = -128;
	coll->badCeiling = 100;
	coll->old.x = item->pos.x;
	coll->old.y = item->pos.y;
	coll->old.z = item->pos.z;
	coll->radius = 100;
	coll->trigger = NULL;
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH | CF_LAVA_IS_PIT | CF_SLOPE_ARE_PITS | CF_SLOPE_ARE_WALLS);

	if (CHK_ANY(InputStatus, IN_LOOK) && Lara.extra_anim == 0 && Lara.look)
		LookLeftRight();
	else
		ResetLook();
	Lara.look = TRUE;

	LaraControlFunctions[item->currentAnimState](item, coll);
	if (item->pos.rotZ >= -ANGLE(2) && item->pos.rotZ <= ANGLE(2))
		item->pos.rotZ = 0;
	else if (item->pos.rotZ < 0)
		item->pos.rotZ += ANGLE(2);
	else
		item->pos.rotZ -= ANGLE(2);

	if (Lara.current_active != 0 && Lara.water_status != LWS_Cheat)
		LaraWaterCurrent(coll);

	AnimateLara(item);
	item->pos.x += item->fallSpeed * phd_sin(Lara.move_angle) >> 16;
	item->pos.z += item->fallSpeed * phd_cos(Lara.move_angle) >> 16;
	LaraBaddieCollision(item, coll);
	if (Lara.skidoo == -1)
		LaraCollisionFunctions[item->currentAnimState](item, coll);
	UpdateLaraRoom(item, 100);
	LaraGun();
	TestTriggers(coll->trigger, FALSE);
}

 /*
  * Inject function
  */
void Inject_LaraSurf() {
	INJECT(0x00431710, LaraSurface);
	//INJECT(0x00431870, lara_as_surfswim);
	//INJECT(0x004318E0, lara_as_surfback);
	//INJECT(0x00431940, lara_as_surfleft);
	//INJECT(0x004319A0, lara_as_surfright);
	//INJECT(0x00431A00, lara_as_surftread);
	//INJECT(0x00431AC0, lara_col_surfswim);
	//INJECT(0x00431B00, lara_col_surfback);
	//INJECT(0x00431B30, lara_col_surfleft);
	//INJECT(0x00431B60, lara_col_surfright);
	//INJECT(0x00431B90, lara_col_surftread);
	//INJECT(0x00431BF0, LaraSurfaceCollision);
	//INJECT(0x00431CF0, LaraTestWaterStepOut);
	//INJECT(0x00431DE0, LaraTestWaterClimbOut);
}

