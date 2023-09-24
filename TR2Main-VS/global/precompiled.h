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

#ifndef TR2MAIN_PRECOMPILED_HEADER
#define TR2MAIN_PRECOMPILED_HEADER

#define FEATURE_NOCD_DATA
#define FEATURE_ASSAULT_SAVE
#define FEATURE_AUDIO_IMPROVED
#define FEATURE_BACKGROUND_IMPROVED
#define FEATURE_CHEAT
#define FEATURE_EXTENDED_LIMITS
#define FEATURE_FFPLAY
#define FEATURE_GAMEPLAY_FIXES
#define FEATURE_GOLD
#define FEATURE_HUD_IMPROVED
#define FEATURE_INPUT_IMPROVED
#define FEATURE_MOD_CONFIG
#define FEATURE_NOLEGACY_OPTIONS
#define FEATURE_PAULD_CDAUDIO
#define FEATURE_SCREENSHOT_IMPROVED
#define FEATURE_SUBFOLDERS
#define FEATURE_VIDEOFX_IMPROVED
#define FEATURE_VIEW_IMPROVED
#define FEATURE_WINDOW_STYLE_FIX

#pragma comment(lib, "dxguid")
#if defined(D3D_9)
#define DIRECT3D_VERSION 0x900
#define DIRECTINPUT_VERSION 0x800
#define DIRECTSOUND_VERSION 0x900
#pragma comment(lib, "d3d9")
#pragma comment(lib, "dinput8")
#else
#define DIRECTDRAW_VERSION 0x500
#define DIRECT3D_VERSION 0x500
#define DIRECTINPUT_VERSION 0x500
#define DIRECTSOUND_VERSION 0x500
#pragma comment(lib, "ddraw")
#pragma comment(lib, "dinput")
#endif
#pragma comment(lib, "dsound")
#pragma comment(lib, "setupapi")
#pragma comment(lib, "hid")
#pragma comment(lib, "xinput")

// Some defines are required for DX9 and above
#if (DIRECT3D_VERSION >= 0x900)
#ifndef FEATURE_NOLEGACY_OPTIONS
#define FEATURE_NOLEGACY_OPTIONS
#endif // !FEATURE_NOLEGACY_OPTIONS
#endif // (DIRECT3D_VERSION >= 0x900)

#include <stdio.h>
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <dinput.h>
#include <dsound.h>

#if (DIRECT3D_VERSION >= 0x900)
#include <dxsdk-d3dx/d3dx9.h>
#else // (DIRECT3D_VERSION >= 0x900)
#include <ddraw.h>
#include <d3d.h>
#endif // (DIRECT3D_VERSION >= 0x900)
#endif // TR2MAIN_PRECOMPILED_HEADER

#ifdef _MSC_VER 
//not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

extern void Log(const char* message, ...);
