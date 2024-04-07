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

#ifndef BOAT_H_INCLUDED
#define BOAT_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */

typedef enum {
	BGF_NOTON,
	BGF_GETRIGHT,
	BGF_GETLEFT,
	BGF_JUMPON,
	BGF_SAMESPOT
} BOAT_GETON_FLAGS; // used for BoatCheckGeton return value.

void InitialiseBoat(short itemNum); // 0x0040CB10
int BoatCheckGeton(short itemNum, COLL_INFO* coll); // 0x0040CB50
void BoatCollision(short itemNum, ITEM_INFO* laraitem, COLL_INFO* coll); // 0x0040CCC0
int TestWaterHeight(ITEM_INFO* item, int zoff, int xoff, PHD_VECTOR* pos); // 0x0040CE20
void DoBoatShift(int itemID); // 0x0040CF20
void DoWakeEffect(ITEM_INFO* item); // 0x0040D0F0
int DoBoatDynamics(int height, int fallspeed, int* y); // 0x0040D270
int BoatDynamics(short itemNum); // 0x0040D2C0
int BoatUserControl(ITEM_INFO* item); // 0x0040D7A0
void BoatAnimation(ITEM_INFO* item, int collide); // 0x0040D930
void BoatControl(short itemNum); // 0x0040DAA0
void GondolaControl(short itemID); // 0x0040E0D0

#endif // BOAT_H_INCLUDED
