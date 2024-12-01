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
#include "game/laraclimb.h"
#include "game/box.h"
#include "game/lara.h"
#include "game/laramisc.h"
#include "global/vars.h"

#define CLIMB_RADIUSR 120
#define CLIMB_RADIUSL 80
#define CLIMB_HEIGHT CLICK(2)
#define CLIMB_STAND_ELEVATION_ANGLE -ANGLE(20)
#define CLIMB_RIGHT_TARGET_ANGLE ANGLE(30)
#define CLIMB_RIGHT_ELEVATION_ANGLE -ANGLE(15)
#define CLIMB_LEFT_TARGET_ANGLE -ANGLE(30)
#define CLIMB_LEFT_ELEVATION_ANGLE -ANGLE(15)
#define CLIMB_END_TARGET_ANGLE -ANGLE(45)
#define CLIMB_DOWN_ELEVATION_ANGLE -ANGLE(45)

void lara_as_climbleft(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);

	Camera.targetAngle = CLIMB_LEFT_TARGET_ANGLE;
	Camera.targetElevation = CLIMB_LEFT_ELEVATION_ANGLE;

	if (!CHK_ANY(InputStatus, IN_LEFT) && !CHK_ANY(InputStatus, IN_STEPL))
		item->goalAnimState = AS_CLIMBSTNC;
}

void lara_as_climbright(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);

	Camera.targetAngle = CLIMB_RIGHT_TARGET_ANGLE;
	Camera.targetElevation = CLIMB_RIGHT_ELEVATION_ANGLE;

	if (!CHK_ANY(InputStatus, IN_RIGHT) && !CHK_ANY(InputStatus, IN_STEPR))
		item->goalAnimState = AS_CLIMBSTNC;
}

void lara_as_climbstnc(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);

	Camera.targetElevation = CLIMB_STAND_ELEVATION_ANGLE;
	if (CHK_ANY(InputStatus, IN_LOOK))
		LookUpDown();

	if (CHK_ANY(InputStatus, IN_LEFT) || CHK_ANY(InputStatus, IN_STEPL))
		item->goalAnimState = AS_CLIMBLEFT;
	else if (CHK_ANY(InputStatus, IN_RIGHT) || CHK_ANY(InputStatus, IN_STEPR))
		item->goalAnimState = AS_CLIMBRIGHT;
	else if (CHK_ANY(InputStatus, IN_JUMP)) {
		item->goalAnimState = AS_BACKJUMP;
		Lara.move_angle = item->pos.rotY + ANGLE(180);
		Lara.gun_status = LGS_Armless;
	}
}

void lara_as_climbing(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);

	Camera.targetElevation = CLIMB_STAND_ELEVATION_ANGLE;
}

void lara_as_climbend(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);

	Camera.flags = FOLLOW_CENTRE;
	Camera.targetElevation = CLIMB_END_TARGET_ANGLE;
}

void lara_as_climbdown(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);

	Camera.targetElevation = CLIMB_DOWN_ELEVATION_ANGLE;
}

void lara_col_climbleft(ITEM_INFO* item, COLL_INFO* coll)
{
	if (LaraCheckForLetGo(item, coll))
		return;
	Lara.move_angle = item->pos.rotY - ANGLE(90);
	int shift;
	BOOL posValid = LaraTestClimbPos(item, coll->radius, -(coll->radius + CLIMB_RADIUSL), -CLIMB_HEIGHT, CLIMB_HEIGHT, &shift);
	LaraDoClimbLeftRight(item, coll, posValid, shift);
}

void lara_col_climbright(ITEM_INFO* item, COLL_INFO* coll)
{
	if (LaraCheckForLetGo(item, coll))
		return;
	Lara.move_angle = item->pos.rotY + ANGLE(90);
	int shift;
	BOOL posValid = LaraTestClimbPos(item, coll->radius, (coll->radius + CLIMB_RADIUSR), -CLIMB_HEIGHT, CLIMB_HEIGHT, &shift);
	LaraDoClimbLeftRight(item, coll, posValid, shift);
}

