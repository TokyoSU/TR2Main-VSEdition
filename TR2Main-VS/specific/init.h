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

#ifndef INIT_H_INCLUDED
#define INIT_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
BOOL S_InitialiseSystem(); // 0x0044D6E0
void ShutdownGame(); // 0x0044D730
void init_game_malloc(); // 0x0044D750
void* game_malloc(DWORD allocSize, DWORD bufIndex); // 0x0044D780
void game_free(DWORD freeSize); // 0x0044D800
void CalculateWibbleTable(); // 0x0044D840
void S_SeedRandom(); // 0x0044D930

#endif // INIT_H_INCLUDED
