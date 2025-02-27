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

#ifndef LOT_H_INCLUDED
#define LOT_H_INCLUDED

#include "global/types.h"

#define MAX_CREATURES 5

 /*
  * Function list
  */

void InitialiseLOTarray(); // 0x00432B10
void DisableBaddieAI(short itemNumber); // 0x00432B70
BOOL EnableBaddieAI(short itemNumber, BOOL always); // 0x00432BC0
void InitialiseSlot(short itemNumber, int baddieSlotID); // 0x00432D70
void CreateZone(ITEM_INFO* item); // 0x00432F80
void ClearLOT(LOT_INFO* LOT); // 0x00433040

#endif // LOT_H_INCLUDED
