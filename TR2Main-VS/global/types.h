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

#ifndef GLOBAL_TYPES_H_INCLUDED
#define GLOBAL_TYPES_H_INCLUDED

 /*
  * Inject macro
  */

#pragma pack(push, 1)
typedef struct {
	BYTE opCode;	// must be 0xE9;
	DWORD offset;	// jump offset
} JMP;
#pragma pack(pop)

#define INJECT(from,to) { \
	((JMP*)(from))->opCode = 0xE9; \
	((JMP*)(from))->offset = (DWORD)(to) - ((DWORD)(from) + sizeof(JMP)); \
}

#ifdef _DEBUG
#define TRACE(func,line) { \
	LogDebug("%s: line %d\n", func, line); \
}
#else
#define TRACE(func,line)
#endif

/*
 * Defined values
 */


 // Filter is presented by an array of poly index and polys number (starting from the index).
 // The filter must be always terminated by an index 0.
 // If the first item has index=~0 then there are no polys of such type to process.
 // If the first item has index=0 and number=0 then all polys of such type must be processed.
#define POLYFILTER_SIZE 256
#define TEXPAGE_CONFIG_NAME "textures/texpages/config.json"

 // General values
#define REQ_LEVEL_VERSION	(45)
#define ARRAY_SIZE(a) _countof(a)
#define NUMBER_ITEMS 1024
#define MAX_SAVEGAME_BUFFER(bufferSize) ((bufferSize * 1024) + 128) // bufferSize in KB

// FPS values
#define FRAMES_PER_SECOND	(30)
#define TICKS_PER_FRAME		(2)
#define TICKS_PER_SECOND	(TICKS_PER_FRAME * FRAMES_PER_SECOND)

// Formula values
#define W2V_SHIFT			(14) // World to View shift value
#define W2V_SCALE			(1<<W2V_SHIFT) // World to View scale value
#define PHD_ONE				(0x10000) // unsigned short int equivalent of 1.0
#define PHD_IONE			(PHD_ONE/4) // signed short int equivalent of 1.0
#define PHD_HALF			(0x100) // half size of PHD_ONE

// Flag check macros
#define CHK_ALL(a,b)	(((a)&(b))==(b))
#define CHK_ANY(a,b)	(((a)&(b))!=0)
#define CHK_NOP(a,b)    (((a)&(b))!=b)

// Geometry values
#define WALL_SIZE           (1024)
#define CLICK_SIZE          (256)
#define WALL_SHIFT			(10)
#define NO_HEIGHT			(-0x7F00)

// AI values
#define HP_DONT_TARGET		(-0x4000) // Lara wont target creature with this values in hitPoints.
#define RANDOM_SEED			(0xD371F947)
#define SUNSET_TIMEOUT		(TICKS_PER_SECOND*60*20) // sunset sets in 20 minutes

// Angle values
#define PHD_360				(PHD_ONE)
#define PHD_180				(PHD_ONE/2)
#define PHD_90				(PHD_ONE/4)
#define PHD_45				(PHD_ONE/8)
#define PHD_270				(PHD_90*3)
#define PHD_DEGREE			(PHD_ONE/360)
#define ANGLE(x)            ((x) * PHD_DEGREE)
#define FROM_ANGLE(x)       ((x) / PHD_DEGREE)

// String macros
#ifdef FEATURE_HUD_IMPROVED
#define K(a) "\x7F" a "\x1F"
#endif // FEATURE_HUD_IMPROVED

// Math macros
#define MIN(a,b)			      (((a)<(b))?(a):(b))
#define MAX(a,b)			      (((a)>(b))?(a):(b))
#define ABS(a)				      (((a)<0)?-(a):(a))
#define SQR(a)				      ((a)*(a))
#define CLAMPL(a,b)			      {if((a)<(b)) (a)=(b);}
#define CLAMPG(a,b)			      {if((a)>(b)) (a)=(b);}
#define CLAMP(a,b,c)		      {if((a)<(b)) (a)=(b); else if((a)>(c)) (a)=(c);}
#define SWAP(a,b,c)			      {(c)=(a); (a)=(b); (b)=(c);}
#define	TRIGMULT2(a,b)		      (((a)*(b))>>W2V_SHIFT)
#define	TRIGMULT3(a,b,c)	      (TRIGMULT2((TRIGMULT2(a,b)),c))
#define	VBUF_VISIBLE(a,b,c)	      ((c.xs - b.xs) * (a.ys - b.ys) - (a.xs - b.xs) * (c.ys - b.ys) > 0)
#define MESHBITS_EXIST(x, value)  (x & 1 << value)
#define MESHBITS_REMOVE(x, value) (x &= ~(1 << value))
#define MESHBITS_GET(x)           (1 << x)

// Distance values
#define BLOCK(x)			((x)*1024)
#define CLICK(x)			((x)*256)
#define HALF_CLICK(x)       ((x)*128)
#define SQR_BLOCK(x)		SQR(BLOCK(x))
#define SQR_CLICK(x)		SQR(CLICK(x))

// Mesh macros
#define MESH_BITS(x) (1 << x)

// Fast conversion macros
#define BYTEn(a,b)			(*((BYTE*)&(a)+b))
#define BYTE0(a)			(LOBYTE(a))
#define BYTE1(a)			(BYTEn(a,1))
#define BYTE2(a)			(BYTEn(a,2))
#define BYTE3(a)			(BYTEn(a,3))

// View distance values
#define VIEW_NEAR			(0x14 * 0x001)
#define VIEW_FAR			(0x14 * 0x400)

// DepthQ fog values
#define DEPTHQ_END			(VIEW_FAR)
#define DEPTHQ_RANGE		(DEPTHQ_END * 2/5)
#define DEPTHQ_START		(DEPTHQ_END - DEPTHQ_RANGE)

// Water effect table parameters
#define WIBBLE_SIZE			(32)
#define MAX_WIBBLE			(2)
#define MAX_SHADE			(0x300)
#define MAX_ROOMLIGHT_UNIT	(0x2000 / (WIBBLE_SIZE/2))

// SW Renderer Detail Settings
#define SW_DETAIL_LOW		(0 * 0x400 * W2V_SCALE)
#define SW_DETAIL_MEDIUM	(3 * 0x400 * W2V_SCALE)
#define SW_DETAIL_HIGH		(6 * 0x400 * W2V_SCALE)
#define SW_DETAIL_ULTRA		(20* 0x400 * W2V_SCALE)

