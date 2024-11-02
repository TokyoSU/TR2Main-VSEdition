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

#ifndef INIT_DISPLAY_H_INCLUDED
#define INIT_DISPLAY_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
void CreateRenderBuffer(); // 0x004489D0
void CreatePictureBuffer(); // 0x00448A80
void ClearBuffers(DWORD flags, DWORD fillColor); // 0x00448AF0
void UpdateFrame(bool needRunMessageLoop, LPRECT rect); // 0x00448DE0
bool RenderInit(); // 0x00448EF0
void RenderStart(bool isReset); // 0x00448F00
void RenderFinish(bool needToClearTextures); // 0x004492B0
bool ApplySettings(APP_SETTINGS* newSettings); // 0x004493A0
void FmvBackToGame(); // 0x004495B0
void GameApplySettings(APP_SETTINGS* newSettings); // 0x004496C0
void UpdateGameResolution(); // 0x00449900
LPCTSTR DecodeErrorMessage(DWORD errorCode); // 0x00449970

#endif // INIT_DISPLAY_H_INCLUDED
