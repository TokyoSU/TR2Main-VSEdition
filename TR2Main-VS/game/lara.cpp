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
#include "game/lara.h"
#include "3dsystem/3d_gen.h"
#include "game/box.h"
#include "game/control.h"
#include "game/collide.h"
#include "game/draw.h"
#include "game/skidoo.h"
#include "game/larafire.h"
#include "game/laramisc.h"
#include "game/laraswim.h"
#include "game/laraclimb.h"
#include "game/larasurf.h"
#include "game/sound.h"
#include "global/vars.h"

#ifdef FEATURE_GAMEPLAY_FIXES
bool IsLowCeilingJumpFix = true;
#endif // FEATURE_GAMEPLAY_FIXES

void (*LaraCollisionFunctions[71])(ITEM_INFO* item, COLL_INFO* coll)
{
    lara_col_walk,
    lara_col_run,
    lara_col_stop,
    lara_col_forwardjump,
    lara_col_fastturn,
    lara_col_fastback,
    lara_col_turn_r,
    lara_col_turn_l,
    lara_col_death,
    lara_col_fastfall,
    lara_col_hang,
    lara_col_reach,
    lara_col_splat,
    lara_col_swim,
    lara_col_fastturn,
    lara_col_compress,
    lara_col_back,
    lara_col_swim,
    lara_col_swim,
    lara_col_special,
    lara_col_fastturn,
    lara_col_stepright,
    lara_col_stepleft,
    lara_col_roll2,
    lara_col_slide,
    lara_col_backjump,
    lara_col_rightjump,
    lara_col_leftjump,
    lara_col_upjump,
    lara_col_fallback,
    lara_col_hangleft,
    lara_col_hangright,
    lara_col_slideback,
    lara_col_surftread,
    lara_col_surfswim,
    lara_col_swim,
    lara_col_special,
    lara_col_special,
    lara_col_special,
    lara_col_special,
    lara_col_special,
    lara_col_special,
    lara_col_special,
    lara_col_special,
    lara_col_uwdeath,
    lara_col_roll,
    lara_col_null,
    lara_col_surfback,
    lara_col_surfleft,
    lara_col_surfright,
    lara_col_special,
    lara_col_special,
    lara_col_swandive,
    lara_col_fastdive,
    lara_col_special,
    lara_col_special,
    lara_col_climbstnc,
    lara_col_climbing,
    lara_col_climbleft,
    lara_col_null,
    lara_col_climbright,
    lara_col_climbdown,
    lara_col_null,
    lara_col_null,
    lara_col_null,
    lara_col_wade,
    lara_col_swim,
    lara_col_special,
    lara_col_null,
    lara_col_null,
    lara_col_null
};

void LaraAboveWater(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->old.x = item->pos.x;
	coll->old.y = item->pos.y;
	coll->old.z = item->pos.z;
	coll->oldAnimState = item->currentAnimState;
	coll->oldAnimNumber = item->animNumber;
	coll->oldFrameNumber = item->frameNumber;
	coll->radius = 100;
	coll->trigger = NULL;
	coll->flags &= ~(CF_LAVA_IS_PIT | CF_SLOPE_ARE_PITS | CF_SLOPE_ARE_WALLS);
	coll->flags |= (CF_ENABLE_BADDIE_PUSH | CF_ENABLE_SPAZ);

	if (CHK_ANY(InputStatus, IN_LOOK) && Lara.extra_anim == 0 && Lara.look)
		LookLeftRight();
	else
		ResetLook();
	Lara.look = TRUE;

	if (Lara.skidoo != -1)
	{
		SkidooControl();
		return;
	}

	if (Lara.extra_anim != 0)
		ExtraFunctions[item->currentAnimState](item, coll);
	else
		LaraControlFunctions[item->currentAnimState](item, coll);

	if (item->pos.rotZ >= -ANGLE(1) && item->pos.rotZ <= ANGLE(1))
		item->pos.rotZ = 0;
	else if (item->pos.rotZ < -ANGLE(1))
		item->pos.rotZ += ANGLE(1);
	else
		item->pos.rotZ -= ANGLE(1);

	if (Lara.turn_rate >= -ANGLE(2) && Lara.turn_rate <= ANGLE(2))
		Lara.turn_rate = 0;
	else if (Lara.turn_rate < -ANGLE(2))
		Lara.turn_rate += ANGLE(2);
	else
		Lara.turn_rate -= ANGLE(2);
	item->pos.rotY += Lara.turn_rate;

	AnimateLara(item);
	if (Lara.extra_anim == 0)
	{
		LaraBaddieCollision(item, coll);
		if (Lara.skidoo == -1)
			LaraCollisionFunctions[item->currentAnimState](item, coll);
	}

	UpdateLaraRoom(item, -381);
	LaraGun();
	TestTriggers(coll->trigger, FALSE);
}

void lara_as_walk(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hitPoints <= 0)
	{
		item->goalAnimState = AS_STOP;
		return;
	}

	if (CHK_ANY(InputStatus, IN_LOOK))
		LookUpDown();

	if (CHK_ANY(InputStatus, IN_LEFT))
	{
		Lara.turn_rate -= 409;
		if (Lara.turn_rate < -728)
			Lara.turn_rate = -728;
	}
	else if (CHK_ANY(InputStatus, IN_RIGHT))
	{
		Lara.turn_rate += 409;
		if (Lara.turn_rate > 728)
			Lara.turn_rate = 728;
	}

	if (CHK_ANY(InputStatus, IN_FORWARD))
	{
		if (Lara.water_status == LWS_Wade)
			item->goalAnimState = AS_WADE;
		else
		{
			if (!CHK_ANY(InputStatus, IN_SLOW))
				item->goalAnimState = AS_RUN;
			else
				item->goalAnimState = AS_WALK;
		}
	}
	else
	{
		item->goalAnimState = AS_STOP;
	}
}

void lara_as_run(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hitPoints <= 0)
	{
		item->goalAnimState = AS_DEATH;
		return;
	}

	if (CHK_ANY(InputStatus, IN_ROLL))
	{
		SetAnimation(item, 146, AS_ROLL);
		return;
	}

	if (CHK_ANY(InputStatus, IN_LOOK))
		LookUpDown();

	if (CHK_ANY(InputStatus, IN_LEFT))
	{
		Lara.turn_rate -= 409;
		if (Lara.turn_rate < -1456)
			Lara.turn_rate = -1456;
		item->pos.rotZ -= 273;
		if (item->pos.rotZ < -2002)
			item->pos.rotZ = -2002;
	}
	else if (CHK_ANY(InputStatus, IN_RIGHT))
	{
		Lara.turn_rate += 409;
		if (Lara.turn_rate > 1456)
			Lara.turn_rate = 1456;
		item->pos.rotZ += 273;
		if (item->pos.rotZ > 2002)
			item->pos.rotZ = 2002;
	}

	bool jump_ok = (item->animNumber == 0 || item->animNumber == 4) && item->animNumber != 6;
	if (CHK_ANY(InputStatus, IN_JUMP) && jump_ok && !item->gravity)
	{
		item->goalAnimState = AS_FORWARDJUMP;
	}
	else if (CHK_ANY(InputStatus, IN_FORWARD))
	{
		if (Lara.water_status == LWS_Wade)
		{
			item->goalAnimState = AS_WADE;
		}
		else
		{
			if (CHK_ANY(InputStatus, IN_SLOW))
				item->goalAnimState = AS_WALK;
			else
				item->goalAnimState = AS_RUN;
		}
	}
	else
	{
		item->goalAnimState = AS_STOP;
	}
}

