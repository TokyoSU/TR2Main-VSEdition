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

// TR2Main global types for a 32-bit IDA database, with optional features disabled.
#pragma pack(push, 1)
// Fixed-width Win32 dependencies for the TR2 executable.
typedef struct __unaligned __declspec(align(1)) TR2_GUID {
    unsigned int Data1;
    unsigned short Data2, Data3;
    unsigned char Data4[8];
} TR2_GUID;

// Direct3D 9 capability data occupies 304 bytes in the Win32 SDK.
typedef struct __unaligned __declspec(align(1)) TR2_D3DCAPS9 {
    unsigned int words[76];
} TR2_D3DCAPS9;

typedef struct __unaligned __declspec(align(1)) { unsigned char opCode; unsigned int offset; } JMP;
typedef struct __unaligned __declspec(align(1)) {
	float sx, sy, sz, rhw;
	unsigned int color, specular;
	float tu, tv;
} D3DTLVERTEX, *LPD3DTLVERTEX;
typedef struct __unaligned __declspec(align(1)) { int Pitch; void *pBits; } DDSDESC, *LPDDSDESC;
typedef struct IDirect3DSurface9 * LPDDS;
typedef struct IDirect3DTexture9 * HWR_TEXHANDLE;

typedef struct __unaligned __declspec(align(1)) {
	unsigned int width;
	unsigned int height;
	unsigned char * bitmap;
} SWR_BUFFER;

typedef enum {
	ID_NONE = -1,
	ID_LARA = 0,
	ID_LARA_PISTOLS,
	ID_LARA_HAIR,
	ID_LARA_SHOTGUN,
	ID_LARA_MAGNUMS,
	ID_LARA_UZIS,
	ID_LARA_M16,
	ID_LARA_GRENADE,
	ID_LARA_HARPOON,
	ID_LARA_FLARE,
	ID_LARA_SKIDOO,
	ID_LARA_BOAT,
	ID_LARA_EXTRA,
	ID_SKIDOO_FAST,
	ID_BOAT,
	ID_DOG,
	ID_CULT1,
	ID_CULT1A,
	ID_CULT1B,
	ID_CULT2,
	ID_CULT3,
	ID_MOUSE,
	ID_DRAGON_FRONT,
	ID_DRAGON_BACK,
	ID_GONDOLA,
	ID_SHARK,
	ID_EEL,
	ID_BIG_EEL,
	ID_BARRACUDA,
	ID_DIVER,
	ID_WORKER1,
	ID_WORKER2,
	ID_WORKER3,
	ID_WORKER4,
	ID_WORKER5,
	ID_JELLY,
	ID_SPIDER_or_WOLF,
	ID_BIG_SPIDER_or_BEAR,
	ID_CROW,
	ID_TIGER,
	ID_BARTOLI,
	ID_XIAN_LORD,
	ID_CHINESE2,
	ID_WARRIOR,
	ID_CHINESE4,
	ID_YETI,
	ID_GIANT_YETI,
	ID_EAGLE,
	ID_BANDIT1,
	ID_BANDIT2,
	ID_BANDIT2B,
	ID_SKIDOO_ARMED,
	ID_SKIDMAN,
	ID_MONK1,
	ID_MONK2,
	ID_FALLING_BLOCK1,
	ID_FALLING_BLOCK2,
	ID_FALLING_BLOCK3,
	ID_PENDULUM1,
	ID_SPIKES,
	ID_ROLLING_BALL1,
	ID_DARTS,
	ID_DART_EMITTER,
	ID_DRAW_BRIDGE,
	ID_TEETH_TRAP,
	ID_LIFT,
	ID_GENERAL,
	ID_MOVABLE_BLOCK1,
	ID_MOVABLE_BLOCK2,
	ID_MOVABLE_BLOCK3,
	ID_MOVABLE_BLOCK4,
	ID_BIG_BOWL,
	ID_WINDOW1,
	ID_WINDOW2,
	ID_WINDOW3,
	ID_WINDOW4,
	ID_PROPELLER1,
	ID_PROPELLER2,
	ID_HOOK,
	ID_FALLING_CEILING,
	ID_SPINNING_BLADE,
	ID_BLADE,
	ID_KILLER_STATUE,
	ID_ROLLING_BALL2,
	ID_ICICLE,
	ID_SPIKE_WALL,
	ID_SPRING_BOARD,
	ID_CEILING_SPIKES,
	ID_BELL,
	ID_WATER_SPRITE,
	ID_SNOW_SPRITE,
	ID_SKIDOO_LARA,
	ID_SWITCH_TYPE1,
	ID_SWITCH_TYPE2,
	ID_PROPELLER3,
	ID_PROPELLER4,
	ID_PENDULUM2,
	ID_MESH_SWAP1,
	ID_MESH_SWAP2,
	ID_LARA_SWAP,
	ID_TEXT_BOX,
	ID_ROLLING_BALL3,
	ID_DEATH_SLIDE,
	ID_SWITCH_TYPE3,
	ID_SWITCH_TYPE4,
	ID_SWITCH_TYPE5,
	ID_DOOR_TYPE1,
	ID_DOOR_TYPE2,
	ID_DOOR_TYPE3,
	ID_DOOR_TYPE4,
	ID_DOOR_TYPE5,
	ID_DOOR_TYPE6,
	ID_DOOR_TYPE7,
	ID_DOOR_TYPE8,
	ID_TRAPDOOR_TYPE1,
	ID_TRAPDOOR_TYPE2,
	ID_TRAPDOOR_TYPE3,
	ID_BRIDGE_FLAT,
	ID_BRIDGE_TILT1,
	ID_BRIDGE_TILT2,
	ID_PASSPORT_OPTION,
	ID_COMPASS_OPTION,
	ID_PHOTO_OPTION,
	ID_PLAYER1,
	ID_PLAYER2,
	ID_PLAYER3,
	ID_PLAYER4,
	ID_PLAYER5,
	ID_PLAYER6,
	ID_PLAYER7,
	ID_PLAYER8,
	ID_PLAYER9,
	ID_PLAYER10,
	ID_PASSPORT_CLOSED,
	ID_COMPASS_ITEM,
	ID_PISTOL_ITEM,
	ID_SHOTGUN_ITEM,
	ID_MAGNUM_ITEM,
	ID_UZI_ITEM,
	ID_HARPOON_ITEM,
	ID_M16_ITEM,
	ID_GRENADE_ITEM,
	ID_PISTOL_AMMO_ITEM,
	ID_SHOTGUN_AMMO_ITEM,
	ID_MAGNUM_AMMO_ITEM,
	ID_UZI_AMMO_ITEM,
	ID_HARPOON_AMMO_ITEM,
	ID_M16_AMMO_ITEM,
	ID_GRENADE_AMMO_ITEM,
	ID_SMALL_MEDIPACK_ITEM,
	ID_LARGE_MEDIPACK_ITEM,
	ID_FLARES_ITEM,
	ID_FLARE_ITEM,
	ID_DETAIL_OPTION,
	ID_SOUND_OPTION,
	ID_CONTROL_OPTION,
	ID_GAMMA_OPTION,
	ID_PISTOL_OPTION,
	ID_SHOTGUN_OPTION,
	ID_MAGNUM_OPTION,
	ID_UZI_OPTION,
	ID_HARPOON_OPTION,
	ID_M16_OPTION,
	ID_GRENADE_OPTION,
	ID_PISTOL_AMMO_OPTION,
	ID_SHOTGUN_AMMO_OPTION,
	ID_MAGNUM_AMMO_OPTION,
	ID_UZI_AMMO_OPTION,
	ID_HARPOON_AMMO_OPTION,
	ID_M16_AMMO_OPTION,
	ID_GRENADE_AMMO_OPTION,
	ID_SMALL_MEDIPACK_OPTION,
	ID_LARGE_MEDIPACK_OPTION,
	ID_FLARES_OPTION,
	ID_PUZZLE_ITEM1,
	ID_PUZZLE_ITEM2,
	ID_PUZZLE_ITEM3,
	ID_PUZZLE_ITEM4,
	ID_PUZZLE_OPTION1,
	ID_PUZZLE_OPTION2,
	ID_PUZZLE_OPTION3,
	ID_PUZZLE_OPTION4,
	ID_PUZZLE_HOLE1,
	ID_PUZZLE_HOLE2,
	ID_PUZZLE_HOLE3,
	ID_PUZZLE_HOLE4,
	ID_PUZZLE_DONE1,
	ID_PUZZLE_DONE2,
	ID_PUZZLE_DONE3,
	ID_PUZZLE_DONE4,
	ID_SECRET1_OR_SECRET_SPRITE,
	ID_SECRET2_OR_SECRET,
	ID_SECRET3,
	ID_KEY_ITEM1,
	ID_KEY_ITEM2,
	ID_KEY_ITEM3,
	ID_KEY_ITEM4,
	ID_KEY_OPTION1,
	ID_KEY_OPTION2,
	ID_KEY_OPTION3,
	ID_KEY_OPTION4,
	ID_KEY_HOLE1,
	ID_KEY_HOLE2,
	ID_KEY_HOLE3,
	ID_KEY_HOLE4,
	ID_PICKUP_ITEM1,
	ID_PICKUP_ITEM2,
	ID_PICKUP_OPTION1,
	ID_PICKUP_OPTION2,
	ID_SPHERE_OF_DOOM1,
	ID_SPHERE_OF_DOOM2,
	ID_SPHERE_OF_DOOM3,
	ID_ALARM_SOUND,
	ID_BIRD_TWEETER1,
	ID_TREX,
	ID_BIRD_TWEETER2,
	ID_CLOCK_CHIMES,
	ID_CANDLE_FLAME_EMITTER,
	ID_DRAGON_BONES1,
	ID_DRAGON_BONES2,
	ID_HOT_LIQUID,
	ID_BOAT_BITS,
	ID_MINE,
	ID_INV_BACKGROUND,
	ID_WEATHER_SPRITE,
	ID_GONG_BONGER,
	ID_DETONATOR1,
	ID_DETONATOR2,
	ID_COPTER,
	ID_EXPLOSION,
	ID_SPLASH,
	ID_BUBBLES,
	ID_UNUSED2,
	ID_BLOOD,
	ID_DART_EFFECT,
	ID_FLARE_FIRE,
	ID_GLOW,
	ID_CANDLE_FLAME_SPRITE,
	ID_RICOCHET,
	ID_TWINKLE,
	ID_GUN_FLASH,
	ID_M16_FLASH,
	ID_BODY_PART,
	ID_CAMERA_TARGET,
	ID_WATERFALL,
	ID_MISSILE_HARPOON,
	ID_MISSILE_FLAME,
	ID_MISSILE_KNIFE,
	ID_ROCKET,
	ID_HARPOON_BOLT,
	ID_LAVA,
	ID_LAVA_EMITTER,
	ID_FLAME,
	ID_FLAME_EMITTER,
	ID_SKYBOX,
	ID_ALPHABET,
	ID_DYING_MONK,
	ID_DING_DONG,
	ID_LARA_ALARM,
	ID_MINI_COPTER,
	ID_WINSTON,
	ID_ASSAULT_DIGITS,
	ID_FINAL_LEVEL_COUNTER,
	ID_CUT_SHOTGUN,
	ID_EARTHQUAKE,
	ID_NUMBER_OBJECTS,
} GAME_OBJECT_ID;

