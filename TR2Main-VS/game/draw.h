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

#ifndef DRAW_H_INCLUDED
#define DRAW_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
#define DrawPhaseCinematic ((int(__cdecl*)(void)) 0x00418920)
#define DrawPhaseGame ((int(__cdecl*)(void)) 0x00418960)
void DrawRooms(short currentRoom); // 0x004189A0
void GetRoomBounds(); // 0x00418C50
void SetRoomBounds(short* ptrObj, int roomNumber, ROOM_INFO* parent); // 0x00418E20
void ClipRoom(ROOM_INFO* room); // 0x004191A0
void PrintRooms(short roomNumber); // 0x00419580
void PrintObjects(short roomNumber); // 0x00419640
void DrawEffect(short fx_id); // 0x00419870
void DrawSpriteItem(ITEM_INFO* item); // 0x004199C0
void DrawDummyItem(ITEM_INFO* item);
void DrawAnimatingItem(ITEM_INFO* item); // 0x00419A50
#define DrawLara ((void(__cdecl*)(ITEM_INFO*)) 0x00419DD0)
void DrawLaraInt(ITEM_INFO* item, short* frame1, short* frame2, int frac, int rate);
#define InitInterpolate ((void(__cdecl*)(int, int)) 0x0041B6F0)
#define phd_PopMatrix_I ((void(__cdecl*)(void)) 0x0041B730)
#define phd_PushMatrix_I ((void(__cdecl*)(void)) 0x0041B760)
#define phd_RotY_I ((void(__cdecl*)(short)) 0x0041B790)
#define phd_RotX_I ((void(__cdecl*)(short)) 0x0041B7D0)
#define phd_RotZ_I ((void(__cdecl*)(short)) 0x0041B810)
#define phd_TranslateRel_I ((void(__cdecl*)(int, int, int)) 0x0041B850)
#define phd_TranslateRel_ID ((void(__cdecl*)(int, int, int, int, int, int)) 0x0041B8A0)
#define phd_RotYXZ_I ((void(__cdecl*)(int, int, int)) 0x0041B8F0)
#define phd_RotYXZsuperpack_I ((void(__cdecl*)(UINT16**, UINT16**, int)) 0x0041B940)
void phd_RotYXZsuperpack(UINT16** pptr, int index); // 0x0041B980
void phd_PutPolygons_I(short* ptrObj, int clip); // 0x0041BA30
#define InterpolateMatrix ((void(__cdecl*)(void)) 0x0041BA60)
#define InterpolateArmMatrix ((void(__cdecl*)(void)) 0x0041BC10)
void DrawGunFlash(int weapon, int clip);
void CalculateObjectLighting(ITEM_INFO* item, short* frame); // 0x0041BE80
#define GetFrames ((int(__cdecl*)(ITEM_INFO*, short**, int*)) 0x0041BF70)
short* GetBoundsAccurate(ITEM_INFO* item); // 0x0041C010
#define GetBestFrame ((short*(__cdecl*)(ITEM_INFO*)) 0x0041C090)
void AddDynamicLight(int x, int y, int z, int intensity, int falloff); // 0x0041C0D0

#endif // DRAW_H_INCLUDED