void lara_col_climbstnc(ITEM_INFO* item, COLL_INFO* coll)
{
	int result_r, result_l, shift_r, shift_l, ledge_r, ledge_l;

	if (LaraCheckForLetGo(item, coll))
		return;
	
	if (item->animNumber != 164)
		return;

	if (CHK_ANY(InputStatus, IN_FORWARD))
	{
		if (item->goalAnimState == AS_NULL)
			return;
		item->goalAnimState = AS_CLIMBSTNC;
		
		result_r = LaraTestClimbUpPos(item, coll->radius, coll->radius + CLIMB_RADIUSR, &shift_r, &ledge_r);
		result_l = LaraTestClimbUpPos(item, coll->radius, -(coll->radius + CLIMB_RADIUSL), &shift_l, &ledge_l);
		if (!result_r || !result_l)
			return;

		if (result_r < 0 || result_l < 0)
		{
			if (ABS(ledge_l - ledge_r) > 120)
				return;

			item->pos.y += (ledge_l + ledge_r) / 2 - CLICK(1);
			item->goalAnimState = AS_NULL;
			return;
		}

		if (shift_r)
		{
			if (shift_l)
			{
				if ((shift_r < 0) ^ (shift_l < 0))
					return;
				else if (shift_r < 0 && shift_r < shift_l)
					shift_l = shift_r;
				else if (shift_r > 0 && shift_r > shift_l)
					shift_l = shift_r;
			}
			else
				shift_l = shift_r;
		}

		item->goalAnimState = AS_CLIMBING;
		item->pos.y += shift_l;
	}
	else if (CHK_ANY(InputStatus, IN_BACK))
	{
		if (item->goalAnimState == AS_HANG)
			return;
		item->goalAnimState = AS_CLIMBSTNC;

		item->pos.y += CLICK(1);
		result_r = LaraTestClimbPos(item, coll->radius, coll->radius + CLIMB_RADIUSR, -CLIMB_HEIGHT, CLIMB_HEIGHT, &shift_r);
		result_l = LaraTestClimbPos(item, coll->radius, -(coll->radius + CLIMB_RADIUSL), -CLIMB_HEIGHT, CLIMB_HEIGHT, &shift_l);
		item->pos.y -= CLICK(1);

		if (!result_r || !result_l)
			return;

		if (shift_r && shift_l)
		{
			if ((shift_r < 0) ^ (shift_l < 0))
				return;
			if (shift_r < 0 && shift_r < shift_l)
				shift_l = shift_r;
			else if (shift_r > 0 && shift_r > shift_l)
				shift_l = shift_r;
		}

		if (result_r == 1 && result_l == 1)
		{
			item->goalAnimState = AS_CLIMBDOWN;
			item->pos.y += shift_l;
		}
		else
		{
			item->goalAnimState = AS_HANG;
		}
	}
}

void lara_col_climbing(ITEM_INFO* item, COLL_INFO* coll)
{
	int result_r, result_l, shift_r, shift_l, ledge_r, ledge_l;
	int yshift, frame;

	if (LaraCheckForLetGo(item, coll))
		return;

	if (item->animNumber == 161)
	{
		frame = item->frameNumber - Anims[161].frameBase;
		if (frame == 0)
			yshift = 0;
		else if (frame == 28 || frame == 29)
			yshift = -CLICK(1);
		else if (frame == 57)
			yshift = -CLICK(2);
		else
			return;
	}
	else return;

	item->pos.y += yshift - CLICK(1);
	result_r = LaraTestClimbUpPos(item, coll->radius, coll->radius + CLIMB_RADIUSR, &shift_r, &ledge_r);
	result_l = LaraTestClimbUpPos(item, coll->radius, -(coll->radius + CLIMB_RADIUSL), &shift_l, &ledge_l);
	item->pos.y += CLICK(1);

	if (!result_r || !result_l || !CHK_ANY(InputStatus, IN_FORWARD))
	{
		item->goalAnimState = AS_CLIMBSTNC;
		if (yshift)
			AnimateLara(item);
		return;
	}

	if (result_r < 0 || result_l < 0)
	{
		item->goalAnimState = AS_CLIMBSTNC;
		AnimateLara(item);
		if (ABS(ledge_l - ledge_r) <= 120)
		{
			item->goalAnimState = AS_NULL;
			item->pos.y += (ledge_r + ledge_l) / 2 - CLICK(1);
		}
		return;
	}

	item->goalAnimState = AS_CLIMBING;
	item->pos.y -= yshift;
}

