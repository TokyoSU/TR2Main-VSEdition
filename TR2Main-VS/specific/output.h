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

#ifndef OUTPUT_H_INCLUDED
#define OUTPUT_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
int GetRenderScale(int unit);
int GetRenderHeightDownscaled();
int GetRenderWidthDownscaled();
int GetRenderHeight(); // 0x00450BA0
int GetRenderWidth(); // 0x00450BB0
void S_InitialisePolyList(BOOL clearBackBuffer); // 0x00450BC0
DWORD S_DumpScreen(); // 0x00450CB0
void S_ClearScreen(); // 0x00450CF0
void S_InitialiseScreen(GF_LEVEL_TYPE levelType); // 0x00450D00
void S_OutputPolyList(); // 0x00450D40
int S_GetObjectBounds(short* bPtr); // 0x00450D80
void S_InsertBackPolygon(int x0, int y0, int x1, int y1); // 0x00450FF0
void S_PrintShadow(short radius, short* bPtr, ITEM_INFO* item); // 0x00451040
void S_CalculateLight(int x, int y, int z, short roomNumber); // 0x00451240
void S_CalculateStaticLight(short adder); // 0x00451540
void S_CalculateStaticMeshLight(int x, int y, int z, int shade1, int shade2, ROOM_INFO* room); // 0x00451580
void S_LightRoom(ROOM_INFO* room); // 0x004516B0
void S_DrawHealthBar(int percent); // 0x004518C0
void S_DrawEnemyHealthBar(int percent, int originalHP); // New bar.
void S_DrawAirBar(int percent); // 0x00451A90
void AnimateTextures(int nTicks); // 0x00451C90
void S_SetupBelowWater(BOOL underwater); // 0x00451D50
void S_SetupAboveWater(BOOL underwater); // 0x00451DB0
void S_AnimateTextures(int nTicks); // 0x00451DE0
void S_DisplayPicture(LPCTSTR fileName, BOOL reallocGame); // 0x00451EA0
void S_SyncPictureBufferPalette(); // 0x00451FB0
void S_DontDisplayPicture(); // 0x00452030
void ScreenDump(); // 0x00452040
void ScreenPartialDump(); // 0x00452050
void FadeToPal(int fadeValue, RGB888* palette); // 0x00452060
void ScreenClear(bool isPhdWinSize); // 0x00452230
void S_CopyScreenToBuffer(); // 0x00452260
void S_CopyBufferToScreen(); // 0x00452310
BOOL DecompPCX(LPCBYTE pcx, DWORD pcxSize, LPBYTE pic, RGB888* pal); // 0x00452360

// NOTE: this function is not presented in the original game
int GetPcxResolution(LPCBYTE pcx, DWORD pcxSize, DWORD* width, DWORD* height);

#endif // OUTPUT_H_INCLUDED