// ClearBuffers flags
#define CLRB_PrimaryBuffer			(0x0001)
#define CLRB_BackBuffer				(0x0002)
#define CLRB_ThirdBuffer			(0x0004)
#define CLRB_ZBuffer				(0x0008)
#define CLRB_RenderBuffer			(0x0010)
#define CLRB_PictureBuffer			(0x0020)
#define CLRB_WindowedPrimaryBuffer	(0x0040)
#define CLRB_Reserved				(0x0080)
#define CLRB_PhdWinSize				(0x0100)

// TextStrInfo flags
#define TIF_Active					(0x0001)
#define TIF_Flash					(0x0002)
#define TIF_RotateH					(0x0004)
#define TIF_RotateV					(0x0008)
#define TIF_CentreH					(0x0010)
#define TIF_CentreV					(0x0020)
#define TIF_Hide					(0x0040)
#define TIF_Right					(0x0080)
#define TIF_Bottom					(0x0100)
#define TIF_Bgnd					(0x0200)
#define TIF_Outline					(0x0400)
#define TIF_Multiline				(0x0800)

// GameFlow flags
#define GFF_DemoVersion				(0x0001)
#define GFF_TitleDisabled			(0x0002)
#define GFF_CheatModeCheckDisabled	(0x0004)
#define GFF_NoInputTimeout			(0x0008)
#define GFF_LoadSaveDisabled		(0x0010)
#define GFF_ScreenSizingDisabled	(0x0020)
#define GFF_LockoutOptionRing		(0x0040)
#define GFF_DozyCheatEnabled		(0x0080)
#define GFF_UseSecurityTag			(0x0100)
#define GFF_GymEnabled				(0x0200)
#define GFF_SelectAnyLevel			(0x0400)
#define GFF_EnableCheatCode			(0x0800)

// Input Status Flags
#define IN_FORWARD			(0x00000001)
#define IN_BACK				(0x00000002)
#define IN_LEFT				(0x00000004)
#define IN_RIGHT			(0x00000008)
#define IN_JUMP				(0x00000010)
#define IN_DRAW				(0x00000020)
#define IN_ACTION			(0x00000040)
#define IN_SLOW				(0x00000080)
#define IN_OPTION			(0x00000100)
#define IN_LOOK				(0x00000200)
#define IN_STEPL			(0x00000400)
#define IN_STEPR			(0x00000800)
#define IN_ROLL				(0x00001000)
#define IN_PAUSE			(0x00002000)
#define IN_RESERVED1		(0x00004000)
#define IN_RESERVED2		(0x00008000)
#define IN_DOZYCHEAT		(0x00010000)
#define IN_STUFFCHEAT		(0x00020000)
#define IN_DEBUGINFO		(0x00040000)
#define IN_FLARE			(0x00080000)
#define IN_SELECT			(0x00100000)
#define IN_DESELECT			(0x00200000)
#define IN_SAVE				(0x00400000)
#define IN_LOAD				(0x00800000)

// Gameflow directions
#define GF_START_GAME		(0x0000)
#define GF_START_SAVEDGAME	(0x0100)
#define GF_START_CINE		(0x0200)
#define GF_START_FMV		(0x0300)
#define GF_START_DEMO		(0x0400)
#define GF_EXIT_TO_TITLE	(0x0500)
#define GF_LEVEL_COMPLETE	(0x0600)
#define GF_EXIT_GAME		(0x0700)
#define GF_EXIT_TO_OPTION	(0x0800)
#define GF_TITLE_DESELECT	(0x0900)
#define GF_ERROR			(-1)

// Room flags
#define NO_ROOM							(255)
#define ROOM_UNDERWATER					(0x0001)
#define ROOM_HORIZON					(0x0008)
#define ROOM_DAMAGE						(0x0010)
#define ROOM_OUTSIDE					(0x0020)
#define ROOM_INSIDE						(0x0040)
#define ROOM_QUICKSAND					(0x0080)
#define ROOM_EFFECT						(0x0100) // Room glow and movement flags, Light mode should be 2 !
#define ROOM_REFLECTION					(0x0200) // Light mode should be 2 !
#define ROOM_SNOW                       (0x0400)
#define ROOM_RAIN                       (0x0800)
#define ROOM_FLIP                       (0x1000) // Does this room is a flipped one ?

// SFX flags
#define SFX_UNDERWATER		(1)
#define SFX_ALWAYS			(2)
#define SFX_PITCH           (4)

// SFX flags for animation command.
#define SFX_LANDANDWATER    (0x0000)
#define SFX_LANDONLY        (0x4000)
#define SFX_WATERONLY       (0x8000)

// Sprite flags
#define SPR_RGB			(0x00FFFFFF)
#define SPR_ABS			(0x01000000)
#define SPR_SEMITRANS	(0x02000000)
#define SPR_SCALE		(0x04000000)
#define SPR_SHADE		(0x08000000)
#define SPR_TINT		(0x10000000)
#define SPR_BLEND_ADD	(0x20000000)
#define SPR_BLEND_SUB	(0x40000000)
#define SPR_BLEND_QRT	(SPR_BLEND_ADD|SPR_BLEND_SUB)
#define SPR_BLEND		(SPR_BLEND_QRT)
#define SPR_ITEM		(0x80000000)

// Item flags
#define IFL_DRAWN       (0x0020) // Clearing body.
#define IFL_ONESHOT     (0x0100)
#define IFL_CODEBITS	(0x3E00)
#define IFL_REVERSE		(0x4000)
#define IFL_KILLED      (0x8000)
#define IFL_CLEARBODY	-(0x8000)

// Glow tint colors
#define GLOW_FLARE_COLOR	(0xFF8080) // Flare
#define GLOW_PISTOL_COLOR	(0x3F380F) // Pistol/Magnums/Uzi gunfire
#define GLOW_M16_COLOR		(0x7F701F) // M16 gunfire
#define GLOW_GUNSHOT_COLOR	(0x7F701F) // Skidoo/Enemy gunfire

// Collision types
#define COLL_FRONT		(0x01)
#define COLL_LEFT		(0x02)
#define COLL_RIGHT		(0x04)
#define COLL_TOP		(0x08)
#define COLL_TOPFRONT	(0x10)
#define COLL_CLAMP		(0x20)

