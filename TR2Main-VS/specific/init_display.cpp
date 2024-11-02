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
#include "game/health.h"
#include "specific/display.h"
#include "specific/init_display.h"
#include "specific/file.h"
#include "specific/hwr.h"
#include "specific/init_3d.h"
#include "specific/output.h"
#include "specific/texture.h"
#include "specific/winmain.h"
#include "specific/winvid.h"
#include "global/vars.h"

#if defined(FEATURE_BACKGROUND_IMPROVED)
extern DWORD BGND_PictureWidth;
extern DWORD BGND_PictureHeight;
#endif

#ifdef FEATURE_BACKGROUND_IMPROVED
#include "modding/background_new.h"
#endif // FEATURE_BACKGROUND_IMPROVED

// Related to ERROR_CODE enum
static LPCTSTR ErrorStringTable[] = {
	"OK",
	"PreferredAdapterNotFound",
	"CantCreateWindow",
	"CantCreateDirectDraw",
	"CantInitRenderer",
	"CantCreateDirectInput",
	"CantCreateKeyboardDevice",
	"CantSetKBCooperativeLevel",
	"CantSetKBDataFormat",
	"CantAcquireKeyboard",
	"CantSetDSCooperativeLevel",
	"DD_SetExclusiveMode",
	"DD_ClearExclusiveMode",
	"SetDisplayMode",
	"CreateScreenBuffers",
	"GetBackBuffer",
	"CreatePalette",
	"SetPalette",
	"CreatePrimarySurface",
	"CreateBackBuffer",
	"CreateClipper",
	"SetClipperHWnd",
	"SetClipper",
	"CreateZBuffer",
	"AttachZBuffer",
	"CreateRenderBuffer",
	"CreatePictureBuffer",
	"D3D_Create",
	"CreateDevice",
	"CreateViewport",
	"AddViewport",
	"SetViewport2",
	"SetCurrentViewport",
	"ClearRenderBuffer",
	"UpdateRenderInfo",
	"GetThirdBuffer",
	"GoFullScreen",
	"GoWindowed",
	"WrongBitDepth",
	"GetPixelFormat",
	"GetDisplayMode",
};

#ifdef FEATURE_VIDEOFX_IMPROVED
extern DWORD ReflectionMode;
#endif // FEATURE_VIDEOFX_IMPROVED
SWR_BUFFER RenderBuffer = { 0, 0, NULL };
SWR_BUFFER PictureBuffer = { 0, 0, NULL };

static bool SWRBufferCreate(SWR_BUFFER* buffer, DWORD width, DWORD height) {
	if (!buffer || !width || !height) return false;
	buffer->width = width;
	buffer->height = height;
	buffer->bitmap = (LPBYTE)calloc(1, width * height);
	return (buffer->bitmap != NULL);
}

static void SWRBufferFree(SWR_BUFFER* buffer) {
	if (!buffer || !buffer->bitmap) return;
	free(buffer->bitmap);
	buffer->bitmap = NULL;
}

static bool SWRBufferClear(SWR_BUFFER* buffer, BYTE value) {
	if (!buffer || !buffer->bitmap || !buffer->width || !buffer->height) return false;
	memset(buffer->bitmap, value, buffer->width * buffer->height);
	return true;
}

LPDDS CaptureBufferSurface = NULL;
static void FreeCaptureBuffer() {
	if (CaptureBufferSurface != NULL) {
		CaptureBufferSurface->Release();
		CaptureBufferSurface = NULL;
	}
}

static int CreateCaptureBuffer() {
	FreeCaptureBuffer();

	if FAILED(D3DDev->CreateRenderTarget(GameVidWidth, GameVidHeight, D3DFMT_X8R8G8B8, D3DMULTISAMPLE_NONE, 0, TRUE, &CaptureBufferSurface, NULL))
		return -1;
	D3DDev->ColorFill(CaptureBufferSurface, NULL, RGBA_MAKE(0, 0, 0, 0));

	DDSDESC desc;
	if FAILED(CaptureBufferSurface->LockRect(&desc, NULL, 0)) {
		FreeCaptureBuffer();
		return -1;
	}
	CaptureBufferSurface->UnlockRect();
	return 0;
}

void CreateRenderBuffer() {
	SWRBufferFree(&RenderBuffer);
	if (!SWRBufferCreate(&RenderBuffer, GameVidWidth, GameVidHeight))
		throw ERR_CreateRenderBuffer;
}

void CreatePictureBuffer() {
	SWRBufferFree(&PictureBuffer);
	if (!SWRBufferCreate(&PictureBuffer, BGND_PictureWidth, BGND_PictureHeight))
		throw ERR_CreatePictureBuffer;
}

