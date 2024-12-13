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

void lara_as_walk(ITEM_INFO* item, COLL_INFO* coll); // 0x00427880
void lara_as_run(ITEM_INFO* item, COLL_INFO* coll); // 0x00427910
void lara_as_stop(ITEM_INFO* item, COLL_INFO* coll); // 0x00427A60
void lara_as_forwardjump(ITEM_INFO* item, COLL_INFO* coll); // 0x00427BB0
void lara_as_fastback(ITEM_INFO* item, COLL_INFO* coll); // 0x00427C90
void lara_as_turn_r(ITEM_INFO* item, COLL_INFO* coll); // 0x00427CF0
void lara_as_turn_l(ITEM_INFO* item, COLL_INFO* coll); // 0x00427D80
void lara_as_death(ITEM_INFO* item, COLL_INFO* coll); // 0x00427E10
void lara_as_fastfall(ITEM_INFO* item, COLL_INFO* coll); // 0x00427E30
void lara_as_hang(ITEM_INFO* item, COLL_INFO* coll); // 0x00427E70
void lara_as_reach(ITEM_INFO* item, COLL_INFO* coll); // 0x00427ED0
void lara_as_splat(ITEM_INFO* item, COLL_INFO* coll); // 0x00427EF0
void lara_as_compress(ITEM_INFO* item, COLL_INFO* coll); // 0x00427F00
void lara_as_back(ITEM_INFO* item, COLL_INFO* coll); // 0x00428010
void lara_as_null(ITEM_INFO* item, COLL_INFO* coll); // 0x004280A0
void lara_as_fastturn(ITEM_INFO* item, COLL_INFO* coll); // 0x004280B0
void lara_as_stepright(ITEM_INFO* item, COLL_INFO* coll); // 0x00428100
void lara_as_stepleft(ITEM_INFO* item, COLL_INFO* coll); // 0x00428180
void lara_as_slide(ITEM_INFO* item, COLL_INFO* coll); // 0x00428200
void lara_as_backjump(ITEM_INFO* item, COLL_INFO* coll); // 0x00428230
void lara_as_rightjump(ITEM_INFO* item, COLL_INFO* coll); // 0x00428280
void lara_as_leftjump(ITEM_INFO* item, COLL_INFO* coll); // 0x004282C0
void lara_as_upjump(ITEM_INFO* item, COLL_INFO* coll); // 0x00428300
void lara_as_fallback(ITEM_INFO* item, COLL_INFO* coll); // 0x00428320
void lara_as_hangleft(ITEM_INFO* item, COLL_INFO* coll); // 0x00428350
void lara_as_hangright(ITEM_INFO* item, COLL_INFO* coll); // 0x00428390
void lara_as_slideback(ITEM_INFO* item, COLL_INFO* coll); // 0x004283D0
void lara_as_pushblock(ITEM_INFO* item, COLL_INFO* coll); // 0x004283F0
void lara_as_ppready(ITEM_INFO* item, COLL_INFO* coll); // 0x00428420
void lara_as_pickup(ITEM_INFO* item, COLL_INFO* coll); // 0x00428450
void lara_as_pickupflare(ITEM_INFO* item, COLL_INFO* coll); // 0x00428480
void lara_as_switchon(ITEM_INFO* item, COLL_INFO* coll); // 0x004284E0
void lara_as_usekey(ITEM_INFO* item, COLL_INFO* coll); // 0x00428520
void lara_as_special(ITEM_INFO* item, COLL_INFO* coll); // 0x00428550
void lara_as_swandive(ITEM_INFO* item, COLL_INFO* coll); // 0x00428570
void lara_as_fastdive(ITEM_INFO* item, COLL_INFO* coll); // 0x004285A0
void lara_as_waterout(ITEM_INFO* item, COLL_INFO* coll); // 0x00428600
void lara_as_wade(ITEM_INFO* item, COLL_INFO* coll); // 0x00428620
void lara_as_deathslide(ITEM_INFO* item, COLL_INFO* coll); // 0x004286F0

