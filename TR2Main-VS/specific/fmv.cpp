/*
 * Copyright (c) 2017-2023 Michael Chaban. All rights reserved.
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
#include "specific/fmv.h"
#include "specific/file.h"
#include "specific/init_display.h"
#include "specific/input.h"
#include "specific/sndpc.h"
#include "global/vars.h"

 // GetProcAddress Macro
#define GET_DLL_PROC(dll, proc) { \
	*(FARPROC *)&(proc) = GetProcAddress((dll), #proc); \
	if( proc == NULL ) throw #proc; \
}

#ifdef FEATURE_FFPLAY
#define FFPLAY_DLL_NAME "ffplay.dll"
static HMODULE hFFplay = NULL;

// Imports from ffplay.dll
static int(__stdcall* ffplay_init)(HWND, int, const char*);
static int(__stdcall* ffplay_play_video)(const char*, int, int, int, int);
static int(__stdcall* ffplay_cleanup)(void);

static const char videoExts[][4] = {
	"MP4",
	"BIK",
	"RPL",
};

static bool FFplayInit() {
	if (hFFplay != NULL) {
		return true;
	}

	hFFplay = LoadLibrary(FFPLAY_DLL_NAME);
	if (hFFplay == NULL) {
		// failed to load DLL
		return false;
	}

	try {
		GET_DLL_PROC(hFFplay, ffplay_init);
		GET_DLL_PROC(hFFplay, ffplay_play_video);
		GET_DLL_PROC(hFFplay, ffplay_cleanup);
	}
	catch (LPCTSTR procName) {
		// failed to load one of the procs
		FreeLibrary(hFFplay);
		hFFplay = NULL;
		return false;
	}

	if (0 != ffplay_init(HGameWindow, 2, "winmm")) {
		// failed to init FFplay
		FreeLibrary(hFFplay);
		hFFplay = NULL;
		return false;
	}

	return true;
}

void FFplayCleanup() {
	if (hFFplay != NULL) {
		ffplay_cleanup();
		FreeLibrary(hFFplay);
		hFFplay = NULL;
	}
}
#endif // FEATURE_FFPLAY

bool FMV_Init() {
#ifdef FEATURE_FFPLAY
	if (FFplayInit()) {
		return true;
	}
#endif // FEATURE_FFPLAY
	return false;
}

void FMV_Cleanup() {
#ifdef FEATURE_FFPLAY
	FFplayCleanup();
#endif // FEATURE_FFPLAY
}

bool PlayFMV(LPCTSTR fileName) {
	LPCTSTR fullPath;

	if (SavedAppSettings.DisableFMV)
		return IsGameToExit;

	S_CDStop();
	ShowCursor(FALSE);
	RenderFinish(true);
	IsFmvPlaying = TRUE;

	fullPath = GetFullPath(fileName);
	WinPlayFMV(fullPath, true);
	WinStopFMV(true);

	IsFmvPlaying = FALSE;
	if (!IsGameToExit)
		FmvBackToGame();
	ShowCursor(TRUE);

	return IsGameToExit;
}

void WinPlayFMV(LPCTSTR fileName, bool isPlayback) {
#ifdef FEATURE_FFPLAY
	if (hFFplay != NULL) {
		char extFileName[256] = { 0 };
		char* extension;

		strncpy(extFileName, fileName, sizeof(extFileName) - 1);
		extension = PathFindExtension(extFileName);
		if (extension == NULL) {
			extension = strchr(extFileName, 0);
			*extension = '.';
		}
		for (unsigned int i = 0; i < sizeof(videoExts) / 4; ++i) {
			memcpy(extension + 1, videoExts[i], 4);
			if (INVALID_FILE_ATTRIBUTES != GetFileAttributes(extFileName)) {
				ffplay_play_video(extFileName, 0, 0, 0, 100);
				return;
			}
		}
		ffplay_play_video(fileName, 0, 0, 0, 100);
		return;
	}
#endif // FEATURE_FFPLAY
}

void WinStopFMV(bool isPlayback) {

}

bool IntroFMV(LPCTSTR fileName1, LPCTSTR fileName2) {
	LPCTSTR fullPath;

	if (SavedAppSettings.DisableFMV)
		return IsGameToExit;

	ShowCursor(FALSE);
	RenderFinish(true);
	IsFmvPlaying = TRUE;

	fullPath = GetFullPath(fileName1);
	WinPlayFMV(fullPath, true);
	WinStopFMV(true);

	fullPath = GetFullPath(fileName2);
	WinPlayFMV(fullPath, true);
	WinStopFMV(true);

	IsFmvPlaying = FALSE;
	if (!IsGameToExit)
		FmvBackToGame();
	ShowCursor(TRUE);

	return IsGameToExit;
}

/*
 * Inject function
 */
void Inject_Fmv() {
	INJECT(0x0044BE50, PlayFMV);
	INJECT(0x0044BED0, WinPlayFMV);
	INJECT(0x0044C1B0, WinStopFMV);
	INJECT(0x0044C200, IntroFMV);
}