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
#define ShiftItem ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x004134E0)
#define UpdateLaraRoom ((void(__cdecl*)(ITEM_INFO*, int)) 0x00413520)
#define GetTiltType ((short(__cdecl*)(FLOOR_INFO*, int, int, int)) 0x00413580) // floor, x, y, z
#define LaraBaddieCollision ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x00413620)
#define EffectSpaz ((void(__cdecl*)(ITEM_INFO*,COLL_INFO*)) 0x004137C0)
#define CreatureCollision ((void(__cdecl*)(short, ITEM_INFO *, COLL_INFO *)) 0x00413840)
#define ObjectCollision ((void(__cdecl*)(short, ITEM_INFO *, COLL_INFO *)) 0x004138C0)
#define DoorCollision ((void(__cdecl*)(short, ITEM_INFO *, COLL_INFO *)) 0x00413920)
#define TrapCollision ((void(__cdecl*)(short, ITEM_INFO *, COLL_INFO *)) 0x004139A0)
#define ItemPushLara ((void(__cdecl*)(ITEM_INFO*, ITEM_INFO*, COLL_INFO*, BOOL, BOOL)) 0x00413A10)
#define TestBoundsCollide ((int(__cdecl*)(ITEM_INFO*, ITEM_INFO*, int)) 0x00413D20)
#define TestLaraPosition ((int(__cdecl*)(short*,ITEM_INFO*,ITEM_INFO*)) 0x00413DF0)
#define AlignLaraPosition ((void(__cdecl*)(PHD_VECTOR*,ITEM_INFO*,ITEM_INFO*)) 0x00413F30)
#define MoveLaraPosition ((int(__cdecl*)(PHD_VECTOR*,ITEM_INFO*,ITEM_INFO*)) 0x00414070)
#define Move3DPosTo3DPos ((int(__cdecl*)(PHD_3DPOS*,PHD_3DPOS*,int,short)) 0x00414200)

#endif // COLLIDE_H_INCLUDED
