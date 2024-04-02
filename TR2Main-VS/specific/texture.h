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

#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
  // NOTE: these functions are not presented in the original game
#ifdef FEATURE_VIDEOFX_IMPROVED
void FreeEnvmapTexture();
bool SetEnvmapTexture(LPDDS surface);
HWR_TEXHANDLE GetEnvmapTextureHandle();
#endif // FEATURE_VIDEOFX_IMPROVED
DWORD GetMaxTextureSize();
int GetTextureSideByPage(int page);
int GetTextureSideByHandle(HWR_TEXHANDLE handle);

void CopyBitmapPalette(RGB888* srcPal, BYTE* srcBitmap, int bitmapSize, RGB888* destPal); // 0x00455990
BYTE FindNearestPaletteEntry(RGB888* palette, int red, int green, int blue, bool ignoreSysPalette); // 0x00455AD0
void SyncSurfacePalettes(void* srcData, int width, int height, int srcPitch, RGB888* srcPalette, void* dstData, int dstPitch, RGB888* dstPalette, bool preserveSysPalette); // 0x00455BA0
int CreateTexturePalette(RGB888* pal); // 0x00455C50
int GetFreePaletteIndex(); // 0x00455CE0
void FreePalette(int paletteIndex); // 0x00455D00
void SafeFreePalette(int paletteIndex); // 0x00455D30
#if (DIRECT3D_VERSION >= 0x900)
int CreateTexturePage(int width, int height, bool alpha);
#else // (DIRECT3D_VERSION >= 0x900)
int CreateTexturePage(int width, int height, LPDIRECTDRAWPALETTE palette); // 0x00455D80
#endif // (DIRECT3D_VERSION >= 0x900)
int GetFreeTexturePageIndex(); // 0x00455DF0
#if (DIRECT3D_VERSION < 0x900)
bool CreateTexturePageSurface(TEXPAGE_DESC* desc); // 0x00455E10
bool TexturePageInit(TEXPAGE_DESC* page); // 0x00455EB0
LPDIRECT3DTEXTURE2 Create3DTexture(LPDDS surface); // 0x00456030
#endif // (DIRECT3D_VERSION < 0x900)
void SafeFreeTexturePage(int pageIndex); // 0x00456060
void FreeTexturePage(int pageIndex); // 0x00456080
#if (DIRECT3D_VERSION < 0x900)
void TexturePageReleaseVidMemSurface(TEXPAGE_DESC* page); // 0x004560C0
#endif // (DIRECT3D_VERSION < 0x900)
void FreeTexturePages(); // 0x00456100
#if (DIRECT3D_VERSION < 0x900)
bool LoadTexturePage(int pageIndex, bool reset); // 0x00456130
bool ReloadTextures(bool reset); // 0x004561E0
#endif // (DIRECT3D_VERSION < 0x900)
HWR_TEXHANDLE GetTexturePageHandle(int pageIndex); // 0x00456220
int AddTexturePage8(int width, int height, BYTE* pageBuffer, int palIndex); // 0x00456260
int AddTexturePage16(int width, int height, BYTE* pageBuffer); // 0x00456360
#if (DIRECT3D_VERSION >= 0x900)
int AddTexturePage32(int width, int height, BYTE* pageBuffer, bool alpha); // NOTE: this function is not presented in the original game
int AddExternalTexture(LPCTSTR fileName, bool alpha); // NOTE: this function is not presented in the original game
bool IsExternalTexture(int page); // NOTE: this function is not presented in the original game
#else // (DIRECT3D_VERSION >= 0x900)
HRESULT CALLBACK EnumTextureFormatsCallback(LPDDSDESC lpDdsd, LPVOID lpContext); // 0x00456500
HRESULT EnumerateTextureFormats(); // 0x00456620
#endif // (DIRECT3D_VERSION >= 0x900)
void CleanupTextures(); // 0x00456650
bool InitTextures(); // 0x00456660

#endif // TEXTURE_H_INCLUDED
