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
#include "specific/output.h"
#include "3dsystem/3d_gen.h"
#include "3dsystem/phd_math.h"
#include "game/gameflow.h"
#include "specific/background.h"
#include "specific/display.h"
#include "specific/file.h"
#include "specific/game.h"
#include "specific/hwr.h"
#include "specific/init.h"
#include "specific/init_display.h"
#include "specific/init_sound_xaudio.h"
#include "specific/input.h"
#include "specific/texture.h"
#include "specific/utils.h"
#include "specific/winvid.h"
#include "global/vars.h"

#if defined(FEATURE_MOD_CONFIG)
#include "modding/mod_utils.h"
#endif

#if defined(FEATURE_HUD_IMPROVED)
#include "modding/psx_bar.h"

DWORD HealthBarMode = 2;
bool PsxBarPosEnabled = true;
double GameGUI_Scale = 1.0;
double InvGUI_Scale = 1.0;

// TODO: change it to slider in the option in-game !
static double UI_CalcScaleFromScreenHeight(int height)
{
	if (height <= 240)
		return 0.50;
	else if (height <= 480) // 480p
		return 1.00;
	else if (height <= 720) // 720p
		return 1.50;
	else if (height <= 1080) // 1080p
		return 2.00;
	else if (height <= 1440) // 1440p
		return 2.50;
	else if (height <= 1600) // 1600p
		return 3.00;
	else if (height <= 2160) // 4K
		return 5.00;
	return 7.00; // 8K and above
}
#endif // FEATURE_HUD_IMPROVED

#ifdef FEATURE_VIEW_IMPROVED
extern int CalculateFogShade(int depth);
#endif // FEATURE_VIEW_IMPROVED

#ifdef FEATURE_VIDEOFX_IMPROVED
DWORD ShadowMode = 1;
#endif // FEATURE_VIDEOFX_IMPROVED

#ifdef FEATURE_BACKGROUND_IMPROVED
#include "modding/background_new.h"
#include <sound.h>

extern DWORD BGND_PictureWidth;
extern DWORD BGND_PictureHeight;
extern bool BGND_IsCaptured;

extern DWORD InvBackgroundMode;
extern DWORD StatsBackgroundMode;
extern bool IsFadeToBlack;

DWORD PauseBackgroundMode = 1;
#endif // FEATURE_BACKGROUND_IMPROVED

typedef struct ShadowInfo_t {
	short x;
	short y;
	short z;
	short radius;
	short polyCount;
	short vertexCount;
	POS_3D vertex[32]; // original size was 8
} SHADOW_INFO;

// NOTE: there was no such backup in the original game
PHD_TEXTURE TextureBackupUV[ARRAY_SIZE(PhdTextureInfo)];

static bool SWR_StretchBlt(SWR_BUFFER* dstBuf, RECT* dstRect, SWR_BUFFER* srcBuf, RECT* srcRect) {
	if (!srcBuf || !srcBuf->bitmap || !srcBuf->width || !srcBuf->height ||
		!dstBuf || !dstBuf->bitmap || !dstBuf->width || !dstBuf->height)
	{
		return false;
	}

	int sx = 0;
	int sy = 0;
	int sw = srcBuf->width;
	int sh = srcBuf->height;
	if (srcRect) {
		sx = srcRect->left;
		sy = srcRect->top;
		sw = srcRect->right;
		sh = srcRect->bottom;
		CLAMP(sx, 0, (int)srcBuf->width);
		CLAMP(sy, 0, (int)srcBuf->height);
		CLAMP(sw, 0, (int)srcBuf->width);
		CLAMP(sh, 0, (int)srcBuf->height);
		sw -= sx;
		sh -= sy;
		if (!sw || !sh) return false;
	}

	int dx = 0;
	int dy = 0;
	int dw = dstBuf->width;
	int dh = dstBuf->height;
	if (dstRect) {
		dx = dstRect->left;
		dy = dstRect->top;
		dw = dstRect->right;
		dh = dstRect->bottom;
		CLAMP(dx, 0, (int)dstBuf->width);
		CLAMP(dy, 0, (int)dstBuf->height);
		CLAMP(dw, 0, (int)dstBuf->width);
		CLAMP(dh, 0, (int)dstBuf->height);
		dw -= dx;
		dh -= dy;
		if (!dw || !dh) return false;
	}

	if (dw < 0) {
		dx += dw;
		dw = -dw;
		sx += sw;
		sw = -sw;
	}

	if (dh < 0) {
		dy += dh;
		dh = -dh;
		sy += sh;
		sh = -sh;
	}

	int* x = (int*)malloc(sizeof(int) * dw);
	if (!x) return false;

	for (int i = 0; i < dw; ++i) {
		x[i] = i * sw / dw;
	}

	for (int j = 0; j < dh; ++j) {
		int y = j * sh / dh;
		LPBYTE src = srcBuf->bitmap + srcBuf->width * y + sx;
		LPBYTE dst = dstBuf->bitmap + dstBuf->width * j + dx;
		for (int i = 0; i < dw; ++i) {
			dst[i] = src[x[i]];
		}
	}

	free(x);
	return true;
}

int GetRenderScale(int unit, bool enableGUIScaling) {
#if defined(FEATURE_HUD_IMPROVED)
	double uiScale = UI_CalcScaleFromScreenHeight(PhdWinMaxY);
	int baseWidth = 0;
	int baseHeight = 0;
	if (enableGUIScaling)
	{
		baseWidth = int(double(PhdWinMaxX) / uiScale);
		baseHeight = int(double(PhdWinMaxY) / uiScale);
	}
	else
	{
		baseWidth = PhdWinMaxX;
		baseHeight = PhdWinMaxY;
	}
#else // !FEATURE_HUD_IMPROVED
	int baseWidth = 640;
	int baseHeight = 480;
#endif // FEATURE_HUD_IMPROVED
	int scaleX = (PhdWinWidth > baseWidth) ? MulDiv(PhdWinWidth, unit, baseWidth) : unit;
	int scaleY = (PhdWinHeight > baseHeight) ? MulDiv(PhdWinHeight, unit, baseHeight) : unit;
	return MIN(scaleX, scaleY);
}

int GetRenderScaleCustSize(int unit, int width, int height, bool enableGUIScaling) {
#if defined(FEATURE_HUD_IMPROVED)
	double uiScale = UI_CalcScaleFromScreenHeight(PhdWinHeight);
	int baseWidth = 0;
	int baseHeight = 0;
	if (enableGUIScaling)
	{
		baseWidth = int(double(width) / (IsVidSizeLock ? InvGUI_Scale : uiScale));
		baseHeight = int(double(height) / (IsVidSizeLock ? InvGUI_Scale : uiScale));
	}
	else
	{
		baseWidth = width;
		baseHeight = height;
	}
#else // !FEATURE_HUD_IMPROVED
	int baseWidth = 640;
	int baseHeight = 480;
#endif // FEATURE_HUD_IMPROVED
	int scaleX = (PhdWinWidth > baseWidth) ? MulDiv(PhdWinWidth, unit, baseWidth) : unit;
	int scaleY = (PhdWinHeight > baseHeight) ? MulDiv(PhdWinHeight, unit, baseHeight) : unit;
	return MIN(scaleX, scaleY);
}

int GetRenderDownscaled(int unit, bool enableGUIScaling) {
	return unit * PHD_ONE / GetRenderScale(PHD_ONE, enableGUIScaling);
}

int GetRenderHeightDownscaled(bool enableGUIScaling) {
	return GetRenderDownscaled(PhdWinHeight, enableGUIScaling);
}

