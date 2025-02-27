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

#include "precompiled.h"
#include "specific/winvid.h"
#include "specific/init_3d.h"
#include "specific/init_display.h"
#include "specific/sndpc.h"
#include "global/resource.h"
#include "global/vars.h"
#include "global/memmem.h"

DISPLAY_ADAPTER CurrentDisplayAdapter;

#if defined(FEATURE_WINDOW_STYLE_FIX)
static void setWindowStyle(bool isFullScreen) {
	static const DWORD fullScreenFlags = WS_POPUP;
	static const DWORD windowedFlags = WS_OVERLAPPEDWINDOW; // WS_BORDER|WS_DLGFRAME|WS_SYSMENU|WS_SIZEBOX|WS_MINIMIZEBOX|WS_MAXIMIZEBOX
	DWORD style = GetWindowLong(HGameWindow, GWL_STYLE);
	style &= ~(isFullScreen ? windowedFlags : fullScreenFlags);
	style |= (isFullScreen ? fullScreenFlags : windowedFlags);
	SetWindowLong(HGameWindow, GWL_STYLE, style);
}
#endif // FEATURE_WINDOW_STYLE_FIX

#if defined(FEATURE_NOLEGACY_OPTIONS)
bool AvoidInterlacedVideoModes = false;
#endif // FEATURE_NOLEGACY_OPTIONS

#if defined(FEATURE_INPUT_IMPROVED)
#include "modding/raw_input.h"
#include "modding/joy_output.h"
#endif // FEATURE_INPUT_IMPROVED

static bool InsertDisplayModeInListSorted(DISPLAY_MODE_LIST* modeList, DISPLAY_MODE* srcMode) {
	DISPLAY_MODE_NODE* node = NULL;
	DISPLAY_MODE* dstMode = NULL;

	if (!modeList->head || CompareVideoModes(srcMode, &modeList->head->body)) {
		dstMode = InsertDisplayModeInListHead(modeList);
		goto FILL;
	}
	for (node = modeList->head; node; node = node->next) {
		if (CompareVideoModes(srcMode, &node->body)) {
			dstMode = InsertDisplayMode(modeList, node);
			goto FILL;
		}
		else if (!memcmp(srcMode, &node->body, sizeof(DISPLAY_MODE))) {
			return false;
		}
	}
	dstMode = InsertDisplayModeInListTail(modeList);

FILL:
	if (dstMode) {
		*dstMode = *srcMode;
		return true;
	}
	return false;
}

static bool DisplayModeListCopy(DISPLAY_MODE_LIST* dst, DISPLAY_MODE_LIST* src) {
	if (dst == NULL || src == NULL || dst == src)
		return false;

	DISPLAY_MODE_NODE* node;
	DISPLAY_MODE* dstMode;

	DisplayModeListDelete(dst);
	for (node = src->head; node; node = node->next) {
		dstMode = InsertDisplayModeInListTail(dst);
		*dstMode = node->body;
	}
	return true;
}

bool FlaggedStringCopy(STRING_FLAGGED* dst, STRING_FLAGGED* src) {
	if (dst == NULL || src == NULL || dst == src || !src->isPresented)
		return false;

	size_t srcLen = lstrlen(src->lpString);

	dst->isPresented = false;
	dst->lpString = new char[srcLen + 1];

	if (dst->lpString == NULL)
		return false;

	if (srcLen > 0) {
		lstrcpy(dst->lpString, src->lpString);
	}
	else {
		*dst->lpString = 0;
	}
	dst->isPresented = true;
	return true;
}

void GameWindowCalculateSizeFromClient(int* width, int* height) {
	DWORD style, styleEx;
	RECT rect = { 0, 0, *width, *height };

	style = GetWindowLong(HGameWindow, GWL_STYLE);
	styleEx = GetWindowLong(HGameWindow, GWL_EXSTYLE);
	AdjustWindowRectEx(&rect, style, FALSE, styleEx);

	*width = rect.right - rect.left;
	*height = rect.bottom - rect.top;
}

void GameWindowCalculateSizeFromClientByZero(int* width, int* height) {
	DWORD style, styleEx;
	RECT rect = { 0, 0, 0, 0 };

	style = GetWindowLong(HGameWindow, GWL_STYLE);
	styleEx = GetWindowLong(HGameWindow, GWL_EXSTYLE);
	AdjustWindowRectEx(&rect, style, FALSE, styleEx);

	*width += rect.left - rect.right;
	*height += rect.top - rect.bottom;;
}

void WinVidSetMinWindowSize(int width, int height) {
	MinWindowClientWidth = width;
	MinWindowClientHeight = height;
	GameWindowCalculateSizeFromClient(&width, &height);
	MinWindowWidth = width;
	MinWindowHeight = height;
	IsMinWindowSizeSet = true;
}

