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

#ifndef LARA_H_INCLUDED
#define LARA_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */

void LaraAboveWater(ITEM_INFO* item, COLL_INFO* coll); // 0x00427560
#define LookUpDown ((void(__cdecl*)(void)) 0x00427700)
#define LookLeftRight ((void(__cdecl*)(void)) 0x00427770)
#define ResetLook ((void(__cdecl*)(void)) 0x004277F0)

  // 0x00427880:		lara_as_walk
  // 0x00427910:		lara_as_run
  // 0x00427A60:		lara_as_stop
  // 0x00427BB0:		lara_as_forwardjump
  // ----------:		lara_as_pose
  // 0x00427C90:		lara_as_fastback
  // 0x00427CF0:		lara_as_turn_r
  // 0x00427D80:		lara_as_turn_l
  // 0x00427E10:		lara_as_death
  // 0x00427E30:		lara_as_fastfall
  // 0x00427E70:		lara_as_hang
  // 0x00427ED0:		lara_as_reach
  // 0x00427EF0:		lara_as_splat
  // ----------:		lara_as_land
  // 0x00427F00:		lara_as_compress
  // 0x00428010:		lara_as_back
  // 0x004280A0:		lara_as_null
  // 0x004280B0:		lara_as_fastturn
  // 0x00428100:		lara_as_stepright
  // 0x00428180:		lara_as_stepleft
  // 0x00428200:		lara_as_slide
  // 0x00428230:		lara_as_backjump
  // 0x00428280:		lara_as_rightjump
  // 0x004282C0:		lara_as_leftjump
  // 0x00428300:		lara_as_upjump
  // 0x00428320:		lara_as_fallback
  // 0x00428350:		lara_as_hangleft
  // 0x00428390:		lara_as_hangright
  // 0x004283D0:		lara_as_slideback
  // 0x004283F0:		lara_as_pushblock
  // ----------:		lara_as_pullblock
  // 0x00428420:		lara_as_ppready
  // 0x00428450:		lara_as_pickup
  // 0x00428480:		lara_as_pickupflare
  // 0x004284E0:		lara_as_switchon
  // ----------:		lara_as_switchoff
  // 0x00428520:		lara_as_usekey
  // ----------:		lara_as_usepuzzle
  // ----------:		lara_as_roll
  // ----------:		lara_as_roll2
  // 0x00428550:		lara_as_special
  // ----------:		lara_as_usemidas
  // ----------:		lara_as_diemidas
  // 0x00428570:		lara_as_swandive
  // 0x004285A0:		lara_as_fastdive
  // ----------:		lara_as_gymnast
  // 0x00428600:		lara_as_waterout
  // ----------:		lara_as_laratest1
  // ----------:		lara_as_laratest2
  // ----------:		lara_as_laratest3
  // 0x00428620:		lara_as_wade
  // ----------:		lara_as_twist
  // ----------:		lara_as_kick
  // 0x004286F0:		lara_as_deathslide
  // 0x00428790:		extra_as_breath
  // ----------:		extra_as_plunger
  // 0x004287E0:		extra_as_yetikill
  // 0x00428830:		extra_as_sharkkill
  // 0x004288D0:		extra_as_airlock
  // 0x004288F0:		extra_as_gongbong
  // 0x00428910:		extra_as_dinokill
  // 0x00428970:		extra_as_pulldagger
  // 0x00428A30:		extra_as_startanim
  // 0x00428A80:		extra_as_starthouse
  // 0x00428B30:		extra_as_finalanim
  // 0x00428BE0:		LaraFallen
  // 0x00428C40:		LaraCollideStop
  
#define lara_col_walk ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00428D00)
#define lara_col_run ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00428EA0)
#define lara_col_stop ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429020)
#define lara_col_forwardjump ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x004290B0)
#define lara_col_fastback ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429190)
#define lara_col_turn_r ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429250)
#define lara_col_turn_l ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x004292F0)
#define lara_col_death ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429310)
#define lara_col_fastfall ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429380)
#define lara_col_hang ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429420)
#define lara_col_reach ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429550)
#define lara_col_splat ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x004295E0)
#define lara_col_compress ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429640)
#define lara_col_back ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x004296E0)
#define lara_col_fastturn ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x004297E0)
#define lara_col_stepright ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429800)
#define lara_col_stepleft ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x004298C0)
#define lara_col_slide ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x004298E0)
#define lara_col_backjump ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429900)
#define lara_col_rightjump ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429930)
#define lara_col_leftjump ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429960)
#define lara_col_upjump ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429990)
#define lara_col_fallback ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429AD0)
#define lara_col_hangleft ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429B60)
#define lara_col_hangright ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429BA0)
#define lara_col_slideback ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429BE0)
#define lara_col_roll ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429C10)
#define lara_col_roll2 ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429CB0)
#define lara_col_special ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429D80)
#define lara_col_swandive ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429DA0)
#define lara_col_fastdive ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429E10)
#define lara_col_wade ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00429E90)
#define lara_default_col ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x0042A000)
extern void lara_col_null(ITEM_INFO* item, COLL_INFO* coll);

// ----------:		lara_col_pose
// ----------:		lara_col_land
// ----------:		lara_col_null
// ----------:		lara_col_pushblock
// ----------:		lara_col_pullblock
// ----------:		lara_col_ppready
// ----------:		lara_col_pickup
// ----------:		lara_col_switchon
// ----------:		lara_col_switchoff
// ----------:		lara_col_usekey
// ----------:		lara_col_usepuzzle
// ----------:		lara_col_usemidas
// ----------:		lara_col_diemidas
// ----------:		lara_col_gymnast
// ----------:		lara_col_waterout
// ----------:		lara_col_laratest1
// ----------:		lara_col_laratest2
// ----------:		lara_col_laratest3
// ----------:		lara_col_twist
// ----------:		lara_col_deathslide

void lara_col_jumper(ITEM_INFO* item, COLL_INFO* coll); // 0x0042A040
#define lara_col_kick ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x0042A120)

#define GetLaraCollisionInfo ((void (__cdecl*)(ITEM_INFO *item, COLL_INFO *coll)) 0x0042A130)

// 0x0042A170:		lara_slide_slope
// 0x0042A260:		LaraHitCeiling
// 0x0042A2D0:		LaraDeflectEdge

#define LaraDeflectEdgeJump ((void (__cdecl*)(ITEM_INFO *item, COLL_INFO *coll)) 0x0042A350)

// 0x0042A4D0:		LaraSlideEdgeJump
// 0x0042A5C0:		TestWall
// 0x0042A6D0:		LaraTestHangOnClimbWall
// 0x0042A7E0:		LaraTestClimbStance
// 0x0042A8A0:		LaraHangTest
// 0x0042AC00:		LaraTestEdgeCatch
// 0x0042ACB0:		LaraTestHangJumpUp
// 0x0042AE20:		LaraTestHangJump
// 0x0042AFC0:		TestHangSwingIn
// 0x0042B080:		TestLaraVault
// 0x0042B370:		TestLaraSlide
// 0x0042B4A0:		LaraFloorFront

#define LaraLandedBad ((bool (__cdecl*)(ITEM_INFO *item, COLL_INFO *coll)) 0x0042B520)
void GetLaraJointAbsPosition(PHD_VECTOR* pos, int meshID); // 0x0042B5E0
// NOTE: new argument meshID, to get the right id when it's interpolated.
void GetLJAInt(ITEM_INFO* item, PHD_VECTOR* pos, short* frame0, short* frame1, int frac, int rate, int meshID); // 0x0042B970

#endif // LARA_H_INCLUDED
