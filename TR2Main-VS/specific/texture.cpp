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
#include "specific/texture.h"
#include "3dsystem/3d_gen.h"
#include "specific/hwr.h"
#include "specific/winvid.h"
#include "global/vars.h"
#include <limits.h>

#if defined(FEATURE_BACKGROUND_IMPROVED)
#include "modding/background_new.h"
#endif // FEATURE_BACKGROUND_IMPROVED

#if defined(FEATURE_EXTENDED_LIMITS)
PHD_TEXTURE PhdTextureInfo[MAX_OBJECT_TEXTURES];
BYTE LabTextureUVFlags[MAX_OBJECT_TEXTURES];
BYTE* TexturePageBuffer8[MAX_TEXTURE_PAGES];
HWR_TEXHANDLE HWR_PageHandles[MAX_TEXTURE_PAGES];
int HWR_TexturePageIndexes[MAX_TEXTURE_PAGES];
#endif // FEATURE_EXTENDED_LIMITS

#if defined(FEATURE_EXTENDED_LIMITS) || defined(FEATURE_BACKGROUND_IMPROVED)
TEXPAGE_DESC TexturePages[MAX_TEXTURE_PAGES];
#else // defined(FEATURE_EXTENDED_LIMITS) || defined(FEATURE_BACKGROUND_IMPROVED)
TEXPAGE_DESC TexturePages[32];
#endif // defined(FEATURE_EXTENDED_LIMITS) || defined(FEATURE_BACKGROUND_IMPROVED)

RGB888* TexturePalettes[256];
#if defined(FEATURE_VIDEOFX_IMPROVED)
DWORD ReflectionMode = 2;
static LPDIRECT3DTEXTURE9 EnvmapTexture = NULL;
static HWR_TEXHANDLE EnvmapTextureHandle = 0;

static DWORD GetEnvmapSide() {
	static const DWORD mapside[] = { 64, 256, 1024 };
	if (ReflectionMode < 1 || ReflectionMode > 3) return 0;
	DWORD side = MIN(mapside[3 - ReflectionMode], GetMaxTextureSize());
	DWORD sideLimit = MIN(GameVidWidth, GameVidHeight);
	while (side > sideLimit) side >>= 1;
	return side;
}

static bool CreateEnvmapTexture() {
	if (EnvmapTexture != NULL) FreeEnvmapTexture();
	DWORD side = GetEnvmapSide();
	if (!side) return false;
	if (EnvmapTexture) return true;
	return SUCCEEDED(D3DDev->CreateTexture(side, side, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &EnvmapTexture, 0));
}

void FreeEnvmapTexture() {
	if (EnvmapTexture) {
		EnvmapTexture->Release();
		EnvmapTexture = NULL;
	}
	EnvmapTextureHandle = 0;
}

bool SetEnvmapTexture(LPDDS surface) {
	EnvmapTextureHandle = 0;

	// Getting centred square area of the screen
	int side = MIN(GameVidWidth, GameVidHeight);
	int x = (GameVidWidth - side) / 2;
	int y = (GameVidHeight - side) / 2;
	RECT srcRect = {
		.left = GameVidRect.left + x,
		.top = GameVidRect.top + y,
		.right = GameVidRect.left + x + side,
		.bottom = GameVidRect.top + y + side,
	};
	if (EnvmapTexture == NULL && !CreateEnvmapTexture()) return false;
	LPDDS texSurface = NULL;
	if SUCCEEDED(EnvmapTexture->GetSurfaceLevel(0, &texSurface)) {
		if SUCCEEDED(D3DDev->StretchRect(surface, &srcRect, texSurface, NULL, D3DTEXF_LINEAR)) {
			EnvmapTextureHandle = EnvmapTexture;
		}
		texSurface->Release();
	}
	return (EnvmapTextureHandle != 0);
}

HWR_TEXHANDLE GetEnvmapTextureHandle() {
	return EnvmapTextureHandle;
}
#endif // FEATURE_VIDEOFX_IMPROVED

DWORD GetMaxTextureSize() {
	return MIN(CurrentDisplayAdapter.caps.MaxTextureWidth, CurrentDisplayAdapter.caps.MaxTextureHeight);
}

int GetTextureSideByPage(int page) {
	if (page < 0) return 256;
	page = HWR_TexturePageIndexes[page];
	if (page < 0) return 256;
	return TexturePages[page].width;
}

