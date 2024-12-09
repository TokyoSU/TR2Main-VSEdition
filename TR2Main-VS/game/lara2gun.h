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

#ifndef LARA2GUN_H_INCLUDED
#define LARA2GUN_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
void set_pistol_arm(LARA_ARM* arm, int frame); // 0x0042D000
void draw_pistols(int weaponType); // 0x0042D050
void undraw_pistols(int weaponType); // 0x0042D0D0
void ready_pistols(int weaponType); // 0x0042D300
void draw_pistol_meshes(int weaponType); // 0x0042D360
void undraw_pistol_mesh_left(int weaponType); // 0x0042D3B0
void undraw_pistol_mesh_right(int weaponType); // 0x0042D3F0
void PistolHandler(int weaponType); // 0x0042D430
void AnimatePistols(int gunType); // 0x0042D5C0

#endif // LARA2GUN_H_INCLUDED