// Trigger/floordata control bits
#define END_BIT		 (0x8000)
#define VALUE_BITS	 (0x03FF)
#define DATA_TYPE	 (0x1F)
#define TRIG_TYPE(T) ((T >> 8) & 0x3F)
#define TRIG_BITS(T) ((T & 0x3FFF) >> 10)

// Target types for CalculateTarget()
#define NO_TARGET			(0x0000)
#define PRIME_TARGET		(0x0001)
#define SECONDARY_TARGET	(0x0002)

/*
 * DirectX type definitions
 */

#ifndef RGB_MAKE
#define RGBA_SETALPHA(rgba, x)		(((x) << 24) | ((rgba) & 0x00ffffff))
#define RGBA_GETALPHA(rgb)			((rgb) >> 24)
#define RGBA_GETRED(rgb)			(((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb)			(((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)			((rgb) & 0xff)
#define RGBA_MAKE(r, g, b, a)		((D3DCOLOR) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))
#define RGB_GETRED(rgb)				(((rgb) >> 16) & 0xff)
#define RGB_GETGREEN(rgb)			(((rgb) >> 8) & 0xff)
#define RGB_GETBLUE(rgb)			((rgb) & 0xff)
#define RGB_MAKE(r, g, b)			((D3DCOLOR) (((r) << 16) | ((g) << 8) | (b)))
#endif // RGB_MAKE

#ifndef D3DFVF_TLVERTEX
#define D3DFVF_TLVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX1)
#endif // D3DFVF_TLVERTEX

#define VTXBUF_LEN (256)

typedef struct {
	D3DVALUE sx, sy, sz, rhw;
	D3DCOLOR color, specular;
	D3DVALUE tu, tv;
} D3DTLVERTEX, *LPD3DTLVERTEX;
typedef D3DLOCKED_RECT DDSDESC, *LPDDSDESC;
typedef LPDIRECT3DSURFACE9 LPDDS;
typedef LPDIRECT3DTEXTURE9 HWR_TEXHANDLE;

typedef struct {
	DWORD width;
	DWORD height;
	LPBYTE bitmap;
} SWR_BUFFER;

/*
 * Enums
 */

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
	ID_WINDOW1, // Smashable (weapon)
	ID_WINDOW2, // Smashable (jump)
	ID_WINDOW3, // Smashable (weapon)
	ID_WINDOW4, // Smashable (jump)
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
	ID_BELL, // Smashable (weapon)
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
	ID_PASSPORT_OPTION, // SECRET_1
	ID_COMPASS_OPTION,  // SECRET_2
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
	ID_PASSPORT_CLOSED, // SECRET_3
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
	ID_SECRET1,
	ID_SECRET2,
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
	ID_DINO,
	ID_BIRD_TWEETER2,
	ID_CLOCK_CHIMES,
	ID_CANDLE_FLAME_EMITTER,
	ID_DRAGON_BONES2,
	ID_DRAGON_BONES3,
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
	ID_BUBBLE_EMITTER,
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

#ifdef FEATURE_INPUT_IMPROVED
typedef enum {
	JOY_DirectInput,
	JOY_XInput,
	JOY_RawInput,
} JOY_INTERFACE;
#endif // FEATURE_INPUT_IMPROVED

typedef enum {
#ifdef FEATURE_HUD_IMPROVED
	CTRL_Joystick,
	CTRL_Custom,
	CTRL_Default,
#else // FEATURE_HUD_IMPROVED
	CTRL_Default,
	CTRL_Custom,
#endif // FEATURE_HUD_IMPROVED
} CONTROL_LAYOUT_PAGE;

typedef enum {
	INV_GameMode,
	INV_TitleMode,
	INV_KeysMode,
	INV_SaveMode,
	INV_LoadMode,
	INV_DeathMode,
	INV_PauseMode, // NOTE: not presented in the original game
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
#ifdef FEATURE_HUD_IMPROVED
	KM_Step,
#endif // FEATURE_HUD_IMPROVED
	KM_Slow,
	KM_Jump,
	KM_Action,
#ifdef FEATURE_HUD_IMPROVED
	KM_Roll,
	KM_WeaponDraw,
	KM_Flare,
	KM_Look,
	KM_Option,
	KM_Pause,
#else // FEATURE_HUD_IMPROVED
	KM_WeaponDraw,
	KM_Flare,
	KM_Look,
	KM_Roll,
	KM_Option,
#endif // FEATURE_HUD_IMPROVED
} KEYMAP;

typedef enum {
	ST_AvgZ,
	ST_MaxZ,
	ST_FarZ,
} SORTTYPE;

typedef enum {
	DRAW_Opaque,
	DRAW_ColorKey,
	DRAW_Semitrans, // NOTE: semitrans is not presented in the original code
} DRAWTYPE;

