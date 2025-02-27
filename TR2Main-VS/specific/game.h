/*
 * Copyright (c) 2017-2019 Michael Chaban. All rights reserved.
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

#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
short StartGame(int levelID, GF_LEVEL_TYPE levelType); // 0x0044C550
int GameLoop(BOOL demoMode); // 0x0044C6A0
int LevelCompleteSequence(); // 0x0044C740
int LevelStats(int levelID); // 0x0044C750
int GameStats(int levelID); // 0x0044C920
int GetRandomControl(); // 0x0044CA40
void SeedRandomControl(int seed); // 0x0044CA60
int GetRandomDraw(); // 0x0044CA70
int GetRandomDrawWithNeg();
int GetRandomDrawWithNegInt();
int GetRandom(int min, int max);
void SeedRandomDraw(int seed); // 0x0044CA90
void GetValidLevelsList(REQUEST_INFO* req); // 0x0044CAA0
void GetSavedGamesList(REQUEST_INFO* req); // 0x0044CAF0
void DisplayCredits(); // 0x0044CB40
BOOL S_FrontEndCheck(); // 0x0044CD80
BOOL S_SaveGame(LPCVOID saveData, DWORD saveSize, int slotNumber); // 0x0044CEF0
BOOL S_LoadGame(LPVOID saveData, DWORD saveSize, int saveNumber); // 0x0044D010

#endif // GAME_H_INCLUDED
