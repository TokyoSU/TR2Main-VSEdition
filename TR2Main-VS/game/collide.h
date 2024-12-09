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

#ifndef COLLIDE_H_INCLUDED
#define COLLIDE_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
void GetCollisionInfo(COLL_INFO* coll, int x, int y, int z, short roomID, int height); // 0x004128D0
int FindGridShift(int src, int dest); // 0x00412F90
int CollideStaticObjects(COLL_INFO* coll, int x, int y, int z, short roomID, int hite); // 0x00412FC0
void GetNearByRooms(int x, int y, int z, int r, int h, short roomID); // 0x004133B0
void GetNewRoom(int x, int y, int z, short roomID); // 0x00413480
void ShiftItem(ITEM_INFO* item, COLL_INFO* coll); // 0x004134E0
void UpdateLaraRoom(ITEM_INFO* item, int height); // 0x00413520
short GetTiltType(FLOOR_INFO* floor, int x, int y, int z); // 0x00413580
void LaraBaddieCollision(ITEM_INFO* item, COLL_INFO* coll); // 0x00413620
void EffectSpaz(ITEM_INFO* item, COLL_INFO* coll); // 0x004137C0
void CreatureCollision(short itemNumber, ITEM_INFO* laraItem, COLL_INFO* coll); // 0x00413840
void ObjectCollision(short itemNumber, ITEM_INFO* laraItem, COLL_INFO* coll); // 0x004138C0
void DoorCollision(short itemNumber, ITEM_INFO* laraItem, COLL_INFO* coll); // 0x00413920
void TrapCollision(short itemNumber, ITEM_INFO* laraItem, COLL_INFO* coll); // 0x004139A0
void ItemPushLara(ITEM_INFO* item, ITEM_INFO* laraItem, COLL_INFO* coll, BOOL isSpazEnabled, BOOL isPushEnabled); // 0x00413A10
BOOL TestBoundsCollide(ITEM_INFO* item, ITEM_INFO* laraItem, int radius); // 0x00413D20
BOOL TestLaraPosition(short* bounds, ITEM_INFO* item, ITEM_INFO* laraitem); // 0x00413DF0
void AlignLaraPosition(PHD_VECTOR* vec, ITEM_INFO* item, ITEM_INFO* laraitem); // 0x00413F30
BOOL MoveLaraPosition(PHD_VECTOR* vec, ITEM_INFO* item, ITEM_INFO* laraItem); // 0x00414070
BOOL Move3DPosTo3DPos(PHD_3DPOS* srcpos, PHD_3DPOS* destpos, int velocity, short angadd); // 0x00414200
bool IsCollidingOnFloorLift(int x, int z, int ix, int iz, short itemAngle);

#endif // COLLIDE_H_INCLUDED