void WinVidClearMinWindowSize() {
	IsMinWindowSizeSet = false;
}

void WinVidSetMaxWindowSize(int width, int height) {
	MaxWindowClientWidth = width;
	MaxWindowClientHeight = height;
	GameWindowCalculateSizeFromClient(&width, &height);
	MaxWindowWidth = width;
	MaxWindowHeight = height;
	IsMaxWindowSizeSet = true;
}

void WinVidClearMaxWindowSize() {
	IsMaxWindowSizeSet = false;
}

int CalculateWindowWidth(int width, int height) {
	switch (SavedAppSettings.AspectMode) {
	case AM_4_3:
		return height * 4 / 3;
	case AM_16_9:
		return height * 16 / 9;
	default:
		break;
	}
	return width;
}

int CalculateWindowHeight(int width, int height) {
	switch (SavedAppSettings.AspectMode) {
	case AM_4_3:
		return width * 3 / 4;
	case AM_16_9:
		return width * 9 / 16;
	default:
		break;
	}
	return height;
}

bool WinVidGetMinMaxInfo(LPMINMAXINFO info) {
	if (!IsGameWindowCreated)
		return false;

	if (IsGameFullScreen) {
		info->ptMinTrackSize.x = FullScreenWidth;
		info->ptMinTrackSize.y = FullScreenHeight;

		info->ptMaxTrackSize.x = FullScreenWidth;
		info->ptMaxTrackSize.y = FullScreenHeight;
		info->ptMaxSize.x = FullScreenWidth;
		info->ptMaxSize.y = FullScreenHeight;
		return true;
	}

	if (IsMinWindowSizeSet) {
		info->ptMinTrackSize.x = MinWindowWidth;
		info->ptMinTrackSize.y = MinWindowHeight;
	}

	if (IsMinMaxInfoSpecial) {
		int newWindowWidth = GameWindowWidth;
		int newWindowHeight = GameWindowHeight;
		GameWindowCalculateSizeFromClient(&newWindowWidth, &newWindowHeight);

		info->ptMaxTrackSize.x = newWindowWidth;
		info->ptMaxTrackSize.y = newWindowHeight;
		info->ptMaxSize.x = newWindowWidth;
		info->ptMaxSize.y = newWindowHeight;
	}
	else if (IsMaxWindowSizeSet) {
		info->ptMaxTrackSize.x = MaxWindowWidth;
		info->ptMaxTrackSize.y = MaxWindowHeight;
		info->ptMaxSize.x = MaxWindowWidth;
		info->ptMaxSize.y = MaxWindowHeight;
	}

	return (IsMinWindowSizeSet || IsMaxWindowSizeSet);
}

HWND WinVidFindGameWindow() {
	return FindWindow(GameClassName, GameWindowName);
}

bool WinVidSpinMessageLoop(bool needWait) {
	static int messageLoopCounter = 0;
	MSG msg;

	if (IsMessageLoopClosed)
		return false;

	++messageLoopCounter;
	do {
		if (needWait)
			WaitMessage();
		else
			needWait = true;

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT) {
				AppResultCode = msg.wParam;
				IsGameToExit = true;
				StopInventory = true;
				IsMessageLoopClosed = true;
				--messageLoopCounter;
				return false;
			}
		}
	} while (!IsGameWindowActive || IsGameWindowMinimized);

	--messageLoopCounter;
	return true;
}

void WinVidShowGameWindow(int nCmdShow) {
	if (nCmdShow != SW_SHOW || !IsGameWindowShow) {
		IsGameWindowUpdating = TRUE;
		ShowWindow(HGameWindow, nCmdShow);
		UpdateWindow(HGameWindow);
		IsGameWindowUpdating = FALSE;
		IsGameWindowShow = TRUE;
	}
}

void WinVidHideGameWindow() {
	if (IsGameWindowShow) {
		IsGameWindowUpdating = TRUE;
		ShowWindow(HGameWindow, SW_HIDE);
		UpdateWindow(HGameWindow);
		IsGameWindowUpdating = FALSE;
		IsGameWindowShow = FALSE;
	}
}

void WinVidSetGameWindowSize(int width, int height) {
	GameWindowCalculateSizeFromClient(&width, &height);
	SetWindowPos(HGameWindow, NULL, 0, 0, width, height, SWP_NOCOPYBITS | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
}

bool WinVidGetDisplayMode(DISPLAY_MODE* dispMode) {
	bool d3dClean = false;
	if (D3D == NULL) {
		if (!D3DCreate()) return false;
		d3dClean = true;
	}
	D3DDISPLAYMODE mode;
	HRESULT res = D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);
	if (d3dClean) D3DRelease();
	if (SUCCEEDED(res) && mode.Format == D3DFMT_X8R8G8B8) {
		dispMode->width = mode.Width;
		dispMode->height = mode.Height;
		dispMode->bpp = 32;
		dispMode->vga = VGA_NoVga;
		return true;
	}
	return false;
}