void lara_as_stop(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hitPoints <= 0)
	{
		item->goalAnimState = AS_DEATH;
		return;
	}

	if (CHK_ANY(InputStatus, IN_ROLL) && Lara.water_status != LWS_Wade)
	{
		SetAnimation(item, 146, AS_ROLL);
		return;
	}

	item->goalAnimState = AS_STOP;

	if (CHK_ANY(InputStatus, IN_LOOK))
		LookUpDown();

	bool isQuicksand = CHK_ANY(Rooms[item->roomNumber].flags, ROOM_QUICKSAND);
	if (isQuicksand)
	{
		if (CHK_ANY(InputStatus, IN_LEFT))
			item->goalAnimState = AS_TURN_L;
		else if (CHK_ANY(InputStatus, IN_RIGHT))
			item->goalAnimState = AS_TURN_R;
	}
	else
	{
		if (CHK_ANY(InputStatus, IN_STEPL))
			item->goalAnimState = AS_STEPLEFT;
		else if (CHK_ANY(InputStatus, IN_STEPR))
			item->goalAnimState = AS_STEPRIGHT;
		else if (CHK_ANY(InputStatus, IN_LEFT))
			item->goalAnimState = AS_TURN_L;
		else if (CHK_ANY(InputStatus, IN_RIGHT))
			item->goalAnimState = AS_TURN_R;
	}

	if (Lara.water_status == LWS_Wade)
	{
		if (CHK_ANY(InputStatus, IN_JUMP) && !isQuicksand)
			item->goalAnimState = AS_COMPRESS;

		if (CHK_ANY(InputStatus, IN_FORWARD))
		{
			if (CHK_ANY(InputStatus, IN_SLOW) || isQuicksand)
				lara_as_wade(item, coll);
			else
				lara_as_walk(item, coll);
		}
		else if (CHK_ANY(InputStatus, IN_BACK))
			lara_as_back(item, coll);
	}
	else
	{
		if (CHK_ANY(InputStatus, IN_JUMP))
			item->goalAnimState = AS_COMPRESS;

		else if (CHK_ANY(InputStatus, IN_FORWARD))
		{
			if (CHK_ANY(InputStatus, IN_SLOW))
				lara_as_walk(item, coll);
			else
				lara_as_run(item, coll);
		}

		else if (CHK_ANY(InputStatus, IN_BACK))
		{
			if (CHK_ANY(InputStatus, IN_SLOW))
				lara_as_back(item, coll);
			else
				item->goalAnimState = AS_FASTBACK;
		}
	}
}

void lara_as_forwardjump(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->goalAnimState == AS_SWANDIVE || item->goalAnimState == AS_REACH)
		item->goalAnimState = AS_FORWARDJUMP;

	if (item->goalAnimState != AS_DEATH && item->goalAnimState != AS_STOP && item->goalAnimState != AS_RUN)
	{
		if (CHK_ANY(InputStatus, IN_ACTION) && Lara.gun_status == LGS_Armless)
			item->goalAnimState = AS_REACH;
		if (CHK_ANY(InputStatus, IN_ROLL) || CHK_ANY(InputStatus, IN_BACK))
			item->goalAnimState = AS_TWIST;
		if (CHK_ANY(InputStatus, IN_SLOW) && Lara.gun_status == LGS_Armless)
			item->goalAnimState = AS_SWANDIVE;
		if (item->fallSpeed > 131)
			item->goalAnimState = AS_FASTFALL;
	}

	if (CHK_ANY(InputStatus, IN_LEFT))
	{
		Lara.turn_rate -= 409;
		if (Lara.turn_rate < -546)
			Lara.turn_rate = -546;
	}
	else if (CHK_ANY(InputStatus, IN_RIGHT))
	{
		Lara.turn_rate += 409;
		if (Lara.turn_rate > 546)
			Lara.turn_rate = 546;
	}
}

void lara_as_fastback(ITEM_INFO* item, COLL_INFO* coll)
{
	item->goalAnimState = AS_STOP;

	if (CHK_ANY(InputStatus, IN_LEFT))
	{
		Lara.turn_rate -= 409;
		if (Lara.turn_rate < -1092)
			Lara.turn_rate = -1092;
	}
	else if (CHK_ANY(InputStatus, IN_RIGHT))
	{
		Lara.turn_rate += 409;
		if (Lara.turn_rate > 1092)
			Lara.turn_rate = 1092;
	}
}

void lara_as_turn_r(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hitPoints <= 0)
	{
		item->goalAnimState = AS_STOP;
		return;
	}

	Lara.turn_rate += 409;
	if (Lara.gun_status == LGS_Ready)
	{
		item->goalAnimState = AS_FASTTURN;
	}
	else if (Lara.turn_rate > 728)
	{
		if (!CHK_ANY(InputStatus, IN_SLOW) && !CHK_ANY(Rooms[item->roomNumber].flags, ROOM_QUICKSAND))
			item->goalAnimState = AS_FASTTURN;
		else
			Lara.turn_rate = 728;
	}

	if (CHK_ANY(InputStatus, IN_FORWARD))
	{
		if (Lara.water_status == LWS_Wade)
		{
			item->goalAnimState = AS_WADE;
		}
		else
		{
			if (CHK_ANY(InputStatus, IN_SLOW))
				item->goalAnimState = AS_WALK;
			else
				item->goalAnimState = AS_RUN;
		}
	}
	else if (!CHK_ANY(InputStatus, IN_RIGHT))
	{
		item->goalAnimState = AS_STOP;
	}
}

void lara_as_turn_l(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hitPoints <= 0)
	{
		item->goalAnimState = AS_STOP;
		return;
	}

	Lara.turn_rate -= 409;
	if (Lara.gun_status == LGS_Ready)
	{
		item->goalAnimState = AS_FASTTURN;
	}
	else if (Lara.turn_rate < -728)
	{
		if (!CHK_ANY(InputStatus, IN_SLOW) && !CHK_ANY(Rooms[item->roomNumber].flags, ROOM_QUICKSAND))
			item->goalAnimState = AS_FASTTURN;
		else
			Lara.turn_rate = -728;
	}

	if (CHK_ANY(InputStatus, IN_FORWARD))
	{
		if (Lara.water_status == LWS_Wade)
		{
			item->goalAnimState = AS_WADE;
		}
		else
		{
			if (CHK_ANY(InputStatus, IN_SLOW))
				item->goalAnimState = AS_WALK;
			else
				item->goalAnimState = AS_RUN;
		}
	}
	else if (!CHK_ANY(InputStatus, IN_LEFT))
	{
		item->goalAnimState = AS_STOP;
	}
}

void lara_as_death(ITEM_INFO* item, COLL_INFO* coll)
{
	Lara.look = FALSE;
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);
}

void lara_as_fastfall(ITEM_INFO* item, COLL_INFO* coll)
{
	item->speed = (item->speed * 95) / 100;
	if (item->fallSpeed == 154)
		PlaySoundEffect(30, &item->pos, NULL);
}

void lara_as_hang(ITEM_INFO* item, COLL_INFO* coll)
{
	if (CHK_ANY(InputStatus, IN_LOOK))
		LookUpDown();

	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);
	Camera.targetAngle = ANGLE(0);
	Camera.targetElevation = -ANGLE(60);

	if (CHK_ANY(InputStatus, IN_LEFT | IN_STEPL))
		item->goalAnimState = AS_HANGLEFT;
	else if (CHK_ANY(InputStatus, IN_RIGHT | IN_STEPR))
		item->goalAnimState = AS_HANGRIGHT;
}

