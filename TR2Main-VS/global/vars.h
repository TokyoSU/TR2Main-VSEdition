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

#ifndef GLOBAL_VARS_H_INCLUDED
#define GLOBAL_VARS_H_INCLUDED

 /*
  * String Constants
  */

// Window and class names.
#define GameWindowName		"Tomb Raider II"
#define GameClassName		"Dude:TombRaiderII:DDWndClass"
#define GameDialogName		"Tomb Raider II"
#define DialogClassName		"Dude:TombRaiderII:DWWndClass"
#define MessageBoxName		"Tomb Raider II"

// Variable macros
#define VAR_U_(address, type)			(*(type*)(address)) // uninitialized variable
#define VAR_I_(address, type, value)	(*(type*)(address)) // initialized variable (value is just for info)
#define ARRAY_(address, type, length)	(*(type(*)length)(address)) // array (can be multidimensional)

// Camera flags values.
#define FOLLOW_CENTRE 	1
#define NO_CHUNKY     	2
#define CHASE_OBJECT  	3

// Macros values.
#define WEAPON_UNLIMITED 10001
#define PISTOLS_AMMO_DEFAULT 1000
#define AUTOPISTOLS_AMMO_CLIPS 40
#define UZIS_AMMO_CLIPS 80
#define SHOTGUN_AMMO_CLIPS 12
#define SHOTGUN_AMMO_SHOOT 6
#define HARPOON_AMMO_CLIPS 3
#define M16_AMMO_CLIPS 40
#define GRENADE_AMMO_CLIPS 2
#define FLARE_AMMO_CLIPS 6

// Lara values.
#define STEPUP_HEIGHT (CLICK(3)/2)
#define NO_AIR -1
#define LARA_RESTORE_AIR_PER_TICK 10
#define LARA_AIR_MAX 1800
#define LARA_NO_AIR_DAMAGE_PER_TICK 5
#if defined(FEATURE_MOD_CONFIG)
#define AIR_100(x) (x)
#define AIR_75(x) (AIR_100(x)*3/4)
#define AIR_50(x) (AIR_100(x)/2)
#define AIR_25(x) (AIR_100(x)/4)
#else
#define AIR_100 (LARA_AIR_MAX)
#define AIR_75 (AIR_100*3/4)
#define AIR_50 (AIR_100/2)
#define AIR_25 (AIR_100/4)
#endif

// Max values.
#define MAX_OBJECT_TEXTURES 32768             // From 2000
#define MAX_TEXTURE_PAGES 1024                // From 128
#define MAX_EFFECTS 8192                      // From 100 (Double of weather size, else other effects won't appear)
#define MAX_ITEM_IN_INVENTORY 33
#define MAX_WEATHER_RAIN 4096
#define MAX_WEATHER_RAIN_ALIVE 2048
#define MAX_WEATHER_SNOW 4096
#define MAX_WEATHER_SNOW_ALIVE 2048
#define MAX_CREATURES 5
#define MAX_FLIPMAPS 10
#define MAX_CD 64

/*
 * General Variables
 */
 // 3D insert function variables
extern void(*ins_trans_quad)(int x, int y, int width, int height, int z); // 0x00470318
extern void(*ins_poly_trans8)(PHD_VBUF* vbuf, short shade); // 0x0047032C
extern void(*ins_flat_rect)(int, int, int, int, int, BYTE); // 0x0047805C
#if defined(FEATURE_VIDEOFX_IMPROVED)
extern void(*ins_sprite)(int, int, int, int, int, int, short, DWORD); // 0x004B2A10
#else
extern void(*ins_sprite)(int, int, int, int, int, int, short); // 0x004B2A10
#endif
extern short*(*ins_objectGT3)(short*, int, SORTTYPE); // 0x004B2A1C
extern short*(*ins_objectGT4)(short*, int, SORTTYPE); // 0x004B2A20
extern void(*ins_line)(int, int, int, int, int, BYTE); // 0x004B2AE8
extern short*(*ins_objectG4)(short*, int, SORTTYPE); // 0x004BCAF8
extern short*(*ins_objectG3)(short*, int, SORTTYPE); // 0x004BCB40
extern void(*ins_roomGT3)(FACE3*, int, SORTTYPE); // ----------
extern void(*ins_roomGT4)(FACE4*, int, SORTTYPE); // ----------

#define SfxFunctions			(*(void(__cdecl *(*)[32])(ITEM_INFO*))				0x004641F8)
#define EffectFunctions			(*(void(__cdecl *(*)[32])(ITEM_INFO*))				0x004641F8)
#define ExtraFunctions			(*(void(__cdecl *(*)[11])(ITEM_INFO*,COLL_INFO*))	0x00465DF0)
#define LaraControlFunctions	(*(void(__cdecl *(*)[71])(ITEM_INFO*,COLL_INFO*))	0x00465CD0)
extern void (*LaraCollisionFunctions[71])(ITEM_INFO* item, COLL_INFO* coll); // 0x00465E20

// Initialized variables
#define PerspectiveDistance			VAR_I_(0x00464060, DWORD,			0x3000000)
#define RhwFactor 					VAR_I_(0x0046408C, float,			0x14000000.p0)
#define CineTrackID					VAR_I_(0x004640B0, int,				1)
#define CineTickRate				VAR_I_(0x004640B8, int,				0x8000) // 0x8000 = PHD_ONE/TICKS_PER_FRAME
#define CD_TrackID					VAR_I_(0x004640BC, short,			-1)
#define FlipEffect					VAR_I_(0x004640C4, int,				-1)
#define AssaultBestTime				VAR_I_(0x004641F0, int,				-1)
#define GF_NumSecrets				VAR_I_(0x004642E8, short,			3)
#define CineTargetAngle				VAR_I_(0x00464310, short,			PHD_90)
#define OverlayStatus				VAR_I_(0x004644E0, int,				1)
#define InvMainObjectsCount			VAR_I_(0x004654E0, short,			8)
#ifdef FEATURE_HUD_IMPROVED
extern short InvOptionObjectsCount;
#else // FEATURE_HUD_IMPROVED
#define InvOptionObjectsCount		VAR_I_(0x00465604, short,			4)
#endif // FEATURE_HUD_IMPROVED
#define GymInvOpenEnabled			VAR_I_(0x00465618, BOOL,			TRUE)
#define InventoryChosen				VAR_I_(0x00465A50, short,			-1)
#define InventoryMode				VAR_I_(0x00465A54, INVENTORY_MODE,	INV_TitleMode)
#define SoundVolume					VAR_I_(0x00465A5C, short,			165) // NOTE: value should be 10
#define MusicVolume					VAR_I_(0x00465A60, short,			255) // NOTE: value should be 10
#define BGND_PaletteIndex			VAR_I_(0x00466400, int,				-1)
#define ScreenSizer 				VAR_I_(0x00466480, double,			1.0)
#define GameSizer 					VAR_I_(0x00466488, double,			1.0)
#define FadeValue					VAR_I_(0x00466490, int,				0x100000)
#define FadeLimit					VAR_I_(0x00466494, int,				0x100000)
#define FadeAdder					VAR_I_(0x00466498, int,				0x8000)
#define RandomControl				VAR_I_(0x00466BB0, int,				RANDOM_SEED)
#define RandomDraw					VAR_I_(0x00466BB4, int,				RANDOM_SEED)
#define PaletteIndex				VAR_I_(0x00466BDC, int,				-1)
#define DumpX						VAR_I_(0x00466BE4, short,			25)
#define DumpY						VAR_I_(0x00466BE6, short,			25)
#define DumpWidth					VAR_I_(0x00466BE8, short,			50)
#define DumpHeight					VAR_I_(0x00466BEA, short,			0)
#define DetailLevel					VAR_I_(0x00467724, DWORD,			1)
#define MidSort						VAR_I_(0x0046C2F0, DWORD,			0)
#define FltViewAspect				VAR_I_(0x0046C2F4, float,			0.0)
#define XGen_y0						VAR_I_(0x0046C2F8, int,				0)
#define XGen_y1						VAR_I_(0x0046C2FC, int,				0)

