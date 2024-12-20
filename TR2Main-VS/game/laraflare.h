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

#ifndef LARA_FLARE_H_INCLUDED
#define LARA_FLARE_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
int DoFlareLight(PHD_VECTOR* pos, int flareAge); // 0x0042F840
void DoFlareInHand(int flareAge); // 0x0042F8E0
void DrawFlareInAir(ITEM_INFO* item); // 0x0042F9C0
void CreateFlare(BOOL isFlying); // 0x0042FAC0
void set_flare_arm(int frame); // 0x0042FCA0
void draw_flare(); // 0x0042FCF0
void undraw_flare(); // 0x0042FE60
void draw_flare_meshes(); // 0x00430090
void undraw_flare_meshes(); // 0x004300B0
void ready_flare(); // 0x004300D0
void FlareControl(short itemNumber); // 0x00430110

#endif // LARA_FLARE_H_INCLUDED
