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

#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
BOOL ReadFileSync(HANDLE hFile, LPVOID lpBuffer, DWORD nBytesToRead, LPDWORD lpnBytesRead, LPOVERLAPPED lpOverlapped); // 0x00449980
BOOL LoadTexturePages(HANDLE hFile); // 0x004499D0
BOOL LoadRooms(HANDLE hFile); // 0x00449B60
void AdjustTextureUVs(bool resetUvFix); // 0x00449F00
BOOL LoadObjects(HANDLE hFile); // 0x00449FA0
BOOL LoadSprites(HANDLE hFile); // 0x0044A520
BOOL LoadItems(HANDLE hFile); // 0x0044A660
BOOL LoadDepthQ(HANDLE hFile); // 0x0044A840
BOOL LoadPalettes(HANDLE hFile); // 0x0044A9D0
BOOL LoadCameras(HANDLE hFile); // 0x0044AA50
BOOL LoadSoundEffects(HANDLE hFile); // 0x0044AAB0
BOOL LoadBoxes(HANDLE hFile); // 0x0044AB10
BOOL LoadAnimatedTextures(HANDLE hFile); // 0x0044AD40
BOOL LoadCinematic(HANDLE hFile); // 0x0044ADA0
BOOL LoadDemo(HANDLE hFile); // 0x0044AE20
void LoadDemoExternal(LPCSTR levelName); // 0x0044AEB0
BOOL LoadSamples(HANDLE hFile); // 0x0044AF50
void ChangeFileNameExtension(char* fileName, const char* fileExt); // 0x0044B1C0
LPCTSTR GetFullPath(LPCTSTR path); // 0x0044B200
BOOL SelectDrive(); // 0x0044B230
BOOL LoadLevel(LPCTSTR fileName, int levelID); // 0x0044B310
BOOL S_LoadLevelFile(LPCTSTR fileName, int levelID, GF_LEVEL_TYPE levelType); // 0x0044B560
void S_UnloadLevelFile(); // 0x0044B580
void S_AdjustTexelCoordinates(); // 0x0044B5B0
BOOL S_ReloadLevelGraphics(BOOL reloadPalettes, BOOL reloadTexPages); // 0x0044B5D0
BOOL Read_Strings(DWORD dwCount, char** stringTable, char** stringBuffer, LPDWORD lpBufferSize, HANDLE hFile); // 0x0044B6A0
BOOL S_LoadGameFlow(LPCTSTR fileName); // 0x0044B770

#endif // FILE_H_INCLUDED
