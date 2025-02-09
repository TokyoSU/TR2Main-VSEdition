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

#ifndef LARA_SWIM_H_INCLUDED
#define LARA_SWIM_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */

void LaraUnderWater(ITEM_INFO* item, COLL_INFO* coll); // 0x00432000
void SwimTurn(ITEM_INFO* item); // 0x00432230
void lara_as_swim(ITEM_INFO* item, COLL_INFO* coll); // 0x004322C0
#define lara_as_glide ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00432330)
#define lara_as_tread ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x004323B0)
#define lara_as_dive ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00432440)
#define lara_as_uwdeath ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00432460)
#define lara_as_waterroll ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x004324C0)
#define lara_col_swim ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x004324D0)
#define lara_col_uwdeath ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x004324F0)
int GetWaterDepth(int x, int y, int z, short roomNumber); // 0x00432550
//0x004326F0:		LaraTestWaterDepth
//0x004327C0:		LaraSwimCollision
void LaraWaterCurrent(COLL_INFO* coll); // 0x00432920

#endif // LARA_SWIM_H_INCLUDED