void ClearBuffers(DWORD flags, DWORD fillColor) {
	if (CHK_ANY(flags, CLRB_RenderBuffer))
		SWRBufferClear(&RenderBuffer, 0);

	if (CHK_ANY(flags, CLRB_PictureBuffer))
		SWRBufferClear(&PictureBuffer, 0);
}

void UpdateFrame(bool needRunMessageLoop, LPRECT rect) {
	// This method is pretty fast, so we can do it every frame
	LPDDS backBuffer = NULL;
	if SUCCEEDED(D3DDev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer)) {
		if (SavedAppSettings.RenderMode == RM_Hardware) {
			D3DDev->StretchRect(backBuffer, NULL, CaptureBufferSurface, NULL, D3DTEXF_NONE);
#if defined(FEATURE_VIDEOFX_IMPROVED)
			if (ReflectionMode != 0) SetEnvmapTexture(backBuffer);
#endif // FEATURE_VIDEOFX_IMPROVED
		}
		else {
			D3DDev->StretchRect(CaptureBufferSurface, NULL, backBuffer, NULL, D3DTEXF_NONE);
		}
		backBuffer->Release();
	}
	HRESULT res = D3DDev->Present(NULL, NULL, NULL, NULL);
	if (res == D3DERR_DEVICELOST && !IsGameToExit) {
		FreeCaptureBuffer();
#if defined(FEATURE_VIDEOFX_IMPROVED)
		FreeEnvmapTexture();
#endif // FEATURE_VIDEOFX_IMPROVED
		D3DDeviceCreate(NULL);
		if (SavedAppSettings.RenderMode == RM_Hardware) {
			HWR_InitState();
		}
		CreateCaptureBuffer();
	}
	D3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	if (needRunMessageLoop) {
		WinVidSpinMessageLoop(false);
	}
}

bool RenderInit() {
	return true;
}

void RenderStart(bool isReset) {
	int minWidth;
	int minHeight;
	DISPLAY_MODE dispMode;

	if (SavedAppSettings.FullScreen) {
		// FullScreen mode

		if (SavedAppSettings.VideoMode == NULL)
			throw ERR_GoFullScreen;

		dispMode.width = SavedAppSettings.VideoMode->body.width;
		dispMode.height = SavedAppSettings.VideoMode->body.height;
		dispMode.bpp = SavedAppSettings.VideoMode->body.bpp;
		dispMode.vga = SavedAppSettings.VideoMode->body.vga;

		if (!WinVidGoFullScreen(&dispMode))
			throw ERR_GoFullScreen;

		GameVidWidth = dispMode.width;
		GameVidHeight = dispMode.height;
		GameVidBPP = dispMode.bpp;
	}
	else {
		// Windowed mode

		minWidth = 320;
		minHeight = CalculateWindowHeight(320, 200);
		if (minHeight < 200) {
			minWidth = CalculateWindowWidth(320, 200);
			minHeight = 200;
		}

		WinVidSetMinWindowSize(minWidth, minHeight);
		if (!WinVidGoWindowed(SavedAppSettings.WindowWidth, SavedAppSettings.WindowHeight, &dispMode))
			throw ERR_GoWindowed;

		GameVidWidth = dispMode.width;
		GameVidHeight = dispMode.height;
		GameVidBPP = dispMode.bpp;
	}

#if defined(FEATURE_SCREENSHOT_IMPROVED) || defined(FEATURE_BACKGROUND_IMPROVED)
	FreeCaptureBuffer();
#endif // defined(FEATURE_SCREENSHOT_IMPROVED) || defined(FEATURE_BACKGROUND_IMPROVED)

#if defined(FEATURE_VIDEOFX_IMPROVED)
	FreeEnvmapTexture();
#endif // FEATURE_VIDEOFX_IMPROVED
	D3DDeviceCreate(NULL);
	TextureFormat.bpp = 32;
#if defined(FEATURE_SCREENSHOT_IMPROVED) || defined(FEATURE_BACKGROUND_IMPROVED)
	CreateCaptureBuffer();
#endif // defined(FEATURE_SCREENSHOT_IMPROVED) || defined(FEATURE_BACKGROUND_IMPROVED)

	if (SavedAppSettings.RenderMode == RM_Hardware) {
#if defined(FEATURE_BACKGROUND_IMPROVED)
		BGND2_PrepareCaptureTextures();
#endif // defined(FEATURE_BACKGROUND_IMPROVED)
	}
	else {
		CreateRenderBuffer();
		if (!PictureBuffer.bitmap) {
			CreatePictureBuffer();
		}
	}

	GameVidRect.left = 0;
	GameVidRect.top = 0;
	GameVidRect.right = GameVidWidth;
	GameVidRect.bottom = GameVidHeight;

	DumpWidth = GameVidWidth;
	DumpHeight = GameVidHeight;

	setup_screen_size();
}