// Uninitialized variables
extern int PhdFov;
#define PhdWinTop					VAR_U_(0x0046E300, int)
#define LsAdder						VAR_U_(0x00470308, int)
#define FltWinBottom				VAR_U_(0x0047030C, float)
#define FltResZBuf					VAR_U_(0x00470310, float)
#define FltResZ						VAR_U_(0x00470314, float)
#define PhdWinHeight				VAR_U_(0x0047031C, int)
#define PhdWinCenterX				VAR_U_(0x00470320, int)
#define PhdWinCenterY				VAR_U_(0x00470324, int)
#define PhdLsXRot					VAR_U_(0x00470328, short)
#define FltWinTop					VAR_U_(0x00470330, float)
#define FltWinLeft					VAR_U_(0x00478038, float)
#define PhdWinMinY					VAR_U_(0x0047803C, short)
#define PhdFarZ 					VAR_U_(0x00478048, int)
#define FltRhwOPersp				VAR_U_(0x0047804C, float)
#define PhdWinBottom				VAR_U_(0x00478050, int)
#define PhdPersp					VAR_U_(0x00478054, int)
#define PhdWinLeft 					VAR_U_(0x00478058, int)
#define PhdWinMaxX 					VAR_U_(0x004B29E0, short)
#define PhdNearZ 					VAR_U_(0x004B29E4, int)
#define FltResZORhw 				VAR_U_(0x004B29E8, float)
#define FltFarZ 					VAR_U_(0x004B29EC, float)
#define FltWinCenterX				VAR_U_(0x004B29F0, float)
#define FltWinCenterY				VAR_U_(0x004B29F4, float)
#define PhdScreenHeight				VAR_U_(0x004B29F8, int)
#define PrintSurfacePtr				VAR_U_(0x004B29FC, BYTE*)
#define PhdWinMinX					VAR_U_(0x004B2A00, short)
#define FltPerspONearZ				VAR_U_(0x004B2A04, float)
#define FltRhwONearZ				VAR_U_(0x004B2A08, float)
#define PhdWinMaxY					VAR_U_(0x004B2A0C, short)
#define FltNearZ					VAR_U_(0x004B2A14, float)
#define PhdMatrixPtr				VAR_U_(0x004B2A18, PHD_MATRIX*)
#define FltPersp					VAR_U_(0x004B2AA8, float)
#define MatrixW2V					VAR_U_(0x004B2AB0, PHD_MATRIX)
#define IMRate						VAR_U_(0x00526184, int)
#define IMFrac						VAR_U_(0x005258F0, int)
#define IMPtr						VAR_U_(0x00526188, PHD_MATRIX*)
#define IMStack						ARRAY_(0x005252C0, PHD_MATRIX, [32])
#define InterpolateBounds			ARRAY_(0x005261A0, short, [6])
#define Info3dPtr					VAR_U_(0x004B2AE0, short*)
#define PhdWinWidth					VAR_U_(0x004B2AE4, int)
#define PhdViewDistance				VAR_U_(0x004BCAF0, int)
#define PhdLsYRot					VAR_U_(0x004BCAF4, short)
#define PhdScreenWidth				VAR_U_(0x004BF3C8, int)
#define LsDivider					VAR_U_(0x004BF3CC, int)
#define FltWinRight					VAR_U_(0x004D6B50, float)
#define LsVectorView				VAR_U_(0x004D6B58, PHD_VECTOR)
#define PhdWinRight					VAR_U_(0x004D6BE8, int)
#define SurfaceCount				VAR_U_(0x004D6BEC, DWORD)
#define Sort3dPtr					VAR_U_(0x004D6BF0, SORT_ITEM*)
#define WibbleOffset				VAR_U_(0x004D6BFC, int)
#define IsWibbleEffect				VAR_U_(0x004D6C00, BOOL)
#define IsWaterEffect				VAR_U_(0x004D6C04, BOOL)
#define IsShadeEffect				VAR_U_(0x004D6F68, bool)
#define CineCurrentFrame			VAR_U_(0x004D7770, int)
#define IsChunkyCamera				VAR_U_(0x004D777C, BOOL)
#define HeightType					VAR_U_(0x004D7780, HEIGHT_TYPE)
#define NoInputCounter				VAR_U_(0x004D7784, int)
#define IsResetFlag					VAR_U_(0x004D7788, BOOL)
#define FlipTimer					VAR_U_(0x004D778C, int)
#define LosRoomsCount				VAR_U_(0x004D7790, int)
#define StopInventory				VAR_U_(0x004D7794, BOOL)
#define IsDemoLevelType				VAR_U_(0x004D779C, BOOL)
#define IsDemoLoaded				VAR_U_(0x004D77A0, BOOL)
#define BoundStart					VAR_U_(0x004D77B0, int)
#define BoundEnd					VAR_U_(0x004D77B4, int)
#define IsAssaultTimerDisplay		VAR_U_(0x004D77D0, BOOL)
#define IsAssaultTimerActive		VAR_U_(0x004D77D4, BOOL)
#define AmmoTextInfo				VAR_U_(0x004D791C, TEXT_STR_INFO*)
#define DisplayModeTextInfo			VAR_U_(0x004D7920, TEXT_STR_INFO*)
#define DisplayModeInfoTimer		VAR_U_(0x004D7924, DWORD)
#define InvMainCurrent				VAR_U_(0x004D7928, UINT16)
#define InvKeyObjectsCount			VAR_U_(0x004D792C, UINT16)
#define InvKeysCurrent				VAR_U_(0x004D7930, UINT16)
#define InvOptionCurrent			VAR_U_(0x004D7934, UINT16)
#define InvRingText					VAR_U_(0x004D7944, TEXT_STR_INFO*)
#define InvUpArrow1					VAR_U_(0x004D794C, TEXT_STR_INFO*)
#define InvUpArrow2					VAR_U_(0x004D7950, TEXT_STR_INFO*)
#define InvDownArrow1				VAR_U_(0x004D7954, TEXT_STR_INFO*)
#define InvDownArrow2				VAR_U_(0x004D7958, TEXT_STR_INFO*)
#define InputDB						VAR_U_(0x004D795C, DWORD)
#define IsInventoryActive			VAR_U_(0x004D7968, UINT16)
#define InvDemoMode					VAR_U_(0x004D7990, BOOL)
#define IsInvOptionsDelay			VAR_U_(0x004D79A4, BOOL)
#define InvOptionsDelayCounter		VAR_U_(0x004D79A8, int)
#define SoundOptionLine				VAR_U_(0x004D79AC, UINT16)
#define StatsRequester				VAR_U_(0x004D79B0, REQUEST_INFO)
#define Assault						VAR_U_(0x004D7BC8, ASSAULT_STATS)
#define LevelItemCount				VAR_U_(0x004D7C28, int)
#define HealthBarTimer				VAR_U_(0x004D7C2C, int)
#define CameraCount					VAR_U_(0x004D7C64, DWORD)
#define MinesDetonated				VAR_U_(0x004D7E6C, BOOL)
#define BGND_PictureIsReady			VAR_U_(0x004D7E78, bool)
#define D3DDev						VAR_U_(0x004D7EAC, LPDIRECT3DDEVICE9)
#define D3D							VAR_U_(0x004D7EB0, LPDIRECT3D9)
extern LPDIRECT3DVERTEXBUFFER9 D3DVtx;
#define MinWindowClientHeight		VAR_U_(0x004D7EC0, int)
#define IsGameWindowChanging		VAR_U_(0x004D7EC8, bool)
#define MaxWindowHeight				VAR_U_(0x004D7ECC, int)
#define IsGameWindowCreated			VAR_U_(0x004D7ED4, bool)
#define IsGameWindowUpdating		VAR_U_(0x004D7ED8, bool)
#define IsDDrawGameWindowShow		VAR_U_(0x004D7EDC, bool)
#define MinWindowClientWidth		VAR_U_(0x004D7EE0, int)
#define IsGameWindowShow			VAR_U_(0x004D7EE4, bool)
#define IsMinWindowSizeSet			VAR_U_(0x004D7EE8, bool)
#define MaxWindowClientWidth		VAR_U_(0x004D7EEC, int)
#define GameWindowWidth				VAR_U_(0x004D7EF0, int)
#define IsMinMaxInfoSpecial			VAR_U_(0x004D7EF4, bool)
#define IsGameFullScreen			VAR_U_(0x004D7EF8, bool)
#define IsGameWindowMaximized		VAR_U_(0x004D7EFC, bool)
#define HGameWindow					VAR_U_(0x004D7F00, HWND)
#define GameWindowHeight			VAR_U_(0x004D7F04, int)
#define PrimaryDisplayAdapter		VAR_U_(0x004D7F08, DISPLAY_ADAPTER_NODE*)
extern DISPLAY_ADAPTER CurrentDisplayAdapter;
static TEXPAGES_CONFIG TexPagesConfig;
#define LockedBufferCount			VAR_U_(0x004D8328, DWORD)
#define GameWindowPositionX			VAR_U_(0x004D832C, int)
#define GameWindowPositionY			VAR_U_(0x004D8330, int)
#define DisplayAdapterList			VAR_U_(0x004D8338, DISPLAY_ADAPTER_LIST)
#define MaxWindowClientHeight		VAR_U_(0x004D8344, int)
#define IsMessageLoopClosed			VAR_U_(0x004D8348, bool)
#define MaxWindowWidth				VAR_U_(0x004D834C, int)
#define IsMaxWindowSizeSet			VAR_U_(0x004D8350, bool)
#define AppResultCode				VAR_U_(0x004D8354, DWORD)
#define FullScreenWidth				VAR_U_(0x004D8358, int)
#define FullScreenHeight			VAR_U_(0x004D835C, int)
#define FullScreenBPP				VAR_U_(0x004D8360, int)
#define FullScreenVGA				VAR_U_(0x004D8364, int)
#define IsGameToExit				VAR_U_(0x004D8368, bool)
#define GameWindow_Y				VAR_U_(0x004D836C, int)
#define GameWindow_X				VAR_U_(0x004D8370, int)
#define IsGameWindowMinimized		VAR_U_(0x004D8374, bool)
#define MinWindowWidth				VAR_U_(0x004D8378, int)
#define MinWindowHeight				VAR_U_(0x004D837C, int)
#define IsGameWindowActive			VAR_U_(0x004D8380, bool)
#define CurrentJoystick				VAR_U_(0x004D8388, JOYSTICK)
#define JoystickList				VAR_U_(0x004D8540, JOYSTICK_LIST)
#define DInput						VAR_U_(0x004D854C, LPDIRECTINPUT8)
#define IDID_SysKeyboard			VAR_U_(0x004D8550, LPDIRECTINPUTDEVICE8)
#define IDID_SysJoystick			VAR_U_(0x004D8554, LPDIRECTINPUTDEVICE8)
#define IsVidSizeLock				VAR_U_(0x004D855C, BOOL)
#define SoundAdapterList			VAR_U_(0x004D8960, SOUND_ADAPTER_LIST)
#define IsSoundEnabled				VAR_U_(0x004D8D70, bool)
#define DSound						VAR_U_(0x004D8D74, LPDIRECTSOUND8)
#define CurrentSoundAdapter			VAR_U_(0x004D8E78, SOUND_ADAPTER)
#define PrimarySoundAdapter			VAR_U_(0x004D8E9C, SOUND_ADAPTER_NODE*)
extern SWR_BUFFER RenderBuffer;
extern SWR_BUFFER PictureBuffer;
#define GameVidRect					VAR_U_(0x004D9318, RECT)
#define GameVidWidth				VAR_U_(0x004D9328, int)
#define GameVidHeight				VAR_U_(0x004D932C, int)
#define GameVidBPP					VAR_U_(0x004D9330, int)
#define UvAdd						VAR_U_(0x004D933C, int)
#define HwrTexturePagesCount		VAR_U_(0x004D9350, DWORD)
#define ReadFileBytesCounter		VAR_U_(0x004D9BE0, DWORD)
#define LevelFilePalettesOffset		VAR_U_(0x004D9BE4, LONG)
#define LevelFileTexPagesOffset		VAR_U_(0x004D9BE8, LONG)
#define Meshes						VAR_U_(0x004D9D80, short*)
#define FloorData					VAR_U_(0x004D9D84, short*)
#define TextureInfoCount			VAR_U_(0x004D9E88, DWORD)
#define LevelFileDepthQOffset		VAR_U_(0x004D9E8C, LONG)
#define DriveLetter					VAR_U_(0x004D9E98, char)
#define IsFmvPlaying				VAR_U_(0x004D9E9C, BOOL)
#define MovieContext				VAR_U_(0x004D9EA0, LPVOID)
#define FmvContext					VAR_U_(0x004D9EA4, LPVOID)
#define FmvSoundContext				VAR_U_(0x004D9EA8, LPVOID)
#define SavedGamesCount				VAR_U_(0x004D9EAC, DWORD)
#define CurrentLevel				VAR_U_(0x004D9EB0, int)
#define IsLevelComplete				VAR_U_(0x004D9EB4, BOOL)
#define SaveCounter					VAR_U_(0x004D9EB8, DWORD)
#define CurrentTexSource			VAR_U_(0x00519EC8, HWR_TEXHANDLE)
#define HWR_VertexPtr				VAR_U_(0x00519F50, D3DTLVERTEX*)
#define ZEnableState				VAR_U_(0x00519F54, bool)
#define AlphaBlendEnabler			VAR_U_(0x00519F58, D3DRENDERSTATETYPE)
#define ColorKeyState				VAR_U_(0x00519F5C, bool)
#define ZWriteEnableState			VAR_U_(0x00519F60, bool)
#define GameMemorySize				VAR_U_(0x00519FF0, DWORD)
#define GameAllocMemPointer			VAR_U_(0x00519FF4, BYTE*)
#define GameAllocMemUsed			VAR_U_(0x00519FF8, DWORD)
#define GameAllocMemFree			VAR_U_(0x00519FFC, DWORD)
#define GameMemoryPointer			VAR_U_(0x0051A0BC, BYTE*)
#define InputStatus					VAR_U_(0x0051A1F8, DWORD)
#define IsVidModeLock				VAR_U_(0x0051A1FC, bool)
#define JoyKeys						VAR_U_(0x0051A200, DWORD)
#define GameModule					VAR_U_(0x0051A228, HINSTANCE)
#define CommandLinePtr				VAR_U_(0x0051A22C, LPTSTR)
#define KeyCursor					VAR_U_(0x0051A238, int)
#define PassportTextInfo			VAR_U_(0x0051A2BC, TEXT_STR_INFO*)
#define PhdWinRect					VAR_U_(0x0051B908, RECT)
#define HiRes						VAR_U_(0x0051B918, int)
#define AnimatedTextureRanges		VAR_U_(0x0051B91C, short*)
#define WinVidNeedToResetBuffers	VAR_U_(0x0051BC20, bool)
#define IsWet						VAR_U_(0x0051BC28, BOOL)
#if defined(FEATURE_NOLEGACY_OPTIONS) || defined(FEATURE_VIDEOFX_IMPROVED)
extern APP_SETTINGS ChangedAppSettings;
#else // defined(FEATURE_NOLEGACY_OPTIONS) || defined(FEATURE_VIDEOFX_IMPROVED)
#define ChangedAppSettings			VAR_U_(0x0051BC40, APP_SETTINGS)
#endif // defined(FEATURE_NOLEGACY_OPTIONS) || defined(FEATURE_VIDEOFX_IMPROVED)
#define SE_PropSheetWndHandle		VAR_U_(0x0051BC7C, HWND)
#define SE_OldPropSheetWndProc		VAR_U_(0x0051BC80, WNDPROC)
#define SE_DisplayAdapter			VAR_U_(0x0051BC8C, DISPLAY_ADAPTER_NODE*)
#define SoundDialogHandle			VAR_U_(0x0051BC90, HWND)
#define GraphicsDialogHandle		VAR_U_(0x0051BC9C, HWND)
#define SE_FullScreenMode			VAR_U_(0x0051BCA0, DISPLAY_MODE)
#if defined(FEATURE_NOLEGACY_OPTIONS) || defined(FEATURE_VIDEOFX_IMPROVED)
extern APP_SETTINGS SavedAppSettings;
#else // defined(FEATURE_NOLEGACY_OPTIONS) || defined(FEATURE_VIDEOFX_IMPROVED)
#define SavedAppSettings			VAR_U_(0x0051BCB0, APP_SETTINGS)
#endif // defined(FEATURE_NOLEGACY_OPTIONS) || defined(FEATURE_VIDEOFX_IMPROVED)
#define IsSetupDialogCentered		VAR_U_(0x0051BCF0, bool)
#define IsTitleLoaded				VAR_U_(0x0051BD90, BOOL)
#define S_MasterVolume				VAR_U_(0x0051BD98, DWORD)
#define MciDeviceID					VAR_U_(0x0051BD9C, MCIDEVICEID)
#define CD_LoopTrack				VAR_U_(0x0051BDA0, int)
#define CD_LoopCounter				VAR_U_(0x0051BDA4, int)
#define TextureFormat				VAR_U_(0x0051C1A8, TEXTURE_FORMAT)
#define TexturesAlphaChannel		VAR_U_(0x0051C1FC, bool)
#define TexturesHaveCompatibleMasks	VAR_U_(0x0051C1FD, bool)
#define HKey						VAR_U_(0x0051C290, HKEY)
#define RegKeyDisposition			VAR_U_(0x0051C294, DWORD)
#define TextStringCount				VAR_U_(0x0051D6A0, short)
#define SoundIsActive				VAR_U_(0x0051E6C4, BOOL)
//extern DWORD SampleInfoCount; // 0x0051E6C0
extern std::vector<SAMPLE_INFO> SampleInfos; // 0x0051E9C4
extern SAVEGAME_INFO SaveGame; // 0x0051E9E0: SAVEGAME_INFO struct
extern BYTE* SG_Point; // 0x0051E9C8: BYTE*
extern DWORD SG_Count; // 0x005206A4: DWORD
#define Lara						VAR_U_(0x005206E0, LARA_INFO)
#define LaraItem					VAR_U_(0x005207BC, ITEM_INFO*)
extern FX_INFO Effects[MAX_EFFECTS]; // 0x005207C0
#define NextItemFree				VAR_U_(0x005207C6, short)
#define NextItemActive				VAR_U_(0x005207C8, short)
#define NextEffectFree              VAR_U_(0x005207C4, short)
#define NextEffectActive			VAR_U_(0x005207CA, short)
#define PrevItemActive				VAR_U_(0x005207CC, short)
#define SkipHairPhysics				VAR_U_(0x00521CD0, BOOL)
#define SoundFxCount				VAR_U_(0x00521FDC, DWORD)
#define SoundFx						VAR_U_(0x00521FE0, SOUND_SOURCE_INFO*)
#define AnimFrames					VAR_U_(0x005251B0, short*)
#define MeshPtr						VAR_U_(0x005252B0, short**)
#define OutsideCamera				VAR_U_(0x005252B4, BOOL)
#define DrawRoomsCount				VAR_U_(0x005252B8, int)
#define Anims						VAR_U_(0x005258F4, ANIM_STRUCT*)
#define OutsideBottom				VAR_U_(0x00525B00, int)
#define AnimRanges					VAR_U_(0x00525B04, RANGE_STRUCT*)
#define AnimCommands				VAR_U_(0x00525B08, short*)
#define AnimBones					VAR_U_(0x00525BE8, int*)
#define DynamicLightCount			VAR_U_(0x00525BEC, DWORD)
#define OutsideLeft					VAR_U_(0x00526178, int)
#define AnimChanges					VAR_U_(0x0052617C, CHANGE_STRUCT*)
#define RoomCount					VAR_U_(0x00526180, short)
extern ROOM_INFO* Rooms; // 0x0052618C
#define UnderwaterCamera			VAR_U_(0x00526190, BOOL)
#define SunsetTimer					VAR_U_(0x00526194, DWORD)
#define OutsideRight				VAR_U_(0x00526198, int)
#define OutsideTop					VAR_U_(0x005261AC, int)
#define DemoPtr						VAR_U_(0x005261B0, LPVOID)
#define DemoCount					VAR_U_(0x005261B4, int)
#define FlipStatus					VAR_U_(0x00526240, BOOL)
#define TriggerPtr					VAR_U_(0x00526288, short*)
#define Items						VAR_U_(0x005262F0, ITEM_INFO*)
#define IsCinematicLoaded			VAR_U_(0x005262F4, short)
#define CineFramesCount				VAR_U_(0x005262F6, short)
#define CineFrames					VAR_U_(0x005262F8, CINE_FRAME_INFO*)
#define CinematicPos				VAR_U_(0x00526300, PHD_3DPOS)
#define CineLevelID					VAR_U_(0x00526312, short)
#define CineFrameIdx				VAR_U_(0x00526314, short)
#define Camera						VAR_U_(0x00526320, CAMERA_INFO)
#define Overlaps					VAR_U_(0x005263C8, UINT16*)
#define Boxes						VAR_U_(0x005263CC, BOX_INFO*)
#define BoxesCount					VAR_U_(0x005263D0, DWORD)

