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

#ifndef _3DGEN_H_INCLUDED
#define _3DGEN_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
#ifdef FEATURE_VIDEOFX_IMPROVED
void ClearMeshReflectState();
void SetMeshReflectState(int objID, int meshIdx);
#endif // FEATURE_VIDEOFX_IMPROVED

void phd_GenerateW2V(PHD_3DPOS* viewPos); // 0x00401000
void phd_LookAt(int xsrc, int ysrc, int zsrc, int xtar, int ytar, int ztar, __int16 roll); // 0x004011D0
void phd_GetVectorAngles(int x, int y, int z, VECTOR_ANGLES* angles); // 0x00401250
void phd_RotX(__int16 angle); // 0x004012D0
void phd_RotY(__int16 angle); // 0x00401380
void phd_RotZ(__int16 angle); // 0x00401430
void phd_RotYXZ(__int16 ry, __int16 rx, __int16 rz); // 0x004014E0
void phd_RotYXZpack(DWORD rpack); // 0x004016C0
BOOL phd_TranslateRel(int x, int y, int z); // 0x004018B0
void phd_TranslateAbs(int x, int y, int z); // 0x00401960
void phd_PutPolygons(__int16* ptrObj, int clip); // 0x004019E0
void S_InsertRoom(__int16* ptrObj, BOOL isOutside); // 0x00401AE0
__int16* __cdecl calc_background_light(__int16* ptrObj); // 0x00401BD0
void S_InsertBackground(__int16* ptrObj); // 0x00401C10
void S_InsertInvBgnd(__int16* ptrObj); // ----------
__int16* __cdecl calc_object_vertices(__int16* ptrObj); // 0x00401D50
__int16* __cdecl calc_vertice_light(__int16* ptrObj); // 0x00401F30
__int16* __cdecl calc_roomvert(__int16* ptrObj, BYTE farClip); // 0x004020A0
void phd_RotateLight(__int16 pitch, __int16 yaw); // 0x00402320
void phd_InitPolyList(); // 0x004023F0
void phd_SortPolyList(); // 0x00402420
void do_quickysorty(int left, int right); // 0x00402460
void phd_PrintPolyList(BYTE* surfacePtr); // 0x00402530
void AlterFOV(__int16 fov); // 0x00402570
void phd_SetNearZ(int nearZ); // 0x00402680
void phd_SetFarZ(int farZ); // 0x004026D0
void phd_InitWindow(__int16 x, __int16 y, int width, int height, int nearZ, int farZ, __int16 viewAngle, int screenWidth, int screenHeight); // 0x004026F0
void phd_PopMatrix(); // ----------
void phd_PushMatrix(); // 0x00457510
void phd_PushUnitMatrix(); // 0x0045752E

#endif // _3DGEN_H_INCLUDED