int GetRenderWidthDownscaled(bool enableGUIScaling) {
	return GetRenderDownscaled(PhdWinWidth, enableGUIScaling);
}

int GetRenderHeight() {
	return PhdWinHeight;
}

int GetRenderWidth() {
	return PhdWinWidth;
}

void S_InitialisePolyList(BOOL clearBackBuffer) {
	DWORD flags = 0;

	if (WinVidNeedToResetBuffers) {
		WinVidSpinMessageLoop(false);
		if (SavedAppSettings.FullScreen) {
			flags = CLRB_BackBuffer | CLRB_PrimaryBuffer;
			if (SavedAppSettings.RenderMode == RM_Software)
				flags |= CLRB_RenderBuffer;
			if (SavedAppSettings.TripleBuffering)
				flags |= CLRB_ThirdBuffer;
		}
		else {
			flags = CLRB_WindowedPrimaryBuffer;
			if (SavedAppSettings.RenderMode == RM_Hardware)
				flags |= CLRB_BackBuffer;
			else
				flags |= CLRB_RenderBuffer;
		}
		ClearBuffers(flags, 0);
		clearBackBuffer = FALSE; // already cleared here
		WinVidNeedToResetBuffers = false;
	}

	flags = CLRB_PhdWinSize;
	if (SavedAppSettings.RenderMode == RM_Software) {
		// Software Renderer
		if (clearBackBuffer)
			flags |= CLRB_BackBuffer | CLRB_RenderBuffer;
		ClearBuffers(flags, 0);
	}
	else {
		// Hardware Renderer
		if (clearBackBuffer)
			flags |= CLRB_BackBuffer;
		if (SavedAppSettings.ZBuffer)
			flags |= CLRB_ZBuffer;
		ClearBuffers(flags, 0);
		HWR_BeginScene();
		HWR_EnableZBuffer(true, true);
	}
	phd_InitPolyList();
}

DWORD S_DumpScreen() {
	DWORD ticks = SyncTicks(TICKS_PER_FRAME); // NOTE: there was another code in the original game
	ScreenPartialDump();
	return ticks;
}

void S_ClearScreen() {
	ScreenClear(false);
}

void S_InitialiseScreen(GF_LEVEL_TYPE levelType) {
	if (levelType < 0) {
		// No Level
		FadeToPal(0, GamePalette8);
	}
	else {
		if (levelType != GFL_TITLE) {
			// Not title
			TempVideoRemove();
		}
		// Title or Any Level
		FadeToPal(30, GamePalette8);
	}

	if (SavedAppSettings.RenderMode == RM_Hardware) {
		HWR_InitState();
	}
}

void S_OutputPolyList() {
	DDSDESC desc;

	if (SavedAppSettings.RenderMode == RM_Software) {
		// Software renderer
		phd_SortPolyList();
		// prefetch surface lock
		extern LPDDS CaptureBufferSurface;
		HRESULT rc = CaptureBufferSurface->LockRect(&desc, NULL, D3DLOCK_DONOTWAIT);
		if (FAILED(rc) && rc != D3DERR_WASSTILLDRAWING) {
			return;
		}
		// do software rendering
		extern void PrepareSWR(int pitch, int height);
		PrepareSWR(RenderBuffer.width, RenderBuffer.height);
		phd_PrintPolyList(RenderBuffer.bitmap);
		// finish surface lock
		if (rc == D3DERR_WASSTILLDRAWING && FAILED(CaptureBufferSurface->LockRect(&desc, NULL, 0))) {
			return;
		}
		// copy bitmap to surface
		BYTE* src = RenderBuffer.bitmap;
		for (DWORD i = 0; i < RenderBuffer.height; ++i) {
			DWORD* dst = (DWORD*)((BYTE*)desc.pBits + desc.Pitch * i);
			for (DWORD j = 0; j < RenderBuffer.width; ++j) {
				if (*src) {
					BYTE r = WinVidPalette[*src].peRed;
					BYTE g = WinVidPalette[*src].peGreen;
					BYTE b = WinVidPalette[*src].peBlue;
					*dst++ = RGB_MAKE(r, g, b);
				}
				else {
					*dst++ = 0;
				}
				++src;
			}
		}
		// unlock surface
		CaptureBufferSurface->UnlockRect();
	}
	else {
		// Hardware renderer
		if (!SavedAppSettings.ZBuffer || !SavedAppSettings.DontSortPrimitives) {
			phd_SortPolyList();
		}
		HWR_DrawPolyList();
		D3DDev->EndScene();
	}
}

int S_GetObjectBounds(short* bPtr) {
	int xMin, xMax, yMin, yMax, zMin, zMax;
	int numZ, xv, yv, zv;
	PHD_VECTOR vtx[8];

	if (PhdMatrixPtr->_23 >= PhdFarZ)
		return 0; // object box is out of screen

	xMin = bPtr[0];
	xMax = bPtr[1];
	yMin = bPtr[2];
	yMax = bPtr[3];
	zMin = bPtr[4];
	zMax = bPtr[5];

	vtx[0].x = xMin;	vtx[0].y = yMin;	vtx[0].z = zMin;
	vtx[1].x = xMax;	vtx[1].y = yMin;	vtx[1].z = zMin;
	vtx[2].x = xMax;	vtx[2].y = yMax;	vtx[2].z = zMin;
	vtx[3].x = xMin;	vtx[3].y = yMax;	vtx[3].z = zMin;
	vtx[4].x = xMin;	vtx[4].y = yMin;	vtx[4].z = zMax;
	vtx[5].x = xMax;	vtx[5].y = yMin;	vtx[5].z = zMax;
	vtx[6].x = xMax;	vtx[6].y = yMax;	vtx[6].z = zMax;
	vtx[7].x = xMin;	vtx[7].y = yMax;	vtx[7].z = zMax;

	xMin = yMin = +0x3FFFFFFF;
	xMax = yMax = -0x3FFFFFFF;

	numZ = 0;

	for (int i = 0; i < 8; ++i) {
		zv = PhdMatrixPtr->_20 * vtx[i].x +
			PhdMatrixPtr->_21 * vtx[i].y +
			PhdMatrixPtr->_22 * vtx[i].z +
			PhdMatrixPtr->_23;

		if (zv > PhdNearZ && zv < PhdFarZ) {
			++numZ;
			zv /= PhdPersp;

			xv = (PhdMatrixPtr->_00 * vtx[i].x +
				PhdMatrixPtr->_01 * vtx[i].y +
				PhdMatrixPtr->_02 * vtx[i].z +
				PhdMatrixPtr->_03) / zv;

			if (xMin > xv)
				xMin = xv;
			if (xMax < xv)
				xMax = xv;

			yv = (PhdMatrixPtr->_10 * vtx[i].x +
				PhdMatrixPtr->_11 * vtx[i].y +
				PhdMatrixPtr->_12 * vtx[i].z +
				PhdMatrixPtr->_13) / zv;

			if (yMin > yv)
				yMin = yv;
			if (yMax < yv)
				yMax = yv;
		}
	}

	xMin += PhdWinCenterX;
	xMax += PhdWinCenterX;
	yMin += PhdWinCenterY;
	yMax += PhdWinCenterY;

	if (numZ == 0 || xMin > PhdWinRight || yMin > PhdWinBottom || xMax < PhdWinLeft || yMax < PhdWinTop)
		return 0; // object box is out of screen

	if (numZ < 8 || xMin < 0 || yMin < 0 || xMax > PhdWinMaxX || yMax > PhdWinMaxY)
		return -1; // object box is clipped

	return 1; // object box is totally on screen
}