// Initialized arrays
#define TrackIDs					ARRAY_(0x004642F0, short, [16]) /* = {2, 0}; */
#define InvSpriteMusicVolume		ARRAY_(0x00464718, INVENTORY_SPRITE, [9]) /* = {
	{2, -66, -80, 32, 132,  0, 0x004645DC, ICLR_Gray},
	{2, -66, -65, 32, 132,  0, 0x004645E0, ICLR_Gray},
	{2, -66, -80, 32,   0, 16, 0x004645DC, ICLR_Gray},
	{2,  66, -80, 32,   0, 16, 0x004645E0, ICLR_Gray},
	{3, -65, -79, 32, 131, 14, 0x004645E8, ICLR_Gray},
	{3, -64, -78, 32, 129, 12, 0x004645E8, ICLR_Gray},
	{4, -63, -77, 32, 126, 10, 0x004645F8, ICLR_Red},
	{4, -63, -77, 33, 127, 10, NULL, 0},
	{0,   0,   0,  0,   0,  0, NULL, 0},
}; */
#define InvSpriteSoundVolume		ARRAY_(0x004647E0, INVENTORY_SPRITE, [9]) /* = {
	{2, -66, -56, 32, 132,  0, 0x004645DC, ICLR_Gray},
	{2, -66, -41, 32, 132,  0, 0x004645E0, ICLR_Gray},
	{2, -66, -56, 32,   0, 16, 0x004645DC, ICLR_Gray},
	{2,  66, -56, 32,   0, 16, 0x004645E0, ICLR_Gray},
	{3, -65, -55, 32, 131, 14, 0x004645E8, ICLR_Gray},
	{3, -64, -54, 32, 129, 12, 0x004645E8, ICLR_Gray},
	{4, -63, -53, 32, 126, 10, 0x00464600, ICLR_Blue},
	{4, -63, -53, 33, 127, 10, NULL, 0},
	{0,   0,   0,  0,   0,  0, NULL, 0},
}; */
#define InvSpriteMusicVolumeLow		ARRAY_(0x004648A8, INVENTORY_SPRITE, [9]) /* = {
	{2, -66, -80, 32, 132,  0, 0x00464610, ICLR_Gray},
	{2, -66, -65, 32, 132,  0, 0x00464614, ICLR_Gray},
	{2, -66, -80, 32,   0, 16, 0x00464610, ICLR_Gray},
	{2,  66, -80, 32,   0, 16, 0x00464614, ICLR_Gray},
	{3, -65, -79, 32, 131, 14, 0x00464618, ICLR_Gray},
	{3, -64, -78, 32, 129, 12, 0x00464618, ICLR_Gray},
	{4, -63, -77, 32, 126, 10, 0x00464620, ICLR_Red},
	{4, -63, -77, 33, 127, 10, NULL, 0},
	{0,   0,   0,  0,   0,  0, NULL, 0},
}; */
#define InvSpriteSoundVolumeLow		ARRAY_(0x00464970, INVENTORY_SPRITE, [9]) /* = {
	{2, -66, -56, 32, 132,  0, 0x00464610, ICLR_Gray},
	{2, -66, -41, 32, 132,  0, 0x00464614, ICLR_Gray},
	{2, -66, -56, 32,   0, 16, 0x00464610, ICLR_Gray},
	{2,  66, -56, 32,   0, 16, 0x00464614, ICLR_Gray},
	{3, -65, -55, 32, 131, 14, 0x00464618, ICLR_Gray},
	{3, -64, -54, 32, 129, 12, 0x00464618, ICLR_Gray},
	{4, -63, -53, 32, 126, 10, 0x00464628, ICLR_Blue},
	{4, -63, -53, 33, 127, 10, NULL, 0},
	{0,   0,   0,  0,   0,  0, NULL, 0},
}; */
#define Weapons		ARRAY_(0x00465AE0, WEAPON_INFO, [10]) /* = {
	{ // LGT_Unarmed
		{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, 0, 0, 0, 0, 0, 0, 0,
	},
	{ // LGT_Pistols
		{ -60*PHD_DEGREE,  60*PHD_DEGREE, -60*PHD_DEGREE, 60*PHD_DEGREE},
		{-170*PHD_DEGREE,  60*PHD_DEGREE, -80*PHD_DEGREE, 80*PHD_DEGREE},
		{ -60*PHD_DEGREE, 170*PHD_DEGREE, -80*PHD_DEGREE, 80*PHD_DEGREE},
		10*PHD_DEGREE, 8*PHD_DEGREE, 650, 1, 0x2000, 9, 3, 8,
	},
	{ // LGT_Magnums
		{ -60*PHD_DEGREE,  60*PHD_DEGREE, -60*PHD_DEGREE, 60*PHD_DEGREE},
		{-170*PHD_DEGREE,  60*PHD_DEGREE, -80*PHD_DEGREE, 80*PHD_DEGREE},
		{ -60*PHD_DEGREE, 170*PHD_DEGREE, -80*PHD_DEGREE, 80*PHD_DEGREE},
		10*PHD_DEGREE, 8*PHD_DEGREE, 650, 2, 0x2000, 9, 3, 21,
	},
	{ // LGT_Uzis
		{ -60*PHD_DEGREE,  60*PHD_DEGREE, -60*PHD_DEGREE, 60*PHD_DEGREE},
		{-170*PHD_DEGREE,  60*PHD_DEGREE, -80*PHD_DEGREE, 80*PHD_DEGREE},
		{ -60*PHD_DEGREE, 170*PHD_DEGREE, -80*PHD_DEGREE, 80*PHD_DEGREE},
		10*PHD_DEGREE, 8*PHD_DEGREE, 650, 1, 0x2000, 3, 3, 43,
	},
	{ // LGT_Shotgun
		{-60*PHD_DEGREE, 60*PHD_DEGREE, -55*PHD_DEGREE, 55*PHD_DEGREE},
		{-80*PHD_DEGREE, 80*PHD_DEGREE, -65*PHD_DEGREE, 65*PHD_DEGREE},
		{-80*PHD_DEGREE, 80*PHD_DEGREE, -65*PHD_DEGREE, 65*PHD_DEGREE},
		10*PHD_DEGREE, 0, 500, 3, 0x2000, 9, 3, 45,
	},
	{ // LGT_M16
		{-60*PHD_DEGREE, 60*PHD_DEGREE, -55*PHD_DEGREE, 55*PHD_DEGREE},
		{-80*PHD_DEGREE, 80*PHD_DEGREE, -65*PHD_DEGREE, 65*PHD_DEGREE},
		{-80*PHD_DEGREE, 80*PHD_DEGREE, -65*PHD_DEGREE, 65*PHD_DEGREE},
		10*PHD_DEGREE, 4*PHD_DEGREE, 500, 3, 0x3000, 0, 3, 0,
	},
	{ // LGT_Grenade
		{-60*PHD_DEGREE, 60*PHD_DEGREE, -55*PHD_DEGREE, 55*PHD_DEGREE},
		{-80*PHD_DEGREE, 80*PHD_DEGREE, -65*PHD_DEGREE, 65*PHD_DEGREE},
		{-80*PHD_DEGREE, 80*PHD_DEGREE, -65*PHD_DEGREE, 65*PHD_DEGREE},
		10*PHD_DEGREE, 8*PHD_DEGREE, 500, 30, 0x2000, 0, 2, 0,
	},
	{ // LGT_Harpoon
		{-60*PHD_DEGREE, 60*PHD_DEGREE, -65*PHD_DEGREE, 65*PHD_DEGREE},
		{-80*PHD_DEGREE, 80*PHD_DEGREE, -75*PHD_DEGREE, 75*PHD_DEGREE},
		{-80*PHD_DEGREE, 80*PHD_DEGREE, -75*PHD_DEGREE, 75*PHD_DEGREE},
		10*PHD_DEGREE, 8*PHD_DEGREE, 500, 4, 0x2000, 0, 2, 0,
	},
	{ // LGT_Flare
		{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, 0, 0, 0, 0, 0, 0, 0,
	},
	{ // LGT_Skidoo
		{-30*PHD_DEGREE, 30*PHD_DEGREE, -55*PHD_DEGREE, 55*PHD_DEGREE},
		{-30*PHD_DEGREE, 30*PHD_DEGREE, -55*PHD_DEGREE, 55*PHD_DEGREE},
		{-30*PHD_DEGREE, 30*PHD_DEGREE, -55*PHD_DEGREE, 55*PHD_DEGREE},
		10*PHD_DEGREE, 8*PHD_DEGREE, 400, 3, 0x2000, 0, 2, 43,
	},
}; */
#define SaveSlotFlags				ARRAY_(0x00466B80, short, [16]) /* = {-1, 0}; */
#if defined(FEATURE_HUD_IMPROVED)
extern CONTROL_LAYOUT Layout[3];
#else // FEATURE_HUD_IMPROVED
#define Layout						ARRAY_(0x00466F58, CONTROL_LAYOUT, [2]) /* = {
	{	// Default Layout
		DIK_UP,
		DIK_DOWN,
		DIK_LEFT,
		DIK_RIGHT,
		DIK_DELETE,
		DIK_NEXT,
		DIK_RSHIFT,
		DIK_RMENU,
		DIK_RCONTROL,
		DIK_SPACE,
		DIK_SLASH,
		DIK_NUMPAD0,
		DIK_END,
		DIK_ESCAPE,
	},
	{	// User Layout
		DIK_NUMPAD8,
		DIK_NUMPAD2,
		DIK_NUMPAD4,
		DIK_NUMPAD6,
		DIK_NUMPAD7,
		DIK_NUMPAD9,
		DIK_NUMPAD1,
		DIK_ADD,
		DIK_NUMPADENTER,
		DIK_NUMPAD3,
		DIK_SUBTRACT,
		DIK_NUMPAD0,
		DIK_NUMPAD5,
		DIK_DECIMAL,
	},
}; */
#endif // FEATURE_HUD_IMPROVED

