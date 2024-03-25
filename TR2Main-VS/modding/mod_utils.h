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

#ifndef MOD_UTILS_H_INCLUDED
#define MOD_UTILS_H_INCLUDED

#include "global/types.h"

 // Filter is presented by an array of poly index and polys number (starting from the index).
 // The filter must be always terminated by an index 0.
 // If the first item has index=~0 then there are no polys of such type to process.
 // If the first item has index=0 and number=0 then all polys of such type must be processed.
#define POLYFILTER_SIZE 256

typedef struct { short idx; short num; } POLYINDEX;

typedef struct {
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

typedef struct {
	int PC_xpos, PC_ypos;
	INV_COLOURS PC_color[2]; // Left, Right (ICLR_flags)
	int PSX_xpos, PSX_ypos;
	DWORD PSX_leftcolor[6];
	DWORD PSX_rightcolor[6];
	DWORD PSX_framecolor[6];
} BAR_CONFIG;

typedef bool (*ENUM_POLYS_CB) (short* ptrObj, int vtxCount, bool colored, LPVOID param);

/*
 * Function list
 */
bool EnumeratePolys(short* ptrObj, bool isRoomMesh, ENUM_POLYS_CB callback, POLYFILTER* filter, LPVOID param);

#ifdef FEATURE_MOD_CONFIG
bool IsModConfigLoaded();
bool IsModBarefoot();

bool IsModPistolsAtStart();
bool IsModShotgunAtStart();
bool IsModUzisAtStart();
bool IsModAutopistolsAtStart();
bool IsModM16AtStart();
bool IsModGrenadeAtStart();
bool IsModHarpoonAtStart();

int GetModShotgunAmmoCountAtStart();
int GetModUzisAmmoCountAtStart();
int GetModAutopistolsAmmoCountAtStart();
int GetModM16AmmoCountAtStart();
int GetModGrenadeAmmoCountAtStart();
int GetModHarpoonAmmoCountAtStart();

int GetModFlareCountAtStart();
int GetModSmallMedikitCountAtStart();
int GetModBigMedikitCountAtStart();

int GetModDogHealth();
int GetModMouseHealth();
int GetModCult1Health();
int GetModCult1AHealth();
int GetModCult1BHealth();
int GetModCult2Health();
int GetModSharkHealth();
int GetModTigerHealth();
int GetModBarracudaHealth();
int GetModSmallSpiderHealth();
int GetModWolfHealth();
int GetModBigSpiderHealth();
int GetModBearHealth();
int GetModYetiHealth();
int GetModJellyHealth();
int GetModDiverHealth();
int GetModWorker1Health();
int GetModWorker2Health();
int GetModWorker3Health();
int GetModWorker4Health();
int GetModWorker5Health();
int GetModCult3Health();
int GetModMonk1Health();
int GetModMonk2Health();
int GetModEagleHealth();
int GetModCrowHealth();
int GetModBigEelHealth();
int GetModEelHealth();
int GetModBandit1Health();
int GetModBandit2Health();
int GetModBandit2BHealth();
int GetModSkidmanHealth();
int GetModXianLordHealth();
int GetModWarriorHealth();
int GetModDragonHealth();
int GetModGiantYetiHealth();
int GetModDinoHealth();

bool GetModLaraIgnoreMonkIfNotAngry();
bool GetModMakeMonkAttackLaraDirectly();
bool GetModMakeMercenaryAttackLaraDirectly();

BAR_CONFIG* GetModLaraHealthBar();
BAR_CONFIG* GetModLaraAirBar();
bool IsEnemyBarEnabled();
BAR_CONFIG* GetModEnemyBar();

const char* GetModLoadingPix();
DWORD GetModWaterColor();
bool IsModSemitransConfigLoaded();
POLYINDEX* GetModSemitransAnimtexFilter();
POLYFILTER_NODE* GetModSemitransRoomsFilter();
POLYFILTER_NODE* GetModSemitransStaticsFilter();
POLYFILTER_NODE** GetModSemitransObjectsFilter();
bool IsModReflectConfigLoaded();
POLYFILTER_NODE* GetModReflectStaticsFilter();
POLYFILTER_NODE** GetModReflectObjectsFilter();

void UnloadModConfiguration();
bool LoadModConfiguration(LPCTSTR levelFilePath);
#endif // FEATURE_MOD_CONFIG

#endif // MOD_UTILS_H_INCLUDED