bool WinVidGoFullScreen(DISPLAY_MODE* dispMode) {
	FullScreenWidth = dispMode->width;
	FullScreenHeight = dispMode->height;
	FullScreenBPP = dispMode->bpp;
	FullScreenVGA = dispMode->vga;

#if defined(FEATURE_WINDOW_STYLE_FIX)
	setWindowStyle(true);
#endif // FEATURE_WINDOW_STYLE_FIX

	IsGameFullScreen = true;
	return true;
}

bool WinVidGoWindowed(int width, int height, DISPLAY_MODE* dispMode) {
	int maxWidth, maxHeight;
	RECT rect;

	if (!WinVidGetDisplayMode(dispMode))
		return false;

#if defined(FEATURE_WINDOW_STYLE_FIX)
	setWindowStyle(false);
#endif // FEATURE_WINDOW_STYLE_FIX

	maxWidth = dispMode->width;
	maxHeight = CalculateWindowHeight(dispMode->width, dispMode->height);

	if (maxHeight > dispMode->height) {
		maxHeight = dispMode->height;
		maxWidth = CalculateWindowWidth(dispMode->width, dispMode->height);
	}
	WinVidSetMaxWindowSize(maxWidth, maxHeight);

	if (width > maxWidth || height > maxHeight) {
		width = maxWidth;
		height = maxHeight;
	}

	IsGameFullScreen = false;
	IsGameWindowUpdating = true;
	WinVidSetGameWindowSize(width, height);
	IsGameWindowUpdating = false;

	GetClientRect(HGameWindow, &rect);
	MapWindowPoints(HGameWindow, NULL, (LPPOINT)&rect, 2);

	if ((rect.left > 0 || rect.right < dispMode->width) &&
		(rect.top > 0 || rect.bottom < dispMode->height))
	{
		WinVidShowGameWindow(SW_SHOW);
	}
	else {
		WinVidShowGameWindow(SW_MAXIMIZE);
	}

	dispMode->width = width;
	dispMode->height = height;
	return true;
}

void WinVidSetDisplayAdapter(DISPLAY_ADAPTER* dispAdapter) {
	DISPLAY_MODE dispMode;

	dispAdapter->screenWidth = 0;
	if (!WinVidGetDisplayMode(&dispMode)) return;
	dispMode.width &= ~0x1F;
	if (dispMode.width * 3 / 4 > dispMode.height)
		dispMode.width = (dispMode.height * 4 / 3) & ~0x1F;
	dispAdapter->screenWidth = dispMode.width;
}

bool CompareVideoModes(DISPLAY_MODE* mode1, DISPLAY_MODE* mode2) {
#if defined(FEATURE_NOLEGACY_OPTIONS)
	if (mode1->bpp < mode2->bpp) return true;
	if (mode1->bpp > mode2->bpp) return false;
	if (mode1->width < mode2->width) return true;
	if (mode1->width > mode2->width) return false;
	if (mode1->height < mode2->height) return true;
	if (mode1->height > mode2->height) return false;
#else // !FEATURE_NOLEGACY_OPTIONS
	DWORD square1 = mode1->width * mode1->height;
	DWORD square2 = mode2->width * mode2->height;
	if (square1 < square2) return true;
	if (square1 > square2) return false;
	if (mode1->bpp < mode2->bpp) return true;
	if (mode1->bpp > mode2->bpp) return false;
#endif // FEATURE_NOLEGACY_OPTIONS
	if (mode1->vga < mode2->vga) return true;
	if (mode1->vga > mode2->vga) return false;
	// equal state
	return false;
}

#if defined(FEATURE_NOLEGACY_OPTIONS)
static void DeleteDisplayMode(DISPLAY_MODE_LIST* modeList, DISPLAY_MODE_NODE* node) {
	if (!modeList || !node) return;
	DISPLAY_MODE_NODE* previous = node->previous;
	DISPLAY_MODE_NODE* next = node->next;
	if (previous) previous->next = next;
	if (next) next->previous = previous;
	if (modeList->head == node) modeList->head = next;
	if (modeList->tail == node) modeList->tail = previous;
	if (modeList->dwCount) --modeList->dwCount;
	delete(node);
}