void S_InsertBackPolygon(int x0, int y0, int x1, int y1) {
	ins_flat_rect(PhdWinMinX + x0, PhdWinMinY + y0, PhdWinMinX + x1, PhdWinMinY + y1, PhdFarZ + 1, InvColours[ICLR_Black]);
}

void S_PrintShadow(short radius, short* bPtr, ITEM_INFO* item) {
	static SHADOW_INFO ShadowInfo = {
		0,
		0,
		0,
		0x7FFF,
		1,
		8,
		{{0,0,0}},
	};
	int x0, x1, z0, z1, midX, midZ, xAdd, zAdd;

	x0 = bPtr[0];
	x1 = bPtr[1];
	z0 = bPtr[4];
	z1 = bPtr[5];

	midX = (x0 + x1) / 2;
	xAdd = (x1 - x0) * radius / 0x400;
	midZ = (z0 + z1) / 2;
	zAdd = (z1 - z0) * radius / 0x400;

#ifdef FEATURE_VIDEOFX_IMPROVED
	if (ShadowMode == 1) {
		// The shadow is a circle
		ShadowInfo.vertexCount = 32;
		for (int i = 0; i < ShadowInfo.vertexCount; ++i) {
			int angle = (PHD_180 + i * PHD_360) / ShadowInfo.vertexCount;
			ShadowInfo.vertex[i].x = midX + (xAdd * 2) * phd_sin(angle) / PHD_IONE;
			ShadowInfo.vertex[i].z = midZ + (zAdd * 2) * phd_cos(angle) / PHD_IONE;
		}
	}
	else
#endif // FEATURE_VIDEOFX_IMPROVED
	{
		// The shadow is an octagon
		ShadowInfo.vertexCount = 8;
		ShadowInfo.vertex[0].x = midX - xAdd;
		ShadowInfo.vertex[0].z = midZ + zAdd * 2;
		ShadowInfo.vertex[1].x = midX + xAdd;
		ShadowInfo.vertex[1].z = midZ + zAdd * 2;
		ShadowInfo.vertex[2].x = midX + xAdd * 2;
		ShadowInfo.vertex[2].z = midZ + zAdd;
		ShadowInfo.vertex[3].x = midX + xAdd * 2;
		ShadowInfo.vertex[3].z = midZ - zAdd;
		ShadowInfo.vertex[4].x = midX + xAdd;
		ShadowInfo.vertex[4].z = midZ - zAdd * 2;
		ShadowInfo.vertex[5].x = midX - xAdd;
		ShadowInfo.vertex[5].z = midZ - zAdd * 2;
		ShadowInfo.vertex[6].x = midX - xAdd * 2;
		ShadowInfo.vertex[6].z = midZ - zAdd;
		ShadowInfo.vertex[7].x = midX - xAdd * 2;
		ShadowInfo.vertex[7].z = midZ + zAdd;
	}

	// Update screen parameters
	FltWinLeft = (double)(PhdWinMinX + PhdWinLeft);
	FltWinTop = (double)(PhdWinMinY + PhdWinTop);
	FltWinRight = (double)(PhdWinMinX + PhdWinRight + 1);
	FltWinBottom = (double)(PhdWinMinY + PhdWinBottom + 1);
	FltWinCenterX = (double)(PhdWinMinX + PhdWinCenterX);
	FltWinCenterY = (double)(PhdWinMinY + PhdWinCenterY);

	// Transform and print the shadow
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x, item->floor, item->pos.z);
	phd_RotY(item->pos.rotY);
	if (calc_object_vertices(&ShadowInfo.polyCount)) {
		// NOTE: Here 24 is DepthQ index (shade factor).
		// 0 lightest, 15 no shade, 31 darkest (pitch black).
		// But original code has value 32 supposed to be interpreted as 24 (which means 50% darker)
		// Also 32 is maximum valid value in the original code, though it is DepthQTable range violation.
		// This trick worked because DepthQIndex array was right after DepthQ array in the memory
		// (DepthQIndex is equal to &DepthQ[24].index).This allocation is not guaranteed on some systems, so it was fixed
		ins_poly_trans8(PhdVBuf, 24);
	}
	phd_PopMatrix();
}

struct LIGHT_ROOM
{
	int x = 0, y = 0, z = 0;
	int brightest = 0;
	short ambient = 0;
};

static LIGHT_ROOM S_CalculateRoomStaticLights(int x, int y, int z, short roomNumber)
{
	LIGHT_ROOM light_result;
	ROOM_INFO* room = &Rooms[roomNumber];
	LIGHT_INFO* light = NULL;
	int xdist, ydist, zdist;
	int distance;
	int intensity, falloff;
	int shade;

	light_result.ambient = room->ambient;
	if (room->lightMode != 0) {
		int lightShade = RoomLightShades[room->lightMode];
		for (int i = 0; i < room->numLights; ++i) {
			light = &room->light[i];
			xdist = x - light->x;
			ydist = y - light->y;
			zdist = z - light->z;
			falloff = light->fallOff;
			intensity = light->intensity;
			distance = (SQR(xdist) + SQR(ydist) + SQR(zdist)) >> 12;
			falloff = SQR(falloff) >> 12;
			shade = (falloff * intensity / (falloff + distance)) * lightShade / (WIBBLE_SIZE - 1);
			if (shade > light_result.brightest) {
				light_result.brightest = shade;
				light_result.x = xdist;
				light_result.y = ydist;
				light_result.z = zdist;
			}
		}
	}
	else {
		for (int i = 0; i < room->numLights; ++i) {
			light = &room->light[i];
			xdist = x - light->x;
			ydist = y - light->y;
			zdist = z - light->z;
			falloff = light->fallOff;
			intensity = light->intensity;
			distance = (SQR(xdist) + SQR(ydist) + SQR(zdist)) >> 12;
			falloff = SQR(falloff) >> 12;
			shade = falloff * intensity / (falloff + distance);
			if (shade > light_result.brightest) {
				light_result.brightest = shade;
				light_result.x = xdist;
				light_result.y = ydist;
				light_result.z = zdist;
			}
		}
	}

	return light_result;
}

static int S_CalculateDynamicLights(int x, int y, int z, int adder, bool isStatic)
{
	for (DWORD i = 0; i < DynamicLightCount; ++i) {
		LIGHT_INFO* light = &DynamicLights[i];
		int xDist = x - light->x;
		int yDist = y - light->y;
		int zDist = z - light->z;
		int falloff = light->fallOff;
		short intensity = light->intensity;
		int radius = 1 << falloff;
		if ((xDist >= -radius && xDist <= radius) &&
			(yDist >= -radius && yDist <= radius) &&
			(zDist >= -radius && zDist <= radius))
		{
			int distance = SQR(xDist) + SQR(yDist) + SQR(zDist);
			if (distance <= SQR(radius)) {
				int shade = (1 << intensity) - (distance >> (2 * falloff - intensity));
				if (isStatic)
				{
					adder -= shade;
					if (adder < 0)
						break;
				}
				else
				{
					adder += shade;
				}
			}
		}
	}
	return adder;
}

static void S_CalculateFinalLights(LIGHT_ROOM& light, int adder, bool isStatic)
{
	VECTOR_ANGLES angles;

	if (!isStatic)
		adder /= 2;

	if (adder == 0) {
		LsAdder = light.ambient;
		LsDivider = 0;
	}
	else {
		if (isStatic) {
			LsAdder = ((light.ambient - 0x800) - adder) - 0x500;
			LsDivider = (1 << 24) / adder;
		}
		else {
			LsAdder = light.ambient - adder;
			LsDivider = (1 << 27) / adder;
		}
		phd_GetVectorAngles(light.x, light.y, light.z, &angles);
		phd_RotateLight(angles.rotY, angles.rotX);
	}
}