void RenderFinish(bool needToClearTextures) {
	S_DontDisplayPicture();
	HWR_FreeTexturePages();
	CleanupTextures();
	SWRBufferFree(&PictureBuffer);
	SWRBufferFree(&RenderBuffer);
	FreeCaptureBuffer();
	Direct3DRelease();
}

bool ApplySettings(APP_SETTINGS* newSettings) {
	char modeString[64] = { 0 };
	APP_SETTINGS oldSettings = SavedAppSettings;

	if (newSettings != &SavedAppSettings)
		SavedAppSettings = *newSettings;

	try {
		RenderStart(false);
	}
	catch (...) {
		SavedAppSettings = oldSettings;
		try {
			RenderStart(false);
		}
		catch (...) {
			DISPLAY_MODE_LIST* modeList;
			DISPLAY_MODE_NODE* mode;
			DISPLAY_MODE targetMode;

			SavedAppSettings.PreferredDisplayAdapter = PrimaryDisplayAdapter;
			SavedAppSettings.RenderMode = RM_Software;
			SavedAppSettings.FullScreen = true;
			SavedAppSettings.TripleBuffering = false;

			targetMode.width = 640;
			targetMode.height = 480;
			targetMode.bpp = 0;
			modeList = &PrimaryDisplayAdapter->body.swDispModeList;

#if defined(FEATURE_NOLEGACY_OPTIONS)
			if (modeList->head) {
				targetMode.bpp = modeList->head->body.bpp;
			}
#endif // FEATURE_NOLEGACY_OPTIONS
			for (mode = modeList->head; mode; mode = mode->next) {
				if (!CompareVideoModes(&mode->body, &targetMode))
					break;
			}
			SavedAppSettings.VideoMode = mode ? mode : modeList->tail;
			try {
				RenderStart(false);
			}
			catch (...) {
				S_ExitSystem("Can't reinitialise renderer");
				return false; // the app is terminated here
			}
		}
	}

	S_InitialiseScreen(GFL_NOLEVEL);
	if (SavedAppSettings.RenderMode != oldSettings.RenderMode) {
		S_ReloadLevelGraphics(1, 1);
	}

#if defined(FEATURE_NOLEGACY_OPTIONS)
	snprintf(modeString, sizeof(modeString), "%dx%d", GameVidWidth, GameVidHeight);
#else // FEATURE_NOLEGACY_OPTIONS
	if (SavedAppSettings.FullScreen)
		sprintf(modeString, "%dx%dx%d", GameVidWidth, GameVidHeight, GameVidBPP);
	else
		sprintf(modeString, "%dx%d", GameVidWidth, GameVidHeight);
#endif // FEATURE_NOLEGACY_OPTIONS

	DisplayModeInfo(modeString);
	return true;
}

void FmvBackToGame() {
	try {
		RenderStart(true);
	}
	catch (...) {
		DISPLAY_MODE_LIST* modeList;
		DISPLAY_MODE_NODE* mode;
		DISPLAY_MODE targetMode;

		SavedAppSettings.PreferredDisplayAdapter = PrimaryDisplayAdapter;
		SavedAppSettings.RenderMode = RM_Software;
		SavedAppSettings.FullScreen = true;
		SavedAppSettings.TripleBuffering = false;

		targetMode.width = 640;
		targetMode.height = 480;
		targetMode.bpp = 0;
		modeList = &PrimaryDisplayAdapter->body.swDispModeList;

#if defined(FEATURE_NOLEGACY_OPTIONS)
		if (modeList->head) {
			targetMode.bpp = modeList->head->body.bpp;
		}
#endif // FEATURE_NOLEGACY_OPTIONS
		for (mode = modeList->head; mode; mode = mode->next) {
			if (!CompareVideoModes(&mode->body, &targetMode))
				break;
		}
		SavedAppSettings.VideoMode = mode ? mode : modeList->tail;
		try {
			RenderStart(false);
		}
		catch (...) {
			S_ExitSystem("Can't reinitialise renderer");
			return; // the app is terminated here
		}
	}
	// NOTE: this HWR init was absent in the original code, but must be done here
	if (SavedAppSettings.RenderMode == RM_Hardware) {
		HWR_InitState();
	}
}