void lara_col_climbdown(ITEM_INFO* item, COLL_INFO* coll)
{
	int result_r, result_l, shift_r, shift_l;
	int yshift, frame;

	if (LaraCheckForLetGo(item, coll))
		return;

	if (item->animNumber == 168)
	{
		frame = item->frameNumber - Anims[168].frameBase;
		if (frame == 0)
			yshift = 0;
		else if (frame == 28 || frame == 29)
			yshift = CLICK(1);
		else if (frame == 57)
			yshift = CLICK(2);
		else
			return;
	}
	else
		return;

	item->pos.y += yshift + CLICK(1);
	result_r = LaraTestClimbPos(item, coll->radius, coll->radius + CLIMB_RADIUSR, -CLIMB_HEIGHT, CLIMB_HEIGHT, &shift_r);
	result_l = LaraTestClimbPos(item, coll->radius, -(coll->radius + CLIMB_RADIUSL), -CLIMB_HEIGHT, CLIMB_HEIGHT, &shift_l);
	item->pos.y -= CLICK(1);

	if (!result_r || !result_l || !CHK_ANY(InputStatus, IN_BACK))
	{
		item->goalAnimState = AS_CLIMBSTNC;
		if (yshift)
			AnimateLara(item);
		return;
	}

	if (shift_r && shift_l)
	{
		if ((shift_r < 0) ^ (shift_l < 0))
		{
			item->goalAnimState = AS_CLIMBSTNC;
			AnimateLara(item);
			return;
		}
		if (shift_r < 0 && shift_r < shift_l)
			shift_l = shift_r;
		else if (shift_r > 0 && shift_r > shift_l)
			shift_l = shift_r;
	}

	if (result_r == -1 || result_l == -1)
	{
		SetAnimation(item, 164, AS_HANG);
		AnimateLara(item);
		return;
	}

	item->goalAnimState = AS_CLIMBDOWN;
	item->pos.y -= yshift;
}

 /*
  * Inject function
  */
void Inject_LaraClimb() {
	INJECT(0x0042D8F0, lara_as_climbleft);
	INJECT(0x0042D930, lara_as_climbright);
	INJECT(0x0042D970, lara_as_climbstnc);
	INJECT(0x0042D9F0, lara_as_climbing);
	INJECT(0x0042DA10, lara_as_climbend);
	INJECT(0x0042DA30, lara_as_climbdown);
	INJECT(0x0042DA50, lara_col_climbleft);
	INJECT(0x0042DAB0, lara_col_climbright);
	INJECT(0x0042DB10, lara_col_climbstnc);
	INJECT(0x0042DD20, lara_col_climbing);
	//INJECT(----------, lara_col_climbend);
	INJECT(0x0042DE70, lara_col_climbdown);
	//INJECT(0x0042E010, LaraCheckForLetGo);
	//INJECT(0x0042E0C0, LaraTestClimb);
	//INJECT(0x0042E330, LaraTestClimbPos);
	//INJECT(0x0042E400, LaraDoClimbLeftRight);
	//INJECT(0x0042E4F0, LaraTestClimbUpPos);
}