static void S_CalculateFinalLightFromFog()
{
	int depth = PhdMatrixPtr->_23 >> W2V_SHIFT;
#ifdef FEATURE_VIEW_IMPROVED
	LsAdder += CalculateFogShade(depth);
#else // !FEATURE_VIEW_IMPROVED
	if (depth > DEPTHQ_START) // fog begin
		LsAdder += depth - DEPTHQ_START;
#endif // FEATURE_VIEW_IMPROVED
	if (LsAdder > 0x1FFF) // fog end
		LsAdder = 0x1FFF;
}

void S_CalculateLight(int x, int y, int z, short roomNumber)
{
	LIGHT_ROOM light = S_CalculateRoomStaticLights(x, y, z, roomNumber);
	int adder = S_CalculateDynamicLights(x, y, z, light.brightest, false);
	S_CalculateFinalLights(light, adder, false);
	S_CalculateFinalLightFromFog();
}

void S_CalculateStaticLight(short adder) {
	LsAdder = adder - 0x1000;
	S_CalculateFinalLightFromFog();
}

void S_CalculateStaticMeshLight(int x, int y, int z, int shade1, int shade2, ROOM_INFO* room) {
	int adder = shade1;
	if (room->lightMode != 0) {
		adder += (shade2 - shade1) * RoomLightShades[room->lightMode] / (WIBBLE_SIZE - 1);
	}
	adder = S_CalculateDynamicLights(x, y, z, adder, true);
	S_CalculateStaticLight(adder);
}

void S_LightRoom(ROOM_INFO* room) {
	ROOM_VERTEX* roomVtx;
	int roomVtxCount;
	int shade, falloff, intensity;
	int xPos, yPos, zPos;
	int xDist, yDist, zDist, distance, radius;

	if (room->lightMode != 0) {
		int* roomLightTable = RoomLightTables[RoomLightShades[room->lightMode]].table;
		roomVtxCount = room->data->vtxSize;
		roomVtx = room->data->vertices;
		for (int i = 0; i < roomVtxCount; ++i) {
			short wibble = roomLightTable[roomVtx[i].lightTableValue % WIBBLE_SIZE];
			roomVtx[i].lightAdder = roomVtx[i].lightBase + wibble;
		}
	}
	else if ((room->flags & 0x10) != 0) {
		roomVtxCount = room->data->vtxSize;
		roomVtx = room->data->vertices;
		for (int i = 0; i < roomVtxCount; ++i) {
			roomVtx[i].lightAdder = roomVtx[i].lightBase;
		}
		room->flags &= ~0x10;
	}

	int xMin = 0x400;
	int zMin = 0x400;
	int xMax = 0x400 * (room->xSize - 1);
	int zMax = 0x400 * (room->zSize - 1);

	for (DWORD i = 0; i < DynamicLightCount; ++i) {
		xPos = DynamicLights[i].x - room->x;
		yPos = DynamicLights[i].y;
		zPos = DynamicLights[i].z - room->z;
		falloff = DynamicLights[i].fallOff;
		intensity = DynamicLights[i].intensity;
		radius = 1 << falloff;
		if (xPos + radius >= xMin && zPos + radius >= zMin && xPos - radius <= xMax && zPos - radius <= zMax) {
			room->flags |= 0x10;
			roomVtxCount = room->data->vtxSize;
			roomVtx = room->data->vertices;
			for (int j = 0; j < roomVtxCount; ++j) {
				if (roomVtx[j].lightAdder != 0) {
					xDist = roomVtx[j].x - xPos;
					yDist = roomVtx[j].y - yPos;
					zDist = roomVtx[j].z - zPos;
					if ((xDist >= -radius && xDist <= radius) &&
						(yDist >= -radius && yDist <= radius) &&
						(zDist >= -radius && zDist <= radius))
					{
						distance = SQR(xDist) + SQR(yDist) + SQR(zDist);
						if (distance <= SQR(radius)) {
							shade = (1 << intensity) - (distance >> (2 * falloff - intensity));
							roomVtx[j].lightAdder -= shade;
							if (roomVtx[j].lightAdder < 0)
								roomVtx[j].lightAdder = 0;
						}
					}
				}
			}
		}
	}
}

void S_DrawHealthBar(int percent) {
#ifdef FEATURE_HUD_IMPROVED
	int pixel = GetRenderScale(1);
	int barWidth = GetRenderScale(100);
	CLAMP(barWidth, PhdWinMinX + (pixel * 4), PhdWinMaxX - (pixel * 4));
	int barHeight = GetRenderScale(5);

	BAR_CONFIG& barconfig = Mod.laraBar.health;
	int barXOffset;
	int barYOffset;
	if (barconfig.isCentered)
	{
		int posX = GetRenderScale(barconfig.CENTER_xpos);
		int posY = GetRenderScale(barconfig.CENTER_ypos);
		barXOffset = GetRenderDownscaled(PhdWinCenterX, false) - posX;
		barYOffset = GetRenderDownscaled(PhdWinCenterY, false) - posY;
	}
	else
	{
		barXOffset = GetRenderScale(PsxBarPosEnabled ? barconfig.PSX_xpos : barconfig.PC_xpos);
		barYOffset = GetRenderScale(PsxBarPosEnabled ? barconfig.PSX_ypos : barconfig.PC_ypos);
	}
	CLAMP(barXOffset, PhdWinMinX + (pixel * 2), PhdWinMaxX - (pixel * 2));
	CLAMP(barYOffset, PhdWinMinY + (pixel * 2), PhdWinMaxY - barHeight - (pixel * 2));

	int x0, x1;
	if (PsxBarPosEnabled) {
		x1 = PhdWinMinX + DumpWidth - barXOffset;
		x0 = x1 - barWidth;
	}
	else {
		x0 = PhdWinMinX + barXOffset;
		x1 = x0 + barWidth;
	}

	int y0 = PhdWinMinY + barYOffset;
	int y1 = y0 + barHeight;
	int bar = barWidth * percent / PHD_ONE;

	// Disable underwater shading
	IsShadeEffect = false;

	if (HealthBarMode != 0 && SavedAppSettings.RenderMode == RM_Hardware) {
		if (SavedAppSettings.ZBuffer) {
			PSX_DrawHealthBar(x0, y0, x1, y1, bar, pixel, 255);
		}
		else {
			PSX_InsertHealthBar(x0, y0, x1, y1, bar, pixel, 255);
		}
		return;
	}

	// Frame
	ins_flat_rect(x0 - pixel * 2, y0 - pixel * 2, x1 + pixel * 2, y1 + pixel * 2, PhdNearZ + 50, InvColours[ICLR_White]);
	ins_flat_rect(x0 - pixel * 1, y0 - pixel * 1, x1 + pixel * 2, y1 + pixel * 2, PhdNearZ + 40, InvColours[ICLR_Gray]);
	ins_flat_rect(x0 - pixel * 1, y0 - pixel * 1, x1 + pixel * 1, y1 + pixel * 1, PhdNearZ + 30, InvColours[ICLR_Black]);

	// Health bar
	if (bar > 0) {
#if defined(FEATURE_MOD_CONFIG)
		ins_flat_rect(x0, y0 + pixel * 0, x0 + bar, y0 + barHeight, PhdNearZ + 20, InvColours[barconfig.PC_color[0]]);
		ins_flat_rect(x0, y0 + pixel * 1, x0 + bar, y0 + pixel * 2, PhdNearZ + 10, InvColours[barconfig.PC_color[1]]);
#else
		ins_flat_rect(x0, y0 + pixel * 0, x0 + bar, y0 + barHeight, PhdNearZ + 20, InvColours[ICLR_Red]);
		ins_flat_rect(x0, y0 + pixel * 1, x0 + bar, y0 + pixel * 2, PhdNearZ + 10, InvColours[ICLR_Orange]);
#endif
	}
#else // !FEATURE_HUD_IMPROVED
	int i;

	int barWidth = 100;
	int barHeight = 5;

	int x0 = 8;
	int y0 = 8;
	int x1 = x0 + barWidth;
	int y1 = y0 + barHeight;

	int bar = barWidth * percent / 100;

	// Disable underwater shading
	IsShadeEffect = false;

	// Black background
	for (i = 0; i < (barHeight + 2); ++i)
		ins_line(x0 - 2, y0 + i - 1, x1 + 1, y0 + i - 1, PhdNearZ + 50, InvColours[ICLR_Black]);

	// Dark frame
	ins_line(x0 - 2, y1 + 1, x1 + 2, y1 + 1, PhdNearZ + 40, InvColours[ICLR_Gray]);
	ins_line(x1 + 2, y0 - 2, x1 + 2, y1 + 1, PhdNearZ + 40, InvColours[ICLR_Gray]);

	// Light frame
	ins_line(x0 - 2, y0 - 2, x1 + 2, y0 - 2, PhdNearZ + 30, InvColours[ICLR_White]);
	ins_line(x0 - 2, y1 + 1, x0 - 2, y0 - 2, PhdNearZ + 30, InvColours[ICLR_White]);

	// Health bar
	if (bar > 0) {
		for (i = 0; i < barHeight; ++i)
			ins_line(x0, y0 + i, x0 + bar, y0 + i, PhdNearZ + 20, (i == 1) ? InvColours[ICLR_Orange] : InvColours[ICLR_Red]);
	}
#endif // FEATURE_HUD_IMPROVED
}

