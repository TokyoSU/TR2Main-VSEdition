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

#ifndef HEALTH_H_INCLUDED
#define HEALTH_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
BOOL FlashIt(); // 0x00421980
void DrawAssaultTimer(); // 0x004219B0
void DrawGameInfo(BOOL pickupState); // 0x00421B00
void DrawEnemyHealthBar();
void DrawHealthBar(BOOL flashState); // 0x00421B50
void DrawAirBar(BOOL flashState); // 0x00421C00
void MakeAmmoString(char* str); // 0x00421CA0
void DrawAmmoInfo(); // 0x00421CD0
void InitialisePickUpDisplay(); // 0x00421E20
void DrawPickups(BOOL pickupState); // 0x00421E40
void AddDisplayPickup(short objNumber, int secretType = -1); // 0x00421F40
void DisplayModeInfo(char* modeString); // 0x00421FB0
void DrawModeInfo(); // 0x00422030

#endif // HEALTH_H_INCLUDED
