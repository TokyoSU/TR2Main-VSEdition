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

#ifndef SETUP_H_INCLUDED
#define SETUP_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
BOOL InitialiseLevel(int levelIndex, GF_LEVEL_TYPE type); // 0x0043A330
void InitialiseGameFlags(); // 0x0043A490
void InitialiseLevelFlags(); // 0x0043A500
void BaddyObjects(); // 0x0043A530
void TrapObjects(); // 0x0043B570
void ObjectObjects(); // 0x0043BB70
void InitialiseObjects(); // 0x0043C7C0
void GetCarriedItems(); // 0x0043C830

#endif // SETUP_H_INCLUDED
