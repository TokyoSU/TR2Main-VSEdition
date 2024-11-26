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

#ifndef LARA_CLIMB_H_INCLUDED
#define LARA_CLIMB_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */

void lara_as_climbleft(ITEM_INFO* item, COLL_INFO* coll); // 0x0042D8F0
void lara_as_climbright(ITEM_INFO* item, COLL_INFO* coll); // 0x0042D930
void lara_as_climbstnc(ITEM_INFO* item, COLL_INFO* coll); // 0x0042D970
void lara_as_climbing(ITEM_INFO* item, COLL_INFO* coll); // 0x0042D9F0
void lara_as_climbend(ITEM_INFO* item, COLL_INFO* coll); // 0x0042DA10
void lara_as_climbdown(ITEM_INFO* item, COLL_INFO* coll); // 0x0042DA30
void lara_col_climbleft(ITEM_INFO* item, COLL_INFO* coll); // 0x0042DA50
void lara_col_climbright(ITEM_INFO* item, COLL_INFO* coll); // 0x0042DAB0
void lara_col_climbstnc(ITEM_INFO* item, COLL_INFO* coll); // 0x0042DB10
void lara_col_climbing(ITEM_INFO* item, COLL_INFO* coll); // 0x0042DD20
void lara_col_climbdown(ITEM_INFO* item, COLL_INFO* coll); // 0x0042DE70

#define LaraCheckForLetGo ((BOOL(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x0042E010)
#define LaraTestClimb ((BOOL(__cdecl*)(int x, int y, int z, int xfront, int zfront, int height, short roomNumber, int *shift)) 0x0042E0C0)
#define LaraTestClimbPos ((BOOL(__cdecl*)(ITEM_INFO* item,int front,int right,int origin,int height,int* shift)) 0x0042E330)
#define LaraDoClimbLeftRight ((void(__cdecl*)(ITEM_INFO* item,COLL_INFO* coll,BOOL result,int shift)) 0x0042E400)
#define LaraTestClimbUpPos ((BOOL(__cdecl*)(ITEM_INFO *item, int front, int right, int *shift, int *ledge)) 0x0042E4F0)

#endif // _H_INCLUDED