// Uninitialized arrays
#define GouraudTable				ARRAY_(0x0046C300, GOURAUD_ENTRY, [256])
#if defined(FEATURE_EXTENDED_LIMITS)
extern PHD_SPRITE PhdSpriteInfo[2048];
#else // FEATURE_EXTENDED_LIMITS
#define PhdSpriteInfo				ARRAY_(0x0046E308, PHD_SPRITE, [512])
#endif // FEATURE_EXTENDED_LIMITS
#if defined(FEATURE_EXTENDED_LIMITS) || defined(FEATURE_VIEW_IMPROVED)
extern SORT_ITEM SortBuffer[16000];
extern short Info3dBuffer[480000];
#else // defined(FEATURE_EXTENDED_LIMITS) || defined(FEATURE_VIEW_IMPROVED)
#define SortBuffer					ARRAY_(0x00470338, SORT_ITEM, [4000])
#define Info3dBuffer				ARRAY_(0x00478060, short, [120000])
#endif // defined(FEATURE_EXTENDED_LIMITS) || defined(FEATURE_VIEW_IMPROVED)
#define RandomTable					ARRAY_(0x004B2A28, int, [32])
#if defined(FEATURE_EXTENDED_LIMITS)
extern PHD_TEXTURE PhdTextureInfo[MAX_OBJECT_TEXTURES];
#else // FEATURE_EXTENDED_LIMITS
#define PhdTextureInfo				ARRAY_(0x004B2AF0, PHD_TEXTURE, [0x800])
#endif // FEATURE_EXTENDED_LIMITS
#define ShadesTable					ARRAY_(0x004BCB00, short, [32])
#define MatrixStack					ARRAY_(0x004BCB48, PHD_MATRIX, [40])
#define DepthQTable					ARRAY_(0x004BD2C8, DEPTHQ_ENTRY, [32])
#define DepthQIndex					ARRAY_(0x004BF2C8, BYTE, [256])
#define PhdVBuf						ARRAY_(0x004BF3D0, PHD_VBUF, [1500])
#if defined(FEATURE_EXTENDED_LIMITS)
extern BYTE* TexturePageBuffer8[MAX_TEXTURE_PAGES];
#else // FEATURE_EXTENDED_LIMITS
#define TexturePageBuffer8			ARRAY_(0x004D6AD0, BYTE*, [32])
#endif // FEATURE_EXTENDED_LIMITS
#define WibbleTable					ARRAY_(0x004D6B68, float, [32])
#define GamePalette16				ARRAY_(0x004D7370, PALETTEENTRY, [256])
//#define InvItemText					ARRAY_(0x004D7938, TEXT_STR_INFO*, [2])
extern TEXT_STR_INFO* InvItemText[3]; // 0x004D7938
#define InventoryExtraData			ARRAY_(0x004D7970, int, [8])
#define SfxInfos					ARRAY_(0x004D7C68, SFX_INFO, [32])
#if defined(FEATURE_BACKGROUND_IMPROVED)
extern int BGND_TexturePageIndexes[64];
extern HWR_TEXHANDLE BGND_PageHandles[64];
#else // FEATURE_BACKGROUND_IMPROVED
#define BGND_TexturePageIndexes		ARRAY_(0x004D7E80, int, [5])
#define BGND_PageHandles			ARRAY_(0x004D7E98, HWR_TEXHANDLE, [5])
#endif // FEATURE_BACKGROUND_IMPROVED
#if defined(FEATURE_BACKGROUND_IMPROVED)
extern DWORD SampleFreqs[370];
extern LPDIRECTSOUNDBUFFER SampleBuffers[370];
#else // FEATURE_EXTENDED_LIMITS
#define SampleFreqs					ARRAY_(0x004D8560, DWORD, [256])
#define SampleBuffers				ARRAY_(0x004D8970, LPDIRECTSOUNDBUFFER, [256])
#endif // FEATURE_EXTENDED_LIMITS
#define ChannelSamples				ARRAY_(0x004D8D78, DWORD, [32])
#define ChannelBuffers				ARRAY_(0x004D8DF8, LPDIRECTSOUNDBUFFER, [32])
#define WinVidPalette				ARRAY_(0x004D8EA8, PALETTEENTRY, [256])
#if defined(FEATURE_BACKGROUND_IMPROVED)
extern BYTE LabTextureUVFlags[MAX_OBJECT_TEXTURES];
#else // FEATURE_EXTENDED_LIMITS
#define LabTextureUVFlags			ARRAY_(0x004D93E0, BYTE, [0x800])
#endif // FEATURE_EXTENDED_LIMITS
#define LevelFileName				ARRAY_(0x004D9D88, char, [256])
#if defined(FEATURE_EXTENDED_LIMITS)
extern D3DTLVERTEX HWR_VertexBuffer[32768];
extern HWR_TEXHANDLE HWR_PageHandles[MAX_TEXTURE_PAGES];
extern int HWR_TexturePageIndexes[MAX_TEXTURE_PAGES];
#else // FEATURE_EXTENDED_LIMITS
#define HWR_VertexBuffer			ARRAY_(0x004D9EC8, D3DTLVERTEX, [0x2000])
#define HWR_PageHandles				ARRAY_(0x00519ED0, HWR_TEXHANDLE, [32])
#define HWR_TexturePageIndexes		ARRAY_(0x00519F68, int, [32])
#endif // FEATURE_EXTENDED_LIMITS
#if defined(FEATURE_HUD_IMPROVED)
extern bool ConflictLayout[ARRAY_SIZE(Layout->key)];
#else // FEATURE_HUD_IMPROVED
#define ConflictLayout				ARRAY_(0x0051A0C0, BOOL, [14])
#endif // FEATURE_HUD_IMPROVED
#define DIKeys						ARRAY_(0x0051A0F8, BYTE, [256])
#define ControlTextInfo				ARRAY_(0x0051A2F0, TEXT_STR_INFO*, [2])
#define RoomLightTables				ARRAY_(0x0051A2F8, ROOM_LIGHT_TABLE, [32])
#define WaterPalette				ARRAY_(0x0051B2F8, RGB888, [256])
#define PicPalette					ARRAY_(0x0051B5F8, RGB888, [256])
#define RoomLightShades				ARRAY_(0x0051B8F8, int, [4])
#define GamePalette8				ARRAY_(0x0051B920, RGB888, [256])
#define StringToShow				ARRAY_(0x0051BD10, char, [128])
#define TextInfoTable				ARRAY_(0x0051C820, TEXT_STR_INFO, [64])
#define TheStrings					ARRAY_(0x0051D6C0, STRING_FIXED64, [64])
#define SaveGameStrings1			ARRAY_(0x005207E0, STRING_FIXED50, [24])
#define RequesterItemFlags2			ARRAY_(0x00520CA0, DWORD, [24])
#define RequesterItemFlags1			ARRAY_(0x00520D00, DWORD, [24])
#define InvColours					ARRAY_(0x005216E0, UINT16, [17])
#define SaveGameStrings2			ARRAY_(0x00521720, STRING_FIXED50, [24])
#define SaveGameItemFlags2			ARRAY_(0x00521BE0, DWORD, [24])
#define SaveGameItemFlags1			ARRAY_(0x00521C40, DWORD, [24])
#define PickupInfos					ARRAY_(0x00521CA0, PICKUP_INFO, [12])
#define HairVelocity				ARRAY_(0x00521CE0, PHD_VECTOR, [7])
#define HairPos						ARRAY_(0x00521D40, PHD_3DPOS, [7])
#define Objects						ARRAY_(0x00522000, OBJECT_INFO, [265])
#if defined(FEATURE_EXTENDED_LIMITS)
extern LIGHT_INFO DynamicLights[64];
extern int BoundRooms[1024];
extern short DrawRoomsArray[1024];
extern STATIC_INFO StaticObjects[256];
#else // FEATURE_EXTENDED_LIMITS
#define DynamicLights				ARRAY_(0x005251C0, LIGHT_INFO, [10])
#define BoundRooms					ARRAY_(0x00525900, int, [128])
#define DrawRoomsArray				ARRAY_(0x00525B20, short, [100])
#define StaticObjects				ARRAY_(0x00525C00, STATIC_INFO, [50])
#endif // FEATURE_EXTENDED_LIMITS
#define CDFlags						ARRAY_(0x005261C0, short, [MAX_CD])
#define FlipMaps					ARRAY_(0x00526260, int, [MAX_FLIPMAPS])
#define LosRooms					ARRAY_(0x005262A0, int, [20])
#define GroundZones					ARRAY_(0x005263A0, UINT16*, [8])
#define FlyZones					ARRAY_(0x005263C0, UINT16*, [2])