void S_DrawEnemyHealthBar(int percent, int originalHP) {
	if (IsInventoryActive) return; // NOTE: Remove the bar if lara is in inventory !

#if defined(FEATURE_HUD_IMPROVED)
	BAR_CONFIG& barconfig = Mod.enemyBar;
	int pixel = GetRenderScale(1);
	int barWidth = GetRenderScale(barconfig.basedOnEnemyHealth ? originalHP : 100);
	CLAMP(barWidth, PhdWinMinX + (pixel * 2), PhdWinMaxX - (pixel * 2));
	int barHeight = GetRenderScale(5);
	int barXOffset;
	int barYOffset;
	if (barconfig.isCentered)
	{
		int posX = GetRenderScale(barconfig.CENTER_xpos);
		int posY = GetRenderScale(barconfig.CENTER_ypos);
		posX += barconfig.basedOnEnemyHealth ? (barWidth/2) : GetRenderScale(50);
		barXOffset = GetRenderDownscaled(PhdWinCenterX, false) - posX;
		barYOffset = GetRenderDownscaled(PhdWinCenterY, false) - posY;
	}
	else
	{
		barXOffset = GetRenderScale(PsxBarPosEnabled ? barconfig.PSX_xpos : barconfig.PC_xpos);
		barYOffset = GetRenderScale(PsxBarPosEnabled ? barconfig.PSX_ypos : barconfig.PC_ypos);
	}
	CLAMP(barXOffset, PhdWinMinX + (pixel * 2), PhdWinMaxX - (pixel * 2));
	CLAMP(barYOffset, PhdWinMinY + barHeight + (pixel * 2), PhdWinMaxY - barHeight - (pixel * 2));

	int x0, x1;
	if (PsxBarPosEnabled) {
		x1 = PhdWinMinX + DumpWidth - barXOffset;
		x0 = x1 - barWidth;
	}
	else {
		x0 = PhdWinMinX + barXOffset;
		x1 = x0 + barWidth;
	}

	int y0 = PhdWinMinY + barYOffset;
	int y1 = y0 + barHeight;
	int bar = barWidth * percent / PHD_ONE;

	// Disable underwater shading
	IsShadeEffect = false;

	if (HealthBarMode != 0 && SavedAppSettings.RenderMode == RM_Hardware) {
		if (SavedAppSettings.ZBuffer) {
			PSX_DrawEnemyBar(x0, y0, x1, y1, bar, pixel, 255);
		} else {
			PSX_InsertEnemyBar(x0, y0, x1, y1, bar, pixel, 255);
		}
		return;
	}

	// Frame
	ins_flat_rect(x0 - pixel * 2, y0 - pixel * 2, x1 + pixel * 2, y1 + pixel * 2, PhdNearZ + 50, InvColours[ICLR_White]);
	ins_flat_rect(x0 - pixel * 1, y0 - pixel * 1, x1 + pixel * 2, y1 + pixel * 2, PhdNearZ + 40, InvColours[ICLR_Gray]);
	ins_flat_rect(x0 - pixel * 1, y0 - pixel * 1, x1 + pixel * 1, y1 + pixel * 1, PhdNearZ + 30, InvColours[ICLR_Black]);

	// Health bar
	if (bar > 0) {
#if defined(FEATURE_MOD_CONFIG)
		ins_flat_rect(x0, y0 + pixel * 0, x0 + bar, y0 + barHeight, PhdNearZ + 20, InvColours[barconfig.PC_color[0]]);
		ins_flat_rect(x0, y0 + pixel * 1, x0 + bar, y0 + pixel * 2, PhdNearZ + 10, InvColours[barconfig.PC_color[1]]);
#else
		ins_flat_rect(x0, y0 + pixel * 0, x0 + bar, y0 + barHeight, PhdNearZ + 20, InvColours[ICLR_Red]);
		ins_flat_rect(x0, y0 + pixel * 1, x0 + bar, y0 + pixel * 2, PhdNearZ + 10, InvColours[ICLR_Red]);
#endif
	}
#else // !FEATURE_HUD_IMPROVED
	int i;

	int barWidth = originalHP;
	int barHeight = 5;

	int x0 = 8;
	int y0 = 8;
	int x1 = x0 + barWidth;
	int y1 = y0 + barHeight;

	int bar = barWidth * percent / originalHP;

	// Disable underwater shading
	IsShadeEffect = false;

	// Black background
	for (i = 0; i < (barHeight + 2); ++i)
		ins_line(x0 - 2, y0 + i - 1, x1 + 1, y0 + i - 1, PhdNearZ + 50, InvColours[ICLR_Black]);

	// Dark frame
	ins_line(x0 - 2, y1 + 1, x1 + 2, y1 + 1, PhdNearZ + 40, InvColours[ICLR_Gray]);
	ins_line(x1 + 2, y0 - 2, x1 + 2, y1 + 1, PhdNearZ + 40, InvColours[ICLR_Gray]);

	// Light frame
	ins_line(x0 - 2, y0 - 2, x1 + 2, y0 - 2, PhdNearZ + 30, InvColours[ICLR_White]);
	ins_line(x0 - 2, y1 + 1, x0 - 2, y0 - 2, PhdNearZ + 30, InvColours[ICLR_White]);

	// Health bar
	if (bar > 0) {
		for (i = 0; i < barHeight; ++i)
			ins_line(x0, y0 + i, x0 + bar, y0 + i, PhdNearZ + 20, (i == 1) ? InvColours[ICLR_Orange] : InvColours[ICLR_Red]);
	}
#endif // FEATURE_HUD_IMPROVED
}