int GetTextureSideByHandle(HWR_TEXHANDLE handle) {
	for (DWORD i = 0; i < ARRAY_SIZE(HWR_TexturePageIndexes); ++i) {
		if (HWR_TexturePageIndexes[i] >= 0 && HWR_PageHandles[i] == handle) {
			return TexturePages[HWR_TexturePageIndexes[i]].width;
		}
	}
	return 256;
}

HRESULT LoadTextureFromFile(LPCTSTR fileName, UINT usage, D3DFORMAT format, D3DPOOL pool, LPDIRECT3DTEXTURE9* pResult) {
	stbi_uc* pixels = NULL;
	int width = 0, height = 0, channel = 0;

	if (!PathFileExists(fileName))
	{
		LogWarn("Failed to load texture from file: %s, file not found !", fileName);
		return E_FAIL;
	}

	pixels = stbi_load(fileName, &width, &height, &channel, 4);
	if (pixels == NULL)
	{
		LogWarn("Failed to load texture from file: %s, unknown error !", fileName);
		return E_FAIL;
	}

	if FAILED(D3DDev->CreateTexture(width, height, 1, usage, format, pool, pResult, NULL))
	{
		LogWarn("Failed to load texture from file: %s, texture creation failed !", fileName);
		return E_FAIL;
	}

	D3DLOCKED_RECT lock;
	if SUCCEEDED((*pResult)->LockRect(0, &lock, NULL, D3DLOCK_DISCARD))
	{
		unsigned char* dest = static_cast<unsigned char*>(lock.pBits);
		memcpy(dest, pixels, sizeof(unsigned char) * width * height * channel);
		(*pResult)->UnlockRect(0);
	}

	stbi_image_free(pixels);
	return S_OK;
}

void CopyBitmapPalette(RGB888* srcPal, BYTE* srcBitmap, int bitmapSize, RGB888* destPal) {
	int i, j;
	for (i = 0; i < 256; ++i) {
		SortBuffer[i]._0 = i;
		SortBuffer[i]._1 = 0;
	}
	for (i = 0; i < bitmapSize; ++i) {
		SortBuffer[srcBitmap[i]]._1++;
	}
	do_quickysorty(0, 255);
	// middle palette entries
	for (j = 0; j < 256; ++j) {
		destPal[j] = srcPal[SortBuffer[j]._0];
	}
}

BYTE FindNearestPaletteEntry(RGB888* palette, int red, int green, int blue, bool ignoreSysPalette) {
	int i;
	int diffRed, diffGreen, diffBlue, diffTotal;
	int diffMin = INT_MAX;
	int palStartIdx = 0;
	int palEndIdx = 256;
	BYTE result = 0;
	for (i = palStartIdx; i < palEndIdx; ++i) {
		diffRed = red - palette[i].red;
		diffGreen = green - palette[i].green;
		diffBlue = blue - palette[i].blue;
		diffTotal = diffRed * diffRed + diffGreen * diffGreen + diffBlue * diffBlue;
		if (diffTotal < diffMin) {
			diffMin = diffTotal;
			result = i;
		}
	}
	return result;
}

void SyncSurfacePalettes(void* srcData, int width, int height, int srcPitch, RGB888* srcPalette, void* dstData, int dstPitch, RGB888* dstPalette, bool preserveSysPalette) {
	int i, j;
	BYTE* src, * dst;
	BYTE bufPalette[256] = {};

	for (i = 0; i < 256; ++i) {
		bufPalette[i] = FindNearestPaletteEntry(dstPalette, srcPalette[i].red, srcPalette[i].green, srcPalette[i].blue, preserveSysPalette);
	}

	src = (BYTE*)srcData;
	dst = (BYTE*)dstData;

	for (i = 0; i < height; ++i) {
		for (j = 0; j < width; ++j) {
			*(dst++) = bufPalette[*(src++)];
		}
		src += srcPitch - width;
		dst += dstPitch - width;
	}
}

int CreateTexturePalette(RGB888* pal) {
	int palIndex = GetFreePaletteIndex();
	if (palIndex < 0)
		return -1;

	TexturePalettes[palIndex] = (RGB888*)malloc(sizeof(RGB888) * 256);
	if (TexturePalettes[palIndex] == NULL)
		return -1;

	memcpy(TexturePalettes[palIndex], pal, sizeof(RGB888) * 256);
	return palIndex;
}