void lara_as_reach(ITEM_INFO* item, COLL_INFO* coll)
{
	Camera.targetAngle = ANGLE(85);
	if (item->fallSpeed > 131)
		item->goalAnimState = AS_FASTFALL;
}

void lara_as_splat(ITEM_INFO* item, COLL_INFO* coll)
{
	Lara.look = FALSE;
}

void lara_as_compress(ITEM_INFO* item, COLL_INFO* coll)
{
	if (Lara.water_status != LWS_Wade)
	{
		if (CHK_ANY(InputStatus, IN_FORWARD) && (LaraFloorFront(item, item->pos.rotY, CLICK(1)) >= -384))
		{
			item->goalAnimState = AS_FORWARDJUMP;
			Lara.move_angle = item->pos.rotY;
		}
		else if (CHK_ANY(InputStatus, IN_LEFT) && (LaraFloorFront(item, item->pos.rotY - ANGLE(90), CLICK(1)) >= -384))
		{
			item->goalAnimState = AS_LEFTJUMP;
			Lara.move_angle = item->pos.rotY - ANGLE(90);
		}
		else if (CHK_ANY(InputStatus, IN_RIGHT) && (LaraFloorFront(item, item->pos.rotY + ANGLE(90), CLICK(1)) >= -384))
		{
			item->goalAnimState = AS_RIGHTJUMP;
			Lara.move_angle = item->pos.rotY + ANGLE(90);
		}
		else if (CHK_ANY(InputStatus, IN_BACK) && (LaraFloorFront(item, item->pos.rotY - ANGLE(180), CLICK(1)) >= -384))
		{
			item->goalAnimState = AS_BACKJUMP;
			Lara.move_angle = item->pos.rotY - ANGLE(180);
		}
	}

	if (item->fallSpeed > 131)
		item->goalAnimState = AS_FASTFALL;
}

void lara_as_back(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hitPoints <= 0)
	{
		item->goalAnimState = AS_STOP;
		return;
	}

	if (!CHK_ANY(InputStatus, IN_BACK) || (!CHK_ANY(InputStatus, IN_SLOW) && Lara.water_status != LWS_Wade))
		item->goalAnimState = AS_STOP;
	else
		item->goalAnimState = AS_BACK;

	if (CHK_ANY(InputStatus, IN_LEFT))
	{
		Lara.turn_rate -= 409;
		if (Lara.turn_rate < -728)
			Lara.turn_rate = -728;
	}
	else if (CHK_ANY(InputStatus, IN_RIGHT))
	{
		Lara.turn_rate += 409;
		if (Lara.turn_rate > 728)
			Lara.turn_rate = 728;
	}
}

void lara_as_null(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);
}

void lara_as_fastturn(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hitPoints <= 0)
	{
		item->goalAnimState = AS_STOP;
		return;
	}

	if (Lara.turn_rate < 0)
	{
		Lara.turn_rate = -1456;
		if (!CHK_ANY(InputStatus, IN_LEFT))
			item->goalAnimState = AS_STOP;
	}
	else
	{
		Lara.turn_rate = 1456;
		if (!CHK_ANY(InputStatus, IN_RIGHT))
			item->goalAnimState = AS_STOP;
	}
}

void lara_as_stepright(ITEM_INFO* item, COLL_INFO* coll)
{
	Lara.look = FALSE;
	if (item->hitPoints <= 0)
	{
		item->goalAnimState = AS_STOP;
		return;
	}

	if (!CHK_ANY(InputStatus, IN_STEPR))
		item->goalAnimState = AS_STOP;

	if (CHK_ANY(InputStatus, IN_LEFT))
	{
		Lara.turn_rate -= 409;
		if (Lara.turn_rate < -728)
			Lara.turn_rate = -728;
	}
	else if (CHK_ANY(InputStatus, IN_RIGHT))
	{
		Lara.turn_rate += 409;
		if (Lara.turn_rate > 728)
			Lara.turn_rate = 728;
	}
}

void lara_as_stepleft(ITEM_INFO* item, COLL_INFO* coll)
{
	Lara.look = FALSE;
	if (item->hitPoints <= 0)
	{
		item->goalAnimState = AS_STOP;
		return;
	}

	if (!CHK_ANY(InputStatus, IN_STEPL))
		item->goalAnimState = AS_STOP;

	if (CHK_ANY(InputStatus, IN_LEFT))
	{
		Lara.turn_rate -= 409;
		if (Lara.turn_rate < -728)
			Lara.turn_rate = -728;
	}
	else if (CHK_ANY(InputStatus, IN_RIGHT))
	{
		Lara.turn_rate += 409;
		if (Lara.turn_rate > 728)
			Lara.turn_rate = 728;
	}
}

void lara_as_slide(ITEM_INFO* item, COLL_INFO* coll)
{
	Camera.flags = NO_CHUNKY;
	Camera.targetElevation = -ANGLE(45);
	if (CHK_ANY(InputStatus, IN_JUMP) && !CHK_ANY(InputStatus, IN_BACK))
		item->goalAnimState = AS_FORWARDJUMP;
}

void lara_as_backjump(ITEM_INFO* item, COLL_INFO* coll)
{
	Camera.targetAngle = ANGLE(135);
	if (item->fallSpeed > 131)
		item->goalAnimState = AS_FASTFALL;
	else if (item->goalAnimState == AS_RUN)
		item->goalAnimState = AS_STOP;
	else if (CHK_ANY(InputStatus, IN_FORWARD | IN_ROLL) && item->goalAnimState != AS_STOP)
		item->goalAnimState = AS_TWIST;
}

void lara_as_rightjump(ITEM_INFO* item, COLL_INFO* coll)
{
	Lara.look = FALSE;
	if (item->fallSpeed > 131)
		item->goalAnimState = AS_FASTFALL;
	else if (CHK_ANY(InputStatus, IN_LEFT) && item->goalAnimState != AS_STOP)
		item->goalAnimState = AS_TWIST;
}

void lara_as_leftjump(ITEM_INFO* item, COLL_INFO* coll)
{
	Lara.look = FALSE;
	if (item->fallSpeed > 131)
		item->goalAnimState = AS_FASTFALL;
	else if (CHK_ANY(InputStatus, IN_RIGHT) && item->goalAnimState != AS_STOP)
		item->goalAnimState = AS_TWIST;
}

void lara_as_upjump(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->fallSpeed > 131)
		item->goalAnimState = AS_FASTFALL;
}

void lara_as_fallback(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->fallSpeed > 131)
		item->goalAnimState = AS_FASTFALL;
	if (CHK_ANY(InputStatus, IN_ACTION) && Lara.gun_status == LGS_Armless)
		item->goalAnimState = AS_REACH;
}

void lara_as_hangleft(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);
	Camera.targetAngle = ANGLE(0);
	Camera.targetElevation = -ANGLE(60);
	if (!CHK_ANY(InputStatus, IN_LEFT) && !CHK_ANY(InputStatus, IN_STEPL))
		item->goalAnimState = AS_HANG;
}

void lara_as_hangright(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);
	Camera.targetAngle = ANGLE(0);
	Camera.targetElevation = -ANGLE(60);
	if (!CHK_ANY(InputStatus, IN_RIGHT) && !CHK_ANY(InputStatus, IN_STEPR))
		item->goalAnimState = AS_HANG;
}