static DWORD GetProgressiveDisplayModes(DWORD bpp, DEVMODE* modes, DWORD modeNum) {
	DWORD idx = 0;
	DWORD num = 0;
	if (modes == NULL) {
		DEVMODE mode;
		memset(&mode, 0, sizeof(mode));
		mode.dmSize = sizeof(mode);
		while (EnumDisplaySettings(NULL, idx++, &mode)) {
			if (mode.dmBitsPerPel == bpp && !CHK_ANY(mode.dmDisplayFlags, DM_INTERLACED)) {
				++num;
			}
		}
	}
	else {
		memset(modes, 0, sizeof(DEVMODE) * modeNum);
		while (num < modeNum) {
			modes[num].dmSize = sizeof(DEVMODE);
			if (!EnumDisplaySettings(NULL, idx++, &modes[num])) {
				break;
			}
			if (modes[num].dmBitsPerPel == bpp && !CHK_ANY(modes[num].dmDisplayFlags, DM_INTERLACED)) {
				++num;
			}
		}
	}
	return num;
}

static bool IsModeInList(DISPLAY_MODE* mode, DEVMODE* modes, DWORD modeNum) {
	if (!mode || !modes || !modeNum) return false;
	for (DWORD i = 0; i < modeNum; ++i) {
		if (modes[i].dmPelsWidth == (DWORD)mode->width &&
			modes[i].dmPelsHeight == (DWORD)mode->height &&
			modes[i].dmBitsPerPel == (DWORD)mode->bpp)
		{
			return true;
		}
	}
	return false;
}

static void FilterDisplayModes(DISPLAY_MODE_LIST* modeList) {
	DWORD wlistSize = 0;
	DEVMODE* whitelist = NULL;
	DISPLAY_MODE_NODE* mode, * next;
	int bppMax = 8;
	for (mode = modeList->head; mode; mode = mode->next) {
		CLAMPL(bppMax, mode->body.bpp);
	}
	if (AvoidInterlacedVideoModes) {
		wlistSize = GetProgressiveDisplayModes(bppMax, NULL, 0);
		if (wlistSize) {
			whitelist = (DEVMODE*)malloc(sizeof(DEVMODE) * wlistSize);
			if (whitelist) {
				GetProgressiveDisplayModes(bppMax, whitelist, wlistSize);
			}
		}
	}
	for (mode = modeList->head; mode; mode = next) {
		next = mode->next;
		if (mode->body.bpp < bppMax || (whitelist && !IsModeInList(&mode->body, whitelist, wlistSize))) {
			DeleteDisplayMode(modeList, mode);
		}
	}
	if (whitelist) {
		free(whitelist);
	}
}
#endif // FEATURE_NOLEGACY_OPTIONS

bool WinVidGetDisplayModes() {
	DISPLAY_ADAPTER_NODE* adapter;
	bool d3dClean = false;
	if (D3D == NULL) {
		if (!D3DCreate()) return false;
		d3dClean = true;
	}
	for (adapter = DisplayAdapterList.head; adapter; adapter = adapter->next) {
		UINT num = D3D->GetAdapterModeCount(adapter->body.index, D3DFMT_X8R8G8B8);
		for (UINT i = 0; i < num; ++i) {
			D3DDISPLAYMODE mode;
			if FAILED(D3D->EnumAdapterModes(adapter->body.index, D3DFMT_X8R8G8B8, i, &mode)) continue;
			DISPLAY_MODE videoMode(mode.Width, mode.Height, 32, VGA_NoVga);
			InsertDisplayModeInListSorted(&adapter->body.hwDispModeList, &videoMode);
			InsertDisplayModeInListSorted(&adapter->body.swDispModeList, &videoMode);
		}
		FilterDisplayModes(&adapter->body.hwDispModeList);
	}
	if (d3dClean) D3DRelease();
	return true;
}

bool WinVidInit() {
	AppResultCode = 0;
	return (WinVidRegisterGameWindowClass() &&
		WinVidCreateGameWindow() &&
		WinVidGetDisplayAdapters() &&
		DisplayAdapterList.dwCount &&
		WinVidGetDisplayModes());
}

bool WinVidGetDisplayAdapters() {
	DISPLAY_ADAPTER_NODE* node, * nextNode;

	for (node = DisplayAdapterList.head; node; node = nextNode) {
		nextNode = node->next;
		DisplayModeListDelete(&node->body.swDispModeList);
		DisplayModeListDelete(&node->body.hwDispModeList);
		FlaggedStringDelete(&node->body.driverName);
		FlaggedStringDelete(&node->body.driverDescription);
		delete(node);
	}

	DisplayAdapterList.head = NULL;
	DisplayAdapterList.tail = NULL;
	DisplayAdapterList.dwCount = 0;

	PrimaryDisplayAdapter = NULL;

	if (!EnumerateDisplayAdapters(&DisplayAdapterList))
		return false;

	PrimaryDisplayAdapter = DisplayAdapterList.head;
	return (PrimaryDisplayAdapter != NULL);
}