void GameApplySettings(APP_SETTINGS* newSettings) {
	bool needInitRenderState = false;
	bool needRebuildBuffers = false;
	bool needAdjustTexel = false;
	DISPLAY_MODE dispMode;

	if (newSettings->PreferredDisplayAdapter != SavedAppSettings.PreferredDisplayAdapter ||
		newSettings->PreferredSoundAdapter != SavedAppSettings.PreferredSoundAdapter ||
		newSettings->PreferredJoystick != SavedAppSettings.PreferredJoystick)
	{
		return;
	}

	if (newSettings->ZBuffer != SavedAppSettings.ZBuffer ||
		newSettings->BilinearFiltering != SavedAppSettings.BilinearFiltering)
	{
		char msg[32] = { 0 };
		if (newSettings->ZBuffer != SavedAppSettings.ZBuffer) {
			snprintf(msg, sizeof(msg), "Z Buffer: %s",
				newSettings->ZBuffer ? GF_SpecificStringTable[SSI_On] : GF_SpecificStringTable[SSI_Off]);
		}
		else if (newSettings->BilinearFiltering != SavedAppSettings.BilinearFiltering) {
			snprintf(msg, sizeof(msg), "Bilinear Filter: %s",
				newSettings->BilinearFiltering ? GF_SpecificStringTable[SSI_On] : GF_SpecificStringTable[SSI_Off]);
		}
		if (*msg) DisplayModeInfo(msg);
		needInitRenderState = true;
	}

	if (newSettings->BilinearFiltering != SavedAppSettings.BilinearFiltering) {
		needAdjustTexel = true;
	}

	if (newSettings->RenderMode != SavedAppSettings.RenderMode ||
		newSettings->VideoMode != SavedAppSettings.VideoMode ||
		newSettings->FullScreen != SavedAppSettings.FullScreen)
	{
		ApplySettings(newSettings);
		S_AdjustTexelCoordinates();
		return;
	}

#if defined(FEATURE_VIDEOFX_IMPROVED)
	if (newSettings->LightingMode != SavedAppSettings.LightingMode) {
		const char* levels[3] = {
			"Low",
			"Medium",
			"High",
		};
		char msg[32] = { 0 };
		snprintf(msg, sizeof(msg), "Lighting Contrast: %s", levels[newSettings->LightingMode]);
		DisplayModeInfo(msg);
		needInitRenderState = true;
	}
#endif // FEATURE_VIDEOFX_IMPROVED

	if (!newSettings->FullScreen) {
		if (newSettings->WindowWidth != SavedAppSettings.WindowWidth || newSettings->WindowHeight != SavedAppSettings.WindowHeight) {
			if (!WinVidGoWindowed(newSettings->WindowWidth, newSettings->WindowHeight, &dispMode)) {
				return;
			}
			newSettings->WindowWidth = dispMode.width;
			newSettings->WindowHeight = dispMode.height;
			if (dispMode.width != SavedAppSettings.WindowWidth || dispMode.height != SavedAppSettings.WindowHeight) {
				needRebuildBuffers = true;
			}
		}
	}

	if (needInitRenderState) {
		SavedAppSettings.ZBuffer = newSettings->ZBuffer;
		SavedAppSettings.BilinearFiltering = newSettings->BilinearFiltering;
		setup_screen_size();

#if defined(FEATURE_VIDEOFX_IMPROVED)
		SavedAppSettings.LightingMode = newSettings->LightingMode;
		if (SavedAppSettings.RenderMode == RM_Software) {
			extern void UpdateDepthQ(bool isReset);
			UpdateDepthQ(false);
		}
#endif // FEATURE_VIDEOFX_IMPROVED

		if (SavedAppSettings.RenderMode == RM_Hardware) {
			HWR_InitState();
		}
	}

	if (needRebuildBuffers) {
		ClearBuffers(CLRB_WindowedPrimaryBuffer, 0);
		ApplySettings(newSettings);
	}

	if (needAdjustTexel) {
		S_AdjustTexelCoordinates();
	}
}

void UpdateGameResolution() {
	APP_SETTINGS newSettings = SavedAppSettings;
	char modeString[64] = { 0 };

	newSettings.WindowWidth = GameWindowWidth;
	newSettings.WindowHeight = GameWindowHeight;
	GameApplySettings(&newSettings);

	sprintf(modeString, "%dx%d", GameVidWidth, GameVidHeight);
	DisplayModeInfo(modeString);
}

LPCTSTR DecodeErrorMessage(DWORD errorCode) {
	return ErrorStringTable[errorCode];
}

/*
 * Inject function
 */
void Inject_InitDisplay() {
	INJECT(0x004489D0, CreateRenderBuffer);
	INJECT(0x00448A80, CreatePictureBuffer);
	INJECT(0x00448AF0, ClearBuffers);
	INJECT(0x00448DE0, UpdateFrame);
	INJECT(0x00448EF0, RenderInit);
	INJECT(0x00448F00, RenderStart);
	INJECT(0x004492B0, RenderFinish);
	INJECT(0x004493A0, ApplySettings);
	INJECT(0x004495B0, FmvBackToGame);
	INJECT(0x004496C0, GameApplySettings);
	INJECT(0x00449900, UpdateGameResolution);
	INJECT(0x00449970, DecodeErrorMessage);
}