typedef enum {
	AS_WALK,
	AS_RUN,
	AS_STOP,
	AS_FORWARDJUMP,
	AS_POSE,
	AS_FASTBACK,
	AS_TURN_R,
	AS_TURN_L,
	AS_DEATH,
	AS_FASTFALL,
	AS_HANG,
	AS_REACH,
	AS_SPLAT,
	AS_TREAD,
	AS_LAND,
	AS_COMPRESS,
	AS_BACK,
	AS_SWIM,
	AS_GLIDE,
	AS_NULL,
	AS_FASTTURN,
	AS_STEPRIGHT,
	AS_STEPLEFT,
	AS_HIT,
	AS_SLIDE,
	AS_BACKJUMP,
	AS_RIGHTJUMP,
	AS_LEFTJUMP,
	AS_UPJUMP,
	AS_FALLBACK,
	AS_HANGLEFT,
	AS_HANGRIGHT,
	AS_SLIDEBACK,
	AS_SURFTREAD,
	AS_SURFSWIM,
	AS_DIVE,
	AS_PUSHBLOCK,
	AS_PULLBLOCK,
	AS_PPREADY,
	AS_PICKUP,
	AS_SWITCHON,
	AS_SWITCHOFF,
	AS_USEKEY,
	AS_USEPUZZLE,
	AS_UWDEATH,
	AS_ROLL,
	AS_SPECIAL,
	AS_SURFBACK,
	AS_SURFLEFT,
	AS_SURFRIGHT,
	AS_USEMIDAS,
	AS_DIEMIDAS,
	AS_SWANDIVE,
	AS_FASTDIVE,
	AS_GYMNAST,
	AS_WATEROUT,
	AS_CLIMBSTNC,
	AS_CLIMBING,
	AS_CLIMBLEFT,
	AS_CLIMBEND,
	AS_CLIMBRIGHT,
	AS_CLIMBDOWN,
	AS_LARATEST1,
	AS_LARATEST2,
	AS_LARATEST3,
	AS_WADE,
	AS_WATERROLL,
	AS_FLAREPICKUP,
	AS_TWIST,
	AS_KICK,
	AS_DEATHSLIDE,
	AS_LAST,
} LARA_ANIM_STATES;

typedef enum {
	EXTRA_BREATH,
	EXTRA_PLUNGER,
	EXTRA_YETIKILL,
	EXTRA_SHARKKILL,
	EXTRA_AIRLOCK,
	EXTRA_GONGBONG,
	EXTRA_DINOKILL,
	EXTRA_PULLDAGGER,
	EXTRA_STARTANIM,
	EXTRA_STARTHOUSE,
	EXTRA_FINALANIM,
} LARA_EXTRA_STATES;

typedef enum {
	VGA_NoVga,
	VGA_256Color,
	VGA_ModeX,
	VGA_Standard,
} VGA_MODE;

typedef enum {
	RM_Unknown,
	RM_Software,
	RM_Hardware,
} RENDER_MODE;

typedef enum {
	AM_4_3,
	AM_16_9,
	AM_Any,
} ASPECT_MODE;

typedef enum {
	TAM_Disabled,
	TAM_BilinearOnly,
	TAM_Always,
} TEX_ADJUST_MODE;

typedef enum {
	CTRL_Default,
	CTRL_Custom,

} CONTROL_LAYOUT_PAGE;

typedef enum {
	INV_GameMode,
	INV_TitleMode,
	INV_KeysMode,
	INV_SaveMode,
	INV_LoadMode,
	INV_DeathMode,
	INV_PauseMode,
} INVENTORY_MODE;

typedef enum {
	LGS_Armless,
	LGS_HandBusy,
	LGS_Draw,
	LGS_Undraw,
	LGS_Ready,
	LGS_Special,
} LARA_GUN_STATUS;

typedef enum {
	LGT_Unarmed,
	LGT_Pistols,
	LGT_Magnums,
	LGT_Uzis,
	LGT_Shotgun,
	LGT_M16,
	LGT_Grenade,
	LGT_Harpoon,
	LGT_Flare,
	LGT_Skidoo,
} LARA_GUN_TYPE;

typedef enum {
	CAM_Chase,
	CAM_Fixed,
	CAM_Look,
	CAM_Combat,
	CAM_Cinematic,
	CAM_Heavy,
} CAMERA_TYPE;

typedef enum {
	CFL_None,
	CFL_FollowCenter,
	CFL_NoChunky,
	CFL_ChaseObject,
} CAMERA_FLAG;

typedef enum {
	KM_Forward,
	KM_Back,
	KM_Left,
	KM_Right,
	KM_StepLeft,
	KM_StepRight,

	KM_Slow,
	KM_Jump,
	KM_Action,
	KM_WeaponDraw,
	KM_Flare,
	KM_Look,
	KM_Roll,
	KM_Option,

} KEYMAP;

typedef enum {
	ST_AvgZ,
	ST_MaxZ,
	ST_FarZ,
} SORTTYPE;

typedef enum {
	DRAW_Opaque,
	DRAW_ColorKey,
	DRAW_Semitrans,
} DRAWTYPE;