int GetFreePaletteIndex() {
	for (DWORD i = 0; i < ARRAY_SIZE(TexturePalettes); ++i) {
		if (TexturePalettes[i] == NULL)
			return i;
	}
	return -1;
}

void FreePalette(int paletteIndex) {
	if (TexturePalettes[paletteIndex] != NULL) {
		free(TexturePalettes[paletteIndex]);
		TexturePalettes[paletteIndex] = NULL;
	}
}

void SafeFreePalette(int paletteIndex) {
	if (paletteIndex >= 0) {
		FreePalette(paletteIndex);
	}
}

int CreateTexturePage(int width, int height, bool alpha) {
	int pageIndex = GetFreeTexturePageIndex();
	if (pageIndex < 0)
		return -1;
	memset(&TexturePages[pageIndex], 0, sizeof(TEXPAGE_DESC));
	TexturePages[pageIndex].status = 1;
	TexturePages[pageIndex].width = width;
	TexturePages[pageIndex].height = height;
	if FAILED(D3DDev->CreateTexture(width, height, 1, 0, alpha ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &TexturePages[pageIndex].texture, 0))
		return -1;
	return pageIndex;
}

int GetFreeTexturePageIndex() {
	for (DWORD i = 0; i < ARRAY_SIZE(TexturePages); ++i) {
		if ((TexturePages[i].status & 1) == 0)
			return i;
	}
	return -1;
}

void SafeFreeTexturePage(int pageIndex) {
	if (pageIndex >= 0 && (TexturePages[pageIndex].status & 1) != 0) {
		FreeTexturePage(pageIndex);
	}
}

void FreeTexturePage(int pageIndex) {
	if (TexturePages[pageIndex].texture != NULL) {
		TexturePages[pageIndex].texture->Release();
		TexturePages[pageIndex].texture = NULL;
	}
	TexturePages[pageIndex].status = 0;
}

void FreeTexturePages() {
#if defined(FEATURE_VIDEOFX_IMPROVED)
	FreeEnvmapTexture();
#endif // FEATURE_VIDEOFX_IMPROVED
#if defined(FEATURE_BACKGROUND_IMPROVED)
	BGND2_CleanupCaptureTextures();
#endif // FEATURE_BACKGROUND_IMPROVED
	for (DWORD i = 0; i < ARRAY_SIZE(TexturePages); ++i) {
		if ((TexturePages[i].status & 1) != 0)
			FreeTexturePage(i);
	}
}

HWR_TEXHANDLE GetTexturePageHandle(int pageIndex) {
	if (pageIndex < 0)
		return 0;
	return TexturePages[pageIndex].texture;
}

int AddTexturePage8(int width, int height, BYTE* pageBuffer, int palIndex) {
	int pageIndex = CreateTexturePage(width, height, true);
	if (pageIndex < 0)
		return -1;

	DDSDESC desc;
	if FAILED(TexturePages[pageIndex].texture->LockRect(0, &desc, NULL, 0)) {
		return -1;
	}
	BYTE* src = pageBuffer;
	for (int i = 0; i < height; ++i) {
		DWORD* dst = (DWORD*)((BYTE*)desc.pBits + desc.Pitch * i);
		for (int j = 0; j < width; ++j) {
			if (*src) {
				BYTE r = TexturePalettes[palIndex][*src].red;
				BYTE g = TexturePalettes[palIndex][*src].green;
				BYTE b = TexturePalettes[palIndex][*src].blue;
				*dst++ = RGBA_MAKE(r, g, b, 0xFF);
			}
			else {
				*dst++ = 0;
			}
			++src;
		}
	}
	TexturePages[pageIndex].texture->UnlockRect(0);

	return pageIndex;
}

