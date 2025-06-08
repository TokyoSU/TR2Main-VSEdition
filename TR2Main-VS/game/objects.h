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

#ifndef OBJECTS_H_INCLUDED
#define OBJECTS_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
#define EarthQuake ((void(__cdecl*)(short)) 0x004342C0)
#define ControlCutShotgun ((void(__cdecl*)(short)) 0x004343A0)
#define InitialiseFinalLevel ((void(__cdecl*)(void)) 0x004343E0)
#define FinalLevelCounter ((void(__cdecl*)(short)) 0x004344B0)
#define MiniCopterControl ((void(__cdecl*)(short)) 0x004346C0)
void InitialiseDyingMonk(short itemNumber); // 0x004347A0
#define DyingMonk ((void(__cdecl*)(short)) 0x00434820)
#define ControlGongBonger ((void(__cdecl*)(short)) 0x004348B0)
#define DeathSlideCollision ((void(__cdecl*)(short,ITEM_INFO*,COLL_INFO*)) 0x00434970)
#define ControlDeathSlide ((void(__cdecl*)(short)) 0x00434A30)
void BigBowlControl(short itemNumber); // 0x00434CC0
void BellControl(short itemNumber); // 0x00434DB0
void InitialiseWindow(short itemNumber); // 0x00434E30
void SmashWindow(short itemNumber); // 0x00434EB0
void WindowControl(short itemNumber); // 0x00434F80
void OpenNearestDoor();
void SmashIceControl(short itemNumber); // 0x00435020
#define ShutThatDoor ((void(__cdecl*)(DOORPOS_DATA*)) 0x00435100)
#define OpenThatDoor ((void(__cdecl*)(DOORPOS_DATA*)) 0x00435150)
void InitialiseDoor(short itemNumber); // 0x00435190
void DoorControl(short itemNumber); // 0x00435570
#define OnDrawBridge ((int(__cdecl*)(ITEM_INFO*,int,int)) 0x00435640)
#define DrawBridgeFloor ((void(__cdecl*)(ITEM_INFO*,int,int,int,int*)) 0x00435700)
#define DrawBridgeCeiling ((void(__cdecl*)(ITEM_INFO*,int,int,int,int*)) 0x00435740)
#define DrawBridgeCollision ((void(__cdecl*)(short,ITEM_INFO*,COLL_INFO*)) 0x00435780)
void InitialiseLift(short itemNumber); // 0x004357B0
void LiftControl(short itemNumber); // 0x004357F0
void LiftFloorCeiling(ITEM_INFO* item, int x, int y, int z, int* floor, int* ceiling); // 0x004358D0
void LiftFloor(ITEM_INFO* item, int x, int y, int z, int* height); // 0x00435A50
void LiftCeiling(ITEM_INFO* item, int x, int y, int z, int* height); // 0x00435A90
#define BridgeFlatFloor ((void(__cdecl*)(ITEM_INFO*,int,int,int,int*)) 0x00435AD0)
#define BridgeFlatCeiling ((void(__cdecl*)(ITEM_INFO*,int,int,int,int*)) 0x00435AF0)
#define GetOffset ((int(__cdecl*)(ITEM_INFO*,int,int)) 0x00435B10)
#define BridgeTilt1Floor ((void(__cdecl*)(ITEM_INFO*,int,int,int,int*)) 0x00435B50)
#define BridgeTilt1Ceiling ((void(__cdecl*)(ITEM_INFO*,int,int,int,int*)) 0x00435B80)
#define BridgeTilt2Floor ((void(__cdecl*)(ITEM_INFO*,int,int,int,int*)) 0x00435BC0)
#define BridgeTilt2Ceiling ((void(__cdecl*)(ITEM_INFO*,int,int,int,int*)) 0x00435BF0)
#define CopterControl ((void(__cdecl*)(short)) 0x00435C30)
void GeneralControl(short itemNumber); // 0x00435D40
#define DetonatorControl ((void(__cdecl*)(short)) 0x00435E20)

#endif // OBJECTS_H_INCLUDED
