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

#ifndef CONTROL_H_INCLUDED
#define CONTROL_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */

int ControlPhase(int nTicks, BOOL demoMode); // 0x00414370
void AnimateItem(ITEM_INFO* item); // 0x004146C0
#define GetChange ((int(__cdecl*)(ITEM_INFO*,ANIM_STRUCT*)) 0x00414A30)
#define TranslateItem ((void(__cdecl*)(ITEM_INFO*,int,int,int)) 0x00414AE0)
FLOOR_INFO* GetFloor(int x, int y, int z, short* roomNumber); // 0x00414B40
int GetWaterHeight(int x, int y, int z, short roomNumber); // 0x00414CE0
int GetHeight(FLOOR_INFO* floor, int x, int y, int z); // 0x00414E50
#define RefreshCamera ((void(__cdecl*)(int,short*)) 0x004150D0)
void TestTriggers(short* data, BOOL isHeavy); // 0x004151C0
#define TriggerActive ((int(__cdecl*)(ITEM_INFO*)) 0x004158A0)
int GetCeiling(FLOOR_INFO* floor, int x, int y, int z); // 0x00415900
#define GetDoor ((short(__cdecl*)(FLOOR_INFO*)) 0x00415B60)
int LOS(GAME_VECTOR* start, GAME_VECTOR* target); // 0x00415BB0
int zLOS(GAME_VECTOR* start, GAME_VECTOR* target); // 0x00415C50
int xLOS(GAME_VECTOR* start, GAME_VECTOR* target); // 0x00415F40
int ClipTarget(GAME_VECTOR* start, GAME_VECTOR* target, FLOOR_INFO* floor); // 0x00416230
short ObjectOnLOS(GAME_VECTOR* start, GAME_VECTOR* target); // 0x00416310
void FlipMap(); // 0x00416610
#define RemoveRoomFlipItems ((void(__cdecl*)(ROOM_INFO* room)) 0x004166D0)
#define AddRoomFlipItems ((void(__cdecl*)(ROOM_INFO* room)) 0x00416770)
void TriggerCDTrack(short value, UINT16 flags, short type); // 0x004167D0
void TriggerNormalCDTrack(short value, UINT16 flags, short type); // 0x00416800;

#endif // CONTROL_H_INCLUDED
