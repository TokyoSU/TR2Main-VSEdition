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

#ifndef PICKUP_H_INCLUDED
#define PICKUP_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
void PickUpCollision(short itemNumber, ITEM_INFO* laraitem, COLL_INFO* coll); // 0x00437F20
#define SwitchCollision ((void(__cdecl*)(short, ITEM_INFO*, COLL_INFO*)) 0x004383A0)
#define SwitchCollision2 ((void(__cdecl*)(short, ITEM_INFO*, COLL_INFO*)) 0x004385B0)
#define DetonatorCollision ((void(__cdecl*)(short, ITEM_INFO*, COLL_INFO*)) 0x004386B0)
#define KeyHoleCollision ((void(__cdecl*)(short, ITEM_INFO*, COLL_INFO*)) 0x004388F0)
#define PuzzleHoleCollision ((void(__cdecl*)(short, ITEM_INFO*, COLL_INFO*)) 0x00438B30)
#define SwitchControl ((void(__cdecl*)(short)) 0x00438DF0)
#define SwitchTrigger ((BOOL(__cdecl*)(int, int)) 0x00438E30)
#define KeyTrigger ((BOOL(__cdecl*)(int)) 0x00438EF0)
#define PickupTrigger ((BOOL(__cdecl*)(int)) 0x00438F30)
#define SecretControl ((void(__cdecl*)(short)) 0x00438F70)

#endif // PICKUP_H_INCLUDED
