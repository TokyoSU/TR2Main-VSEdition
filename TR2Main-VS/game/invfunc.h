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

#ifndef INVFUNC_H_INCLUDED
#define INVFUNC_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
void InitColours(); // 0x00423B10
void RingIsOpen(RING_INFO* ring); // 0x00423C20
void RingIsNotOpen(); // 0x00423D90
void RingNotActive(INVENTORY_ITEM* item); // 0x00423E20
void RingActive(); // 0x00424290
void RingExamineSelected(INVENTORY_ITEM* item);
void RingExamineNotSelected();
BOOL Inv_AddItem(GAME_OBJECT_ID itemID); // 0x004242D0
void Inv_InsertItem(INVENTORY_ITEM* item); // 0x00424AE0
int Inv_RequestItem(GAME_OBJECT_ID itemID); // 0x00424C10
void Inv_RemoveAllItems(); // 0x00424C90
BOOL Inv_RemoveItem(GAME_OBJECT_ID itemID); // 0x00424CB0
GAME_OBJECT_ID Inv_GetItemOption(GAME_OBJECT_ID itemID); // 0x00424DC0
void RemoveInventoryText(); // 0x00424FB0
void Inv_RingInit(RING_INFO* ring, short type, INVENTORY_ITEM** itemList, short objCount, short currentObj, INV_MOTION_INFO* motionInfo); // 0x00424FE0
void Inv_RingGetView(RING_INFO* ring, PHD_3DPOS* view); // 0x004250F0
void Inv_RingLight(RING_INFO* ring); // 0x00425150
void Inv_RingCalcAdders(RING_INFO* ring, short rotDuration); // 0x00425190
void Inv_RingDoMotions(RING_INFO* ring); // 0x004251C0
void Inv_RingRotateLeft(RING_INFO* ring); // 0x00425300
void Inv_RingRotateRight(RING_INFO* ring); // 0x00425330
void Inv_RingMotionInit(RING_INFO* ring, short framesCount, short status, short statusTarget); // 0x00425360
void Inv_RingMotionSetup(RING_INFO* ring, short status, short statusTarget, short framesCount); // 0x004253D0
void Inv_RingMotionRadius(RING_INFO* ring, short target); // 0x00425400
void Inv_RingMotionRotation(RING_INFO* ring, short rotation, short target); // 0x00425430
void Inv_RingMotionCameraPos(RING_INFO* ring, short target); // 0x00425460
void Inv_RingMotionCameraPitch(RING_INFO* ring, short target); // 0x00425490
void Inv_RingMotionItemSelect(RING_INFO* ring, INVENTORY_ITEM* item); // 0x004254B0
void Inv_RingMotionItemDeselect(RING_INFO* ring, INVENTORY_ITEM* item); // 0x00425510
INVENTORY_ITEM* Inv_GetItemFromIndex(int index); // From 0 to 33

#endif // INVFUNC_H_INCLUDED