void FlaggedStringDelete(STRING_FLAGGED* item) {
	if (item->isPresented && item->lpString) {
		delete[] item->lpString;
		item->lpString = NULL;
		item->isPresented = false;
	}
}

bool EnumerateDisplayAdapters(DISPLAY_ADAPTER_LIST* displayAdapterList) {
	bool d3dClean = false;
	if (D3D == NULL) {
		if (!D3DCreate()) return false;
		d3dClean = true;
	}
	UINT num = D3D->GetAdapterCount();
	for (UINT i = 0; i < num; ++i) {
		D3DADAPTER_IDENTIFIER9 id;
		D3DCAPS9 caps;
		if (FAILED(D3D->GetAdapterIdentifier(i, 0, &id)) || FAILED(D3D->GetDeviceCaps(i, D3DDEVTYPE_HAL, &caps))) {
			continue;
		}
		DISPLAY_ADAPTER_NODE* listNode = new DISPLAY_ADAPTER_NODE;
		if (listNode == NULL) break;

		listNode->next = NULL;
		listNode->previous = displayAdapterList->tail;

		FlaggedStringsCreate(&listNode->body);
		DisplayModeListInit(&listNode->body.hwDispModeList);
		DisplayModeListInit(&listNode->body.swDispModeList);

		if (!displayAdapterList->head)
			displayAdapterList->head = listNode;

		if (displayAdapterList->tail)
			displayAdapterList->tail->next = listNode;

		displayAdapterList->tail = listNode;
		displayAdapterList->dwCount++;

		listNode->body.adapterGuid = id.DeviceIdentifier;
		listNode->body.lpAdapterGuid = &listNode->body.adapterGuid;

		lstrcpy(listNode->body.driverDescription.lpString, id.Description);
		lstrcpy(listNode->body.driverName.lpString, id.DeviceName);

		listNode->body.index = i;
		listNode->body.caps = caps;
	}
	if (d3dClean) D3DRelease();
	return true;
}

void FlaggedStringsCreate(DISPLAY_ADAPTER* adapter) {
	LPTSTR lpDriverDescription = new char[256];
	LPTSTR lpDriverName = new char[256];

	if (lpDriverDescription) {
		*lpDriverDescription = 0;
		adapter->driverDescription.lpString = lpDriverDescription;
		adapter->driverDescription.isPresented = true;
	}

	if (lpDriverName) {
		*lpDriverName = 0;
		adapter->driverName.lpString = lpDriverName;
		adapter->driverName.isPresented = true;
	}
}

bool WinVidRegisterGameWindowClass() {
	WNDCLASSEXA wndClass;
	memset(&wndClass, 0, sizeof(wndClass));
	wndClass.cbSize = sizeof(wndClass);
	wndClass.style = 0;
	wndClass.lpfnWndProc = WinVidGameWindowProc;
	wndClass.hInstance = GameModule;
	wndClass.hIcon = LoadIcon(GameModule, MAKEINTRESOURCE(IDI_MAINICON));
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.lpszClassName = GameClassName;
	return (RegisterClassEx(&wndClass) != 0);
}

LRESULT CALLBACK WinVidGameWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	HBRUSH hBrush;
	PAINTSTRUCT paint;
#if defined(FEATURE_AUDIO_IMPROVED)
	static DWORD cdVolume = 0;
