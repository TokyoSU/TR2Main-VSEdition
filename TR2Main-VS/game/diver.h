/*
 * Copyright (c) 2017-2019 Michael Chaban. All rights reserved.
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

#ifndef DIVER_H_INCLUDED
#define DIVER_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */

short Harpoon(int x, int y, int z, short speed, short rotY, short roomNum); // 0x00416BF0
int GetWaterSurface(int x, int y, int z, short roomNum); // 0x00416C70
void DiverControl(short itemNumber); // 0x00416D80

#endif // DIVER_H_INCLUDED