/*
 * GameFlow/Inventory Variables
 */
 // GameFlow/Inventory initialized variables
#define LoadGameRequester			VAR_I_(0x00465620, REQUEST_INFO,	"...")
#define SaveGameRequester			VAR_I_(0x00465838, REQUEST_INFO,	"...")
#define InvCompassOption			VAR_I_(0x00464A90, INVENTORY_ITEM,	"...")
#define InvPistolOption				VAR_I_(0x00464AE0, INVENTORY_ITEM,	"...")
#define InvFlareOption				VAR_I_(0x00464B30, INVENTORY_ITEM,	"...")
#define InvShotgunOption			VAR_I_(0x00464B80, INVENTORY_ITEM,	"...")
#define InvMagnumOption				VAR_I_(0x00464BD0, INVENTORY_ITEM,	"...")
#define InvUziOption				VAR_I_(0x00464C20, INVENTORY_ITEM,	"...")
#define InvHarpoonOption			VAR_I_(0x00464C70, INVENTORY_ITEM,	"...")
#define InvM16Option				VAR_I_(0x00464CC0, INVENTORY_ITEM,	"...")
#define InvGrenadeOption			VAR_I_(0x00464D10, INVENTORY_ITEM,	"...")
#define InvPistolAmmoOption			VAR_I_(0x00464D60, INVENTORY_ITEM,	"...")
#define InvShotgunAmmoOption		VAR_I_(0x00464DB0, INVENTORY_ITEM,	"...")
#define InvMagnumAmmoOption			VAR_I_(0x00464E00, INVENTORY_ITEM,	"...")
#define InvUziAmmoOption			VAR_I_(0x00464E50, INVENTORY_ITEM,	"...")
#define InvHarpoonAmmoOption		VAR_I_(0x00464EA0, INVENTORY_ITEM,	"...")
#define InvM16AmmoOption			VAR_I_(0x00464EF0, INVENTORY_ITEM,	"...")
#define InvGrenadeAmmoOption		VAR_I_(0x00464F40, INVENTORY_ITEM,	"...")
#define InvSmallMedipackOption		VAR_I_(0x00464F90, INVENTORY_ITEM,	"...")
#define InvLargeMedipackOption		VAR_I_(0x00464FE0, INVENTORY_ITEM,	"...")
#define InvPickup1Option			VAR_I_(0x00465030, INVENTORY_ITEM,	"...")
#define InvPickup2Option			VAR_I_(0x00465080, INVENTORY_ITEM,	"...")
#define InvPuzzle1Option			VAR_I_(0x004650D0, INVENTORY_ITEM,	"...")
#define InvPuzzle2Option			VAR_I_(0x00465120, INVENTORY_ITEM,	"...")
#define InvPuzzle3Option			VAR_I_(0x00465170, INVENTORY_ITEM,	"...")
#define InvPuzzle4Option			VAR_I_(0x004651C0, INVENTORY_ITEM,	"...")
#define InvKey1Option				VAR_I_(0x00465210, INVENTORY_ITEM,	"...")
#define InvKey2Option				VAR_I_(0x00465260, INVENTORY_ITEM,	"...")
#define InvKey3Option				VAR_I_(0x004652B0, INVENTORY_ITEM,	"...")
#define InvKey4Option				VAR_I_(0x00465300, INVENTORY_ITEM,	"...")
#define InvPassportOption			VAR_I_(0x00465350, INVENTORY_ITEM,	"...")
#define InvDetailOption				VAR_I_(0x004653A0, INVENTORY_ITEM,	"...")
#define InvSoundOption				VAR_I_(0x004653F0, INVENTORY_ITEM,	"...")
#define InvControlOption			VAR_I_(0x00465440, INVENTORY_ITEM,	"...")
#define InvPhotoOption				VAR_I_(0x00465490, INVENTORY_ITEM,	"...")