#endif // FEATURE_AUDIO_IMPROVED

	if (IsFmvPlaying) {
		switch (Msg) {
		case WM_DESTROY:
			IsGameWindowCreated = false;
			HGameWindow = NULL;
			PostQuitMessage(0);
			break;

		case WM_MOVE:
			GameWindowPositionX = (int)(short)LOWORD(lParam);
			GameWindowPositionY = (int)(short)HIWORD(lParam);
			break;

		case WM_ACTIVATEAPP:
			IsGameWindowActive = (wParam != 0);
			break;

		case WM_SYSCOMMAND:
			if (wParam == SC_KEYMENU) return 0;
			break;
		}
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}

	switch (Msg) {
	case WM_CREATE:
		IsGameWindowCreated = true;
		break;

	case WM_DESTROY:
		IsGameWindowCreated = false;
		HGameWindow = NULL;
		PostQuitMessage(0);
		break;

	case WM_MOVE:
		GameWindowPositionX = (int)(short)LOWORD(lParam);
		GameWindowPositionY = (int)(short)HIWORD(lParam);
		break;

	case WM_SIZE:
		switch (wParam) {
		case SIZE_RESTORED:
			IsGameWindowMinimized = false;
			IsGameWindowMaximized = false;
			break;
		case SIZE_MAXIMIZED:
			IsGameWindowMinimized = false;
			IsGameWindowMaximized = true;
			break;
		case SIZE_MINIMIZED:
			IsGameWindowMinimized = true;
			IsGameWindowMaximized = false;
			break;
		default:
			return DefWindowProc(hWnd, Msg, wParam, lParam);
		}

		if (IsGameFullScreen || (LOWORD(lParam) == GameWindowWidth && HIWORD(lParam) == GameWindowHeight))
			break;

		GameWindowWidth = (int)(short)LOWORD(lParam);
		GameWindowHeight = (int)(short)HIWORD(lParam);
		if (IsGameWindowUpdating)
			break;

		UpdateGameResolution();
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &paint);
		if (IsGameFullScreen || D3DDev == NULL || FAILED(D3DDev->TestCooperativeLevel())) {
			hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
			FillRect(hdc, &paint.rcPaint, hBrush);
		}
		else {
			D3DDev->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0, 0);
			D3DDev->Present(NULL, NULL, NULL, NULL);
		}
		EndPaint(hWnd, &paint);
		return 0;

	case WM_ACTIVATE:
		break;

	case WM_ERASEBKGND:
		return 1;

	case WM_ACTIVATEAPP:
#if defined(FEATURE_AUDIO_IMPROVED)
		// NOTE: If CD audio volume is set to zero, music is paused.
		// To resume the music, the volume must be set to non zero value.
		if (wParam && !IsGameWindowActive) {
			if (cdVolume) {
				S_CDVolume(cdVolume);
			}
		}
		else if (!wParam && IsGameWindowActive) {
			cdVolume = S_GetCDVolume();
			S_CDVolume(0);
		}
#endif // FEATURE_AUDIO_IMPROVED
#if defined(FEATURE_INPUT_IMPROVED)
		if (!wParam && IsGameWindowActive) {
			JoyVibrationMute();
		}
#endif // FEATURE_INPUT_IMPROVED
		if (wParam && !IsGameWindowActive && IsGameFullScreen && SavedAppSettings.RenderMode == RM_Hardware)
			WinVidNeedToResetBuffers = true;
		IsGameWindowActive = (wParam != 0);
		break;

	case WM_SETCURSOR:
		if (IsGameFullScreen) {
			SetCursor(NULL);
			return 1;
		}
		break;

	case WM_GETMINMAXINFO:
		if (WinVidGetMinMaxInfo((LPMINMAXINFO)lParam))
			return 0;
		break;

	case WM_NCPAINT:
	case WM_NCLBUTTONDOWN:
	case WM_NCLBUTTONDBLCLK:
	case WM_NCRBUTTONDOWN:
	case WM_NCRBUTTONDBLCLK:
	case WM_NCMBUTTONDOWN:
	case WM_NCMBUTTONDBLCLK:
		if (IsGameFullScreen) return 0;
		break;

	case WM_SYSCOMMAND:
		if (wParam == SC_KEYMENU) return 0;
		break;

	case WM_SIZING:
		WinVidResizeGameWindow(hWnd, wParam, (LPRECT)lParam);
		break;

	case WM_MOVING:
		if (IsGameFullScreen || IsGameWindowMaximized) {
			GetWindowRect(hWnd, (LPRECT)lParam);
			return 1;
		}
		break;

	case WM_ENTERSIZEMOVE:
		IsGameWindowChanging = true;
		break;

	case WM_EXITSIZEMOVE:
		IsGameWindowChanging = false;
		break;
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