typedef enum {
	// Software renderer
	POLY_GTmap,			// gouraud shaded poly (texture)
	POLY_WGTmap,		// gouraud shaded poly (texture + colorkey)
	POLY_GTmap_persp,	// gouraud shaded poly (texture + perspective)
	POLY_WGTmap_persp,	// gouraud shaded poly (texture + colorkey + perspective)
	POLY_line,			// line (color)
	POLY_flat,			// flat shaded poly (color)
	POLY_gouraud,		// gouraud shaded poly (color)
	POLY_trans,			// shadow poly (color + semitransparent)
	POLY_sprite,		// scaled sprite (texture + colorkey)
	// Hardware renderer
	POLY_HWR_GTmap,		// gouraud shaded poly (texture)
	POLY_HWR_WGTmap,	// gouraud shaded poly (texture + colorkey)
	POLY_HWR_gouraud,	// gouraud shaded poly (color)
	POLY_HWR_line,		// line (color)
	POLY_HWR_trans,		// shadow poly (color + semitransparent)
#ifdef FEATURE_VIDEOFX_IMPROVED
	POLY_HWR_WGTmapHalf,// semitransparent gouraud shaded poly (texture + colorkey)
	POLY_HWR_WGTmapAdd,	// additive blended gouraud shaded poly (texture + colorkey)
	POLY_HWR_WGTmapSub,	// subtractive blended gouraud shaded poly (texture + colorkey)
	POLY_HWR_WGTmapQrt,	// quarter blended gouraud shaded poly (texture + colorkey)
	POLY_HWR_half,		// semitransparent gouraud shaded poly (color)
	POLY_HWR_add,		// additive blended gouraud shaded poly (color)
	POLY_HWR_sub,		// subtractive blended gouraud shaded poly (color)
	POLY_HWR_qrt,		// quarter blended gouraud shaded poly (color)
#endif // FEATURE_VIDEOFX_IMPROVED
#ifdef FEATURE_HUD_IMPROVED
	POLY_HWR_healthbar,	// health bar
	POLY_HWR_airbar,	// air bar
	POLY_HWR_enemybar,  // enemy health bar
#endif // FEATURE_HUD_IMPROVED
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
	ITEM_INACTIVE, // Status 1
	ITEM_ACTIVE,
	ITEM_DISABLED, // Status 2
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
#ifdef FEATURE_HUD_IMPROVED
	CHAR_SECRET1 = 0x13,
#else
	CHAR_SECRET1 = 0x7F,
#endif
	CHAR_SECRET2,
	CHAR_SECRET3,
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

/*
 * Structs
 */
#pragma pack(push, 1)

 // NOTE: there were int items in the original code,
 // but it's more important to have wider range
 // since negative numbers are not used anyway
typedef struct SortItem_t {
#ifdef FEATURE_VIEW_IMPROVED
	UINT64 _0;
	UINT64 _1;
#else // FEATURE_VIEW_IMPROVED
	DWORD _0;
	DWORD _1;
#endif // FEATURE_VIEW_IMPROVED
} SORT_ITEM;

typedef struct RGB888_t {
	BYTE red;
	BYTE green;
	BYTE blue;
} RGB888;

typedef struct GouraudFill_t {
	D3DCOLOR clr[4][4];
} GOURAUD_FILL;

typedef struct GouraudOutline_t {
	D3DCOLOR clr[9];
} GOURAUD_OUTLINE;

typedef struct DepthQEntry_t {
	BYTE index[256];
} DEPTHQ_ENTRY;

typedef struct GouraudEntry_t {
	BYTE index[32];
} GOURAUD_ENTRY;

typedef struct DisplayMode_t {
	int width = 0;
	int height = 0;
	int bpp = 0;
	VGA_MODE vga = VGA_NoVga;
	DisplayMode_t() = default;
	DisplayMode_t(int _w, int _h, int _bpp, VGA_MODE _vga) {
		width = _w;
		height = _h;
		bpp = _bpp;
		vga = _vga;
	}
} DISPLAY_MODE;

typedef struct DisplayModeNode_t {
	struct DisplayModeNode_t* next;
	struct DisplayModeNode_t* previous;
	struct DisplayMode_t body;
} DISPLAY_MODE_NODE;

typedef struct DisplayModeList_t {
	struct DisplayModeNode_t* head;
	struct DisplayModeNode_t* tail;
	DWORD dwCount;
} DISPLAY_MODE_LIST;

typedef struct StringFlagged_t {
	LPTSTR lpString;
	bool isPresented;
} STRING_FLAGGED;

typedef struct DisplayAdapter_t {
	LPGUID lpAdapterGuid;
	GUID adapterGuid;
	STRING_FLAGGED driverDescription;
	STRING_FLAGGED driverName;
	UINT index;
	D3DCAPS9 caps;
	DISPLAY_MODE_LIST hwDispModeList;
	DISPLAY_MODE_LIST swDispModeList;
	DWORD screenWidth;
} DISPLAY_ADAPTER;

typedef struct DisplayAdapterNode_t {
	struct DisplayAdapterNode_t* next;
	struct DisplayAdapterNode_t* previous;
	struct DisplayAdapter_t body;
} DISPLAY_ADAPTER_NODE;

typedef struct DisplayAdapterList_t {
	struct DisplayAdapterNode_t* head;
	struct DisplayAdapterNode_t* tail;
	DWORD dwCount;
} DISPLAY_ADAPTER_LIST;

typedef struct SoundAdapter_t {
	GUID* lpAdapterGuid;
	GUID adapterGuid;
	STRING_FLAGGED description;
	STRING_FLAGGED module;
} SOUND_ADAPTER;

typedef struct SoundAdapterNode_t {
	struct SoundAdapterNode_t* next;
	struct SoundAdapterNode_t* previous;
	SOUND_ADAPTER body;
} SOUND_ADAPTER_NODE;

typedef struct SoundAdapterList_t {
	struct SoundAdapterNode_t* head;
	struct SoundAdapterNode_t* tail;
	DWORD dwCount;
} SOUND_ADAPTER_LIST;

typedef struct Joystick_t {
	GUID* lpJoystickGuid;
	GUID joystickGuid;
	STRING_FLAGGED productName;
	STRING_FLAGGED instanceName;
#if defined(FEATURE_INPUT_IMPROVED)
	JOY_INTERFACE iface;
#endif // FEATURE_INPUT_IMPROVED
} JOYSTICK;

typedef struct JoystickNode_t {
	struct JoystickNode_t* next;
	struct JoystickNode_t* previous;
	JOYSTICK body;
} JOYSTICK_NODE;

typedef struct JoystickList_t {
	struct JoystickNode_t* head;
	struct JoystickNode_t* tail;
	DWORD dwCount;
} JOYSTICK_LIST;

typedef struct AppSettings_t {
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
#if defined(FEATURE_VIDEOFX_IMPROVED)
	int LightingMode;
#endif // FEATURE_VIDEOFX_IMPROVED
} APP_SETTINGS;

typedef struct TexPageDesc_t {
	LPDIRECT3DTEXTURE9 texture;
	int width;
	int height;
	int status;
} TEXPAGE_DESC;

typedef struct FloorInfo_t {
	unsigned short index;
	short box;
	BYTE pitRoom;
	char floor;
	BYTE skyRoom;
	char ceiling;
} FLOOR_INFO;

typedef struct PhdUV_t {
	UINT16 u;
	UINT16 v;
} PHD_UV;

typedef struct PhdTexture_t {
	UINT16 drawtype;
	UINT16 tpage;
	PHD_UV uv[4];
} PHD_TEXTURE;

typedef struct ColorBitMasks_t {
	DWORD dwRBitMask;
	DWORD dwGBitMask;
	DWORD dwBBitMask;
	DWORD dwRGBAlphaBitMask;
	DWORD dwRBitDepth;
	DWORD dwGBitDepth;
	DWORD dwBBitDepth;
	DWORD dwRGBAlphaBitDepth;
	DWORD dwRBitOffset;
	DWORD dwGBitOffset;
	DWORD dwBBitOffset;
	DWORD dwRGBAlphaBitOffset;
} COLOR_BIT_MASKS;

typedef struct TextureFormat_t {
	DWORD bpp;
} TEXTURE_FORMAT;

typedef struct TextStrInfo_t {
	DWORD flags;
	UINT16 textFlags;
	UINT16 bgndFlags;
	UINT16 outlFlags;
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

typedef struct StringFixed4_t {
	char str[4];
} STRING_FIXED4;

typedef struct StringFixed50_t {
	char str[50];
} STRING_FIXED50;

typedef struct StringFixed64_t {
	char str[64];
} STRING_FIXED64;

typedef struct PcxHeader_t {
	BYTE manufacturer;
	BYTE version;
	BYTE rle;
	BYTE bpp;
	UINT16 xMin;
	UINT16 yMin;
	UINT16 xMax;
	UINT16 yMax;
	UINT16 h_dpi;
	UINT16 v_dpi;
	RGB888 headerPalette[16];
	BYTE reserved;
	BYTE planes;
	UINT16 bytesPerLine;
	UINT16 palInterpret;
	UINT16 h_res;
	UINT16 v_res;
	BYTE reservedData[54];
} PCX_HEADER;

typedef struct TgaHeader_t {
	UINT8 idLength;
	UINT8 colorMapType;
	UINT8 dataTypeCode;
	UINT16 colorMapOrigin;
	UINT16 colorMapLength;
	UINT8 colorMapDepth;
	UINT16 xOrigin;
	UINT16 yOrigin;
	UINT16 width;
	UINT16 height;
	UINT8 bpp;
	UINT8 imageDescriptor;
} TGA_HEADER;

typedef struct BitmapResource_t {
	LPBITMAPINFO bmpInfo;
	void* bmpData;
	HPALETTE hPalette;
	DWORD flags;
} BITMAP_RESOURCE;

typedef struct RoomLightTable_t {
	int table[32];
} ROOM_LIGHT_TABLE;

typedef struct WavePcmHeader_t {
	DWORD dwRiffChunkID;
	DWORD dwRiffChunkSize;
	DWORD dwFormat;
	DWORD dwFmtSubchunkID;
	DWORD dwFmtSubchunkSize;
	UINT16 wFormatTag;
	short nChannels;
	int nSamplesPerSec;
	int nAvgBytesPerSec;
	short nBlockAlign;
	UINT16 wBitsPerSample;
	DWORD dwDataSubchunkID;
	DWORD dwDataSubchunkSize;
} WAVEPCM_HEADER;

typedef struct SampleInfo_t {
	UINT16 sampleIdx;
	BYTE volume;
	BYTE randomness;
	BYTE radius;
	BYTE pitch;
	BYTE lutCount;
	/// <summary>
	/// 0x0000 NOTHING
	/// 0x0080 RANDOM_SAMPLE
	/// 0x0200 ONE_SHOT_WAIT
	/// 0x0400 ONE_SHOT_REWOUND
	/// 0x0800 LOOPED
	/// 0x1000 NO_PAN
	/// 0x2000 RANDOM_PITCH
	/// 0x4000 RANDOM_VOLUME
	/// </summary>
	UINT16 flags;
} SAMPLE_INFO;

typedef struct SfxInfo_t {
	int volume;
	int pan;
	int sampleIdx;
	int freqFactor;
} SFX_INFO;

typedef struct GameFlow_t {
	int firstOption;
	int titleReplace;
	int onDeath_DemoMode;
	int onDeath_InGame;
	int noInput_Time;
	int onDemo_Interrupt;
	int onDemo_End;
	UINT16 reserved1[18];
	UINT16 num_Levels;
	UINT16 num_Pictures;
	UINT16 num_Titles;
	UINT16 num_Fmvs;
	UINT16 num_Cutscenes;
	UINT16 num_Demos;
	UINT16 titleTrack;
	short singleLevel;
	UINT16 reserved2[16];
	UINT16 flags;
	UINT16 reserved3[3];
	BYTE cypherCode;
	BYTE language;
	BYTE secretTrack;
	BYTE levelCompleteTrack;
	UINT16 reserved4[2];
} GAME_FLOW;

typedef struct RequestInfo_t {
	UINT16 reqFlags;
	UINT16 itemsCount;
	UINT16 selected;
	UINT16 visibleCount;
	UINT16 lineOffset;
	UINT16 lineOldOffset;
	UINT16 pixWidth;
	UINT16 lineHeight;
	short xPos;
	short yPos;
	short zPos;
	UINT16 itemStringLen;
	char* lpItemStrings1;
	char* lpItemStrings2;
	DWORD* lpItemFlags1;
	DWORD* lpItemFlags2;
	DWORD headingFlags1;
	DWORD headingFlags2;
	DWORD backgroundFlags;
	DWORD moreupFlags;
	DWORD moredownFlags;
	DWORD itemFlags1[24];
	DWORD itemFlags2[24];
	TEXT_STR_INFO* headingText1;
	TEXT_STR_INFO* headingText2;
	TEXT_STR_INFO* backgroundText;
	TEXT_STR_INFO* moreupText;
	TEXT_STR_INFO* moredownText;
	TEXT_STR_INFO* itemTexts1[24];
	TEXT_STR_INFO* itemTexts2[24];
	char headingString1[32];
	char headingString2[32];
	DWORD renderWidth;
	DWORD renderHeight;
} REQUEST_INFO;

typedef struct InventoryItem_t {
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

typedef struct StatisticsInfo_t {
	DWORD timer;
	DWORD shots;
	DWORD hits;
	DWORD distance;
	UINT16 kills;
	BYTE secrets;
	BYTE mediPacks;
} STATISTICS_INFO;

typedef struct StartInfo_t {
	UINT16 pistolAmmo;
	UINT16 autopistolAmmo;
	UINT16 uziAmmo;
	UINT16 shotgunAmmo;
	UINT16 m16Ammo;
	UINT16 grenadeAmmo;
	UINT16 harpoonAmmo;
	BYTE smallMedipacks;
	BYTE largeMedipacks;
	BYTE unused;
	BYTE flares;
	BYTE gunStatus;
	BYTE gunType;
	UINT16 available : 1;
	UINT16 has_pistols : 1;
	UINT16 has_autopistol : 1;
	UINT16 has_uzis : 1;
	UINT16 has_shotgun : 1;
	UINT16 has_m16 : 1;
	UINT16 has_grenade : 1;
	UINT16 has_harpoon : 1;
	UINT16 pad : 8;
	UINT16 reserved;
	STATISTICS_INFO statistics;
} START_INFO;

typedef struct SaveGame_t {
	START_INFO start[24];
	STATISTICS_INFO statistics;
	short currentLevel;
	bool bonusFlag;
	BYTE numPickup[2];
	BYTE numPuzzle[4];
	BYTE numKey[4];
	UINT16 reserved;
	BYTE buffer[MAX_SAVEGAME_BUFFER(16)]; // 16KB for savegame buffer.
} SAVEGAME_INFO;

typedef struct Pos2D_t {
	short x;
	short y;
} POS_2D;

typedef struct Pos3D_t {
	short x;
	short y;
	short z;
} POS_3D;

typedef struct PhdVector_t {
	int x;
	int y;
	int z;
} PHD_VECTOR;

typedef struct GameVector_t {
	int x;
	int y;
	int z;
	short roomNumber;
	short boxNumber;
} GAME_VECTOR;

typedef struct ObjectVector_t {
	int x;
	int y;
	int z;
	UINT16 data;
	UINT16 flags;
} OBJECT_VECTOR;

typedef struct VectorAngles_t {
	short rotY;
	short rotX;
} VECTOR_ANGLES;

typedef struct Phd3dPos_t {
	int x;
	int y;
	int z;
	short rotX;
	short rotY;
	short rotZ;
} PHD_3DPOS;

typedef struct ItemInfo_t {
	int floor;
	DWORD touchBits;
	DWORD meshBits;
	short objectID;
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
	short boxNumber;
	short timer;
	UINT16 flags; // see IFL_* defines
	short shade1;
	short shade2;
	short carriedItem;
	LPVOID data;
	PHD_3DPOS pos;
	UINT16 active : 1;
	UINT16 status : 2; // see ITEM_STATUS enum
	UINT16 gravity : 1;
	UINT16 hitStatus : 1;
	UINT16 collidable : 1;
	UINT16 looked_at : 1;
	UINT16 dynamic_light : 1;
	UINT16 clear_body : 1;
} ITEM_INFO;

typedef struct CameraInfo_t {
	GAME_VECTOR pos;
	GAME_VECTOR target;
	CAMERA_TYPE type;
	int shift;
	DWORD flags;
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
	BOOL isLaraMic;
	PHD_VECTOR micPos;
} CAMERA_INFO;

typedef struct CollSide_t {
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

typedef struct CollInfo_t {
	COLL_SIDE sideMid;
	COLL_SIDE sideFront;
	COLL_SIDE sideLeft;
	COLL_SIDE sideRight;
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
	UINT16 flags;
} COLL_INFO;

typedef struct ObjectInfo_t {
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
	UINT16 loaded : 1;
	UINT16 intelligent : 1;
	UINT16 save_position : 1;
	UINT16 save_hitpoints : 1;
	UINT16 save_flags : 1;
	UINT16 save_anim : 1;
	UINT16 semi_transparent : 1;
	UINT16 water_creature : 1;
	UINT16 pad : 8;
} OBJECT_INFO;

typedef struct PhdMatrix_t {
	int _00, _01, _02, _03;
	int _10, _11, _12, _13;
	int _20, _21, _22, _23;
} PHD_MATRIX;

typedef struct DoorPosData_t {
	FLOOR_INFO* floor;
	FLOOR_INFO data;
	short block;
} DOORPOS_DATA;

typedef struct DoorData_t {
	DOORPOS_DATA d1;
	DOORPOS_DATA d1Flip;
	DOORPOS_DATA d2;
	DOORPOS_DATA d2Flip;
} DOOR_DATA;

typedef struct DoorInfo_t {
	short room;
	short x;
	short y;
	short z;
	POS_3D vertex[4];
} DOOR_INFO;

typedef struct DoorInfos_t {
	short wCount;
	DOOR_INFO door[];
} DOOR_INFOS;

typedef struct LightInfo_t {
	int x;
	int y;
	int z;
	short intensity1;
	short intensity2;
	int fallOff1;
	int fallOff2;
} LIGHT_INFO;

typedef struct MeshInfo_t {
	int x;
	int y;
	int z;
	short yRot;
	short shade1;
	short shade2;
	short staticNumber;
} MESH_INFO;

typedef struct RoomVertex_t {
	short x;
	short y;
	short z;
	short lightBase;
	BYTE lightTableValue;
	BYTE flags;
	short lightAdder;
} ROOM_VERTEX;

typedef struct Face4_t {
	short vertices[4];
	short texture;
} FACE4;

typedef struct Face3_t {
	short vertices[3];
	short texture;
} FACE3;

typedef struct RoomSprite_t {
	short vertex;
	short spriteIndex;
} ROOM_SPRITE;

typedef struct RoomData_t {
	USHORT vtxSize;
	ROOM_VERTEX* vertices;
	USHORT gt4Size;
	FACE4* gt4;
	USHORT gt3Size;
	FACE3* gt3;
	USHORT spriteSize;
	ROOM_SPRITE* sprites;
} ROOM_DATA;

typedef struct RoomInfo_t {
	short index;
	ROOM_DATA* data;
	DOOR_INFOS* doors;
	FLOOR_INFO* floor;
	LIGHT_INFO* light;
	MESH_INFO* mesh;
	DWORD flags;
	int x;
	int y;
	int z;
	int minFloor;
	int maxCeiling;
	short xSize;
	short ySize;
	short ambient1;
	short ambient2;
	LIGHT_TYPE lightMode;
	short numLights;
	short numMeshes;
	short boundLeft;
	short boundRight;
	short boundTop;
	short boundBottom;
	UINT16 boundActive;
	short left;
	short right;
	short top;
	short bottom;
	short itemNumber;
	short fxNumber;
	short flippedRoom;
	BYTE reverbType;

	int GetCenter() const {
		return y - (maxCeiling - minFloor);
	}
} ROOM_INFO;

typedef struct LiftData_t {
	int oldY;
	int timer;
} LIFT_DATA;

typedef struct PhdVBuf_t {
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

typedef struct PointInfo_t {
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

typedef struct VertexInfo_t {
	float x;
	float y;
	float rhw;
	float u;
	float v;
	float g;
} VERTEX_INFO;

typedef struct AnimStruct_t {
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

typedef struct ChangeStruct_t {
	short goalAnimState;
	short numberRanges;
	short rangeIndex;
} CHANGE_STRUCT;

typedef struct RangeStruct_t {
	short startFrame;
	short endFrame;
	short linkAnimNum;
	short linkFrameNum;
} RANGE_STRUCT;

typedef struct StaticBounds_t {
	short xMin;
	short xMax;
	short yMin;
	short yMax;
	short zMin;
	short zMax;
} STATIC_BOUNDS;

typedef struct StaticInfo_t {
	short meshIndex;
	UINT16 flags;
	STATIC_BOUNDS drawBounds;
	STATIC_BOUNDS collisionBounds;
} STATIC_INFO;

typedef struct PhdSprite_t {
	UINT16 texPage;
	UINT16 offset;
	UINT16 width;
	UINT16 height;
	short x1;
	short y1;
	short x2;
	short y2;
} PHD_SPRITE;

typedef struct BoxInfo_t {
	BYTE left;
	BYTE right;
	BYTE top;
	BYTE bottom;
	short height;
	UINT16 overlapIndex;
} BOX_INFO;

typedef struct CineFrameInfo_t {
	short xTarget;
	short yTarget;
	short zTarget;
	short zPos;
	short yPos;
	short xPos;
	short fov;
	short roll;
} CINE_FRAME_INFO;

typedef struct ControlLayout_t {
#ifdef FEATURE_HUD_IMPROVED
	BYTE key[16];
#else // FEATURE_HUD_IMPROVED
	UINT16 key[14];
#endif // FEATURE_HUD_IMPROVED
} CONTROL_LAYOUT;

typedef struct InventorySprite_t {
	short shape;
	short x;
	short y;
	short z;
	int param1;
	int param2;
	LPVOID gour;
	UINT16 invColour;
} INVENTORY_SPRITE;

typedef struct AssaultStats_t {
	DWORD bestTime[10];
	DWORD bestFinish[10];
	DWORD finishCount;
} ASSAULT_STATS;

typedef struct PickupInfo_t {
	short timer;
	short sprite;
} PICKUP_INFO;

typedef struct InvMotionInfo_t {
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

typedef struct RingInfo_t {
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

typedef struct BoxNode_t {
	short exitBox;
	UINT16 searchNumber;
	short nextExpansion;
	short boxNumber;
} BOX_NODE;

typedef struct LotInfo_t {
	BOX_NODE* node;
	short head;
	short tail;
	UINT16 searchNumber;
	UINT16 blockMask;
	short step;
	short drop;
	short fly;
	short zoneCount;
	short targetBox;
	short requiredBox;
	PHD_VECTOR target;
} LOT_INFO;

typedef struct FxInfo_t {
	PHD_3DPOS pos;
	DWORD color;
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

typedef struct CreatureInfo_t {
	short headRotation;
	short neckRotation;
	short maximumTurn;
	UINT16 flags;
	short itemNumber;
	MOOD_TYPE mood;
	LOT_INFO LOT;
	PHD_VECTOR target;
	ITEM_INFO* enemy;
} CREATURE_INFO;

static inline CREATURE_INFO* GetCreatureInfo(ITEM_INFO* item) {
	return static_cast<CREATURE_INFO*>(item->data);
}

typedef struct BoatInfo_t {
	int turn;
	int leftFallspeed;
	int rightFallspeed;
	short tiltAngle;
	short extraRotation;
	int water;
	int pitch;
} BOAT_INFO;

typedef struct CandleEmitterData_t {
	short leftID;
	short middleID;
	short rightID;
	bool on;
} CANDLE_EMITTER_DATA;

typedef struct SkidooInfo_t {
	short trackMesh;
	int turn;
	int leftFallspeed;
	int rightFallspeed;
	short tiltAngle;
	short extraRotation;
	int pitch;
} SKIDOO_INFO;

typedef struct LaraArm_t {
	short* frame_base;
	short frame_number;
	short anim_number;
	short lock;
	short y_rot;
	short x_rot;
	short z_rot;
	short flash_gun;
} LARA_ARM;

typedef struct LaraInfo_t {
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
	UINT16 flare_control_left : 1;
	UINT16 flare_control_right : 1;
	UINT16 extra_anim : 1;
	UINT16 look : 1;
	UINT16 burn : 1;
	UINT16 keep_ducked : 1;
	UINT16 can_monkey_swing : 1;
	UINT16 pad : 9;
	int water_surface_dist;
	PHD_VECTOR last_pos;
	FX_INFO* spaz_effect;
	DWORD mesh_effects;
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
	DWORD pistol_ammo;
	DWORD magnum_ammo;
	DWORD uzi_ammo;
	DWORD shotgun_ammo;
	DWORD harpoon_ammo;
	DWORD grenade_ammo;
	DWORD m16_ammo;
	CREATURE_INFO* creature;
} LARA_INFO;

typedef struct BiteInfo_t {
	int x;
	int y;
	int z;
	int meshIndex;
} BITE_INFO;

typedef struct AIInfo_t {
	short zoneNumber;
	short enemyZone;
	int distance;
	BOOL ahead;
	BOOL bite;
	short angle;
	short enemyFacing;
} AI_INFO;

typedef struct Sphere_t {
	int x;
	int y;
	int z;
	int radius;
} SPHERE_INFO;

typedef struct SoundSlot_t {
	PHD_VECTOR pos;
	int volume;
	int originalVolume;
	int pan;
	int sampleIdx;
	int pitch;
	int distance;
} SOUND_SLOT;

typedef struct WeaponInfo_t {
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

typedef struct { short idx; short num; } POLYINDEX;

typedef struct TexPageConfig_t {
	bool isLoaded;
	bool isLegacyColors;
	double adjustment;
#ifdef FEATURE_HUD_IMPROVED
	struct {
		int spacing;
		int xOffset;
		int yOffset;
		double xStretch;
		double yStretch;
	} glyphs[110];
#endif // FEATURE_HUD_IMPROVED
} TEXPAGES_CONFIG;

typedef struct Polyfilter_t {
	short n_vtx, n_gt4, n_gt3, n_g4, n_g3;
	POLYINDEX gt4[POLYFILTER_SIZE];
	POLYINDEX gt3[POLYFILTER_SIZE];
	POLYINDEX g4[POLYFILTER_SIZE];
	POLYINDEX g3[POLYFILTER_SIZE];
} POLYFILTER;

typedef struct PolyfilterNode_t {
	int id;
	POLYFILTER filter;
	struct PolyfilterNode_t* next;
} POLYFILTER_NODE;

typedef struct BarConfig_t {
	bool isCentered;
	bool basedOnEnemyHealth;

	int PC_xpos;
	int PC_ypos;
	INV_COLOURS PC_color[2]; // Left, Right (ICLR_flags)

	int PSX_xpos;
	int PSX_ypos;
	int CENTER_xpos;
	int CENTER_ypos;
	DWORD PSX_leftcolor[6];
	DWORD PSX_rightcolor[6];
	DWORD PSX_framecolor[6];
} BAR_CONFIG;

typedef struct SemiTransConfig_t {
	bool isLoaded;
	POLYINDEX* animtex;
	POLYFILTER_NODE* rooms;
	POLYFILTER_NODE* statics;
	POLYFILTER_NODE* objects[ID_NUMBER_OBJECTS];
} SEMITRANS_CONFIG;

typedef struct ReflectConfig_t {
	bool isLoaded;
	POLYFILTER_NODE* statics;
	POLYFILTER_NODE* objects[ID_NUMBER_OBJECTS];
} REFLECT_CONFIG;

typedef struct LaraBarConfig_t {
	BAR_CONFIG health;
	BAR_CONFIG air;
} LARA_BAR_CONFIG;

typedef struct EnemyHealthInfo_t {
	short dog;
	short mouse;
	short cult1;
	short cult1A;
	short cult1B;
	short cult2;
	short shark;
	short tiger;
	short barracuda;
	short smallSpider; // spider or wolf (separated)
	short wolf; // spider or wolf (separated)
	short bigSpider; // big spider or bear (separated)
	short bear; // big spider or bear (separated)
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

typedef struct CustInventoryItem_t {
	int objectID = -1;
	std::string message;
	int message_pos_x;
	int message_pos_y;
	int yTransSel;
	int zTransSel;
	int xRotPtSel;
	int xRotSel;
	int yRotSel;
	bool canExamine = false;
	bool canRotateManually = false; // Allow object to rotate when selected ?
} CUST_INVENTORY_ITEM;

typedef struct CustUnderwaterInfo_t {
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

static inline ORIENT_AXIS GetOrientAxis(short rotY)
{
	return static_cast<ORIENT_AXIS>((USHORT)(rotY + PHD_45) / PHD_90);
}
static inline ORIENT_AXIS GetOrientAxisInverted(short rotY)
{
	return static_cast<ORIENT_AXIS>((USHORT)((rotY + PHD_180) + PHD_45) / PHD_90);
}
static inline PHD_VECTOR GetOrientAxisDirection(short rotY, int radius = 1) {
	ORIENT_AXIS axis = GetOrientAxis(rotY);
	PHD_VECTOR pos = {};
	switch (axis)
	{
	case NORTH:
		pos.z += radius;
		break;
	case EAST:
		pos.x += radius;
		break;
	case SOUTH:
		pos.z -= radius;
		break;
	case WEST:
		pos.x -= radius;
		break;
	default:
		LogWarn("Axis not valid, found: %d", (int)axis);
		pos.x = 0;
		pos.z = 0;
		break;
	}
	return pos;
}
static inline PHD_VECTOR GetOrientAxisDirectionInverted(short rotY, int radius = 1) {
	ORIENT_AXIS axis = GetOrientAxisInverted(rotY);
	PHD_VECTOR pos = {};
	switch (axis)
	{
	case NORTH:
		pos.z += radius;
		break;
	case EAST:
		pos.x += radius;
		break;
	case SOUTH:
		pos.z -= radius;
		break;
	case WEST:
		pos.x -= radius;
		break;
	default:
		LogWarn("Axis not valid, found: %d", (int)axis);
		pos.x = 0;
		pos.z = 0;
		break;
	}
	return pos;
}

static inline FLOOR_INFO* GetFloorSector(int x, int z, ROOM_INFO* room) {
	return &room->floor[((z - room->z) >> WALL_SHIFT) + ((x - room->x) >> WALL_SHIFT) * room->xSize];
}
static inline FLOOR_INFO* GetFloorSector(ITEM_INFO* item, ROOM_INFO* room) {
	return &room->floor[((item->pos.z - room->z) >> WALL_SHIFT) + ((item->pos.x - room->x) >> WALL_SHIFT) * room->xSize];
}
static inline FLOOR_INFO* GetFloorSector(int x, int z, ROOM_INFO* room, int dx, int dz) {
	return &room->floor[(((z - room->z) >> WALL_SHIFT) + dz) + (((x - room->x) >> WALL_SHIFT) + dx) * room->xSize];
}
static inline FLOOR_INFO* GetFloorSector(ITEM_INFO* item, ROOM_INFO* room, int dx, int dz) {
	return &room->floor[(((item->pos.z - room->z) >> WALL_SHIFT) + dz) + (((item->pos.x - room->x) >> WALL_SHIFT) + dx) * room->xSize];
}

static inline short GetSectorBoxXZ(ITEM_INFO* item, ROOM_INFO* room) {
	FLOOR_INFO* floor = GetFloorSector(item, room);
	if (floor == NULL)
		return -1;
	return floor->box;
}

static inline short GetSectorBoxXZ(ITEM_INFO* item, ROOM_INFO* room, int dx, int dz) {
	FLOOR_INFO* floor = GetFloorSector(item, room, dx, dz);
	if (floor == NULL)
		return -1;
	return floor->box;
}

typedef bool (*ENUM_POLYS_OBJECTS_CB) (short* ptrObj, int vtxCount, bool colored, LPVOID param);
typedef bool (*ENUM_POLYS_FACE4_CB) (FACE4* ptrObj, int faceCount, bool colored, LPVOID param);
typedef bool (*ENUM_POLYS_FACE3_CB) (FACE3* ptrObj, int faceCount, bool colored, LPVOID param);

static std::string GetLowerString(const std::string& value) {
	std::string result = value;
	for (size_t i = 0; i < result.size(); i++)
		result[i] = std::tolower(result[i]);
	return result;
}

#pragma pack(pop)

#endif // GLOBAL_TYPES_H_INCLUDED