void lara_as_slideback(ITEM_INFO* item, COLL_INFO* coll)
{
	if (CHK_ANY(InputStatus, IN_JUMP) && !CHK_ANY(InputStatus, IN_FORWARD))
		item->goalAnimState = AS_BACKJUMP;
}

void lara_as_pushblock(ITEM_INFO* item, COLL_INFO* coll)
{
	Lara.look = FALSE;
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);
	Camera.flags = FOLLOW_CENTRE;
	Camera.targetAngle = ANGLE(35);
	Camera.targetElevation = -ANGLE(25);
}

void lara_as_ppready(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);
	Camera.targetAngle = ANGLE(75);
	if (!CHK_ANY(InputStatus, IN_ACTION))
		item->goalAnimState = AS_STOP;
}

void lara_as_pickup(ITEM_INFO* item, COLL_INFO* coll)
{
	Lara.look = FALSE;
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);
	Camera.flags = FOLLOW_CENTRE;
	Camera.targetAngle = -ANGLE(130);
	Camera.targetElevation = -ANGLE(15);
	Camera.targetDistance = BLOCK(1);
}

void lara_as_pickupflare(ITEM_INFO* item, COLL_INFO* coll)
{
	Lara.look = FALSE;
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);
	Camera.flags = FOLLOW_CENTRE;
	Camera.targetAngle = ANGLE(130);
	Camera.targetElevation = -ANGLE(15);
	Camera.targetDistance = BLOCK(1);

	if (item->frameNumber >= Anims[item->animNumber].frameEnd - 1)
		Lara.gun_status = LGS_Armless;
}

void lara_as_switchon(ITEM_INFO* item, COLL_INFO* coll)
{
	Lara.look = FALSE;
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);
	Camera.flags = FOLLOW_CENTRE;
	Camera.targetAngle = ANGLE(80);
	Camera.targetElevation = -ANGLE(25);
	Camera.targetDistance = BLOCK(1);
	Camera.speed = 6;
}

void lara_as_usekey(ITEM_INFO* item, COLL_INFO* coll)
{
	Lara.look = FALSE;
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);
	Camera.flags = FOLLOW_CENTRE;
	Camera.targetAngle = ANGLE(80);
	Camera.targetElevation = -ANGLE(25);
	Camera.targetDistance = BLOCK(1);
	Camera.speed = 6;
}

void lara_as_special(ITEM_INFO* item, COLL_INFO* coll)
{
	Camera.flags = FOLLOW_CENTRE;
	Camera.targetAngle = ANGLE(170);
	Camera.targetElevation = -ANGLE(25);
}

void lara_as_swandive(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);
	if (item->fallSpeed > 131 && item->goalAnimState != AS_DIVE)
		item->goalAnimState = AS_FASTDIVE;
}

void lara_as_fastdive(ITEM_INFO* item, COLL_INFO* coll)
{
	if (CHK_ANY(InputStatus, IN_ROLL) && item->goalAnimState == AS_FASTDIVE)
		item->goalAnimState = AS_TWIST;
	coll->flags &= ~CF_ENABLE_SPAZ;
	coll->flags |= CF_ENABLE_BADDIE_PUSH;
	item->speed = (item->speed * 95) / 100;
}

void lara_as_waterout(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->flags &= ~(CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);
	Camera.flags = FOLLOW_CENTRE;
}

void lara_as_wade(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hitPoints <= 0)
	{
		item->goalAnimState = AS_STOP;
		return;
	}

	Camera.targetElevation = -ANGLE(22);

	if (CHK_ANY(Rooms[item->roomNumber].flags, ROOM_QUICKSAND))
	{
		if (CHK_ANY(InputStatus, IN_LEFT))
		{
			Lara.turn_rate -= 409;
			if (Lara.turn_rate < -1456 >> 1)
				Lara.turn_rate = -1456 >> 1;
			item->pos.rotZ -= 273;
			if (item->pos.rotZ < -2002 >> 1)
				item->pos.rotZ = -2002 >> 1;
		}
		else if (CHK_ANY(InputStatus, IN_RIGHT))
		{
			Lara.turn_rate += 409;
			if (Lara.turn_rate > 1456 >> 1)
				Lara.turn_rate = 1456 >> 1;
			item->pos.rotZ += 273;
			if (item->pos.rotZ > 2002 >> 1)
				item->pos.rotZ = 2002 >> 1;
		}

		if (CHK_ANY(InputStatus, IN_FORWARD))
			item->goalAnimState = AS_WADE;
		else
			item->goalAnimState = AS_STOP;
	}
	else
	{
		if (CHK_ANY(InputStatus, IN_LEFT))
		{
			Lara.turn_rate -= 409;
			if (Lara.turn_rate < -1456)
				Lara.turn_rate = -1456;
			item->pos.rotZ -= 273;
			if (item->pos.rotZ < -2002)
				item->pos.rotZ = -2002;
		}
		else if (CHK_ANY(InputStatus, IN_RIGHT))
		{
			Lara.turn_rate += 409;
			if (Lara.turn_rate > 1456)
				Lara.turn_rate = 1456;
			item->pos.rotZ += 273;
			if (item->pos.rotZ > 2002)
				item->pos.rotZ = 2002;
		}

		if (CHK_ANY(InputStatus, IN_FORWARD))
		{
			if (Lara.water_status == LWS_AboveWater)
				item->goalAnimState = AS_RUN;
			else
				item->goalAnimState = AS_WADE;
		}
		else
		{
			item->goalAnimState = AS_STOP;
		}
	}
}

void lara_as_deathslide(ITEM_INFO* item, COLL_INFO* coll)
{
	Camera.targetAngle = ANGLE(70);

	short room_number = item->roomNumber;
	GetHeight(GetFloor(item->pos.x, item->pos.y, item->pos.z, &room_number), item->pos.x, item->pos.y, item->pos.z);
	coll->trigger = TriggerPtr;

	if (!CHK_ANY(InputStatus, IN_ACTION))
	{
		item->goalAnimState = AS_FORWARDJUMP;
		AnimateLara(item);
		item->gravity = TRUE;
		item->speed = 100;
		item->fallSpeed = 40;
		Lara.move_angle = item->pos.rotY;
	}
}

BOOL LaraFallen(ITEM_INFO* item, COLL_INFO* coll)
{
	if (coll->middle.floor > STEPUP_HEIGHT && Lara.water_status != LWS_Wade)
	{
		SetAnimation(item, 34, AS_FORWARDJUMP);
		item->fallSpeed = 0;
		item->gravity = TRUE;
		return TRUE;
	}
	return FALSE;
}

void lara_default_col(ITEM_INFO* item, COLL_INFO* coll)
{
	Lara.move_angle = item->pos.rotY;
	coll->badPos = 384;
	coll->badNeg = -384;
	coll->badCeiling = 0;
	coll->flags |= (CF_ENABLE_SPAZ | CF_ENABLE_BADDIE_PUSH);
	GetLaraCollisionInfo(item, coll);
}

void lara_col_null(ITEM_INFO* item, COLL_INFO* coll)
{
	lara_default_col(item, coll);
}