// GameFlow/Inventory uninitialized variables
#define GF_LaraStartAnim			VAR_U_(0x004D77E0, int)
#define GF_SunsetEnabled			VAR_U_(0x004D77E4, UINT16)
#define GF_DeadlyWater				VAR_U_(0x004D77E8, UINT16)
#define GF_NoFloor					VAR_U_(0x004D77EC, UINT16)
#define GF_RemoveWeapons			VAR_U_(0x004D77F0, UINT16)
#define GF_RemoveAmmo				VAR_U_(0x004D77F4, UINT16)
#define GF_Kill2Complete			VAR_U_(0x004D77F8, bool)
#define GF_StartGame				VAR_U_(0x004D77FC, bool)
#define GF_GameFlow					VAR_U_(0x00521DE0, GAME_FLOW)
#define GF_ScriptBuffer				VAR_U_(0x00521E70, short*)
#define GF_LevelNamesStringTable	VAR_U_(0x00521EC4, char**)
#define GF_PictureFilesStringTable	VAR_U_(0x00521EA0, char**)
#define GF_TitleFilesStringTable	VAR_U_(0x00521F48, char**)
#define GF_FmvFilesStringTable		VAR_U_(0x00521F94, char**)
#define GF_LevelFilesStringTable	VAR_U_(0x00521F54, char**)
#define GF_CutsFilesStringTable		VAR_U_(0x00521DC4, char**)
#define GF_GameStringTable			VAR_U_(0x00521EB0, char**)
#define GF_SpecificStringTable		VAR_U_(0x00521F50, char**)
#define GF_Puzzle1StringTable		VAR_U_(0x00521DC0, char**)
#define GF_Puzzle2StringTable		VAR_U_(0x00521E98, char**)
#define GF_Puzzle3StringTable		VAR_U_(0x00521EC0, char**)
#define GF_Puzzle4StringTable		VAR_U_(0x00521E60, char**)
#define GF_Pickup1StringTable		VAR_U_(0x00521E94, char**)
#define GF_Pickup2StringTable		VAR_U_(0x00521F44, char**)
#define GF_Key1StringTable			VAR_U_(0x00521EA4, char**)
#define GF_Key2StringTable			VAR_U_(0x00521E74, char**)
#define GF_Key3StringTable			VAR_U_(0x00521EBC, char**)
#define GF_Key4StringTable			VAR_U_(0x00521E90, char**)
#define GF_LevelNamesStringBuffer	VAR_U_(0x00521EAC, char*)
#define GF_PictureFilesStringBuffer	VAR_U_(0x00521E8C, char*)
#define GF_TitleFilesStringBuffer	VAR_U_(0x00521F4C, char*)
#define GF_FmvFilesStringBuffer		VAR_U_(0x00521E68, char*)
#define GF_LevelFilesStringBuffer	VAR_U_(0x00521E9C, char*)
#define GF_CutsFilesStringBuffer	VAR_U_(0x00521E78, char*)
#define GF_GameStringBuffer			VAR_U_(0x00521EB8, char*)
#define GF_SpecificStringBuffer		VAR_U_(0x00521EB4, char*)
#define GF_Puzzle1StringBuffer		VAR_U_(0x00521EA8, char*)
#define GF_Puzzle2StringBuffer		VAR_U_(0x00521F40, char*)
#define GF_Puzzle3StringBuffer		VAR_U_(0x00521F98, char*)
#define GF_Puzzle4StringBuffer		VAR_U_(0x00521F90, char*)
#define GF_Pickup1StringBuffer		VAR_U_(0x00521E64, char*)
#define GF_Pickup2StringBuffer		VAR_U_(0x00521E88, char*)
#define GF_Key1StringBuffer			VAR_U_(0x00521E6C, char*)
#define GF_Key2StringBuffer			VAR_U_(0x00521E84, char*)
#define GF_Key3StringBuffer			VAR_U_(0x00521F9C, char*)
#define GF_Key4StringBuffer			VAR_U_(0x00521E7C, char*)