typedef enum {

	POLY_GTmap,
	POLY_WGTmap,
	POLY_GTmap_persp,
	POLY_WGTmap_persp,
	POLY_line,
	POLY_flat,
	POLY_gouraud,
	POLY_trans,
	POLY_sprite,

	POLY_HWR_GTmap,
	POLY_HWR_WGTmap,
	POLY_HWR_gouraud,
	POLY_HWR_line,
	POLY_HWR_trans,

} POLYTYPE;

typedef enum {
	ERR_OK,
	ERR_PreferredAdapterNotFound,
	ERR_CantCreateWindow,
	ERR_CantCreateDirectDraw,
	ERR_CantInitRenderer,
	ERR_CantCreateDirectInput,
	ERR_CantCreateKeyboardDevice,
	ERR_CantSetKBCooperativeLevel,
	ERR_CantSetKBDataFormat,
	ERR_CantAcquireKeyboard,
	ERR_CantSetDSCooperativeLevel,
	ERR_DD_SetExclusiveMode,
	ERR_DD_ClearExclusiveMode,
	ERR_SetDisplayMode,
	ERR_CreateScreenBuffers,
	ERR_GetBackBuffer,
	ERR_CreatePalette,
	ERR_SetPalette,
	ERR_CreatePrimarySurface,
	ERR_CreateBackBuffer,
	ERR_CreateClipper,
	ERR_SetClipperHWnd,
	ERR_SetClipper,
	ERR_CreateZBuffer,
	ERR_AttachZBuffer,
	ERR_CreateRenderBuffer,
	ERR_CreatePictureBuffer,
	ERR_D3D_Create,
	ERR_CreateDevice,
	ERR_CreateViewport,
	ERR_AddViewport,
	ERR_SetViewport2,
	ERR_SetCurrentViewport,
	ERR_ClearRenderBuffer,
	ERR_UpdateRenderInfo,
	ERR_GetThirdBuffer,
	ERR_GoFullScreen,
	ERR_GoWindowed,
	ERR_WrongBitDepth,
	ERR_GetPixelFormat,
	ERR_GetDisplayMode,
} ERROR_CODE;

typedef enum {
	GBUF_TempAlloc,
	GBUF_TexturePages,
	GBUF_MeshPointers,
	GBUF_Meshes,
	GBUF_Anims,
	GBUF_Structs,
	GBUF_Ranges,
	GBUF_Commands,
	GBUF_Bones,
	GBUF_Frames,
	GBUF_RoomTextures,
	GBUF_RoomInfos,
	GBUF_RoomMesh,
	GBUF_RoomMeshData,
	GBUF_RoomDoor,
	GBUF_RoomFloor,
	GBUF_RoomLights,
	GBUF_RoomStaticMeshInfos,
	GBUF_FloorData,
	GBUF_Items,
	GBUF_Cameras,
	GBUF_SoundFX,
	GBUF_Boxes,
	GBUF_Overlaps,
	GBUF_GroundZone,
	GBUF_FlyZone,
	GBUF_AnimatingTextureRanges,
	GBUF_CinematicFrames,
	GBUF_LoadDemoBuffer,
	GBUF_SaveDemoBuffer,
	GBUF_CinematicEffects,
	GBUF_MummyHeadTurn,
	GBUF_ExtraDoorstuff,
	GBUF_EffectsArray,
	GBUF_CreatureData,
	GBUF_CreatureLOT,
	GBUF_SampleInfos,
	GBUF_Samples,
	GBUF_SampleOffsets,
	GBUF_RollingBallStuff,
	GBUF_SkidooStuff,
	GBUF_LoadPiccyBuffer,
	GBUF_FMVBuffers,
	GBUF_PolygonBuffers,
	GBUF_OrderTables,
	GBUF_CLUTs,
	GBUF_TextureInfos,
	GBUF_SpriteInfos,
	GBUF_Max
} GAMEALLOC_BUFFER;

typedef enum {
	GSI_Heading_Inventory,
	GSI_Heading_Option,
	GSI_Heading_Items,
	GSI_Heading_GameOver,

	GSI_Passport_LoadGame,
	GSI_Passport_SaveGame,
	GSI_Passport_NewGame,
	GSI_Passport_RestartLevel,
	GSI_Passport_ExitToTitle,
	GSI_Passport_ExitDemo,
	GSI_Passport_ExitGame,
	GSI_Passport_SelectLevel,
	GSI_Passport_SavePosition,

	GSI_Detail_SelectDetail,
	GSI_Detail_High,
	GSI_Detail_Medium,
	GSI_Detail_Low,

	GSI_Keymap_Walk,
	GSI_Keymap_Roll,
	GSI_Keymap_Run,
	GSI_Keymap_Left,
	GSI_Keymap_Right,
	GSI_Keymap_Back,
	GSI_Keymap_StepLeft,
	GSI_Keymap_Reserved1,
	GSI_Keymap_StepRight,
	GSI_Keymap_Reserved2,
	GSI_Keymap_Look,
	GSI_Keymap_Jump,
	GSI_Keymap_Action,
	GSI_Keymap_DrawWeapon,
	GSI_Keymap_Reserved3,
	GSI_Keymap_Inventory,
	GSI_Keymap_Flare,
	GSI_Keymap_Step,

	GSI_InvItem_Statistics,
	GSI_InvItem_Pistols,
	GSI_InvItem_Shotgun,
	GSI_InvItem_Magnums,
	GSI_InvItem_Uzis,
	GSI_InvItem_Harpoon,
	GSI_InvItem_M16,
	GSI_InvItem_Grenade,
	GSI_InvItem_Flare,
	GSI_InvItem_PistolAmmo,
	GSI_InvItem_ShotgunAmmo,
	GSI_InvItem_MagnumAmmo,
	GSI_InvItem_UziAmmo,
	GSI_InvItem_HarpoonAmmo,
	GSI_InvItem_M16Ammo,
	GSI_InvItem_GrenadeAmmo,
	GSI_InvItem_SmallMedipack,
	GSI_InvItem_LargeMedipack,
	GSI_InvItem_Pickup,
	GSI_InvItem_Puzzle,
	GSI_InvItem_Key,
	GSI_InvItem_Game,
	GSI_InvItem_LaraHome,

	GSI_String_Loading,
	GSI_String_TimeTaken,
	GSI_String_SecretsFound,
	GSI_String_Location,
	GSI_String_Kills,
	GSI_String_AmmoUsed,
	GSI_String_Hits,
	GSI_String_SavesPerformed,
	GSI_String_DistanceTravelled,
	GSI_String_HealthPacksUsed,
	GSI_String_ReleaseVersion,
	GSI_String_None,
	GSI_String_Finish,
	GSI_String_BestTimes,
	GSI_String_NoTimesSet,
	GSI_String_NA,
	GSI_String_CurrentPosition,
	GSI_String_FinalStatistics,
	GSI_String_Of,
	GSI_String_StorySoFar,
} GAME_STRING_ID;

typedef enum {
	SSI_DetailLevels,
	SSI_DemoMode,
	SSI_Sound,
	SSI_Controls,
	SSI_Gamma,
	SSI_SetVolumes,
	SSI_UserKeys,
	SSI_SaveFileWarning,
	SSI_TryAgainQuestion,
	SSI_Yes,
	SSI_No,
	SSI_SaveComplete,
	SSI_NoSaveGames,
	SSI_NoneValid,
	SSI_SaveGameQuestion,
	SSI_EmptySlot,
	SSI_Off,
	SSI_On,
	SSI_SetupSoundCard,
	SSI_DefaultKeys,
	SSI_Dozy,
} SPECIFIC_STRING_ID;

typedef enum {
	ICLR_Black,
	ICLR_Gray,
	ICLR_White,
	ICLR_Red,
	ICLR_Orange,
	ICLR_Yellow,
	ICLR_Reserved1,
	ICLR_Reserved2,
	ICLR_Reserved3,
	ICLR_Reserved4,
	ICLR_Reserved5,
	ICLR_Reserved6,
	ICLR_DarkGreen,
	ICLR_Green,
	ICLR_Cyan,
	ICLR_Blue,
	ICLR_Magenta,
} INV_COLOURS;

typedef enum {
	RINGSTATE_OPENING,
	RINGSTATE_OPEN,
	RINGSTATE_CLOSING,
	RINGSTATE_MAIN2OPTION,
	RINGSTATE_MAIN2KEYS,
	RINGSTATE_KEYS2MAIN,
	RINGSTATE_OPTION2MAIN,
	RINGSTATE_SELECTING,
	RINGSTATE_SELECTED,
	RINGSTATE_DESELECTING,
	RINGSTATE_DESELECT,
	RINGSTATE_CLOSING_ITEM,
	RINGSTATE_EXITING_INVENTORY,
	RINGSTATE_DONE
} RING_STATES;