void S_DrawAirBar(int percent) {
#ifdef FEATURE_HUD_IMPROVED
	BAR_CONFIG& barconfig = Mod.laraBar.air;
	int pixel = GetRenderScale(1);
	int barWidth = GetRenderScale(100);
	CLAMP(barWidth, PhdWinMinX + (pixel * 4), PhdWinMaxX - (pixel * 4));
	int barHeight = GetRenderScale(5);
	int barXOffset;
	int barYOffset;
	if (barconfig.isCentered)
	{
		int posX = GetRenderScale(barconfig.CENTER_xpos);
		int posY = GetRenderScale(barconfig.CENTER_ypos);
		barXOffset = GetRenderDownscaled(PhdWinCenterX, false) - posX;
		barYOffset = GetRenderDownscaled(PhdWinCenterY, false) - posY;
	}
	else
	{
		barXOffset = GetRenderScale(PsxBarPosEnabled ? barconfig.PSX_xpos : barconfig.PC_xpos);
		barYOffset = GetRenderScale(PsxBarPosEnabled ? barconfig.PSX_ypos : barconfig.PC_ypos);
	}
	CLAMP(barXOffset, PhdWinMinX + (pixel * 2), PhdWinMaxX - (pixel * 2));
	CLAMP(barYOffset, PhdWinMinY + barHeight + (pixel * 2), PhdWinMaxY - barHeight - (pixel * 2));

	int x0, x1;
	if (PsxBarPosEnabled) {
		x1 = PhdWinMinX + DumpWidth - barXOffset;
		x0 = x1 - barWidth;
	}
	else {
		x0 = PhdWinMinX + barXOffset;
		x1 = x0 + barWidth;
	}

	int y0 = PhdWinMinY + barYOffset;
	int y1 = y0 + barHeight;
	int bar = barWidth * percent / PHD_ONE;

	// Disable underwater shading
	IsShadeEffect = false;

	if (HealthBarMode != 0 && SavedAppSettings.RenderMode == RM_Hardware) {
		if (SavedAppSettings.ZBuffer) {
			PSX_DrawAirBar(x0, y0, x1, y1, bar, pixel, 255);
		}
		else {
			PSX_InsertAirBar(x0, y0, x1, y1, bar, pixel, 255);
		}
		return;
	}

	// Frame
	ins_flat_rect(x0 - pixel * 2, y0 - pixel * 2, x1 + pixel * 2, y1 + pixel * 2, PhdNearZ + 50, InvColours[ICLR_White]);
	ins_flat_rect(x0 - pixel * 1, y0 - pixel * 1, x1 + pixel * 2, y1 + pixel * 2, PhdNearZ + 40, InvColours[ICLR_Gray]);
	ins_flat_rect(x0 - pixel * 1, y0 - pixel * 1, x1 + pixel * 1, y1 + pixel * 1, PhdNearZ + 30, InvColours[ICLR_Black]);

	// Air bar
	if (bar > 0) {
#if defined(FEATURE_MOD_CONFIG)
		ins_flat_rect(x0, y0 + pixel * 0, x0 + bar, y0 + barHeight, PhdNearZ + 20, InvColours[barconfig.PC_color[0]]);
		ins_flat_rect(x0, y0 + pixel * 1, x0 + bar, y0 + pixel * 2, PhdNearZ + 10, InvColours[barconfig.PC_color[1]]);
#else
		ins_flat_rect(x0, y0 + pixel * 0, x0 + bar, y0 + barHeight, PhdNearZ + 20, InvColours[ICLR_Blue]);
		ins_flat_rect(x0, y0 + pixel * 1, x0 + bar, y0 + pixel * 2, PhdNearZ + 10, InvColours[ICLR_White]);
#endif
	}
#else // !FEATURE_HUD_IMPROVED
	int i;

	int barWidth = 100;
	int barHeight = 5;

	int x1 = DumpWidth - 10;
	int x0 = x1 - barWidth;
	int y0 = 8;
	int y1 = y0 + barHeight;

	int bar = barWidth * percent / 100;

	// Disable underwater shading
	IsShadeEffect = false;

	// Black background
	for (i = 0; i < (barHeight + 2); ++i)
		ins_line(x0 - 2, y0 + i - 1, x1 + 1, y0 + i - 1, PhdNearZ + 50, InvColours[ICLR_Black]);

	// Dark frame
	ins_line(x0 - 2, y1 + 1, x1 + 2, y1 + 1, PhdNearZ + 40, InvColours[ICLR_Gray]);
	ins_line(x1 + 2, y0 - 2, x1 + 2, y1 + 1, PhdNearZ + 40, InvColours[ICLR_Gray]);

	// Light frame
	ins_line(x0 - 2, y0 - 2, x1 + 2, y0 - 2, PhdNearZ + 30, InvColours[ICLR_White]);
	ins_line(x0 - 2, y1 + 1, x0 - 2, y0 - 2, PhdNearZ + 30, InvColours[ICLR_White]);

	// Air bar
	if (bar > 0) {
		for (i = 0; i < barHeight; ++i)
			ins_line(x0, y0 + i, x0 + bar, y0 + i, PhdNearZ + 20, (i == 1) ? InvColours[ICLR_White] : InvColours[ICLR_Blue]);
	}
#endif // FEATURE_HUD_IMPROVED
}

void AnimateTextures(int nTicks) {
	static int tickComp = 0;
	short i, j;
	short* ptr;
	PHD_TEXTURE temp1, temp2;

	tickComp += nTicks;
	while (tickComp > TICKS_PER_FRAME * 5) {
		ptr = AnimatedTextureRanges;
		i = *(ptr++);
		for (; i > 0; --i, ++ptr) {
			j = *(ptr++);
			temp1 = PhdTextureInfo[*ptr];
			temp2 = TextureBackupUV[*ptr];
			for (; j > 0; --j, ++ptr) {
				PhdTextureInfo[ptr[0]] = PhdTextureInfo[ptr[1]];
				TextureBackupUV[ptr[0]] = TextureBackupUV[ptr[1]];
			}
			PhdTextureInfo[*ptr] = temp1;
			TextureBackupUV[*ptr] = temp2;
		}
		tickComp -= TICKS_PER_FRAME * 5;
	}
}

void S_SetupBelowWater(BOOL underwater) {
	if (IsWet != underwater) {
		FadeToPal(1, underwater ? WaterPalette : GamePalette8);
		IsWet = underwater;
	}
	IsWaterEffect = true;
	IsShadeEffect = true;
	IsWibbleEffect = !underwater;
}

void S_SetupAboveWater(BOOL underwater) {
	IsWaterEffect = false;
	IsShadeEffect = underwater;
	IsWibbleEffect = underwater;
}

void S_AnimateTextures(int nTicks) {
	WibbleOffset = (WibbleOffset + nTicks / TICKS_PER_FRAME) % WIBBLE_SIZE;
	RoomLightShades[1] = GetRandomDraw() & (WIBBLE_SIZE - 1);
	RoomLightShades[2] = (WIBBLE_SIZE - 1) * (phd_sin(WibbleOffset * PHD_360 / WIBBLE_SIZE) + PHD_IONE) / 2 / PHD_IONE;

	if (GF_SunsetEnabled) {
		// NOTE: in the original game there was: SunsetTimer += nTicks;
		// so the timer was reset every time when the saved game is loaded
		SunsetTimer = SaveGame.statistics.timer * TICKS_PER_FRAME;
		CLAMPG(SunsetTimer, SUNSET_TIMEOUT);
		RoomLightShades[3] = (WIBBLE_SIZE - 1) * SunsetTimer / SUNSET_TIMEOUT;
	}
	AnimateTextures(nTicks);
}