// GameFlow/Inventory arrays
#define InvMainQtys					ARRAY_(0x004654E8, UINT16, [23]) /* = {1, 1, 1, 1, 1, 1, 1, 1, 1, 0}; */
#define InvMainList					ARRAY_(0x00465518, INVENTORY_ITEM*, [23]) /* = {
	&InvCompassOption,
	&InvFlareOption,
	&InvPistolOption,
	&InvShotgunOption,
	&InvMagnumOption,
	&InvUziOption,
	&InvM16Option,
	&InvGrenadeOption,
	&InvHarpoonOption,
	&InvLargeMedipackOption,
	&InvSmallMedipackOption,
	NULL,
}; */
#define InvKeysQtys					ARRAY_(0x00465578, UINT16, [23]) /* = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}; */
#define InvKeysList					ARRAY_(0x004655A8, INVENTORY_ITEM*, [23]) /* = {
	&InvPuzzle1Option,
	&InvPuzzle2Option,
	&InvPuzzle3Option,
	&InvPuzzle4Option,
	&InvKey1Option,
	&InvKey2Option,
	&InvKey3Option,
	&InvKey4Option,
	&InvPickup1Option,
	&InvPickup2Option,
	NULL,
}; */
#if defined(FEATURE_HUD_IMPROVED)
extern INVENTORY_ITEM* InvOptionList[5];
extern TEXT_STR_INFO* CtrlTextA[ARRAY_SIZE(Layout->key)];
extern TEXT_STR_INFO* CtrlTextB[ARRAY_SIZE(Layout->key)];
#else // FEATURE_HUD_IMPROVED
#define InvOptionList				ARRAY_(0x00465608, INVENTORY_ITEM*, [4]) /* = {
	&InvPassportOption,
	&InvControlOption,
	&InvSoundOption,
	&InvPhotoOption
}; */
#define CtrlTextA					ARRAY_(0x0051A248, TEXT_STR_INFO*, [14])
#define CtrlTextB					ARRAY_(0x0051A280, TEXT_STR_INFO*, [14])
#endif // FEATURE_HUD_IMPROVED
#define GF_ScriptTable				ARRAY_(0x00521EE0, short*, [24])
#define GF_DemoLevels				ARRAY_(0x00521F60, UINT16, [24])
#define GF_SecretInvItems			ARRAY_(0x00521FA0, char, [27])
#define GF_Add2InvItems				ARRAY_(0x00521FC0, char, [27])
#define PickUpBounds				ARRAY_(0x00465FF0, short, [12])
#define PickUpPosition				VAR_U_(0x00466008, PHD_VECTOR)
#define PickUpBoundsUW				ARRAY_(0x00466030, short, [12])
#define PickUpPositionUW			VAR_U_(0x00466048, PHD_VECTOR)
#define BaddiesSlots                VAR_U_(0x005206C0, CREATURE_INFO*)
#define BaddiesSlotsUsedCount           VAR_U_(0x004D7C40, int)
extern short* SampleLut; // 0x0051E6E0
extern SOUND_SLOT LaSlot[32]; // 0x004D7C70
#define DashTimer                   VAR_U_(0x005206E2, short)
#define ClearBag                    VAR_U_(0x005207CC, short)
#define HoldStates                  ARRAY_(0x00465CB0, short, [16])

extern DWORD SampleLutCount;
extern PHD_VECTOR CamPos;
extern int SmokeWindX;
extern int SmokeWindZ;

#endif // GLOBAL_VARS_H_INCLUDED