typedef enum {
	LWS_AboveWater,
	LWS_Underwater,
	LWS_Surface,
	LWS_Cheat,
	LWS_Wade,
} LARA_WATER_STATES;

typedef enum {
	RING_Main,
	RING_Option,
	RING_Keys,
} RING_TYPE;

typedef enum {
	DOOR_CLOSED,
	DOOR_OPEN
} DOOR_STATE;

typedef enum
{
	ZT_Skelly,
	ZT_Basic,
	ZT_Water,
	ZT_Human,
	ZT_Fly,
} ZONE_TYPES;

typedef enum : short
{
	LIT_None,
	LIT_Default,
	LIT_Reflection,
	LIT_Glow,
	LIT_Flicker,
	LIT_Sunset,
	LIT_Mist,
	LIT_MaxLightType
} LIGHT_TYPE;

typedef enum {
	GFL_NOLEVEL = -1,
	GFL_TITLE,
	GFL_NORMAL,
	GFL_SAVED,
	GFL_DEMO,
	GFL_CUTSCENE,
	GFL_STORY,
	GFL_QUIET,
	GFL_MIDSTORY,
} GF_LEVEL_TYPE;

typedef enum {
	GFE_PICTURE,
	GFE_LIST_START,
	GFE_LIST_END,
	GFE_PLAYFMV,
	GFE_STARTLEVEL,
	GFE_CUTSCENE,
	GFE_LEVCOMPLETE,
	GFE_DEMOPLAY,
	GFE_JUMPTO_SEQ,
	GFE_END_SEQ,
	GFE_SETTRACK,
	GFE_SUNSET,
	GFE_LOADINGPIC,
	GFE_DEADLY_WATER,
	GFE_REMOVE_WEAPONS,
	GFE_GAMECOMPLETE,
	GFE_CUTANGLE,
	GFE_NOFLOOR,
	GFE_ADD2INV,
	GFE_STARTANIM,
	GFE_NUMSECRETS,
	GFE_KILL2COMPLETE,
	GFE_REMOVE_AMMO,
} GF_EVENTS;

typedef enum {
	MOOD_BORED,
	MOOD_ATTACK,
	MOOD_ESCAPE,
	MOOD_STALK,
} MOOD_TYPE;

typedef enum {
	ITEM_INACTIVE,
	ITEM_ACTIVE,
	ITEM_DISABLED,
	ITEM_INVISIBLE,
} ITEM_STATUS;

typedef enum {
	ADDINV_PISTOL,
	ADDINV_SHOTGUN,
	ADDINV_MAGNUM,
	ADDINV_UZI,
	ADDINV_HARPOON,
	ADDINV_M16,
	ADDINV_GRENADE,
	ADDINV_PISTOL_AMMO,
	ADDINV_SHOTGUN_AMMO,
	ADDINV_MAGNUM_AMMO,
	ADDINV_UZI_AMMO,
	ADDINV_HARPOON_AMMO,
	ADDINV_M16_AMMO,
	ADDINV_GRENADE_AMMO,
	ADDINV_FLARE,
	ADDINV_SMALL_MEDIPACK,
	ADDINV_LARGE_MEDIPACK,
	ADDINV_PICKUP1,
	ADDINV_PICKUP2,
	ADDINV_PUZZLE1,
	ADDINV_PUZZLE2,
	ADDINV_PUZZLE3,
	ADDINV_PUZZLE4,
	ADDINV_KEY1,
	ADDINV_KEY2,
	ADDINV_KEY3,
	ADDINV_KEY4,
} ADDINV_TYPES;

typedef enum {
	CHAR_SECRET1 = 0x7F,

	CHAR_SECRET2,
	CHAR_SECRET3,
	CHAR_SECRET4,
} CHAR_SECRETS;

typedef enum {
	LM_Hips,
	LM_ThighL,
	LM_CalfL,
	LM_FootL,
	LM_ThighR,
	LM_CalfR,
	LM_FootR,
	LM_Torso,
	LM_UArmR,
	LM_LArmR,
	LM_HandR,
	LM_UArmL,
	LM_LArmL,
	LM_HandL,
	LM_Head
} LARA_MESHES;

typedef enum {
	HT_WALL,
	HT_SMALL_SLOPE,
	HT_BIG_SLOPE,
} HEIGHT_TYPE;

typedef enum {
	TT_TRIGGER,
	TT_PAD,
	TT_SWITCH,
	TT_KEY,
	TT_PICKUP,
	TT_HEAVY,
	TT_ANTIPAD,
	TT_COMBAT,
	TT_DUMMY,
	TT_ANTITRIGGER,
} TRIGGER_TYPE;

typedef enum
{
	TO_OBJECT,
	TO_CAMERA,
	TO_SINK,
	TO_FLIPMAP,
	TO_FLIPON,
	TO_FLIPOFF,
	TO_TARGET,
	TO_FINISH,
	TO_CD,
	TO_FLIPEFFECT,
	TO_SECRET,
	TO_BODYBAG
} TRIGOBJ_TYPES;

typedef enum {
	FT_FLOOR,
	FT_DOOR,
	FT_TILT,
	FT_ROOF,
	FT_TRIGGER,
	FT_LAVA,
	FT_CLIMB,
} FLOOR_TYPE;

typedef struct __unaligned __declspec(align(1)) SortItem_t {
	unsigned int _0;
	unsigned int _1;

} SORT_ITEM;

typedef struct __unaligned __declspec(align(1)) RGB888_t {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} RGB888;

typedef struct __unaligned __declspec(align(1)) GouraudFill_t {
	unsigned int clr[4][4];
} GOURAUD_FILL;

typedef struct __unaligned __declspec(align(1)) GouraudOutline_t {
	unsigned int clr[9];
} GOURAUD_OUTLINE;

typedef struct __unaligned __declspec(align(1)) DepthQEntry_t {
	unsigned char index[256];
} DEPTHQ_ENTRY;

typedef struct __unaligned __declspec(align(1)) GouraudEntry_t {
	unsigned char index[32];
} GOURAUD_ENTRY;

typedef struct __unaligned __declspec(align(1)) DisplayMode_t {
	int width;
	int height;
	int bpp;
	VGA_MODE vga;
} DISPLAY_MODE;

typedef struct __unaligned __declspec(align(1)) DisplayModeNode_t {
	struct DisplayModeNode_t* next;
	struct DisplayModeNode_t* previous;
	struct DisplayMode_t body;
} DISPLAY_MODE_NODE;

typedef struct __unaligned __declspec(align(1)) DisplayModeList_t {
	struct DisplayModeNode_t* head;
	struct DisplayModeNode_t* tail;
	unsigned int dwCount;
} DISPLAY_MODE_LIST;

typedef struct __unaligned __declspec(align(1)) StringFlagged_t {
	char * lpString;
	bool isPresented;
} STRING_FLAGGED;

typedef struct __unaligned __declspec(align(1)) DisplayAdapter_t {
	TR2_GUID * lpAdapterGuid;
	TR2_GUID adapterGuid;
	STRING_FLAGGED driverDescription;
	STRING_FLAGGED driverName;
	unsigned int index;
	TR2_D3DCAPS9 caps;
	DISPLAY_MODE_LIST hwDispModeList;
	DISPLAY_MODE_LIST swDispModeList;
	unsigned int screenWidth;
} DISPLAY_ADAPTER;

typedef struct __unaligned __declspec(align(1)) DisplayAdapterNode_t {
	struct DisplayAdapterNode_t* next;
	struct DisplayAdapterNode_t* previous;
	struct DisplayAdapter_t body;
} DISPLAY_ADAPTER_NODE;

typedef struct __unaligned __declspec(align(1)) DisplayAdapterList_t {
	struct DisplayAdapterNode_t* head;
	struct DisplayAdapterNode_t* tail;
	unsigned int dwCount;
} DISPLAY_ADAPTER_LIST;

typedef struct __unaligned __declspec(align(1)) SoundAdapter_t {
	TR2_GUID* lpAdapterGuid;
	TR2_GUID adapterGuid;
	STRING_FLAGGED description;
	STRING_FLAGGED module;
} SOUND_ADAPTER;