void S_DisplayPicture(LPCTSTR fileName, BOOL isTitle) {
#ifdef FEATURE_BACKGROUND_IMPROVED
	if (!isTitle) {
		init_game_malloc();
	}
	BGND2_LoadPicture(fileName, isTitle, FALSE);
#else // !FEATURE_BACKGROUND_IMPROVED
	DWORD bytesRead;
	HANDLE hFile;
	DWORD fileSize;
	DWORD bitmapSize;
	BYTE* fileData;
	BYTE* bitmapData;
	LPCTSTR fullPath;

	fullPath = GetFullPath(fileName);
	hFile = CreateFile(fullPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	if (!isTitle)
		init_game_malloc();

	fileSize = GetFileSize(hFile, NULL);
	fileData = (BYTE*)game_malloc(fileSize, GBUF_LoadPiccyBuffer);
	ReadFile(hFile, fileData, fileSize, &bytesRead, NULL);
	CloseHandle(hFile);

	bitmapSize = 640 * 480;
	bitmapData = (BYTE*)game_malloc(bitmapSize, GBUF_LoadPiccyBuffer);
	DecompPCX(fileData, fileSize, bitmapData, PicPalette);

	if (SavedAppSettings.RenderMode == RM_Software) {
#if (DIRECT3D_VERSION >= 0x900)
		if (PictureBuffer.bitmap != NULL)
			memcpy(PictureBuffer.bitmap, bitmapData, PictureBuffer.width * PictureBuffer.height);
#else // (DIRECT3D_VERSION >= 0x900)
		WinVidCopyBitmapToBuffer(PictureBufferSurface, bitmapData);
#endif // (DIRECT3D_VERSION >= 0x900)
	}
	else {
		BGND_Make640x480(bitmapData, PicPalette);
	}

	if (!isTitle) {
#if (DIRECT3D_VERSION >= 0x900)
		memcpy(GamePalette8, PicPalette, sizeof(GamePalette8));
#else // (DIRECT3D_VERSION >= 0x900)
		CopyBitmapPalette(PicPalette, bitmapData, bitmapSize, GamePalette8);
#endif // (DIRECT3D_VERSION >= 0x900)
	}

	game_free(fileSize + bitmapSize);
#endif // FEATURE_BACKGROUND_IMPROVED
}

void S_SyncPictureBufferPalette() {
	if (PictureBuffer.bitmap == NULL) return;
	SyncSurfacePalettes(PictureBuffer.bitmap, PictureBuffer.width, PictureBuffer.height, PictureBuffer.width, PicPalette, PictureBuffer.bitmap, PictureBuffer.width, GamePalette8, TRUE);
	memcpy(PicPalette, GamePalette8, sizeof(PicPalette));
}

void S_DontDisplayPicture() {
	if (SavedAppSettings.RenderMode == RM_Hardware) {
		BGND_Free();
		BGND_PictureIsReady = false;
	}
}

void ScreenDump() {
	UpdateFrame(true, NULL);
}

void ScreenPartialDump() {
	UpdateFrame(true, &PhdWinRect);
}

void FadeToPal(int fadeValue, RGB888* palette) {
	int i, j;
	int palStartIdx = 0;
	int palEndIdx = 256;
	PALETTEENTRY fadePal[256] = {};

	if (SavedAppSettings.RenderMode != RM_Software)
		return;

	if (fadeValue <= 1) {
		for (i = palStartIdx; i < palEndIdx; ++i) {
			WinVidPalette[i].peRed = palette[i].red;
			WinVidPalette[i].peGreen = palette[i].green;
			WinVidPalette[i].peBlue = palette[i].blue;
		}
		S_InitialisePolyList(FALSE);
		S_OutputPolyList();
		return;
	}

	for (i = palStartIdx; i < palEndIdx; ++i) {
		fadePal[i] = WinVidPalette[i];
	}

	for (j = 0; j <= fadeValue; ++j) {
#if defined(FEATURE_BACKGROUND_IMPROVED)
		if (S_UpdateInput()) return;
#endif // FEATURE_BACKGROUND_IMPROVED
		for (i = palStartIdx; i < palEndIdx; ++i) {
			WinVidPalette[i].peRed = fadePal[i].peRed + (palette[i].red - fadePal[i].peRed) * j / fadeValue;
			WinVidPalette[i].peGreen = fadePal[i].peGreen + (palette[i].green - fadePal[i].peGreen) * j / fadeValue;
			WinVidPalette[i].peBlue = fadePal[i].peBlue + (palette[i].blue - fadePal[i].peBlue) * j / fadeValue;
		}
		S_InitialisePolyList(FALSE);
		S_OutputPolyList();
		S_DumpScreen();
	}
}

void ScreenClear(bool isPhdWinSize) {
	DWORD flags = (SavedAppSettings.RenderMode == RM_Hardware) ? CLRB_BackBuffer : CLRB_RenderBuffer;
	if (isPhdWinSize)
		flags |= CLRB_PhdWinSize;
	ClearBuffers(flags, 0);
}

void S_CopyScreenToBuffer() {
#if defined(FEATURE_BACKGROUND_IMPROVED)
	DWORD bgndMode = 0;
	if (IsFadeToBlack) {
		bgndMode = 0;
	}
	else if (IsInventoryActive) {
		bgndMode = InvBackgroundMode;
	}
	else if (GF_CurrentEvent() == GFE_LEVCOMPLETE) {
		bgndMode = StatsBackgroundMode;
	}
	DWORD width = PhdWinWidth;
	DWORD height = PhdWinHeight;
#else // !FEATURE_BACKGROUND_IMPROVED
	DWORD width = 640;
	DWORD height = 480;
#endif // FEATURE_BACKGROUND_IMPROVED

	if (SavedAppSettings.RenderMode == RM_Software) {
#if defined(FEATURE_BACKGROUND_IMPROVED)
		if (PictureBuffer.bitmap == NULL ||
			PictureBuffer.width != width ||
			PictureBuffer.height != height)
		{
			BGND_PictureWidth = width;
			BGND_PictureHeight = height;
			try {
				CreatePictureBuffer();
			}
			catch (...) {
				return;
			}
		}
#endif // FEATURE_BACKGROUND_IMPROVED

		SWR_StretchBlt(&PictureBuffer, NULL, &RenderBuffer, &GameVidRect);
#if defined(FEATURE_BACKGROUND_IMPROVED)
		if (InventoryMode != INV_PauseMode || PauseBackgroundMode != 0)
#endif // FEATURE_BACKGROUND_IMPROVED
		{
			BYTE* ptr = PictureBuffer.bitmap;
			DWORD num = PictureBuffer.width * PictureBuffer.height;
			for (DWORD i = 0; i < num; ++i) {
				ptr[i] = DepthQIndex[ptr[i]];
			}
		}
		memcpy(PicPalette, GamePalette8, sizeof(PicPalette));
	}
#if defined(FEATURE_BACKGROUND_IMPROVED)
	else if (bgndMode == 0) {
		BGND2_CapturePicture();
	}
#endif // FEATURE_BACKGROUND_IMPROVED
}

void S_CopyBufferToScreen() {
	DWORD color = SavedAppSettings.LightingMode ? 0xFF808080 : 0xFFFFFFFF;
#if defined(FEATURE_BACKGROUND_IMPROVED)
	DWORD bgndMode = 0;
	if (IsFadeToBlack) {
		bgndMode = 0;
	}
	else if (IsInventoryActive) {
		bgndMode = InvBackgroundMode;
	}
	else if (GF_CurrentEvent() == GFE_LEVCOMPLETE) {
		bgndMode = StatsBackgroundMode;
	}
#endif // FEATURE_BACKGROUND_IMPROVED

	if (SavedAppSettings.RenderMode == RM_Software) {
		if (PictureBuffer.bitmap == NULL) {
			return;
		}
		if (memcmp(GamePalette8, PicPalette, sizeof(PicPalette))) {
			S_SyncPictureBufferPalette();
		}
#if defined(FEATURE_BACKGROUND_IMPROVED)
		RECT rect = PhdWinRect;
		BGND2_CalculatePictureRect(&rect);
		ClearBuffers(CLRB_RenderBuffer, 0);
		SWR_StretchBlt(&RenderBuffer, &rect, &PictureBuffer, NULL);
	}
	else if (BGND_PictureIsReady && (!BGND_IsCaptured || bgndMode == 0)) {
		HWR_EnableZBuffer(false, false);
		RECT rect = PhdWinRect;
		if (!BGND_IsCaptured) {
			BGND2_LoadPicture(NULL, FALSE, TRUE); // reload picture if required
		}
		BGND_DrawInGameBlack(); // draw black background for picture margins
		BGND2_CalculatePictureRect(&rect);
		BGND2_DrawTextures(&rect, color);
		if (BGND_IsCaptured) {
			if (InventoryMode != INV_PauseMode) {
				BGND2_FadeTo(128, -12); // the captured background image fades out to 50%
			}
			else if (PauseBackgroundMode != 0) {
				BGND2_FadeTo(128, -128); // the captured background image instantly gets 50%
			}
		}

#else // !FEATURE_BACKGROUND_IMPROVED
		SWR_StretchBlt(&RenderBuffer, &GameVidRect, &PictureBuffer, NULL);
	}
	else if (BGND_PictureIsReady) {
		BGND_GetPageHandles();
		HWR_EnableZBuffer(false, false);

		static const int tileX[4] = { 0, 256, 512, 640 };
		static const int tileY[3] = { 0, 256, 480 };
		int i, x[4], y[3];

		for (i = 0; i < 4; ++i)
			x[i] = tileX[i] * PhdWinWidth / 640 + PhdWinMinX;
		for (i = 0; i < 3; ++i)
			y[i] = tileY[i] * PhdWinHeight / 480 + PhdWinMinY;

		DrawTextureTile(x[0], y[0], x[1] - x[0], y[1] - y[0], BGND_PageHandles[0],
			0, 0, 256, 256, color, color, color, color);
		DrawTextureTile(x[1], y[0], x[2] - x[1], y[1] - y[0], BGND_PageHandles[1],
			0, 0, 256, 256, color, color, color, color);
		DrawTextureTile(x[2], y[0], x[3] - x[2], y[1] - y[0], BGND_PageHandles[2],
			0, 0, 128, 256, color, color, color, color);
		DrawTextureTile(x[0], y[1], x[1] - x[0], y[2] - y[1], BGND_PageHandles[3],
			0, 0, 256, 224, color, color, color, color);
		DrawTextureTile(x[1], y[1], x[2] - x[1], y[2] - y[1], BGND_PageHandles[4],
			0, 0, 256, 224, color, color, color, color);
		DrawTextureTile(x[2], y[1], x[3] - x[2], y[2] - y[1], BGND_PageHandles[2],
			128, 0, 128, 224, color, color, color, color);
#endif // FEATURE_BACKGROUND_IMPROVED
		HWR_EnableZBuffer(true, true);
	}
	else {
		BGND_DrawInGameBackground();
	}
}

BOOL DecompPCX(LPCBYTE pcx, DWORD pcxSize, LPBYTE pic, RGB888* pal) {
	PCX_HEADER* header;
	DWORD w, h, width, height, pitch;
	LPCBYTE src;
	LPBYTE dst;

	header = (PCX_HEADER*)pcx;
	width = header->xMax - header->xMin + 1;
	height = header->yMax - header->yMin + 1;

	if (header->manufacturer != 10 ||
		header->version < 5 ||
		header->bpp != 8 ||
		header->rle != 1 ||
		header->planes != 1 ||
		width * height == 0)
	{
		return FALSE;
	}

	src = pcx + sizeof(PCX_HEADER);
	dst = pic;
	pitch = width + width % 2; // add padding if required
	h = 0;
	w = 0;

	// NOTE: PCX decoder slightly redesigned to be more compatible and stable
	while (h < height) {
		if ((*src & 0xC0) == 0xC0) {
			BYTE n = (*src++) & 0x3F;
			BYTE c = *src++;
			if (n > 0) {
				if (w < width) {
					CLAMPG(n, width - w);
					memset(dst, c, n);
					dst += n;
				}
				w += n;
			}
		}
		else {
			*dst++ = *src++;
			++w;
		}
		if (w >= pitch) {
			w = 0;
			++h;
		}
	}

	if (pal != NULL)
		memcpy(pal, pcx + pcxSize - sizeof(RGB888) * 256, sizeof(RGB888) * 256);

	return TRUE;
}

// NOTE: this function is not presented in the original game
int GetPcxResolution(LPCBYTE pcx, DWORD pcxSize, DWORD* width, DWORD* height) {
	PCX_HEADER* header;

	if (pcx == NULL || pcxSize <= sizeof(PCX_HEADER) || width == NULL || height == NULL) {
		return -1;
	}

	header = (PCX_HEADER*)pcx;
	*width = header->xMax - header->xMin + 1;
	*height = header->yMax - header->yMin + 1;

	if (header->manufacturer != 10 ||
		header->version < 5 ||
		header->bpp != 8 ||
		header->rle != 1 ||
		header->planes != 1 ||
		*width == 0 ||
		*height == 0)
	{
		return -1;
	}

	return 0;
}

/*
 * Inject function
 */
void Inject_Output() {
	INJECT(0x00450BA0, GetRenderHeight);
	INJECT(0x00450BB0, GetRenderWidth);
	INJECT(0x00450BC0, S_InitialisePolyList);
	INJECT(0x00450CB0, S_DumpScreen);
	INJECT(0x00450CF0, S_ClearScreen);
	INJECT(0x00450D00, S_InitialiseScreen);
	INJECT(0x00450D40, S_OutputPolyList);
	INJECT(0x00450D80, S_GetObjectBounds);
	INJECT(0x00450FF0, S_InsertBackPolygon);
	INJECT(0x00451040, S_PrintShadow);
	INJECT(0x00451240, S_CalculateLight);
	INJECT(0x00451540, S_CalculateStaticLight);
	INJECT(0x00451580, S_CalculateStaticMeshLight);
	INJECT(0x004516B0, S_LightRoom);
	INJECT(0x004518C0, S_DrawHealthBar);
	INJECT(0x00451A90, S_DrawAirBar);
	INJECT(0x00451C90, AnimateTextures);
	INJECT(0x00451D50, S_SetupBelowWater);
	INJECT(0x00451DB0, S_SetupAboveWater);
	INJECT(0x00451DE0, S_AnimateTextures);
	INJECT(0x00451EA0, S_DisplayPicture);
	INJECT(0x00451FB0, S_SyncPictureBufferPalette);
	INJECT(0x00452030, S_DontDisplayPicture);
	INJECT(0x00452040, ScreenDump);
	INJECT(0x00452050, ScreenPartialDump);
	INJECT(0x00452060, FadeToPal);
	INJECT(0x00452230, ScreenClear);
	INJECT(0x00452260, S_CopyScreenToBuffer);
	INJECT(0x00452310, S_CopyBufferToScreen);
	INJECT(0x00452360, DecompPCX);
}