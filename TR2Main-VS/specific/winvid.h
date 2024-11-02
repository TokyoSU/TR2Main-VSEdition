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

#ifndef WINVID_H_INCLUDED
#define WINVID_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
void GameWindowCalculateSizeFromClient(int* width, int* height); // 0x00444D20
void GameWindowCalculateSizeFromClientByZero(int* width, int* height); // 0x00444DA0
void WinVidSetMinWindowSize(int width, int height); // 0x00444E10
void WinVidClearMinWindowSize(); // 0x00444E60
void WinVidSetMaxWindowSize(int width, int height); // 0x00444E70
void WinVidClearMaxWindowSize(); // 0x00444EC0
int CalculateWindowWidth(int width, int height); // 0x00444ED0
int CalculateWindowHeight(int width, int height); // 0x00444F20
bool WinVidGetMinMaxInfo(LPMINMAXINFO info); // 0x00444F50
HWND WinVidFindGameWindow(); // 0x00445060
bool WinVidSpinMessageLoop(bool needWait); // 0x00445080
void WinVidShowGameWindow(int nCmdShow); // 0x00445170
void WinVidHideGameWindow(); // 0x004451C0
void WinVidSetGameWindowSize(int width, int height); // 0x00445200
bool WinVidGetDisplayMode(DISPLAY_MODE* dispMode); // 0x00445740
bool WinVidGoFullScreen(DISPLAY_MODE* dispMode); // 0x004457D0
bool WinVidGoWindowed(int width, int height, DISPLAY_MODE* dispMode); // 0x00445860
void WinVidSetDisplayAdapter(DISPLAY_ADAPTER* dispAdapter); // 0x00445970
bool CompareVideoModes(DISPLAY_MODE* mode1, DISPLAY_MODE* mode2); // 0x00445A50
bool WinVidGetDisplayModes(); // 0x00445AA0
bool WinVidInit(); // 0x00445EC0
bool WinVidGetDisplayAdapters(); // 0x00445F00
void FlaggedStringDelete(STRING_FLAGGED* item); // 0x00445FB0
bool EnumerateDisplayAdapters(DISPLAY_ADAPTER_LIST* displayAdapterList); // 0x00445FD0
void FlaggedStringsCreate(DISPLAY_ADAPTER* adapter); // 0x004461B0
bool WinVidRegisterGameWindowClass(); // 0x004461F0
LRESULT CALLBACK WinVidGameWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam); // 0x00446260
void WinVidResizeGameWindow(HWND hWnd, int edge, LPRECT rect); // 0x00446870
bool WinVidCheckGameWindowPalette(HWND hWnd); // 0x00446A50
bool WinVidCreateGameWindow(); // 0x00446B10
void WinVidFreeWindow(); // 0x00446BE0
void WinVidExitMessage(); // 0x00446C10
DISPLAY_ADAPTER_NODE* WinVidGetDisplayAdapter(GUID* lpGuid); // 0x00446C60
void WinVidStart(); // 0x00446CB0
void WinVidFinish(); // 0x00447030
void DisplayModeListInit(DISPLAY_MODE_LIST* pList); // 0x00447050
void DisplayModeListDelete(DISPLAY_MODE_LIST* pList); // 0x00447060
DISPLAY_MODE* InsertDisplayMode(DISPLAY_MODE_LIST* modeList, DISPLAY_MODE_NODE* before); // 0x004470A0
DISPLAY_MODE* InsertDisplayModeInListHead(DISPLAY_MODE_LIST* modeList); // 0x004470C0
DISPLAY_MODE* InsertDisplayModeInListTail(DISPLAY_MODE_LIST* modeList); // 0x00447110

#endif // WINVID_H_INCLUDED