typedef struct __unaligned __declspec(align(1)) SoundAdapterNode_t {
	struct SoundAdapterNode_t* next;
	struct SoundAdapterNode_t* previous;
	SOUND_ADAPTER body;
} SOUND_ADAPTER_NODE;

typedef struct __unaligned __declspec(align(1)) SoundAdapterList_t {
	struct SoundAdapterNode_t* head;
	struct SoundAdapterNode_t* tail;
	unsigned int dwCount;
} SOUND_ADAPTER_LIST;

typedef struct __unaligned __declspec(align(1)) Joystick_t {
	TR2_GUID* lpJoystickGuid;
	TR2_GUID joystickGuid;
	STRING_FLAGGED productName;
	STRING_FLAGGED instanceName;

} JOYSTICK;

typedef struct __unaligned __declspec(align(1)) JoystickNode_t {
	struct JoystickNode_t* next;
	struct JoystickNode_t* previous;
	JOYSTICK body;
} JOYSTICK_NODE;

typedef struct __unaligned __declspec(align(1)) JoystickList_t {
	struct JoystickNode_t* head;
	struct JoystickNode_t* tail;
	unsigned int dwCount;
} JOYSTICK_LIST;

typedef struct __unaligned __declspec(align(1)) AppSettings_t {
	DISPLAY_ADAPTER_NODE* PreferredDisplayAdapter;
	SOUND_ADAPTER_NODE* PreferredSoundAdapter;
	JOYSTICK_NODE* PreferredJoystick;
	DISPLAY_MODE_NODE* VideoMode;
	RENDER_MODE RenderMode;
	int WindowWidth;
	int WindowHeight;
	ASPECT_MODE AspectMode;
	bool PerspectiveCorrect;
	bool Dither;
	bool ZBuffer;
	bool BilinearFiltering;
	bool TripleBuffering;
	bool FullScreen;
	bool SoundEnabled;
	bool LaraMic;
	bool JoystickEnabled;
	bool Disable16BitTextures;
	bool DontSortPrimitives;
	bool FlipBroken;
	bool DisableFMV;
	TEX_ADJUST_MODE TexelAdjustMode;
	int NearestAdjustment;
	int LinearAdjustment;

} APP_SETTINGS;

typedef struct __unaligned __declspec(align(1)) TexPageDesc_t {
	struct IDirect3DTexture9 * texture;
	int width;
	int height;
	int status;
} TEXPAGE_DESC;

typedef struct __unaligned __declspec(align(1)) FloorInfo_t {
	unsigned short index;
	short box;
	unsigned char pitRoom;
	char floor;
	unsigned char skyRoom;
	char ceiling;
} FLOOR_INFO;

typedef struct __unaligned __declspec(align(1)) PhdUV_t {
	unsigned short u;
	unsigned short v;
} PHD_UV;

typedef struct __unaligned __declspec(align(1)) PhdTexture_t {
	unsigned short drawtype;
	unsigned short tpage;
	PHD_UV uv[4];
} PHD_TEXTURE;

typedef struct __unaligned __declspec(align(1)) ColorBitMasks_t {
	unsigned int dwRBitMask;
	unsigned int dwGBitMask;
	unsigned int dwBBitMask;
	unsigned int dwRGBAlphaBitMask;
	unsigned int dwRBitDepth;
	unsigned int dwGBitDepth;
	unsigned int dwBBitDepth;
	unsigned int dwRGBAlphaBitDepth;
	unsigned int dwRBitOffset;
	unsigned int dwGBitOffset;
	unsigned int dwBBitOffset;
	unsigned int dwRGBAlphaBitOffset;
} COLOR_BIT_MASKS;

typedef struct __unaligned __declspec(align(1)) TextureFormat_t {
	unsigned int bpp;
} TEXTURE_FORMAT;

typedef struct __unaligned __declspec(align(1)) TextStrInfo_t {
	unsigned int flags;
	unsigned short textFlags;
	unsigned short bgndFlags;
	unsigned short outlFlags;
	short xPos;
	short yPos;
	short zPos;
	short letterSpacing;
	short wordSpacing;
	short flashRate;
	short flashCount;
	short bgndColor;
	GOURAUD_FILL* bgndGour;
	short outlColour;
	GOURAUD_OUTLINE* outlGour;
	short bgndSizeX;
	short bgndSizeY;
	short bgndOffX;
	short bgndOffY;
	short bgndOffZ;
	int scaleH;
	int scaleV;
	char* pString;
} TEXT_STR_INFO;

typedef struct __unaligned __declspec(align(1)) StringFixed4_t {
	char str[4];
} STRING_FIXED4;

typedef struct __unaligned __declspec(align(1)) StringFixed50_t {
	char str[50];
} STRING_FIXED50;

typedef struct __unaligned __declspec(align(1)) StringFixed64_t {
	char str[64];
} STRING_FIXED64;

typedef struct __unaligned __declspec(align(1)) PcxHeader_t {
	unsigned char manufacturer;
	unsigned char version;
	unsigned char rle;
	unsigned char bpp;
	unsigned short xMin;
	unsigned short yMin;
	unsigned short xMax;
	unsigned short yMax;
	unsigned short h_dpi;
	unsigned short v_dpi;
	RGB888 headerPalette[16];
	unsigned char reserved;
	unsigned char planes;
	unsigned short bytesPerLine;
	unsigned short palInterpret;
	unsigned short h_res;
	unsigned short v_res;
	unsigned char reservedData[54];
} PCX_HEADER;

typedef struct __unaligned __declspec(align(1)) TgaHeader_t {
	unsigned char idLength;
	unsigned char colorMapType;
	unsigned char dataTypeCode;
	unsigned short colorMapOrigin;
	unsigned short colorMapLength;
	unsigned char colorMapDepth;
	unsigned short xOrigin;
	unsigned short yOrigin;
	unsigned short width;
	unsigned short height;
	unsigned char bpp;
	unsigned char imageDescriptor;
} TGA_HEADER;

typedef struct __unaligned __declspec(align(1)) BitmapResource_t {
	struct tagBITMAPINFO * bmpInfo;
	void* bmpData;
	struct HPALETTE__ * hPalette;
	unsigned int flags;
} BITMAP_RESOURCE;

typedef struct __unaligned __declspec(align(1)) RoomLightTable_t {
	int table[32];
} ROOM_LIGHT_TABLE;

typedef struct __unaligned __declspec(align(1)) WavePcmHeader_t {
	unsigned int dwRiffChunkID;
	unsigned int dwRiffChunkSize;
	unsigned int dwFormat;
	unsigned int dwFmtSubchunkID;
	unsigned int dwFmtSubchunkSize;
	unsigned short wFormatTag;
	short nChannels;
	int nSamplesPerSec;
	int nAvgBytesPerSec;
	short nBlockAlign;
	unsigned short wBitsPerSample;
	unsigned int dwDataSubchunkID;
	unsigned int dwDataSubchunkSize;
} WAVEPCM_HEADER;

typedef struct __unaligned __declspec(align(1)) SampleInfo_t {
	unsigned short sampleIdx;
	short volume;
	short randomness;
	unsigned char radius;
	unsigned char pitch;
	unsigned char lutCount;

	unsigned short flags;
} SAMPLE_INFO;

typedef struct __unaligned __declspec(align(1)) SfxInfo_t {
	int volume;
	int pan;
	int sampleIdx;
	int freqFactor;
} SFX_INFO;

typedef struct __unaligned __declspec(align(1)) GameFlow_t {
	int firstOption;
	int titleReplace;
	int onDeath_DemoMode;
	int onDeath_InGame;
	int noInput_Time;
	int onDemo_Interrupt;
	int onDemo_End;
	unsigned short reserved1[18];
	unsigned short num_Levels;
	unsigned short num_Pictures;
	unsigned short num_Titles;
	unsigned short num_Fmvs;
	unsigned short num_Cutscenes;
	unsigned short num_Demos;
	unsigned short titleTrack;
	short singleLevel;
	unsigned short reserved2[16];
	unsigned short flags;
	unsigned short reserved3[3];
	unsigned char cypherCode;
	unsigned char language;
	unsigned char secretTrack;
	unsigned char levelCompleteTrack;
	unsigned short reserved4[2];
} GAME_FLOW;

