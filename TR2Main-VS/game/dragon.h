/*
 * Copyright (c) 2017-2024 Michael Chaban. All rights reserved.
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

#ifndef DRAGON_H_INCLUDED
#define DRAGON_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
void ControlTwinkle(short fxNum); // 0x00417780
void CreateBartoliLight(short itemNumber); // 0x00417900
short DragonFire(int x, int y, int z, short speed, short rotY, short roomNum); // 0x004179E0
#define DragonCollision ((void(__cdecl*)(short, ITEM_INFO *, COLL_INFO *)) 0x00417A90)
#define DragonBones ((void(__cdecl*)(short)) 0x00417D80)
#define DragonControl ((void(__cdecl*)(short)) 0x00417E60)
#define InitialiseBartoli ((void(__cdecl*)(short)) 0x004183B0)
#define BartoliControl ((void(__cdecl*)(short)) 0x004184D0)
void DinoControl(short itemNumber); // 0x00418670

#endif // DRAGON_H_INCLUDED