void WinVidResizeGameWindow(HWND hWnd, int edge, LPRECT rect) {
	bool isShiftPressed;
	int width, height;

	if (IsGameFullScreen) {
		rect->left = 0;
		rect->top = 0;
		rect->right = FullScreenWidth;
		rect->bottom = FullScreenHeight;
	}

	isShiftPressed = (GetAsyncKeyState(VK_SHIFT) < 0);
	width = rect->right - rect->left;
	height = rect->bottom - rect->top;
	GameWindowCalculateSizeFromClientByZero(&width, &height);

	if (edge == WMSZ_TOP || edge == WMSZ_BOTTOM) {
		if (isShiftPressed)
			height &= ~0x1F;
		width = CalculateWindowWidth(width, height);
	}
	else {
		if (isShiftPressed)
			width &= ~0x1F;
		height = CalculateWindowHeight(width, height);
	}

	if (IsMinWindowSizeSet) {
		if (width < MinWindowClientWidth)
			width = MinWindowClientWidth;
		if (height < MinWindowClientHeight)
			height = MinWindowClientHeight;
	}

	if (IsMaxWindowSizeSet) {
		if (width > MaxWindowClientWidth)
			width = MaxWindowClientWidth;
		if (height > MaxWindowClientHeight)
			height = MaxWindowClientHeight;
	}

	GameWindowCalculateSizeFromClient(&width, &height);

	switch (edge) {
	case WMSZ_TOPLEFT:
		rect->left = rect->right - width;
		rect->top = rect->bottom - height;
		break;

	case WMSZ_RIGHT:
	case WMSZ_BOTTOM:
	case WMSZ_BOTTOMRIGHT:
		rect->right = rect->left + width;
		rect->bottom = rect->top + height;
		break;

	case WMSZ_LEFT:
	case WMSZ_BOTTOMLEFT:
		rect->left = rect->right - width;
		rect->bottom = rect->top + height;
		break;

	case WMSZ_TOP:
	case WMSZ_TOPRIGHT:
		rect->right = rect->left + width;
		rect->top = rect->bottom - height;
		break;
	}
}

bool WinVidCheckGameWindowPalette(HWND hWnd) {
	HDC hdc;
	PALETTEENTRY sysPalette[256];
	RGB888 bufPalette[256];

	hdc = GetDC(hWnd);
	if (hdc == NULL)
		return false;

	GetSystemPaletteEntries(hdc, 0, 256, sysPalette);
	ReleaseDC(hWnd, hdc);

	for (int i = 0; i < 256; ++i) {
		bufPalette[i].red = sysPalette[i].peRed;
		bufPalette[i].green = sysPalette[i].peGreen;
		bufPalette[i].blue = sysPalette[i].peBlue;
	}

	return (!memcmp(bufPalette, GamePalette8, sizeof(bufPalette)));
}

bool WinVidCreateGameWindow() {
	RECT rect;

	IsGameWindowActive = true;
	IsGameWindowShow = true;
	IsDDrawGameWindowShow = false;
	IsMessageLoopClosed = false;
	IsGameWindowUpdating = false;
	IsGameWindowMinimized = false;
	IsGameWindowMaximized = false;
	IsGameWindowCreated = false;
	IsGameFullScreen = false;
	IsMinMaxInfoSpecial = false;
	IsGameWindowChanging = false;
	WinVidClearMinWindowSize();
	WinVidClearMaxWindowSize();

	HGameWindow = CreateWindowEx(WS_EX_APPWINDOW, GameClassName, GameWindowName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, GameModule, NULL);
	if (HGameWindow == NULL)
		return false;

	GetWindowRect(HGameWindow, &rect);
	GameWindow_X = rect.left;
	GameWindow_Y = rect.top;
	WinVidHideGameWindow();

	return true;
}

void WinVidFreeWindow() {
	WinVidExitMessage();
	UnregisterClass(GameClassName, GameModule);
}

void WinVidExitMessage() {
	if (HGameWindow && IsWindow(HGameWindow)) {
		PostMessage(HGameWindow, WM_CLOSE, 0, 0);
		while (WinVidSpinMessageLoop(false)) /* just wait */;
		HGameWindow = NULL;
	}
}

DISPLAY_ADAPTER_NODE* WinVidGetDisplayAdapter(GUID* lpGuid) {
	DISPLAY_ADAPTER_NODE* adapter;

	if (lpGuid != NULL) {
		for (adapter = DisplayAdapterList.head; adapter; adapter = adapter->next) {
			if (!memcmp(&adapter->body.adapterGuid, lpGuid, sizeof(GUID)))
				return adapter;
		}
	}
	return PrimaryDisplayAdapter;
}

void WinVidStart() {
	if (SavedAppSettings.PreferredDisplayAdapter == NULL)
		throw ERR_CantInitRenderer;

	DISPLAY_ADAPTER* preferred = &SavedAppSettings.PreferredDisplayAdapter->body;
	CurrentDisplayAdapter = *preferred;
	FlaggedStringCopy(&CurrentDisplayAdapter.driverDescription, &preferred->driverDescription);
	FlaggedStringCopy(&CurrentDisplayAdapter.driverName, &preferred->driverName);
	DisplayModeListInit(&CurrentDisplayAdapter.hwDispModeList);
	DisplayModeListCopy(&CurrentDisplayAdapter.hwDispModeList, &preferred->hwDispModeList);
	DisplayModeListInit(&CurrentDisplayAdapter.swDispModeList);
	DisplayModeListCopy(&CurrentDisplayAdapter.swDispModeList, &preferred->swDispModeList);
}

void WinVidFinish() {

}