typedef struct __unaligned __declspec(align(1)) RequestInfo_t {
	unsigned short reqFlags;
	unsigned short itemsCount;
	unsigned short selected;
	unsigned short visibleCount;
	unsigned short lineOffset;
	unsigned short lineOldOffset;
	unsigned short pixWidth;
	unsigned short lineHeight;
	short xPos;
	short yPos;
	short zPos;
	unsigned short itemStringLen;
	char* lpItemStrings1;
	char* lpItemStrings2;
	unsigned int* lpItemFlags1;
	unsigned int* lpItemFlags2;
	unsigned int headingFlags1;
	unsigned int headingFlags2;
	unsigned int backgroundFlags;
	unsigned int moreupFlags;
	unsigned int moredownFlags;
	unsigned int itemFlags1[24];
	unsigned int itemFlags2[24];
	TEXT_STR_INFO* headingText1;
	TEXT_STR_INFO* headingText2;
	TEXT_STR_INFO* backgroundText;
	TEXT_STR_INFO* moreupText;
	TEXT_STR_INFO* moredownText;
	TEXT_STR_INFO* itemTexts1[24];
	TEXT_STR_INFO* itemTexts2[24];
	char headingString1[32];
	char headingString2[32];
	unsigned int renderWidth;
	unsigned int renderHeight;
} REQUEST_INFO;

typedef struct __unaligned __declspec(align(1)) InventoryItem_t {
	char* lpString;
	short objectID;
	short framesTotal;
	short currentFrame;
	short goalFrame;
	short openFrame;
	short animDirection;
	short animSpeed;
	short animCount;
	short xRotPtSel;
	short xRotPt;
	short xRotSel;
	short xRot;
	short yRotSel;
	short yRot;
	short zRot;
	int yTransSel;
	int yTrans;
	int zTransSel;
	int zTrans;
	int meshesSel;
	int meshesDrawn;
	short invPos;
	void* sprites;
	int zRotExamine;
	int reserved1;
	int reserved2;
	int reserved3;
} INVENTORY_ITEM;

typedef struct __unaligned __declspec(align(1)) StatisticsInfo_t {
	unsigned int timer;
	unsigned int shots;
	unsigned int hits;
	unsigned int distance;
	unsigned short kills;
	unsigned char secrets;
	unsigned char mediPacks;
} STATISTICS_INFO;

typedef struct __unaligned __declspec(align(1)) StartInfo_t {
	unsigned short pistolAmmo;
	unsigned short autopistolAmmo;
	unsigned short uziAmmo;
	unsigned short shotgunAmmo;
	unsigned short m16Ammo;
	unsigned short grenadeAmmo;
	unsigned short harpoonAmmo;
	unsigned char smallMedipacks;
	unsigned char largeMedipacks;
	unsigned char unused;
	unsigned char flares;
	unsigned char gunStatus;
	unsigned char gunType;
	unsigned short available : 1;
	unsigned short has_pistols : 1;
	unsigned short has_autopistol : 1;
	unsigned short has_uzis : 1;
	unsigned short has_shotgun : 1;
	unsigned short has_m16 : 1;
	unsigned short has_grenade : 1;
	unsigned short has_harpoon : 1;
	unsigned short pad : 8;
	unsigned short reserved;
	STATISTICS_INFO statistics;
} START_INFO;

typedef struct __unaligned __declspec(align(1)) SaveGame_t {
	START_INFO start[24];
	STATISTICS_INFO statistics;
	short currentLevel;
	bool bonusFlag;
	unsigned char numPickup[2];
	unsigned char numPuzzle[4];
	unsigned char numKey[4];
	unsigned short reserved;
	unsigned char buffer[16512];
} SAVEGAME_INFO;

typedef struct __unaligned __declspec(align(1)) Pos2D_t {
	short x;
	short y;
} POS_2D;

typedef struct __unaligned __declspec(align(1)) Pos3D_t {
	short x;
	short y;
	short z;
} POS_3D;

typedef struct __unaligned __declspec(align(1)) PhdVector_t {
	int x;
	int y;
	int z;
} PHD_VECTOR;

typedef struct __unaligned __declspec(align(1)) GameVector_t {
	int x;
	int y;
	int z;
	short roomNumber;
	short boxNumber;
} GAME_VECTOR;

typedef struct __unaligned __declspec(align(1)) ObjectVector_t {
	int x;
	int y;
	int z;
	unsigned short data;
	unsigned short flags;
} OBJECT_VECTOR;

typedef struct __unaligned __declspec(align(1)) VectorAngles_t {
	short rotY;
	short rotX;
} VECTOR_ANGLES;

typedef struct __unaligned __declspec(align(1)) Phd3dPos_t {
	int x;
	int y;
	int z;
	short rotX;
	short rotY;
	short rotZ;
} PHD_3DPOS;

typedef struct __unaligned __declspec(align(1)) SoundSourceInfo_t {
	int x;
	int y;
	int z;
	unsigned short soundID;
	unsigned short flags;
} SOUND_SOURCE_INFO;

typedef struct __unaligned __declspec(align(1)) ItemInfo_t {
	int floor;
	unsigned int touchBits;
	unsigned int meshBits;
	unsigned short objectID;
	short currentAnimState;
	short goalAnimState;
	short requiredAnimState;
	short animNumber;
	short frameNumber;
	short roomNumber;
	short nextItem;
	short nextActive;
	short speed;
	short fallSpeed;
	short hitPoints;
	unsigned short boxNumber;
	short timer;
	unsigned short flags;
	short shade1;
	short ocb;
	short carriedItem;
	void * data;
	PHD_3DPOS pos;
	unsigned short active : 1;
	unsigned short status : 2;
	unsigned short gravity : 1;
	unsigned short hitStatus : 1;
	unsigned short collidable : 1;
	unsigned short looked_at : 1;
	unsigned short dynamic_light : 1;
	unsigned short clear_body : 1;
} ITEM_INFO;

typedef struct __unaligned __declspec(align(1)) CameraInfo_t {
	GAME_VECTOR pos;
	GAME_VECTOR target;
	CAMERA_TYPE type;
	int shift;
	unsigned int flags;
	int fixedCamera;
	int numberFrames;
	int bounce;
	int underwater;
	int targetDistance;
	int targetSquare;
	short targetAngle;
	short actualAngle;
	short targetElevation;
	short box;
	short number;
	short last;
	short timer;
	short speed;
	ITEM_INFO* item;
	ITEM_INFO* last_item;
	OBJECT_VECTOR* fixed;
	int isLaraMic;
	PHD_VECTOR micPos;
} CAMERA_INFO;

typedef struct __unaligned __declspec(align(1)) CollSide_t {
	int floor;
	int ceiling;
	int type;
} COLL_SIDE;

enum COLL_FLAGS
{
	CF_SLOPE_ARE_WALLS = 1 << 0,
	CF_SLOPE_ARE_PITS = 1 << 1,
	CF_LAVA_IS_PIT = 1 << 2,
	CF_ENABLE_BADDIE_PUSH = 1 << 3,
	CF_ENABLE_SPAZ = 1 << 4
};

typedef struct __unaligned __declspec(align(1)) CollInfo_t {
	COLL_SIDE middle;
	COLL_SIDE front;
	COLL_SIDE left;
	COLL_SIDE right;
	int radius;
	int badPos;
	int badNeg;
	int badCeiling;
	PHD_VECTOR shift;
	PHD_VECTOR old;
	short oldAnimState;
	short oldAnimNumber;
	short oldFrameNumber;
	short facing;
	short quadrant;
	short collType;
	short* trigger;
	char xTilt;
	char zTilt;
	char hitByBaddie;
	char hitStatic;
	unsigned short flags;
} COLL_INFO;

typedef struct __unaligned __declspec(align(1)) ObjectInfo_t {
	short nMeshes;
	short meshIndex;
	int boneIndex;
	short* frameBase;
	void(__cdecl* initialise)(short itemNumber);
	void(__cdecl* control)(short itemNumber);
	void(__cdecl* floor)(ITEM_INFO* item, int x, int y, int z, int* height);
	void(__cdecl* ceiling)(ITEM_INFO* item, int x, int y, int z, int* height);
	void(__cdecl* drawRoutine)(ITEM_INFO* item);
	void(__cdecl* collision)(short itemNumber, ITEM_INFO* laraItem, COLL_INFO* coll);
	short animIndex;
	short hitPoints;
	short pivotLength;
	short radius;
	short shadowSize;
	unsigned short loaded : 1;
	unsigned short intelligent : 1;
	unsigned short save_position : 1;
	unsigned short save_hitpoints : 1;
	unsigned short save_flags : 1;
	unsigned short save_anim : 1;
	unsigned short semi_transparent : 1;
	unsigned short water_creature : 1;
	unsigned short pad : 8;
} OBJECT_INFO;

