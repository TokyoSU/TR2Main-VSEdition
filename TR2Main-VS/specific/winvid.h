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
#if (DIRECT3D_VERSION < 0x900)
bool DDrawCreate(LPGUID lpGUID); // 0x00444C80
void DDrawRelease(); // 0x00444CE0
#endif // (DIRECT3D_VERSION < 0x900)
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
#if (DIRECT3D_VERSION < 0x900)
bool ShowDDrawGameWindow(bool active); // 0x00445240
bool HideDDrawGameWindow(); // 0x004452F0
HRESULT DDrawSurfaceCreate(LPDDSDESC dsp, LPDDS* surface); // 0x00445380
HRESULT DDrawSurfaceRestoreLost(LPDDS surface1, LPDDS surface2, bool blank); // 0x004453D0
bool WinVidClearBuffer(LPDDS surface, LPRECT rect, DWORD fillColor); // 0x00445420
HRESULT WinVidBufferLock(LPDDS surface, LPDDSDESC desc, DWORD flags); // 0x00445470
HRESULT WinVidBufferUnlock(LPDDS surface, LPDDSDESC desc); // 0x004454B0
bool WinVidCopyBitmapToBuffer(LPDDS surface, BYTE* bitmap); // 0x004454E0
DWORD GetRenderBitDepth(DWORD dwRGBBitCount); // 0x00445570
void WinVidGetColorBitMasks(COLOR_BIT_MASKS* bm, LPDDPIXELFORMAT pixelFormat); // 0x00445600
void BitMaskGetNumberOfBits(DWORD bitMask, DWORD* bitDepth, DWORD* bitOffset); // 0x00445680
DWORD CalculateCompatibleColor(COLOR_BIT_MASKS* mask, int red, int green, int blue, int alpha); // 0x004456D0
#endif // (DIRECT3D_VERSION < 0x900)
bool WinVidGetDisplayMode(DISPLAY_MODE* dispMode); // 0x00445740
bool WinVidGoFullScreen(DISPLAY_MODE* dispMode); // 0x004457D0
bool WinVidGoWindowed(int width, int height, DISPLAY_MODE* dispMode); // 0x00445860
void WinVidSetDisplayAdapter(DISPLAY_ADAPTER* dispAdapter); // 0x00445970
bool CompareVideoModes(DISPLAY_MODE* mode1, DISPLAY_MODE* mode2); // 0x00445A50
bool WinVidGetDisplayModes(); // 0x00445AA0
#if (DIRECT3D_VERSION < 0x900)
HRESULT WINAPI EnumDisplayModesCallback(LPDDSDESC lpDDSurfaceDesc, LPVOID lpContext); // 0x00445B00
#endif // (DIRECT3D_VERSION < 0x900)
bool WinVidInit(); // 0x00445EC0
bool WinVidGetDisplayAdapters(); // 0x00445F00
void FlaggedStringDelete(STRING_FLAGGED* item); // 0x00445FB0
bool EnumerateDisplayAdapters(DISPLAY_ADAPTER_LIST* displayAdapterList); // 0x00445FD0
#if (DIRECT3D_VERSION < 0x900)
BOOL WINAPI EnumDisplayAdaptersCallback(GUID FAR* lpGUID, LPTSTR lpDriverDescription, LPTSTR lpDriverName, LPVOID lpContext); // 0x00445FF0
#endif // (DIRECT3D_VERSION < 0x900)
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