void DisplayModeListInit(DISPLAY_MODE_LIST* pList) {
	pList->head = NULL;
	pList->tail = NULL;
	pList->dwCount = 0;
}

void DisplayModeListDelete(DISPLAY_MODE_LIST* pList) {
	DISPLAY_MODE_NODE* node;
	DISPLAY_MODE_NODE* nextNode = NULL;

	for (node = pList->head; node;) {
		nextNode = node->next;
		delete(node);
		node = nextNode;
	}
	DisplayModeListInit(pList);
}

DISPLAY_MODE* InsertDisplayMode(DISPLAY_MODE_LIST* modeList, DISPLAY_MODE_NODE* before) {
	if (!before || !before->previous)
		return InsertDisplayModeInListHead(modeList);

	DISPLAY_MODE_NODE* node = new DISPLAY_MODE_NODE;
	if (!node)
		return NULL;

	before->previous->next = node;
	node->previous = before->previous;

	before->previous = node;
	node->next = before;

	modeList->dwCount++;
	return &node->body;
}

DISPLAY_MODE* InsertDisplayModeInListHead(DISPLAY_MODE_LIST* modeList) {
	DISPLAY_MODE_NODE* node = new DISPLAY_MODE_NODE;
	if (!node)
		return NULL;

	node->next = modeList->head;
	node->previous = NULL;

	if (modeList->head)
		modeList->head->previous = node;

	if (!modeList->tail)
		modeList->tail = node;

	modeList->head = node;
	modeList->dwCount++;
	return &node->body;
}

DISPLAY_MODE* InsertDisplayModeInListTail(DISPLAY_MODE_LIST* modeList) {
	DISPLAY_MODE_NODE* node = new DISPLAY_MODE_NODE;
	if (!node)
		return NULL;

	node->next = NULL;
	node->previous = modeList->tail;

	if (modeList->tail)
		modeList->tail->next = node;

	if (!modeList->head)
		modeList->head = node;

	modeList->tail = node;
	modeList->dwCount++;
	return &node->body;
}

/*
 * Inject function
 */
void Inject_WinVid() {
	INJECT(0x00444D20, GameWindowCalculateSizeFromClient);
	INJECT(0x00444DA0, GameWindowCalculateSizeFromClientByZero);
	INJECT(0x00444E10, WinVidSetMinWindowSize);
	INJECT(0x00444E60, WinVidClearMinWindowSize);
	INJECT(0x00444E70, WinVidSetMaxWindowSize);
	INJECT(0x00444EC0, WinVidClearMaxWindowSize);
	INJECT(0x00444ED0, CalculateWindowWidth);
	INJECT(0x00444F20, CalculateWindowHeight);
	INJECT(0x00444F50, WinVidGetMinMaxInfo);
	INJECT(0x00445060, WinVidFindGameWindow);
	INJECT(0x00445080, WinVidSpinMessageLoop);
	INJECT(0x00445170, WinVidShowGameWindow);
	INJECT(0x004451C0, WinVidHideGameWindow);
	INJECT(0x00445200, WinVidSetGameWindowSize);
	INJECT(0x00445740, WinVidGetDisplayMode);
	INJECT(0x004457D0, WinVidGoFullScreen);
	INJECT(0x00445860, WinVidGoWindowed);
	INJECT(0x00445970, WinVidSetDisplayAdapter);
	INJECT(0x00445A50, CompareVideoModes);
	INJECT(0x00445AA0, WinVidGetDisplayModes);
	INJECT(0x00445EC0, WinVidInit);
	INJECT(0x00445F00, WinVidGetDisplayAdapters);
	INJECT(0x00445FB0, FlaggedStringDelete);
	INJECT(0x00445FD0, EnumerateDisplayAdapters);
	INJECT(0x004461B0, FlaggedStringsCreate);
	INJECT(0x004461F0, WinVidRegisterGameWindowClass);
	INJECT(0x00446260, WinVidGameWindowProc);
	INJECT(0x00446870, WinVidResizeGameWindow);
	INJECT(0x00446A50, WinVidCheckGameWindowPalette);
	INJECT(0x00446B10, WinVidCreateGameWindow);
	INJECT(0x00446BE0, WinVidFreeWindow);
	INJECT(0x00446C10, WinVidExitMessage);
	INJECT(0x00446C60, WinVidGetDisplayAdapter);
	INJECT(0x00446CB0, WinVidStart);
	INJECT(0x00447030, WinVidFinish);
	INJECT(0x00447050, DisplayModeListInit);
	INJECT(0x00447060, DisplayModeListDelete);
	INJECT(0x004470C0, InsertDisplayModeInListHead);
	INJECT(0x00447110, InsertDisplayModeInListTail);
}