typedef struct __unaligned __declspec(align(1)) PhdMatrix_t {
	int _00, _01, _02, _03;
	int _10, _11, _12, _13;
	int _20, _21, _22, _23;
} PHD_MATRIX;

typedef struct __unaligned __declspec(align(1)) DoorPosData_t {
	FLOOR_INFO* floor;
	FLOOR_INFO data;
	short boxNumber;
} DOORPOS_DATA;

typedef struct __unaligned __declspec(align(1)) DoorData_t {
	DOORPOS_DATA d1;
	DOORPOS_DATA d1Flip;
	DOORPOS_DATA d2;
	DOORPOS_DATA d2Flip;
} DOOR_DATA;

typedef struct __unaligned __declspec(align(1)) DoorInfo_t {
	short room;
	short x;
	short y;
	short z;
	POS_3D vertex[4];
} DOOR_INFO;

typedef struct __unaligned __declspec(align(1)) DoorInfos_t {
	short wCount;
	DOOR_INFO door[];
} DOOR_INFOS;

typedef struct __unaligned __declspec(align(1)) LightInfo_t {
	int x;
	int y;
	int z;
	short intensity;
	int fallOff;

} LIGHT_INFO;

typedef struct __unaligned __declspec(align(1)) MeshInfo_t {
	int x;
	int y;
	int z;
	short yRot;
	short shade1;
	short shade2;
	short staticNumber;
} MESH_INFO;

typedef struct __unaligned __declspec(align(1)) RoomVertex_t {
	short x;
	short y;
	short z;
	short lightBase;
	unsigned char lightTableValue;
	unsigned char flags;
	short lightAdder;
} ROOM_VERTEX;

typedef struct __unaligned __declspec(align(1)) Face4_t {
	short vertices[4];
	short texture;
} FACE4;

typedef struct __unaligned __declspec(align(1)) Face3_t {
	short vertices[3];
	short texture;
} FACE3;

typedef struct __unaligned __declspec(align(1)) RoomSprite_t {
	short vertex;
	short spriteIndex;
} ROOM_SPRITE;

typedef struct __unaligned __declspec(align(1)) RoomData_t {
	unsigned short vtxSize;
	ROOM_VERTEX* vertices;
	unsigned short gt4Size;
	FACE4* gt4;
	unsigned short gt3Size;
	FACE3* gt3;
	unsigned short spriteSize;
	ROOM_SPRITE* sprites;
} ROOM_DATA;

typedef struct __unaligned __declspec(align(1)) RoomInfo_t {
	short index;
	ROOM_DATA* data;
	DOOR_INFOS* doors;
	FLOOR_INFO* floor;
	LIGHT_INFO* light;
	MESH_INFO* meshList;
	unsigned int flags;
	int x;
	int y;
	int z;
	int minFloor;
	int maxCeiling;
	short zSize;
	short xSize;
	short ambient;
	short ambient2;
	LIGHT_TYPE lightMode;
	short numLights;
	short numMeshes;
	short boundLeft;
	short boundRight;
	short boundTop;
	short boundBottom;
	unsigned short boundActive;
	short left;
	short right;
	short top;
	short bottom;
	short itemNumber;
	short fxNumber;
	short flippedRoom;
	unsigned char reverbType;
} ROOM_INFO;

typedef struct __unaligned __declspec(align(1)) LiftData_t {
	int oldY;
	int timer;
} LIFT_DATA;

typedef struct __unaligned __declspec(align(1)) PhdVBuf_t {
	float xv;
	float yv;
	float zv;
	float rhw;
	float xs;
	float ys;
	short clip;
	short g;
	short u;
	short v;
} PHD_VBUF;

typedef struct __unaligned __declspec(align(1)) PointInfo_t {
	float xv;
	float yv;
	float zv;
	float rhw;
	float xs;
	float ys;
	float u;
	float v;
	float g;
} POINT_INFO;

typedef struct __unaligned __declspec(align(1)) VertexInfo_t {
	float x;
	float y;
	float rhw;
	float u;
	float v;
	float g;
} VERTEX_INFO;

typedef struct __unaligned __declspec(align(1)) AnimStruct_t {
	short* framePtr;
	short interpolation;
	short currentAnimState;
	int velocity;
	int acceleration;
	short frameBase;
	short frameEnd;
	short jumpAnimNum;
	short jumpFrameNum;
	short numberChanges;
	short changeIndex;
	short numberCommands;
	short commandIndex;
} ANIM_STRUCT;

typedef struct __unaligned __declspec(align(1)) ChangeStruct_t {
	short goalAnimState;
	short numberRanges;
	short rangeIndex;
} CHANGE_STRUCT;

typedef struct __unaligned __declspec(align(1)) RangeStruct_t {
	short startFrame;
	short endFrame;
	short linkAnimNum;
	short linkFrameNum;
} RANGE_STRUCT;

typedef struct __unaligned __declspec(align(1)) StaticBounds_t {
	short xMin;
	short xMax;
	short yMin;
	short yMax;
	short zMin;
	short zMax;
} STATIC_BOUNDS;

typedef struct __unaligned __declspec(align(1)) StaticInfo_t {
	short meshIndex;
	unsigned short flags;
	STATIC_BOUNDS drawBounds;
	STATIC_BOUNDS collisionBounds;
} STATIC_INFO;

typedef struct __unaligned __declspec(align(1)) PhdSprite_t {
	unsigned short texPage;
	unsigned short offset;
	unsigned short width;
	unsigned short height;
	short x1;
	short y1;
	short x2;
	short y2;
} PHD_SPRITE;

typedef struct __unaligned __declspec(align(1)) BoxInfo_t {
	unsigned char left;
	unsigned char right;
	unsigned char top;
	unsigned char bottom;
	short height;
	unsigned short overlapIndex;
} BOX_INFO;

typedef struct __unaligned __declspec(align(1)) CineFrameInfo_t {
	short xTarget;
	short yTarget;
	short zTarget;
	short zPos;
	short yPos;
	short xPos;
	unsigned short fov;
	unsigned short roll;
} CINE_FRAME_INFO;

typedef struct __unaligned __declspec(align(1)) ControlLayout_t {
	unsigned short key[14];

} CONTROL_LAYOUT;

typedef struct __unaligned __declspec(align(1)) InventorySprite_t {
	short shape;
	short x;
	short y;
	short z;
	int param1;
	int param2;
	void * gour;
	unsigned short invColour;
} INVENTORY_SPRITE;

typedef struct __unaligned __declspec(align(1)) AssaultStats_t {
	unsigned int bestTime[10];
	unsigned int bestFinish[10];
	unsigned int finishCount;
} ASSAULT_STATS;

typedef struct __unaligned __declspec(align(1)) PickupInfo_t {
	short timer;
	short sprite;
} PICKUP_INFO;

typedef struct __unaligned __declspec(align(1)) InvMotionInfo_t {
	short framesCount;
	short status;
	short statusTarget;
	short radiusTarget;
	short radiusRate;
	short cameraTarget_y;
	short cameraRate_y;
	short cameraTarget_pitch;
	short cameraRate_pitch;
	short rotateTarget;
	short rotateRate;
	short itemTarget_xRotPt;
	short itemRate_xRotPt;
	short itemTarget_xRot;
	short itemRate_xRot;
	int itemTarget_yTrans;
	int itemRate_yTrans;
	int itemTarget_zTrans;
	int itemRate_zTrans;
	int misc;
} INV_MOTION_INFO;

typedef struct __unaligned __declspec(align(1)) RingInfo_t {
	INVENTORY_ITEM** itemList;
	short type;
	short radius;
	short cameraPitch;
	short isRotating;
	short rotCount;
	short currentObj;
	short targetObj;
	short objCount;
	short angleAdder;
	short rotAdder;
	short rotAdderL;
	short rotAdderR;
	PHD_3DPOS ringPos;
	PHD_3DPOS camera;
	PHD_VECTOR light;
	INV_MOTION_INFO* motionInfo;
} RING_INFO;