void lara_col_jumper(ITEM_INFO* item, COLL_INFO* coll) {
	coll->badPos = 32512;
	coll->badNeg = -384;
	coll->badCeiling = 192;

	GetLaraCollisionInfo(item, coll);
	LaraDeflectEdgeJump(item, coll);

	if (item->fallSpeed > 0 && coll->middle.floor <= 0) {
		item->goalAnimState = LaraLandedBad(item, coll) ? AS_DEATH : AS_STOP;
		item->fallSpeed = 0;
		item->gravity = 0;
		item->pos.y += coll->middle.floor;
	}

	// NOTE: Low ceiling check must be skipped because it produces the bug
	// Core Design removed this check in later game releases
#ifdef FEATURE_GAMEPLAY_FIXES
	if (IsLowCeilingJumpFix) return;
#endif // FEATURE_GAMEPLAY_FIXES
	if (ABS(coll->middle.ceiling - coll->middle.floor) < 762) {
		item->currentAnimState = AS_FASTFALL;
		item->goalAnimState = AS_FASTFALL;
		item->animNumber = 32;
		item->frameNumber = Anims[item->animNumber].frameBase + 1;
		item->speed /= 4;
		Lara.move_angle += PHD_180;
		if (item->fallSpeed <= 0)
			item->fallSpeed = 1;
	}
}

void lara_col_kick(ITEM_INFO* item, COLL_INFO* coll)
{
}

void GetLaraCollisionInfo(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->facing = Lara.move_angle;
	GetCollisionInfo(coll, item->pos.x, item->pos.y, item->pos.z, item->roomNumber, 762);
}

void LaraSlideSlope(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->badPos = 32512;
	coll->badNeg = -512;
	coll->badCeiling = 0;
	GetLaraCollisionInfo(item, coll);
	if (LaraHitCeiling(item, coll))
		return;
	LaraDeflectEdge(item, coll);
	if (coll->middle.floor > 200)
	{
		SetAnimation(item,
			item->currentAnimState == AS_SLIDE ? 34 : 93,
			item->currentAnimState == AS_SLIDE ? AS_FORWARDJUMP : AS_FALLBACK
		);
		item->fallSpeed = 0;
		item->gravity = TRUE;
		return;
	}
	TestLaraSlide(item, coll);
	item->pos.y += coll->middle.floor;

	if ((ABS(coll->xTilt)) <= 2 && (ABS(coll->zTilt)) <= 2)
		item->goalAnimState = AS_STOP;
}

