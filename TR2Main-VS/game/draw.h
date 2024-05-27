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
int DrawPhaseCinematic(); // 0x00418920
int DrawPhaseGame(); // 0x00418960
void DrawRooms(short currentRoom); // 0x004189A0
void GetRoomBounds(); // 0x00418C50
void SetRoomBounds(DOOR_INFO* ptrObj, ROOM_INFO* parent); // 0x00418E20
void ClipRoom(ROOM_INFO* room); // 0x004191A0
void PrintRooms(short roomNumber); // 0x00419580
void PrintObjects(short roomNumber); // 0x00419640
void DrawEffect(short fx_id); // 0x00419870
void DrawSpriteItem(ITEM_INFO* item); // 0x004199C0
void DrawDummyItem(ITEM_INFO* item); // ----------
void DrawAnimatingItem(ITEM_INFO* item); // 0x00419A50
void DrawLara(ITEM_INFO* item); // 0x00419DD0
void DrawLaraInt(ITEM_INFO* item, short* frame1, short* frame2, int frac, int rate);
void InitInterpolate(int frac, int rate); // 0x0041B6F0
void phd_PopMatrix_I(); // 0x0041B730
void phd_PushMatrix_I(); // 0x0041B760
void phd_RotY_I(short angle); // 0x0041B790
void phd_RotX_I(short angle); // 0x0041B7D0
void phd_RotZ_I(short angle); // 0x0041B810
void phd_TranslateRel_I(int x, int y, int z); // 0x0041B850
void phd_TranslateRel_ID(int x1, int y1, int z1, int x2, int y2, int z2); // 0x0041B8A0
void phd_RotYXZ_I(short rotY, short rotX, short rotZ); // 0x0041B8F0
void phd_RotYXZsuperpack_I(UINT16** mptr, UINT16** mptr2, int clip); // 0x0041B940
void phd_RotYXZsuperpack(UINT16** pptr, int index); // 0x0041B980
void phd_PutPolygons_I(short* ptrObj, int clip); // 0x0041BA30
#define InterpolateMatrix ((void(__cdecl*)(void)) 0x0041BA60)
#define InterpolateArmMatrix ((void(__cdecl*)(void)) 0x0041BC10)
void DrawGunFlash(int weapon, int clip);
void CalculateObjectLighting(ITEM_INFO* item, short* frame); // 0x0041BE80
int GetFrames(ITEM_INFO* item, short* frames[2], int* rate); // 0x0041BF70
short* GetBoundsAccurate(ITEM_INFO* item); // 0x0041C010
#define GetBestFrame ((short*(__cdecl*)(ITEM_INFO*)) 0x0041C090)
void AddDynamicLight(int x, int y, int z, int intensity, int falloff); // 0x0041C0D0

#endif // DRAW_H_INCLUDED