typedef struct __unaligned __declspec(align(1)) BoxNode_t {
	unsigned short exitBox;
	unsigned short searchNumber;
	unsigned short nextExpansion;
	unsigned short boxNumber;
} BOX_NODE;

typedef struct __unaligned __declspec(align(1)) LotInfo_t {
	BOX_NODE* node;
	unsigned short head;
	unsigned short tail;
	unsigned short searchNumber;
	unsigned short blockMask;
	short step;
	short drop;
	short fly;
	short zoneCount;
	unsigned short targetBox;
	unsigned short requiredBox;
	PHD_VECTOR target;
} LOT_INFO;

typedef struct __unaligned __declspec(align(1)) FxInfo_t {
	PHD_3DPOS pos;
	unsigned int color;
	short roomNumber;
	short objectID;
	short nextFx;
	short nextActive;
	short speed;
	short fallspeed;
	short frameNumber;
	short counter;
	short shade;
	short scale;
} FX_INFO;

typedef struct __unaligned __declspec(align(1)) CreatureInfo_t {
	short headRotation;
	short neckRotation;
	short maximumTurn;
	unsigned short flags;
	short itemNumber;
	MOOD_TYPE mood;
	LOT_INFO LOT;
	PHD_VECTOR target;
	ITEM_INFO* enemy;
} CREATURE_INFO;

typedef struct __unaligned __declspec(align(1)) BoatInfo_t {
	int turn;
	int leftFallspeed;
	int rightFallspeed;
	short tiltAngle;
	short extraRotation;
	int water;
	int pitch;
} BOAT_INFO;

typedef struct __unaligned __declspec(align(1)) CandleEmitterData_t {
	short leftID;
	short middleID;
	short rightID;
	bool on;
} CANDLE_EMITTER_DATA;

typedef struct __unaligned __declspec(align(1)) SkidooInfo_t {
	short trackMesh;
	int turn;
	int leftFallspeed;
	int rightFallspeed;
	short tiltAngle;
	short extraRotation;
	int pitch;
} SKIDOO_INFO;

typedef struct __unaligned __declspec(align(1)) LaraArm_t {
	short* frame_base;
	short frame_number;
	short anim_number;
	short lock;
	short y_rot;
	short x_rot;
	short z_rot;
	short flash_gun;
} LARA_ARM;

typedef struct __unaligned __declspec(align(1)) LaraInfo_t {
	short item_number;
	short gun_status;
	short gun_type;
	short request_gun_type;
	short last_gun_type;
	short calc_fallspeed;
	short water_status;
	short climb_status;
	short pose_count;
	short hit_frame;
	short hit_direction;
	short air;
	short dive_count;
	short death_count;
	short current_active;
	short spaz_effect_count;
	short flare_age;
	short skidoo;
	short weapon_item;
	short back_gun;
	short flare_frame;
	unsigned short flare_control_left : 1;
	unsigned short flare_control_right : 1;
	unsigned short extra_anim : 1;
	unsigned short look : 1;
	unsigned short burn : 1;
	unsigned short keep_ducked : 1;
	unsigned short can_monkey_swing : 1;
	unsigned short pad : 9;
	int water_surface_dist;
	PHD_VECTOR last_pos;
	FX_INFO* spaz_effect;
	unsigned int mesh_effects;
	short* mesh_ptrs[15];
	ITEM_INFO* target;
	short target_angles[2];
	short turn_rate;
	short move_angle;
	short head_y_rot;
	short head_x_rot;
	short head_z_rot;
	short torso_y_rot;
	short torso_x_rot;
	short torso_z_rot;
	LARA_ARM left_arm;
	LARA_ARM right_arm;
	unsigned int pistol_ammo;
	unsigned int magnum_ammo;
	unsigned int uzi_ammo;
	unsigned int shotgun_ammo;
	unsigned int harpoon_ammo;
	unsigned int grenade_ammo;
	unsigned int m16_ammo;
	CREATURE_INFO* creature;
} LARA_INFO;

typedef struct __unaligned __declspec(align(1)) BiteInfo_t {
	int x;
	int y;
	int z;
	int meshIndex;
} BITE_INFO;

typedef struct __unaligned __declspec(align(1)) AIInfo_t {
	unsigned short zoneNumber;
	unsigned short enemyZone;
	int distance;
	int ahead;
	int bite;
	short angle;
	short enemyFacing;
} AI_INFO;

typedef struct __unaligned __declspec(align(1)) Sphere_t {
	int x;
	int y;
	int z;
	int radius;
} SPHERE_INFO;

typedef struct __unaligned __declspec(align(1)) SoundSlot_t {
	PHD_VECTOR pos;
	int volume;
	int originalVolume;
	int pan;
	int sampleIdx;
	int pitch;
	int distance;
} SOUND_SLOT;

typedef struct __unaligned __declspec(align(1)) WeaponInfo_t {
	short lockAngles[4];
	short leftAngles[4];
	short rightAngles[4];
	short aimSpeed;
	short shotAccuracy;
	int gunHeight;
	int damage;
	int targetDist;
	short recoilFrame;
	short flashTime;
	short sampleNum;
} WEAPON_INFO;

typedef struct __unaligned __declspec(align(1)) { short idx; short num; } POLYINDEX;

typedef struct __unaligned __declspec(align(1)) TexPageConfig_t {
	bool isLoaded;
	bool isLegacyColors;
	double adjustment;

} TEXPAGES_CONFIG;

typedef struct __unaligned __declspec(align(1)) Polyfilter_t {
	short n_vtx, n_gt4, n_gt3, n_g4, n_g3;
	POLYINDEX gt4[256];
	POLYINDEX gt3[256];
	POLYINDEX g4[256];
	POLYINDEX g3[256];
} POLYFILTER;

typedef struct __unaligned __declspec(align(1)) PolyfilterNode_t {
	int id;
	POLYFILTER filter;
	struct PolyfilterNode_t* next;
} POLYFILTER_NODE;

typedef struct __unaligned __declspec(align(1)) BarConfig_t {
	bool isCentered;
	bool basedOnEnemyHealth;

	int PC_xpos;
	int PC_ypos;
	INV_COLOURS PC_color[2];

	int PSX_xpos;
	int PSX_ypos;
	int CENTER_xpos;
	int CENTER_ypos;
	unsigned int PSX_leftcolor[6];
	unsigned int PSX_rightcolor[6];
	unsigned int PSX_framecolor[6];
} BAR_CONFIG;

typedef struct __unaligned __declspec(align(1)) SemiTransConfig_t {
	bool isLoaded;
	POLYINDEX* animtex;
	POLYFILTER_NODE* rooms;
	POLYFILTER_NODE* statics;
	POLYFILTER_NODE* objects[ID_NUMBER_OBJECTS];
} SEMITRANS_CONFIG;

typedef struct __unaligned __declspec(align(1)) ReflectConfig_t {
	bool isLoaded;
	POLYFILTER_NODE* statics;
	POLYFILTER_NODE* objects[ID_NUMBER_OBJECTS];
} REFLECT_CONFIG;

typedef struct __unaligned __declspec(align(1)) LaraBarConfig_t {
	BAR_CONFIG health;
	BAR_CONFIG air;
} LARA_BAR_CONFIG;

typedef struct __unaligned __declspec(align(1)) EnemyHealthInfo_t {
	short dog;
	short mouse;
	short cult1;
	short cult1A;
	short cult1B;
	short cult2;
	short shark;
	short tiger;
	short barracuda;
	short smallSpider;
	short wolf;
	short bigSpider;
	short bear;
	short yeti;
	short jelly;
	short diver;
	short worker1;
	short worker2;
	short worker3;
	short worker4;
	short worker5;
	short cult3;
	short monk1;
	short monk2;
	short eagle;
	short crow;
	short bigEel;
	short eel;
	short bandit1;
	short bandit2;
	short bandit2B;
	short skidman;
	short xianLord;
	short warrior;
	short dragon;
	short giantYeti;
	short dino;
} ENEMY_HEALTH_INFO;

typedef struct __unaligned __declspec(align(1)) CustUnderwaterInfo_t {
	short noAirDamagePerTick;
	short restoreAirPerTick;
	short maxAir;
	bool unlimitedAir;
} CUST_UNDERWATER_INFO;

typedef enum OrientAxis_e
{
	NORTH = 0,
	EAST,
	SOUTH,
	WEST
} ORIENT_AXIS;
#pragma pack(pop)