#define extra_as_breath ((void(__cdecl*)(ITEM_INFO*, COLL_INFO*)) 0x00428790)
#define extra_as_yetikill ((void(__cdecl*)(ITEM_INFO*, COLL_INFO*)) 0x004287E0)
#define extra_as_sharkkill ((void(__cdecl*)(ITEM_INFO*, COLL_INFO*)) 0x00428830)
#define extra_as_airlock ((void(__cdecl*)(ITEM_INFO*, COLL_INFO*)) 0x004288D0)
#define extra_as_gongbong ((void(__cdecl*)(ITEM_INFO*, COLL_INFO*)) 0x004288F0)
#define extra_as_dinokill ((void(__cdecl*)(ITEM_INFO*, COLL_INFO*)) 0x00428910)
#define extra_as_pulldagger ((void(__cdecl*)(ITEM_INFO*, COLL_INFO*)) 0x00428970)
#define extra_as_startanim ((void(__cdecl*)(ITEM_INFO*, COLL_INFO*)) 0x00428A30)
#define extra_as_starthouse ((void(__cdecl*)(ITEM_INFO*, COLL_INFO*)) 0x00428A80)
#define extra_as_finalanim ((void(__cdecl*)(ITEM_INFO*, COLL_INFO*)) 0x00428B30)

BOOL LaraFallen(ITEM_INFO* item, COLL_INFO* coll); // 0x00428BE0
#define LaraCollideStop ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00428C40)

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
void lara_default_col(ITEM_INFO* item, COLL_INFO* coll); // 0x0042A000
void lara_col_null(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_jumper(ITEM_INFO* item, COLL_INFO* coll); // 0x0042A040
void lara_col_kick(ITEM_INFO* item, COLL_INFO* coll); // 0x0042A120

void GetLaraCollisionInfo(ITEM_INFO* item, COLL_INFO* coll); // 0x0042A130
void LaraSlideSlope(ITEM_INFO* item, COLL_INFO* coll); // 0x0042A170
#define LaraHitCeiling ((BOOL(__cdecl*)(ITEM_INFO *item, COLL_INFO *coll)) 0x0042A260)
#define LaraDeflectEdge ((BOOL(__cdecl*)(ITEM_INFO *item, COLL_INFO *coll)) 0x0042A2D0)
#define LaraDeflectEdgeJump ((void(__cdecl*)(ITEM_INFO *item, COLL_INFO *coll)) 0x0042A350)
#define LaraSlideEdgeJump ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x0042A4D0)
#define TestWall ((BOOL(__cdecl*)(ITEM_INFO*,int,int,int)) 0x0042A5C0)
#define LaraTestHangOnClimbWall ((BOOL(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x0042A6D0)
#define LaraTestClimbStance ((BOOL(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x0042A7E0)
#define LaraHangTest ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x0042A8A0)
#define LaraTestEdgeCatch ((BOOL(__cdecl*)(ITEM_INFO*,COLL_INFO*,int*)) 0x0042AC00)
#define LaraTestHangJumpUp ((BOOL(__cdecl*)(ITEM_INFO*,COLL_INFO*,int*)) 0x0042ACB0)
#define LaraTestHangJump ((BOOL(__cdecl*)(ITEM_INFO*,COLL_INFO*,int*)) 0x0042AE20)
#define TestHangSwingIn ((BOOL(__cdecl*)(ITEM_INFO*,short)) 0x0042AFC0)
#define TestLaraVault ((BOOL(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x0042B080)
#define TestLaraSlide ((BOOL(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x0042B370)
#define LaraFloorFront ((short(__cdecl*)(ITEM_INFO*,short,int)) 0x0042B4A0)
#define LaraLandedBad ((bool(__cdecl*)(ITEM_INFO *item, COLL_INFO *coll)) 0x0042B520)

void GetLaraJointAbsPosition(PHD_VECTOR* pos, int meshID); // 0x0042B5E0
void GetLaraJointAbsPositionInterpolated(ITEM_INFO* item, PHD_VECTOR* pos, short* frame0, short* frame1, int frac, int rate, int meshID); // 0x0042B970

#endif // LARA_H_INCLUDED