int AddTexturePage16(int width, int height, BYTE* pageBuffer) {
	int pageIndex = CreateTexturePage(width, height, true);
	if (pageIndex < 0)
		return -1;
	DDSDESC desc;
	if FAILED(TexturePages[pageIndex].texture->LockRect(0, &desc, NULL, 0)) {
		return -1;
	}
	UINT16* src = (UINT16*)pageBuffer;
	for (int i = 0; i < height; ++i) {
		DWORD* dst = (DWORD*)((BYTE*)desc.pBits + desc.Pitch * i);
		for (int j = 0; j < width; ++j) {
			BYTE r = ((*src >> 7) & 0xF8) | ((*src >> 13) & 0x07);
			BYTE g = ((*src >> 2) & 0xF8) | ((*src >> 7) & 0x07);
			BYTE b = ((*src << 3) & 0xF8) | ((*src >> 2) & 0x07);
			BYTE a = (*src & 0x8000) ? 0xFF : 0;
			*dst++ = RGBA_MAKE(r, g, b, a);
			++src;
		}
	}
	TexturePages[pageIndex].texture->UnlockRect(0);
	return pageIndex;
}

// NOTE: this function is not presented in the original game
int AddTexturePage32(int width, int height, BYTE* pageBuffer, bool alpha) {
	int pageIndex = CreateTexturePage(width, height, alpha);
	if (pageIndex < 0)
		return -1;

	DDSDESC desc;
	if FAILED(TexturePages[pageIndex].texture->LockRect(0, &desc, NULL, 0)) {
		return -1;
	}
	DWORD* src = (DWORD*)pageBuffer;
	for (int i = 0; i < height; ++i) {
		DWORD* dst = (DWORD*)((BYTE*)desc.pBits + desc.Pitch * i);
		memcpy(dst, src, sizeof(DWORD) * width);
		src += width;
	}
	TexturePages[pageIndex].texture->UnlockRect(0);

	return pageIndex;
}

// NOTE: this function is not presented in the original game
int AddExternalTexture(LPCTSTR fileName, bool alpha) {
	int pageIndex = GetFreeTexturePageIndex();
	if (pageIndex < 0)
		return -1;

	memset(&TexturePages[pageIndex], 0, sizeof(TEXPAGE_DESC));

	HRESULT res = LoadTextureFromFile(fileName, NULL, alpha ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &TexturePages[pageIndex].texture);
	if FAILED(res)
		return -1;

	D3DSURFACE_DESC desc;
	if FAILED(TexturePages[pageIndex].texture->GetLevelDesc(0, &desc)) {
		TexturePages[pageIndex].texture->Release();
		TexturePages[pageIndex].texture = NULL;
		return -1;
	}

	TexturePages[pageIndex].status = 1 | 2;
	TexturePages[pageIndex].width = desc.Width;
	TexturePages[pageIndex].height = desc.Height;
	return pageIndex;
}

// NOTE: this function is not presented in the original game
bool IsExternalTexture(int page) {
	if (page < 0 || page >= (int)ARRAY_SIZE(HWR_TexturePageIndexes))
		return false;

	page = HWR_TexturePageIndexes[page];
	if (page < 0 || page >= (int)ARRAY_SIZE(TexturePages))
		return false;

	return CHK_ALL(TexturePages[page].status, 1 | 2);
}

void CleanupTextures() {
	FreeTexturePages();
	for (DWORD i = 0; i < ARRAY_SIZE(TexturePalettes); ++i) {
		if (TexturePalettes[i] != NULL)
			FreePalette(i);
	}
}

bool InitTextures() {
	memset(TexturePages, 0, sizeof(TexturePages));
	memset(TexturePalettes, 0, sizeof(TexturePalettes));
	return true;
}

/*
 * Inject function
 */
void Inject_Texture() {
	INJECT(0x00455990, CopyBitmapPalette);
	INJECT(0x00455AD0, FindNearestPaletteEntry);
	INJECT(0x00455BA0, SyncSurfacePalettes);
	INJECT(0x00455C50, CreateTexturePalette);
	INJECT(0x00455CE0, GetFreePaletteIndex);
	INJECT(0x00455D00, FreePalette);
	INJECT(0x00455D30, SafeFreePalette);
	INJECT(0x00455EB0, CreateTexturePage);
	INJECT(0x00455DF0, GetFreeTexturePageIndex);
	INJECT(0x00456060, SafeFreeTexturePage);
	INJECT(0x00456080, FreeTexturePage);
	INJECT(0x00456100, FreeTexturePages);
	INJECT(0x00456220, GetTexturePageHandle);
	INJECT(0x00456260, AddTexturePage8);
	INJECT(0x00456360, AddTexturePage16);
	INJECT(0x00456650, CleanupTextures);
	INJECT(0x00456660, InitTextures);
}