// NOTE: Custom implementation to get absolute position for lara joint, the old one was messy and was not accurate (it also didn't include all bones).
void GetLaraJointAbsPosition(PHD_VECTOR* pos, int meshID)
{
	ITEM_INFO* item = LaraItem;
	OBJECT_INFO* obj = &Objects[item->objectID];
	UINT16* rot1 = NULL, *rot1copy = NULL;
	int* bones = &AnimBones[obj->boneIndex];
	int rate = 0, frac = 0, frame = 0;
	short* frames[2]{}, *framePtr = NULL, *hitframePtr = NULL;
	short interp = 0;

	frac = GetFrames(item, frames, &rate);
	if (frac)
	{
		GetLaraJointAbsPositionInterpolated(item, pos, frames[0], frames[1], frac, rate, meshID);
		return;
	}

	short hitDir = Lara.hit_direction;
	if (hitDir >= 0)
	{
		switch (hitDir)
		{
		case 0:
			hitframePtr = Anims[125].framePtr;
			interp = Anims[125].interpolation;
			break;
		case 1:
			hitframePtr = Anims[126].framePtr;
			interp = Anims[126].interpolation;
			break;
		case 2:
			hitframePtr = Anims[127].framePtr;
			interp = Anims[127].interpolation;
			break;
		case 3:
			hitframePtr = Anims[128].framePtr;
			interp = Anims[128].interpolation;
			break;
		}
		if (hitframePtr == NULL)
			return;
		framePtr = &hitframePtr[Lara.hit_frame * (interp >> 8)];
	}
	else
	{
		framePtr = frames[0];
	}
	if (framePtr == NULL)
		return;

	phd_PushUnitMatrix();
	PhdMatrixPtr->_03 = 0;
	PhdMatrixPtr->_13 = 0;
	PhdMatrixPtr->_23 = 0;
	phd_RotYXZ(item->pos.rotY, item->pos.rotX, item->pos.rotZ);

	phd_PushMatrix();
	bones = &AnimBones[obj->boneIndex];
	rot1 = (UINT16*)framePtr + 9;

	phd_TranslateRel(framePtr[6], framePtr[7], framePtr[8]);
	phd_RotYXZsuperpack(&rot1, 0);
	if (meshID == LM_Hips)
	{
		phd_TranslateRel(pos->x, pos->y, pos->z);
		pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
		pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
		pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
	}

	phd_PushMatrix();
	phd_TranslateRel(bones[1], bones[2], bones[3]);
	phd_RotYXZsuperpack(&rot1, 0);
	if (meshID == LM_ThighL)
	{
		phd_TranslateRel(pos->x, pos->y, pos->z);
		pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
		pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
		pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
	}
	phd_TranslateRel(bones[5], bones[6], bones[7]);
	phd_RotYXZsuperpack(&rot1, 0);
	if (meshID == LM_CalfL)
	{
		phd_TranslateRel(pos->x, pos->y, pos->z);
		pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
		pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
		pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
	}
	phd_TranslateRel(bones[9], bones[10], bones[11]);
	phd_RotYXZsuperpack(&rot1, 0);
	if (meshID == LM_FootL)
	{
		phd_TranslateRel(pos->x, pos->y, pos->z);
		pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
		pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
		pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
	}
	phd_PopMatrix();

	phd_PushMatrix();
	phd_TranslateRel(bones[13], bones[W2V_SHIFT], bones[15]);
	phd_RotYXZsuperpack(&rot1, 0);
	if (meshID == LM_ThighR)
	{
		phd_TranslateRel(pos->x, pos->y, pos->z);
		pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
		pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
		pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
	}
	phd_TranslateRel(bones[17], bones[18], bones[19]);
	phd_RotYXZsuperpack(&rot1, 0);
	if (meshID == LM_CalfR)
	{
		phd_TranslateRel(pos->x, pos->y, pos->z);
		pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
		pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
		pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
	}
	phd_TranslateRel(bones[21], bones[22], bones[23]);
	phd_RotYXZsuperpack(&rot1, 0);
	if (meshID == LM_FootR)
	{
		phd_TranslateRel(pos->x, pos->y, pos->z);
		pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
		pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
		pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
	}
	phd_PopMatrix();

	phd_TranslateRel(bones[25], bones[26], bones[27]);
	if (Lara.weapon_item != -1 && Lara.gun_type == LGT_M16
	&& (Items[Lara.weapon_item].currentAnimState == 0
	|| Items[Lara.weapon_item].currentAnimState == 2
	|| Items[Lara.weapon_item].currentAnimState == 4))
	{
		frame = Lara.right_arm.frame_number * (Anims[Lara.right_arm.anim_number].interpolation >> 8) + 9;
		rot1 = (UINT16*)&Lara.right_arm.frame_base[frame];
		phd_RotYXZsuperpack(&rot1, 7);
	}
	else {
		phd_RotYXZsuperpack(&rot1, 0);
	}
	phd_RotYXZ(Lara.torso_y_rot, Lara.torso_x_rot, Lara.torso_z_rot);
	if (meshID == LM_Torso)
	{
		phd_TranslateRel(pos->x, pos->y, pos->z);
		pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
		pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
		pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
	}

	phd_PushMatrix();
	phd_TranslateRel(bones[53], bones[54], bones[55]);
	rot1copy = rot1;
	phd_RotYXZsuperpack(&rot1, 6);
	rot1 = rot1copy;
	phd_RotYXZ(Lara.head_y_rot, Lara.head_x_rot, Lara.head_z_rot);
	if (meshID == LM_Head)
	{
		phd_TranslateRel(pos->x, pos->y, pos->z);
		pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
		pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
		pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
	}
	phd_PopMatrix();

	int gunType = LGT_Unarmed;
	if (Lara.gun_status == LGS_Ready
	|| Lara.gun_status == LGS_Special
	|| Lara.gun_status == LGS_Draw
	|| Lara.gun_status == LGS_Undraw)
	{
		gunType = Lara.gun_type;
	}

	switch (gunType) {
	case LGT_Unarmed:
	case LGT_Flare:
		phd_PushMatrix();
		phd_TranslateRel(bones[29], bones[30], bones[31]);
		phd_RotYXZsuperpack(&rot1, 0);
		if (meshID == LM_UArmR)
		{
			phd_TranslateRel(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel(bones[33], bones[34], bones[35]);
		phd_RotYXZsuperpack(&rot1, 0);
		if (meshID == LM_LArmR)
		{
			phd_TranslateRel(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel(bones[37], bones[38], bones[39]);
		phd_RotYXZsuperpack(&rot1, 0);
		if (meshID == LM_HandR)
		{
			phd_TranslateRel(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bones[41], bones[42], bones[43]);
		if (Lara.flare_control_left) {
			frame = (Anims[Lara.left_arm.anim_number].interpolation >> 8) * (Lara.left_arm.frame_number - Anims[Lara.left_arm.anim_number].frameBase) + 9;
			rot1 = (UINT16*)&Lara.left_arm.frame_base[frame];
			phd_RotYXZsuperpack(&rot1, 11);
		}
		else {
			phd_RotYXZsuperpack(&rot1, 0);
		}
		if (meshID == LM_UArmL)
		{
			phd_TranslateRel(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}

		phd_TranslateRel(bones[45], bones[46], bones[47]);
		phd_RotYXZsuperpack(&rot1, 0);
		if (meshID == LM_LArmL)
		{
			phd_TranslateRel(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}

		phd_TranslateRel(bones[49], bones[50], bones[51]);
		phd_RotYXZsuperpack(&rot1, 0);
		if (meshID == LM_HandL)
		{
			phd_TranslateRel(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		break;
	case LGT_Pistols:
	case LGT_Magnums:
	case LGT_Uzis:
		phd_PushMatrix();
		phd_TranslateRel(bones[29], bones[30], bones[31]);
		phd_RotYXZ(Lara.right_arm.y_rot, Lara.right_arm.x_rot, Lara.right_arm.z_rot);
		frame = (Anims[Lara.right_arm.anim_number].interpolation >> 8) * (Lara.right_arm.frame_number - Anims[Lara.right_arm.anim_number].frameBase) + 9;
		rot1 = (UINT16*)&Lara.right_arm.frame_base[frame];
		phd_RotYXZsuperpack(&rot1, 8);
		if (meshID == LM_UArmR)
		{
			phd_TranslateRel(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel(bones[33], bones[34], bones[35]);
		phd_RotYXZsuperpack(&rot1, 0);
		if (meshID == LM_LArmR)
		{
			phd_TranslateRel(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel(bones[37], bones[38], bones[39]);
		phd_RotYXZsuperpack(&rot1, 0);
		if (meshID == LM_HandR)
		{
			phd_TranslateRel(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bones[41], bones[42], bones[43]);
		phd_RotYXZ(Lara.left_arm.y_rot, Lara.left_arm.x_rot, Lara.left_arm.z_rot);
		frame = (Anims[Lara.left_arm.anim_number].interpolation >> 8) * (Lara.left_arm.frame_number - Anims[Lara.left_arm.anim_number].frameBase) + 9;
		rot1 = (UINT16*)&Lara.left_arm.frame_base[frame];
		phd_RotYXZsuperpack(&rot1, 11);
		if (meshID == LM_UArmL)
		{
			phd_TranslateRel(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel(bones[45], bones[46], bones[47]);
		phd_RotYXZsuperpack(&rot1, 0);
		if (meshID == LM_LArmL)
		{
			phd_TranslateRel(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel(bones[49], bones[50], bones[51]);
		phd_RotYXZsuperpack(&rot1, 0);
		if (meshID == LM_HandL)
		{
			phd_TranslateRel(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		break;
	case LGT_Shotgun:
	case LGT_M16:
	case LGT_Grenade:
	case LGT_Harpoon:
		phd_PushMatrix();
		phd_TranslateRel(bones[29], bones[30], bones[31]);
		frame = Lara.right_arm.frame_number * (Anims[Lara.right_arm.anim_number].interpolation >> 8) + 9;
		rot1 = (UINT16*)&Lara.right_arm.frame_base[frame];
		phd_RotYXZsuperpack(&rot1, 8);
		if (meshID == LM_UArmR)
		{
			phd_TranslateRel(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel(bones[33], bones[34], bones[35]);
		phd_RotYXZsuperpack(&rot1, 0);
		if (meshID == LM_LArmL)
		{
			phd_TranslateRel(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel(bones[37], bones[38], bones[39]);
		phd_RotYXZsuperpack(&rot1, 0);
		if (meshID == LM_HandR)
		{
			phd_TranslateRel(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bones[41], bones[42], bones[43]);
		phd_RotYXZsuperpack(&rot1, 0);
		if (meshID == LM_UArmL)
		{
			phd_TranslateRel(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel(bones[45], bones[46], bones[47]);
		phd_RotYXZsuperpack(&rot1, 0);
		if (meshID == LM_LArmL)
		{
			phd_TranslateRel(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel(bones[49], bones[50], bones[51]);
		phd_RotYXZsuperpack(&rot1, 0);
		if (meshID == LM_HandL)
		{
			phd_TranslateRel(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		break;
	default:
		break;
	}
	phd_PopMatrix();
	phd_PopMatrix();
	phd_PopMatrix();
}

void GetLaraJointAbsPositionInterpolated(ITEM_INFO* item, PHD_VECTOR* pos, short* frame1, short* frame2, int frac, int rate, int meshID)
{
	OBJECT_INFO* obj = &Objects[item->objectID];
	UINT16* rot1, *rot2, *rot1copy, *rot2copy;
	int frame, *bones;

	phd_PushUnitMatrix();
	PhdMatrixPtr->_03 = 0;
	PhdMatrixPtr->_13 = 0;
	PhdMatrixPtr->_23 = 0;
	phd_RotYXZ(item->pos.rotY, item->pos.rotX, item->pos.rotZ);

	phd_PushMatrix();
	bones = &AnimBones[obj->boneIndex];
	rot1 = (UINT16*)frame1 + 9;
	rot2 = (UINT16*)frame2 + 9;

	InitInterpolate(frac, rate);
	phd_TranslateRel_ID(frame1[6], frame1[7], frame1[8], frame2[6], frame2[7], frame2[8]);
	phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
	if (meshID == LM_Hips)
	{
		phd_TranslateRel_I(pos->x, pos->y, pos->z);
		pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
		pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
		pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
	}

	phd_PushMatrix_I();
	phd_TranslateRel_I(bones[1], bones[2], bones[3]);
	phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
	if (meshID == LM_ThighL)
	{
		phd_TranslateRel_I(pos->x, pos->y, pos->z);
		pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
		pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
		pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
	}
	phd_TranslateRel_I(bones[5], bones[6], bones[7]);
	phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
	if (meshID == LM_CalfL)
	{
		phd_TranslateRel_I(pos->x, pos->y, pos->z);
		pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
		pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
		pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
	}
	phd_TranslateRel_I(bones[9], bones[10], bones[11]);
	phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
	if (meshID == LM_FootL)
	{
		phd_TranslateRel_I(pos->x, pos->y, pos->z);
		pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
		pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
		pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
	}
	phd_PopMatrix_I();

	phd_PushMatrix_I();
	phd_TranslateRel_I(bones[13], bones[W2V_SHIFT], bones[15]);
	phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
	if (meshID == LM_ThighR)
	{
		phd_TranslateRel_I(pos->x, pos->y, pos->z);
		pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
		pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
		pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
	}
	phd_TranslateRel_I(bones[17], bones[18], bones[19]);
	phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
	if (meshID == LM_CalfR)
	{
		phd_TranslateRel_I(pos->x, pos->y, pos->z);
		pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
		pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
		pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
	}
	phd_TranslateRel_I(bones[21], bones[22], bones[23]);
	phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
	if (meshID == LM_FootR)
	{
		phd_TranslateRel_I(pos->x, pos->y, pos->z);
		pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
		pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
		pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
	}
	phd_PopMatrix_I();

	phd_TranslateRel_I(bones[25], bones[26], bones[27]);
	if (Lara.weapon_item != -1 && Lara.gun_type == LGT_M16
	&& (Items[Lara.weapon_item].currentAnimState == 0
	||  Items[Lara.weapon_item].currentAnimState == 2
	||  Items[Lara.weapon_item].currentAnimState == 4))
	{
		frame = Lara.right_arm.frame_number * (Anims[Lara.right_arm.anim_number].interpolation >> 8) + 9;
		rot1 = rot2 = (UINT16*)&Lara.right_arm.frame_base[frame];
		phd_RotYXZsuperpack_I(&rot1, &rot2, 7);
	}
	else {
		phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
	}
	phd_RotYXZ_I(Lara.torso_y_rot, Lara.torso_x_rot, Lara.torso_z_rot);
	if (meshID == LM_Torso)
	{
		phd_TranslateRel_I(pos->x, pos->y, pos->z);
		pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
		pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
		pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
	}

	phd_PushMatrix_I();
	phd_TranslateRel_I(bones[53], bones[54], bones[55]);
	rot1copy = rot1;
	rot2copy = rot2;
	phd_RotYXZsuperpack_I(&rot1, &rot2, 6);
	rot1 = rot1copy;
	rot2 = rot2copy;
	phd_RotYXZ_I(Lara.head_y_rot, Lara.head_x_rot, Lara.head_z_rot);
	if (meshID == LM_Head)
	{
		phd_TranslateRel_I(pos->x, pos->y, pos->z);
		pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
		pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
		pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
	}
	phd_PopMatrix_I();

	int gunType = LGT_Unarmed;
	if (Lara.gun_status == LGS_Ready
	|| Lara.gun_status == LGS_Special
	|| Lara.gun_status == LGS_Draw
	|| Lara.gun_status == LGS_Undraw)
	{
		gunType = Lara.gun_type;
	}

	switch (gunType) {
	case LGT_Unarmed:
	case LGT_Flare:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bones[29], bones[30], bones[31]);
		phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
		if (meshID == LM_UArmR)
		{
			phd_TranslateRel_I(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel_I(bones[33], bones[34], bones[35]);
		phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
		if (meshID == LM_LArmR)
		{
			phd_TranslateRel_I(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel_I(bones[37], bones[38], bones[39]);
		phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
		if (meshID == LM_HandR)
		{
			phd_TranslateRel_I(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bones[41], bones[42], bones[43]);
		if (Lara.flare_control_left) {
			frame = (Anims[Lara.left_arm.anim_number].interpolation >> 8) * (Lara.left_arm.frame_number - Anims[Lara.left_arm.anim_number].frameBase) + 9;
			rot1 = rot2 = (UINT16*)&Lara.left_arm.frame_base[frame];
			phd_RotYXZsuperpack_I(&rot1, &rot2, 11);
		}
		else {
			phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
		}
		if (meshID == LM_UArmL)
		{
			phd_TranslateRel_I(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}

		phd_TranslateRel_I(bones[45], bones[46], bones[47]);
		phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
		if (meshID == LM_LArmL)
		{
			phd_TranslateRel_I(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}

		phd_TranslateRel_I(bones[49], bones[50], bones[51]);
		phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
		if (meshID == LM_HandL)
		{
			phd_TranslateRel_I(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		break;
	case LGT_Pistols:
	case LGT_Magnums:
	case LGT_Uzis:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bones[29], bones[30], bones[31]);
		InterpolateArmMatrix();
		phd_RotYXZ(Lara.right_arm.y_rot, Lara.right_arm.x_rot, Lara.right_arm.z_rot);
		frame = (Anims[Lara.right_arm.anim_number].interpolation >> 8) * (Lara.right_arm.frame_number - Anims[Lara.right_arm.anim_number].frameBase) + 9;
		rot1 = (UINT16*)&Lara.right_arm.frame_base[frame];
		phd_RotYXZsuperpack(&rot1, 8);
		if (meshID == LM_UArmR)
		{
			phd_TranslateRel_I(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel(bones[33], bones[34], bones[35]);
		phd_RotYXZsuperpack(&rot1, 0);
		if (meshID == LM_LArmR)
		{
			phd_TranslateRel_I(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel(bones[37], bones[38], bones[39]);
		phd_RotYXZsuperpack(&rot1, 0);
		if (meshID == LM_HandR)
		{
			phd_TranslateRel_I(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bones[41], bones[42], bones[43]);
		InterpolateArmMatrix();
		phd_RotYXZ(Lara.left_arm.y_rot, Lara.left_arm.x_rot, Lara.left_arm.z_rot);
		frame = (Anims[Lara.left_arm.anim_number].interpolation >> 8) * (Lara.left_arm.frame_number - Anims[Lara.left_arm.anim_number].frameBase) + 9;
		rot1 = (UINT16*)&Lara.left_arm.frame_base[frame];
		phd_RotYXZsuperpack(&rot1, 11);
		if (meshID == LM_UArmL)
		{
			phd_TranslateRel_I(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel(bones[45], bones[46], bones[47]);
		phd_RotYXZsuperpack(&rot1, 0);
		if (meshID == LM_LArmL)
		{
			phd_TranslateRel_I(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel(bones[49], bones[50], bones[51]);
		phd_RotYXZsuperpack(&rot1, 0);
		if (meshID == LM_HandL)
		{
			phd_TranslateRel_I(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		break;
	case LGT_Shotgun:
	case LGT_M16:
	case LGT_Grenade:
	case LGT_Harpoon:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bones[29], bones[30], bones[31]);
		frame = Lara.right_arm.frame_number * (Anims[Lara.right_arm.anim_number].interpolation >> 8) + 9;
		rot1 = rot2 = (UINT16*)&Lara.right_arm.frame_base[frame];
		phd_RotYXZsuperpack_I(&rot1, &rot2, 8);
		if (meshID == LM_UArmR)
		{
			phd_TranslateRel_I(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel_I(bones[33], bones[34], bones[35]);
		phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
		if (meshID == LM_LArmR)
		{
			phd_TranslateRel_I(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel_I(bones[37], bones[38], bones[39]);
		phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
		if (meshID == LM_HandR)
		{
			phd_TranslateRel_I(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bones[41], bones[42], bones[43]);
		phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
		if (meshID == LM_UArmL)
		{
			phd_TranslateRel_I(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel_I(bones[45], bones[46], bones[47]);
		phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
		if (meshID == LM_LArmL)
		{
			phd_TranslateRel_I(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		phd_TranslateRel_I(bones[49], bones[50], bones[51]);
		phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
		if (meshID == LM_HandL)
		{
			phd_TranslateRel_I(pos->x, pos->y, pos->z);
			pos->x = (PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			pos->y = (PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			pos->z = (PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
		}
		break;
	default:
		break;
	}
	phd_PopMatrix_I();
	phd_PopMatrix();
	phd_PopMatrix();
}

/*
 * Inject function
 */
void Inject_Lara() {
	INJECT(0x00427560, LaraAboveWater);

	//INJECT(0x00427700, LookUpDown);
	//INJECT(0x00427770, LookLeftRight);
	//INJECT(0x004277F0, ResetLook);

	INJECT(0x00427880, lara_as_walk);
	INJECT(0x00427910, lara_as_run);
	INJECT(0x00427A60, lara_as_stop);
	INJECT(0x00427BB0, lara_as_forwardjump);
	INJECT(0x00427C90, lara_as_fastback);
	INJECT(0x00427CF0, lara_as_turn_r);
	INJECT(0x00427D80, lara_as_turn_l);
	INJECT(0x00427E10, lara_as_death);
	INJECT(0x00427E30, lara_as_fastfall);
	INJECT(0x00427E70, lara_as_hang);
	INJECT(0x00427ED0, lara_as_reach);
	INJECT(0x00427EF0, lara_as_splat);
	INJECT(0x00427F00, lara_as_compress);
	INJECT(0x00428010, lara_as_back);
	INJECT(0x004280A0, lara_as_null);
	INJECT(0x004280B0, lara_as_fastturn);
	INJECT(0x00428100, lara_as_stepright);
	INJECT(0x00428180, lara_as_stepleft);
	INJECT(0x00428200, lara_as_slide);
	INJECT(0x00428230, lara_as_backjump);
	INJECT(0x00428280, lara_as_rightjump);
	INJECT(0x004282C0, lara_as_leftjump);
	INJECT(0x00428300, lara_as_upjump);
	INJECT(0x00428320, lara_as_fallback);
	INJECT(0x00428350, lara_as_hangleft);
	INJECT(0x00428390, lara_as_hangright);
	INJECT(0x004283D0, lara_as_slideback);
	INJECT(0x004283F0, lara_as_pushblock);
	INJECT(0x00428420, lara_as_ppready);
	INJECT(0x00428450, lara_as_pickup);
	INJECT(0x00428480, lara_as_pickupflare);
	INJECT(0x004284E0, lara_as_switchon);
	INJECT(0x00428520, lara_as_usekey);
	INJECT(0x00428550, lara_as_special);
	INJECT(0x00428570, lara_as_swandive);
	INJECT(0x004285A0, lara_as_fastdive);
	INJECT(0x00428600, lara_as_waterout);
	INJECT(0x00428620, lara_as_wade);
	INJECT(0x004286F0, lara_as_deathslide);
	
	//INJECT(0x00428790, extra_as_breath);
	//INJECT(0x004287E0, extra_as_yetikill);
	//INJECT(0x00428830, extra_as_sharkkill);
	//INJECT(0x004288D0, extra_as_airlock);
	//INJECT(0x004288F0, extra_as_gongbong);
	//INJECT(0x00428910, extra_as_dinokill);
	//INJECT(0x00428970, extra_as_pulldagger);
	//INJECT(0x00428A30, extra_as_startanim);
	//INJECT(0x00428A80, extra_as_starthouse);
	//INJECT(0x00428B30, extra_as_finalanim);
	
	INJECT(0x00428BE0, LaraFallen);
	//INJECT(0x00428C40, LaraCollideStop);
	
	//INJECT(0x00428D00, lara_col_walk);
	//INJECT(0x00428EA0, lara_col_run);
	//INJECT(0x00429020, lara_col_stop);
	//INJECT(0x004290B0, lara_col_forwardjump);
	//INJECT(0x00429190, lara_col_fastback);
	//INJECT(0x00429250, lara_col_turn_r);
	//INJECT(0x004292F0, lara_col_turn_l);
	//INJECT(0x00429310, lara_col_death);
	//INJECT(0x00429380, lara_col_fastfall);
	//INJECT(0x00429420, lara_col_hang);
	//INJECT(0x00429550, lara_col_reach);
	//INJECT(0x004295E0, lara_col_splat);
	//INJECT(0x00429640, lara_col_compress);
	//INJECT(0x004296E0, lara_col_back);
	//INJECT(0x004297E0, lara_col_fastturn);
	//INJECT(0x00429800, lara_col_stepright);
	//INJECT(0x004298C0, lara_col_stepleft);
	//INJECT(0x004298E0, lara_col_slide);
	//INJECT(0x00429900, lara_col_backjump);
	//INJECT(0x00429930, lara_col_rightjump);
	//INJECT(0x00429960, lara_col_leftjump);
	//INJECT(0x00429990, lara_col_upjump);
	//INJECT(0x00429AD0, lara_col_fallback);
	//INJECT(0x00429B60, lara_col_hangleft);
	//INJECT(0x00429BA0, lara_col_hangright);
	//INJECT(0x00429BE0, lara_col_slideback);
	//INJECT(0x00429C10, lara_col_roll);
	//INJECT(0x00429CB0, lara_col_roll2);
	//INJECT(0x00429D80, lara_col_special);
	//INJECT(0x00429DA0, lara_col_swandive);
	//INJECT(0x00429E10, lara_col_fastdive);
	//INJECT(0x00429E90, lara_col_wade);
	INJECT(0x0042A000, lara_default_col);
	INJECT(0x0042A040, lara_col_jumper);
	INJECT(0x0042A120, lara_col_kick);
	
	INJECT(0x0042A130, GetLaraCollisionInfo);
	INJECT(0x0042A170, LaraSlideSlope);
	//INJECT(0x0042A260, LaraHitCeiling);
	//INJECT(0x0042A2D0, LaraDeflectEdge);
	//INJECT(0x0042A350, LaraDeflectEdgeJump);
	//INJECT(0x0042A4D0, LaraSlideEdgeJump);
	//INJECT(0x0042A5C0, TestWall);
	//INJECT(0x0042A6D0, LaraTestHangOnClimbWall);
	//INJECT(0x0042A7E0, LaraTestClimbStance);
	//INJECT(0x0042A8A0, LaraHangTest);
	//INJECT(0x0042AC00, LaraTestEdgeCatch);
	//INJECT(0x0042ACB0, LaraTestHangJumpUp);
	//INJECT(0x0042AE20, LaraTestHangJump);
	//INJECT(0x0042AFC0, TestHangSwingIn);
	//INJECT(0x0042B080, TestLaraVault);
	//INJECT(0x0042B370, TestLaraSlide);
	//INJECT(0x0042B4A0, LaraFloorFront);
	//INJECT(0x0042B520, LaraLandedBad);

	INJECT(0x0042B5E0, GetLaraJointAbsPosition);
	INJECT(0x0042B970, GetLaraJointAbsPositionInterpolated);
}