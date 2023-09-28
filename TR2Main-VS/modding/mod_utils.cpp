/*
 * Copyright (c) 2017-2021 Michael Chaban. All rights reserved.
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
#include "modding/mod_utils.h"
#include "modding/json_utils.h"
#include "global/vars.h"

#ifdef FEATURE_MOD_CONFIG
#define MOD_CONFIG_NAME "TR2Main.json"

typedef struct {
	bool isLoaded;
	POLYINDEX* animtex;
	POLYFILTER_NODE* rooms;
	POLYFILTER_NODE* statics;
	POLYFILTER_NODE* objects[ID_NUMBER_OBJECTS];
} SEMITRANS_CONFIG;

typedef struct {
	bool isLoaded;
	POLYFILTER_NODE* statics;
	POLYFILTER_NODE* objects[ID_NUMBER_OBJECTS];
} REFLECT_CONFIG;

typedef struct {
	bool isLoaded;
	bool isBarefoot;

	bool pistolsAtStart;
	bool shotgunAtStart;
	bool uzisAtStart;
	bool autopistolsAtStart;
	bool m16AtStart;
	bool grenadeAtStart;
	bool harpoonAtStart;

	int shotgunAmmoAtStart;
	int uzisAmmoAtStart;
	int autopistolsAmmoAtStart;
	int m16AmmoAtStart;
	int grenadeAmmoAtStart;
	int harpoonAmmoAtStart;

	int smallMedikitAtStart;
	int bigMedikitAtStart;
	int flareCountAtStart;
	
	int dogHealth;
	int mouseHealth;
	int cult1Health;
	int cult1AHealth;
	int cult1BHealth;
	int cult2Health;
	int sharkHealth;
	int tigerHealth;
	int barracudaHealth;
	int smallSpiderHealth; // spider or wolf (separated)
	int wolfHealth;
	int bigSpiderHealth; // big spider or bear (separated)
	int bearHealth;
	int yetiHealth;
	int jellyHealth;
	int diverHealth;
	int worker1Health;
	int worker2Health;
	int worker3Health;
	int worker4Health;
	int worker5Health;
	int cult3Health;
	int monk1Health;
	int monk2Health;
	int eagleHealth;
	int crowHealth;
	int bigEelHealth;
	int eelHealth;
	int bandit1Health;
	int bandit2Health;
	int bandit2BHealth;
	int skidmanHealth;
	int xianLordHealth;
	int warriorHealth;
	int dragonHealth;
	int giantYetiHealth;
	int dinoHealth;

	bool laraIgnoreMonkIfNotAngry;
	bool makeMonkAttackLaraFirst;
	bool makeMercenaryAttackLaraFirst;

	char loadingPix[256];
	DWORD waterColor;

	BAR_CONFIG healthbar;
	BAR_CONFIG airbar;
	BAR_CONFIG enemyhealthbar;

	SEMITRANS_CONFIG semitrans;
	REFLECT_CONFIG reflect;
} MOD_CONFIG;

static MOD_CONFIG ModConfig;

static POLYFILTER* CreatePolyfilterNode(POLYFILTER_NODE** root, int id) {
	if (root == NULL) return NULL;
	POLYFILTER_NODE* node = (POLYFILTER_NODE*)malloc(sizeof(POLYFILTER_NODE));
	if (node == NULL) return NULL;
	node->id = id;
	node->next = *root;
	memset(&node->filter, 0, sizeof(node->filter));
	*root = node;
	return &node->filter;
}

static void FreePolyfilterNodes(POLYFILTER_NODE** root) {
	if (root == NULL) return;
	POLYFILTER_NODE* node = *root;
	while (node) {
		POLYFILTER_NODE* next = node->next;
		free(node);
		node = next;
	}
	*root = NULL;
}
#endif // FEATURE_MOD_CONFIG

static bool IsCompatibleFilter(short* ptrObj, bool isRoomMesh, POLYFILTER* filter) {
	if (!ptrObj || !filter || !filter->n_vtx) return true;
	if (!isRoomMesh) {
		ptrObj += 5; // skip x, y, z, radius, flags
	}
	short num = *(ptrObj++); // get vertex counter
	if (num != filter->n_vtx) return false;
	ptrObj += num * (isRoomMesh ? 6 : 3); // skip vertices
	if (!isRoomMesh) {
		num = *(ptrObj++); // get normal counter
		ptrObj += (num > 0) ? num * 3 : ABS(num); // skip normals/shades
	}
	num = *(ptrObj++); // get gt4 number
	if (num != filter->n_gt4) return false;
	ptrObj += num * 5; // skip gt4 polys
	num = *(ptrObj++); // get gt3 number
	if (num != filter->n_gt3) return false;
	if (!isRoomMesh) {
		ptrObj += num * 4; // skip gt3 polys
		num = *(ptrObj++); // get g4 number
		if (num != filter->n_g4) return false;
		ptrObj += num * 5; // skip g4 polys
		num = *(ptrObj++); // get g3 number
		if (num != filter->n_g3) return false;
	}
	return true;
}

static short* EnumeratePolysSpecific(short* ptrObj, int vtxCount, bool colored, ENUM_POLYS_CB callback, POLYINDEX* filter, LPVOID param) {
	int polyNumber = *ptrObj++;
	if (filter == NULL || (!filter[0].idx && !filter[0].num)) {
		for (int i = 0; i < polyNumber; ++i) {
			if (!callback(ptrObj, vtxCount, colored, param)) return NULL;
			ptrObj += vtxCount + 1;
		}
	}
	else {
		int polyIndex = 0;
		for (int i = 0; i < POLYFILTER_SIZE; i++) {
			if (filter[i].idx < polyIndex || filter[i].idx >= polyNumber) {
				break;
			}
			int skip = filter[i].idx - polyIndex;
			if (skip > 0) {
				ptrObj += skip * (vtxCount + 1);
				polyIndex += skip;
			}
			int number = MIN(filter[i].num, polyNumber - polyIndex);
			for (int j = 0; j < number; ++j) {
				if (!callback(ptrObj, vtxCount, colored, param)) return NULL;
				ptrObj += vtxCount + 1;
			}
			polyIndex += number;
		}
		ptrObj += (polyNumber - polyIndex) * (vtxCount + 1);
	}
	return ptrObj;
}

bool EnumeratePolys(short* ptrObj, bool isRoomMesh, ENUM_POLYS_CB callback, POLYFILTER* filter, LPVOID param) {
	if (ptrObj == NULL || callback == NULL) return false; // wrong parameters
	if (!IsCompatibleFilter(ptrObj, isRoomMesh, filter)) return false; // filter is not compatible

	short num;
	if (!isRoomMesh) {
		ptrObj += 5; // skip x, y, z, radius, flags
	}
	num = *(ptrObj++); // get vertex counter
	ptrObj += num * (isRoomMesh ? 6 : 3); // skip vertices
	if (!isRoomMesh) {
		num = *(ptrObj++); // get normal counter
		ptrObj += (num > 0) ? num * 3 : ABS(num); // skip normals/shades
	}
	ptrObj = EnumeratePolysSpecific(ptrObj, 4, false, callback, filter ? filter->gt4 : NULL, param); // enumerate textured quads
	if (ptrObj == NULL) return true;
	ptrObj = EnumeratePolysSpecific(ptrObj, 3, false, callback, filter ? filter->gt3 : NULL, param); // enumerate textured triangles
	if (!isRoomMesh) {
		if (ptrObj == NULL) return true;
		ptrObj = EnumeratePolysSpecific(ptrObj, 4, true, callback, filter ? filter->g4 : NULL, param); // enumerate colored quads
		if (ptrObj == NULL) return true;
		ptrObj = EnumeratePolysSpecific(ptrObj, 3, true, callback, filter ? filter->g3 : NULL, param); // enumerate colored triangles
	}
	return true;
}

#ifdef FEATURE_MOD_CONFIG
bool IsModConfigLoaded() {
	return ModConfig.isLoaded;
}

bool IsModBarefoot() {
	return ModConfig.isBarefoot;
}

bool IsModPistolsAtStart() {
	return ModConfig.pistolsAtStart;
}

bool IsModShotgunAtStart() {
	return ModConfig.shotgunAtStart;
}

bool IsModUzisAtStart() {
	return ModConfig.uzisAtStart;
}

bool IsModAutopistolsAtStart() {
	return ModConfig.autopistolsAtStart;
}

bool IsModM16AtStart() {
	return ModConfig.m16AtStart;
}

bool IsModGrenadeAtStart() {
	return ModConfig.grenadeAtStart;
}

bool IsModHarpoonAtStart() {
	return ModConfig.harpoonAtStart;
}

int GetModShotgunAmmoCountAtStart() {
	return ModConfig.shotgunAmmoAtStart * (SHOTGUN_AMMO_CLIPS / 2);
}

int GetModUzisAmmoCountAtStart() {
	return ModConfig.uzisAmmoAtStart;
}

int GetModAutopistolsAmmoCountAtStart() {
	return ModConfig.autopistolsAmmoAtStart;
}

int GetModM16AmmoCountAtStart() {
	return ModConfig.m16AmmoAtStart;
}

int GetModGrenadeAmmoCountAtStart() {
	return ModConfig.grenadeAmmoAtStart;
}

int GetModHarpoonAmmoCountAtStart() {
	return ModConfig.harpoonAmmoAtStart;
}

int GetModFlareCountAtStart() {
	return ModConfig.flareCountAtStart;
}

int GetModSmallMedikitCountAtStart() {
	return ModConfig.smallMedikitAtStart;
}

int GetModBigMedikitCountAtStart() {
	return ModConfig.bigMedikitAtStart;
}

int GetModDogHealth() {
	return ModConfig.dogHealth;
}

int GetModMouseHealth() {
	return ModConfig.mouseHealth;
}

int GetModCult1Health() {
	return ModConfig.cult1Health;
}

int GetModCult1AHealth() {
	return ModConfig.cult1AHealth;
}

int GetModCult1BHealth() {
	return ModConfig.cult1BHealth;
}

int GetModCult2Health() {
	return ModConfig.cult2Health;
}

int GetModSharkHealth() {
	return ModConfig.sharkHealth;
}

int GetModTigerHealth() {
	return ModConfig.tigerHealth;
}

int GetModBarracudaHealth() {
	return ModConfig.barracudaHealth;
}

int GetModSmallSpiderHealth() {
	return ModConfig.smallSpiderHealth;
}

int GetModWolfHealth() {
	return ModConfig.wolfHealth;
}

int GetModBigSpiderHealth() {
	return ModConfig.bigSpiderHealth;
}

int GetModBearHealth() {
	return ModConfig.bearHealth;
}

int GetModYetiHealth() {
	return ModConfig.yetiHealth;
}

int GetModJellyHealth() {
	return ModConfig.jellyHealth;
}

int GetModDiverHealth() {
	return ModConfig.diverHealth;
}

int GetModWorker1Health() {
	return ModConfig.worker1Health;
}

int GetModWorker2Health() {
	return ModConfig.worker2Health;
}

int GetModWorker3Health() {
	return ModConfig.worker3Health;
}

int GetModWorker4Health() {
	return ModConfig.worker4Health;
}

int GetModWorker5Health() {
	return ModConfig.worker5Health;
}

int GetModCult3Health() {
	return ModConfig.cult3Health;
}

int GetModMonk1Health() {
	return ModConfig.monk1Health;
}

int GetModMonk2Health() {
	return ModConfig.monk2Health;
}

int GetModEagleHealth() {
	return ModConfig.eagleHealth;
}

int GetModCrowHealth() {
	return ModConfig.crowHealth;
}

int GetModBigEelHealth() {
	return ModConfig.bigEelHealth;
}

int GetModEelHealth() {
	return ModConfig.eelHealth;
}

int GetModBandit1Health() {
	return ModConfig.bandit1Health;
}

int GetModBandit2Health() {
	return ModConfig.bandit2Health;
}

int GetModBandit2BHealth() {
	return ModConfig.bandit2BHealth;
}

int GetModSkidmanHealth() {
	return ModConfig.skidmanHealth;
}

int GetModXianLordHealth() {
	return ModConfig.xianLordHealth;
}

int GetModWarriorHealth() {
	return ModConfig.warriorHealth;
}

int GetModDragonHealth() {
	return ModConfig.dragonHealth;
}

int GetModGiantYetiHealth() {
	return ModConfig.giantYetiHealth;
}

int GetModDinoHealth() {
	return ModConfig.dinoHealth;
}

bool GetModLaraIgnoreMonkIfNotAngry() {
	return ModConfig.laraIgnoreMonkIfNotAngry;
}

bool GetModMakeMonkAttackLaraDirectly() {
	return ModConfig.makeMonkAttackLaraFirst;
}

bool GetModMakeMercenaryAttackLaraDirectly() {
	return ModConfig.makeMercenaryAttackLaraFirst;
}

BAR_CONFIG* GetModLaraHealthBar() {
	return &ModConfig.healthbar;
}

BAR_CONFIG* GetModLaraAirBar() {
	return &ModConfig.airbar;
}

BAR_CONFIG* GetModEnemyBar() {
	return &ModConfig.enemyhealthbar;
}

const char* GetModLoadingPix() {
	return *ModConfig.loadingPix ? ModConfig.loadingPix : NULL;
}

DWORD GetModWaterColor() {
	return ModConfig.waterColor;
}

bool IsModSemitransConfigLoaded() {
	return ModConfig.semitrans.isLoaded;
}

POLYINDEX* GetModSemitransAnimtexFilter() {
	return ModConfig.semitrans.animtex;
}

POLYFILTER_NODE* GetModSemitransRoomsFilter() {
	return ModConfig.semitrans.rooms;
}

POLYFILTER_NODE* GetModSemitransStaticsFilter() {
	return ModConfig.semitrans.statics;
}

POLYFILTER_NODE** GetModSemitransObjectsFilter() {
	return ModConfig.semitrans.objects;
}

bool IsModReflectConfigLoaded() {
	return ModConfig.reflect.isLoaded;
}

POLYFILTER_NODE* GetModReflectStaticsFilter() {
	return ModConfig.reflect.statics;
}

POLYFILTER_NODE** GetModReflectObjectsFilter() {
	return ModConfig.reflect.objects;
}

static int ParsePolyString(const char* str, POLYINDEX* lst, DWORD lstLen) {
	if (!lst || !lstLen) {
		return -1;
	}

	lst[0].idx = ~0;
	lst[0].num = ~0;

	POLYINDEX* lstBuf = (POLYINDEX*)malloc(lstLen * sizeof(POLYINDEX));
	if (lstBuf == NULL) {
		return -2;
	}

	char* strBuf = _strdup(str);
	if (strBuf == NULL) {
		free(lstBuf);
		return -2;
	}

	DWORD bufLen = 0;
	char* token = strtok(strBuf, ",");
	while (token != NULL) {
		char* range = strchr(token, '-');
		if (range) {
			int from = atoi(token);
			int to = atoi(range + 1);
			lstBuf[bufLen].idx = MIN(to, from);
			lstBuf[bufLen].num = ABS(to - from) + 1;
		}
		else {
			lstBuf[bufLen].idx = atoi(token);
			lstBuf[bufLen].num = 1;
		}
		if (++bufLen >= lstLen) {
			break;
		}
		token = strtok(NULL, ",");
	}

	free(strBuf);
	if (!bufLen) {
		free(lstBuf);
		return 0;
	}

	for (DWORD i = 0; i < bufLen - 1; ++i) {
		for (DWORD j = i + 1; j < bufLen; ++j) {
			if (lstBuf[i].idx > lstBuf[j].idx) {
				POLYINDEX t;
				SWAP(lstBuf[i], lstBuf[j], t);
			}
		}
	}

	lst[0] = lstBuf[0];
	DWORD resLen = 1;

	for (DWORD i = 1; i < bufLen; ++i) {
		int bound = lst[resLen - 1].idx + lst[resLen - 1].num;
		if (lstBuf[i].idx > bound) {
			lst[resLen] = lstBuf[i];
			++resLen;
		}
		else {
			int ext = lstBuf[i].idx + lstBuf[i].num;
			if (ext > bound) {
				lst[resLen - 1].num += ext - bound;
			}
		}
	}
	if (resLen < lstLen) {
		lst[resLen].idx = 0;
		lst[resLen].num = 0;
	}

	free(lstBuf);
	return resLen;
}

static int ParsePolyValue(json_value* value, POLYINDEX* lst, DWORD lstLen) {
	if (!lst || !lstLen) {
		return -1;
	}

	lst[0].idx = ~0;
	lst[0].num = ~0;
	if (value == NULL) {
		return 0;
	}

	const char* str = value->u.string.ptr;
	if (!str || !*str || !strcasecmp(str, "none")) {
		return 0;
	}
	if (!strcasecmp(str, "all")) {
		lst[0].idx = 0;
		lst[0].num = 0;
		return 1;
	}
	return ParsePolyString(str, lst, lstLen);
}

static bool ParsePolyfilterConfiguration(json_value* root, const char* name, POLYFILTER_NODE** pNodes) {
	FreePolyfilterNodes(pNodes);
	if (root == NULL || root->type != json_array || !name || !*name) {
		return false;
	}
	for (DWORD i = 0; i < root->u.array.length; ++i) {
		json_value* item = root->u.array.values[i];
		json_value* field = GetJsonField(item, json_integer, name, NULL);
		if (!field || field->u.integer < 0) continue;
		POLYFILTER* filter = CreatePolyfilterNode(pNodes, (int)field->u.integer);
		if (!filter) continue;
		field = GetJsonField(item, json_object, "filter", NULL);
		if (field) {
			filter->n_vtx = GetJsonIntegerFieldValue(field, "v", 0);
			filter->n_gt4 = GetJsonIntegerFieldValue(field, "t4", 0);
			filter->n_gt3 = GetJsonIntegerFieldValue(field, "t3", 0);
			filter->n_g4 = GetJsonIntegerFieldValue(field, "c4", 0);
			filter->n_g3 = GetJsonIntegerFieldValue(field, "c3", 0);
		}
		json_value* t4list = GetJsonField(item, json_string, "t4list", NULL);
		json_value* t3list = GetJsonField(item, json_string, "t3list", NULL);
		json_value* c4list = GetJsonField(item, json_string, "c4list", NULL);
		json_value* c3list = GetJsonField(item, json_string, "c3list", NULL);
		// If no lists presented, consider that lists set to "all"
		if (t4list || t3list || c4list || c3list) {
			ParsePolyValue(t4list, filter->gt4, ARRAY_SIZE(filter->gt4));
			ParsePolyValue(t3list, filter->gt3, ARRAY_SIZE(filter->gt3));
			ParsePolyValue(c4list, filter->g4, ARRAY_SIZE(filter->g4));
			ParsePolyValue(c3list, filter->g3, ARRAY_SIZE(filter->g3));
		}
	}
	return true;
}

static bool ParseSemitransConfiguration(json_value* root) {
	if (root == NULL || root->type != json_object) {
		return false;
	}
	json_value* field = NULL;

	field = GetJsonField(root, json_string, "animtex", NULL);
	if (field) {
		if (ModConfig.semitrans.animtex) {
			free(ModConfig.semitrans.animtex);
			ModConfig.semitrans.animtex = NULL;
		}
		if (strcasecmp(field->u.string.ptr, "auto")) {
			ModConfig.semitrans.animtex = (POLYINDEX*)malloc(sizeof(POLYINDEX) * POLYFILTER_SIZE);
			if (ModConfig.semitrans.animtex) {
				ParsePolyValue(field, ModConfig.semitrans.animtex, POLYFILTER_SIZE);
			}
		}
	}
	json_value* objects = GetJsonField(root, json_array, "objects", NULL);
	if (objects) {
		for (DWORD i = 0; i < objects->u.array.length; ++i) {
			json_value* object = objects->u.array.values[i];
			field = GetJsonField(object, json_integer, "object", NULL);
			if (!field || field->u.integer < 0 || field->u.integer >= ARRAY_SIZE(ModConfig.semitrans.objects)) continue;
			ParsePolyfilterConfiguration(GetJsonField(object, json_array, "meshes", NULL), "mesh", &ModConfig.semitrans.objects[field->u.integer]);
		}
	}
	ParsePolyfilterConfiguration(GetJsonField(root, json_array, "statics", NULL), "static", &ModConfig.semitrans.statics);
	ParsePolyfilterConfiguration(GetJsonField(root, json_array, "rooms", NULL), "room", &ModConfig.semitrans.rooms);
	ModConfig.semitrans.isLoaded = true;
	return true;
}

static bool ParseReflectConfiguration(json_value* root) {
	if (root == NULL || root->type != json_object) {
		return false;
	}

	json_value* field = NULL;
	json_value* objects = GetJsonField(root, json_array, "objects", NULL);
	if (objects) {
		for (DWORD i = 0; i < objects->u.array.length; ++i) {
			json_value* object = objects->u.array.values[i];
			field = GetJsonField(object, json_integer, "object", NULL);
			if (!field || field->u.integer < 0 || field->u.integer >= ARRAY_SIZE(ModConfig.reflect.objects)) continue;
			ParsePolyfilterConfiguration(GetJsonField(object, json_array, "meshes", NULL), "mesh", &ModConfig.reflect.objects[field->u.integer]);
		}
	}

	ParsePolyfilterConfiguration(GetJsonField(root, json_array, "statics", NULL), "static", &ModConfig.reflect.statics);
	ModConfig.reflect.isLoaded = true;
	return true;
}

static bool ParseBooleanConfigByName(json_value* root, const char* name, bool defaultValue = false) {
	json_value* field = GetJsonField(root, json_boolean, name, NULL);
	if (field) {
		return (bool)field->u.boolean;
	}
	return defaultValue;
}

static int ParseIntegerConfigByName(json_value* root, const char* name, int defaultValue = -1) {
	json_value* field = GetJsonField(root, json_integer, name, NULL);
	if (field) {
		return (int)field->u.integer;
	}
	return defaultValue;
}

static DWORD ParseLongConfigByName(json_value* root, const char* name, DWORD defaultValue = -1) {
	json_value* field = GetJsonField(root, json_integer, name, NULL);
	if (field) {
		return (DWORD)field->u.integer;
	}
	return defaultValue;
}

static DWORD ParseColorConfigByName(json_value* root, const char* name, DWORD defaultValue = -1) {
	json_value* field = GetJsonField(root, json_string, name, NULL);
	if (field && field->u.string.length == 6) {
		return strtol(field->u.string.ptr, NULL, 16);
	}
	return defaultValue;
}

static bool ParseHealthBarConfiguration(json_value* root, BAR_CONFIG* barConfig) {
	if (root == NULL || root->type != json_object) {
		return false;
	}
	barConfig->PC_xpos = ParseIntegerConfigByName(root, "PC_x", 8);
	barConfig->PC_ypos = ParseIntegerConfigByName(root, "PC_y", 8);
	barConfig->PC_color[0] = (INV_COLOURS)ParseIntegerConfigByName(root, "PC_color0", 3);
	barConfig->PC_color[1] = (INV_COLOURS)ParseIntegerConfigByName(root, "PC_color1", 4);
	barConfig->PSX_xpos = ParseIntegerConfigByName(root, "PSX_x", 20);
	barConfig->PSX_ypos = ParseIntegerConfigByName(root, "PSX_y", 18);
	barConfig->PSX_leftcolor[0] = ParseColorConfigByName(root, "PSX_leftcolor0", RGB_MAKE(0x68, 0, 0));
	barConfig->PSX_leftcolor[1] = ParseColorConfigByName(root, "PSX_leftcolor1", RGB_MAKE(0x70, 0, 0));
	barConfig->PSX_leftcolor[2] = ParseColorConfigByName(root, "PSX_leftcolor2", RGB_MAKE(0x98, 0, 0));
	barConfig->PSX_leftcolor[3] = ParseColorConfigByName(root, "PSX_leftcolor3", RGB_MAKE(0xD8, 0, 0));
	barConfig->PSX_leftcolor[4] = ParseColorConfigByName(root, "PSX_leftcolor4", RGB_MAKE(0xE4, 0, 0));
	barConfig->PSX_leftcolor[5] = ParseColorConfigByName(root, "PSX_leftcolor5", RGB_MAKE(0xF0, 0, 0));
	barConfig->PSX_rightcolor[0] = ParseColorConfigByName(root, "PSX_rightcolor0", RGB_MAKE(0, 0x44, 0));
	barConfig->PSX_rightcolor[1] = ParseColorConfigByName(root, "PSX_rightcolor1", RGB_MAKE(0, 0x74, 0));
	barConfig->PSX_rightcolor[2] = ParseColorConfigByName(root, "PSX_rightcolor2", RGB_MAKE(0, 0x9C, 0));
	barConfig->PSX_rightcolor[3] = ParseColorConfigByName(root, "PSX_rightcolor3", RGB_MAKE(0, 0xD4, 0));
	barConfig->PSX_rightcolor[4] = ParseColorConfigByName(root, "PSX_rightcolor4", RGB_MAKE(0, 0xE8, 0));
	barConfig->PSX_rightcolor[5] = ParseColorConfigByName(root, "PSX_rightcolor5", RGB_MAKE(0, 0xFC, 0));
	barConfig->PSX_framecolor[0] = ParseColorConfigByName(root, "PSX_framecolor0", RGB_MAKE(0, 0, 0));
	barConfig->PSX_framecolor[1] = ParseColorConfigByName(root, "PSX_framecolor1", RGB_MAKE(0, 0, 0));
	barConfig->PSX_framecolor[2] = ParseColorConfigByName(root, "PSX_framecolor2", RGB_MAKE(0x50, 0x84, 0x84));
	barConfig->PSX_framecolor[3] = ParseColorConfigByName(root, "PSX_framecolor3", RGB_MAKE(0xA0, 0xA0, 0xA0));
	barConfig->PSX_framecolor[4] = ParseColorConfigByName(root, "PSX_framecolor4", RGB_MAKE(0x28, 0x42, 0x42));
	barConfig->PSX_framecolor[5] = ParseColorConfigByName(root, "PSX_framecolor5", RGB_MAKE(0x50, 0x50, 0x50));
	return true;
}

static bool ParseAirBarConfiguration(json_value* root, BAR_CONFIG* barConfig) {
	if (root == NULL || root->type != json_object) {
		return false;
	}
	barConfig->PC_xpos = ParseIntegerConfigByName(root, "PC_x", 8);
	barConfig->PC_ypos = ParseIntegerConfigByName(root, "PC_y", 8);
	barConfig->PC_color[0] = (INV_COLOURS)ParseIntegerConfigByName(root, "PC_color0", 3);
	barConfig->PC_color[1] = (INV_COLOURS)ParseIntegerConfigByName(root, "PC_color1", 4);
	barConfig->PSX_xpos = ParseIntegerConfigByName(root, "PSX_x", 20);
	barConfig->PSX_ypos = ParseIntegerConfigByName(root, "PSX_y", 18);
	barConfig->PSX_leftcolor[0] = ParseColorConfigByName(root, "PSX_leftcolor0", RGB_MAKE(0, 0x40, 0x54));
	barConfig->PSX_leftcolor[1] = ParseColorConfigByName(root, "PSX_leftcolor1", RGB_MAKE(0, 0x50, 0x64));
	barConfig->PSX_leftcolor[2] = ParseColorConfigByName(root, "PSX_leftcolor2", RGB_MAKE(0, 0x68, 0x74));
	barConfig->PSX_leftcolor[3] = ParseColorConfigByName(root, "PSX_leftcolor3", RGB_MAKE(0, 0x78, 0x84));
	barConfig->PSX_leftcolor[4] = ParseColorConfigByName(root, "PSX_leftcolor4", RGB_MAKE(0, 0x84, 0x8E));
	barConfig->PSX_leftcolor[5] = ParseColorConfigByName(root, "PSX_leftcolor5", RGB_MAKE(0, 0x90, 0x98));
	barConfig->PSX_rightcolor[0] = ParseColorConfigByName(root, "PSX_rightcolor0", RGB_MAKE(0, 0x40, 0));
	barConfig->PSX_rightcolor[1] = ParseColorConfigByName(root, "PSX_rightcolor1", RGB_MAKE(0, 0x50, 0));
	barConfig->PSX_rightcolor[2] = ParseColorConfigByName(root, "PSX_rightcolor2", RGB_MAKE(0, 0x68, 0));
	barConfig->PSX_rightcolor[3] = ParseColorConfigByName(root, "PSX_rightcolor3", RGB_MAKE(0, 0x78, 0));
	barConfig->PSX_rightcolor[4] = ParseColorConfigByName(root, "PSX_rightcolor4", RGB_MAKE(0, 0x84, 0));
	barConfig->PSX_rightcolor[5] = ParseColorConfigByName(root, "PSX_rightcolor5", RGB_MAKE(0, 0x90, 0));
	barConfig->PSX_framecolor[0] = ParseColorConfigByName(root, "PSX_framecolor0", RGB_MAKE(0, 0, 0));
	barConfig->PSX_framecolor[1] = ParseColorConfigByName(root, "PSX_framecolor1", RGB_MAKE(0, 0, 0));
	barConfig->PSX_framecolor[2] = ParseColorConfigByName(root, "PSX_framecolor2", RGB_MAKE(0x50, 0x84, 0x84));
	barConfig->PSX_framecolor[3] = ParseColorConfigByName(root, "PSX_framecolor3", RGB_MAKE(0xA0, 0xA0, 0xA0));
	barConfig->PSX_framecolor[4] = ParseColorConfigByName(root, "PSX_framecolor4", RGB_MAKE(0x28, 0x42, 0x42));
	barConfig->PSX_framecolor[5] = ParseColorConfigByName(root, "PSX_framecolor5", RGB_MAKE(0x50, 0x50, 0x50));
	return true;
}

static bool ParseEnemyBarConfiguration(json_value* root, BAR_CONFIG* barConfig) {
	if (root == NULL || root->type != json_object) {
		return false;
	}
	barConfig->PC_xpos = ParseIntegerConfigByName(root, "PC_x", 8);
	barConfig->PC_ypos = ParseIntegerConfigByName(root, "PC_y", 8);
	barConfig->PC_color[0] = (INV_COLOURS)ParseIntegerConfigByName(root, "PC_color0", 3);
	barConfig->PC_color[1] = (INV_COLOURS)ParseIntegerConfigByName(root, "PC_color1", 4);
	barConfig->PSX_xpos = ParseIntegerConfigByName(root, "PSX_x", 20);
	barConfig->PSX_ypos = ParseIntegerConfigByName(root, "PSX_y", 18);
	barConfig->PSX_leftcolor[0] = ParseColorConfigByName(root, "PSX_leftcolor0", RGB_MAKE(0x68, 0, 0));
	barConfig->PSX_leftcolor[1] = ParseColorConfigByName(root, "PSX_leftcolor1", RGB_MAKE(0x70, 0, 0));
	barConfig->PSX_leftcolor[2] = ParseColorConfigByName(root, "PSX_leftcolor2", RGB_MAKE(0x98, 0, 0));
	barConfig->PSX_leftcolor[3] = ParseColorConfigByName(root, "PSX_leftcolor3", RGB_MAKE(0xD8, 0, 0));
	barConfig->PSX_leftcolor[4] = ParseColorConfigByName(root, "PSX_leftcolor4", RGB_MAKE(0xE4, 0, 0));
	barConfig->PSX_leftcolor[5] = ParseColorConfigByName(root, "PSX_leftcolor5", RGB_MAKE(0xFF, 0, 0));
	barConfig->PSX_rightcolor[0] = ParseColorConfigByName(root, "PSX_rightcolor0", RGB_MAKE(0x31, 0, 0));
	barConfig->PSX_rightcolor[1] = ParseColorConfigByName(root, "PSX_rightcolor1", RGB_MAKE(0x3A, 0, 0));
	barConfig->PSX_rightcolor[2] = ParseColorConfigByName(root, "PSX_rightcolor2", RGB_MAKE(0x40, 0, 0));
	barConfig->PSX_rightcolor[3] = ParseColorConfigByName(root, "PSX_rightcolor3", RGB_MAKE(0x78, 0, 0));
	barConfig->PSX_rightcolor[4] = ParseColorConfigByName(root, "PSX_rightcolor4", RGB_MAKE(0x9E, 0, 0));
	barConfig->PSX_rightcolor[5] = ParseColorConfigByName(root, "PSX_rightcolor5", RGB_MAKE(0xDC, 0, 0));
	barConfig->PSX_framecolor[0] = ParseColorConfigByName(root, "PSX_framecolor0", RGB_MAKE(0, 0, 0));
	barConfig->PSX_framecolor[1] = ParseColorConfigByName(root, "PSX_framecolor1", RGB_MAKE(0, 0, 0));
	barConfig->PSX_framecolor[2] = ParseColorConfigByName(root, "PSX_framecolor2", RGB_MAKE(0x50, 0x84, 0x84));
	barConfig->PSX_framecolor[3] = ParseColorConfigByName(root, "PSX_framecolor3", RGB_MAKE(0xA0, 0xA0, 0xA0));
	barConfig->PSX_framecolor[4] = ParseColorConfigByName(root, "PSX_framecolor4", RGB_MAKE(0x28, 0x42, 0x42));
	barConfig->PSX_framecolor[5] = ParseColorConfigByName(root, "PSX_framecolor5", RGB_MAKE(0x50, 0x50, 0x50));
	return true;
}

static bool ParseDefaultLevelConfiguration(json_value* root) {
	if (root == NULL || root->type != json_object) {
		return false;
	}

	ModConfig.pistolsAtStart = ParseBooleanConfigByName(root, "pistolsatstart", true);
	ModConfig.shotgunAtStart = ParseBooleanConfigByName(root, "shotgunatstart", true);
	ModConfig.uzisAtStart = ParseBooleanConfigByName(root, "uzisatstart");
	ModConfig.autopistolsAtStart = ParseBooleanConfigByName(root, "autopistolsatstart");
	ModConfig.m16AtStart = ParseBooleanConfigByName(root, "m16atstart");
	ModConfig.grenadeAtStart = ParseBooleanConfigByName(root, "grenadeatstart");
	ModConfig.harpoonAtStart = ParseBooleanConfigByName(root, "harpoonatstart");

	ModConfig.shotgunAmmoAtStart = ParseIntegerConfigByName(root, "shotgunammoatstart", ModConfig.shotgunAtStart ? 12 : 0);
	ModConfig.uzisAmmoAtStart = ParseIntegerConfigByName(root, "uzisammoatstart");
	ModConfig.autopistolsAmmoAtStart = ParseIntegerConfigByName(root, "autopistolsammoatstart");
	ModConfig.m16AmmoAtStart = ParseIntegerConfigByName(root, "m16ammoatstart");
	ModConfig.grenadeAmmoAtStart = ParseIntegerConfigByName(root, "grenadeammoatstart");
	ModConfig.harpoonAmmoAtStart = ParseIntegerConfigByName(root, "harpoonammoatstart");

	ModConfig.flareCountAtStart = ParseIntegerConfigByName(root, "flaresatstart", 2);
	ModConfig.smallMedikitAtStart = ParseIntegerConfigByName(root, "smallmedikitatstart", 1);
	ModConfig.bigMedikitAtStart = ParseIntegerConfigByName(root, "bigmedikitatstart", 1);

	ParseHealthBarConfiguration(GetJsonField(root, json_object, "larahealthbar", NULL), &ModConfig.healthbar);
	ParseAirBarConfiguration(GetJsonField(root, json_object, "laraairbar", NULL), &ModConfig.airbar);
	ParseEnemyBarConfiguration(GetJsonField(root, json_object, "enemyhealthbar", NULL), &ModConfig.enemyhealthbar);

	ParseSemitransConfiguration(GetJsonField(root, json_object, "semitransparent", NULL));
	ParseReflectConfiguration(GetJsonField(root, json_object, "reflective", NULL));
	return true;
}

static bool ParseLevelConfiguration(json_value* root) {
	if (root == NULL || root->type != json_object) {
		return false;
	}
	json_value* field = NULL;
	field = GetJsonField(root, json_string, "picture", NULL);
	if (field) {
		snprintf(ModConfig.loadingPix, sizeof(ModConfig.loadingPix), "data\\%.*s.pcx", field->u.string.length, field->u.string.ptr);
	}
	field = GetJsonField(root, json_string, "watercolor", NULL);
	if (field && field->u.string.length == 6) {
		ModConfig.waterColor = strtol(field->u.string.ptr, NULL, 16);
	}

	ModConfig.isBarefoot = ParseBooleanConfigByName(root, "barefoot");

	ModConfig.dogHealth = ParseIntegerConfigByName(root, "dogHealth", 10);
	ModConfig.mouseHealth = ParseIntegerConfigByName(root, "mouseHealth", 4);
	ModConfig.cult1Health = ParseIntegerConfigByName(root, "cult1Health", 25);
	ModConfig.cult1AHealth = ParseIntegerConfigByName(root, "cult1AHealth", 25);
	ModConfig.cult1BHealth = ParseIntegerConfigByName(root, "cult1BHealth", 25);
	ModConfig.cult2Health = ParseIntegerConfigByName(root, "cult2Health", 60);
	ModConfig.sharkHealth = ParseIntegerConfigByName(root, "sharkHealth", 30);
	ModConfig.tigerHealth = ParseIntegerConfigByName(root, "tigerHealth", 20);
	ModConfig.barracudaHealth = ParseIntegerConfigByName(root, "barracudaHealth", 12);
	ModConfig.smallSpiderHealth = ParseIntegerConfigByName(root, "smallSpiderHealth", 5);
	ModConfig.wolfHealth = ParseIntegerConfigByName(root, "wolfHealth", 10);
	ModConfig.bigSpiderHealth = ParseIntegerConfigByName(root, "bigSpiderHealth", 40);
	ModConfig.bearHealth = ParseIntegerConfigByName(root, "bearHealth", 30);
	ModConfig.yetiHealth = ParseIntegerConfigByName(root, "yetiHealth", 30);
	ModConfig.jellyHealth = ParseIntegerConfigByName(root, "jellyHealth", 10);
	ModConfig.diverHealth = ParseIntegerConfigByName(root, "diverHealth", 20);
	ModConfig.worker1Health = ParseIntegerConfigByName(root, "worker1Health", 25);
	ModConfig.worker2Health = ParseIntegerConfigByName(root, "worker2Health", 20);
	ModConfig.worker3Health = ParseIntegerConfigByName(root, "worker3Health", 27);
	ModConfig.worker4Health = ParseIntegerConfigByName(root, "worker4Health", 27);
	ModConfig.worker5Health = ParseIntegerConfigByName(root, "worker5Health", 20);
	ModConfig.cult3Health = ParseIntegerConfigByName(root, "cult3Health", 150);
	ModConfig.monk1Health = ParseIntegerConfigByName(root, "monk1Health", 30);
	ModConfig.monk2Health = ParseIntegerConfigByName(root, "monk2Health", 30);
	ModConfig.eagleHealth = ParseIntegerConfigByName(root, "eagleHealth", 20);
	ModConfig.crowHealth = ParseIntegerConfigByName(root, "crowHealth", 15);
	ModConfig.bigEelHealth = ParseIntegerConfigByName(root, "bigEelHealth", 20);
	ModConfig.eelHealth = ParseIntegerConfigByName(root, "eelHealth", 5);
	ModConfig.bandit1Health = ParseIntegerConfigByName(root, "bandit1Health", 45);
	ModConfig.bandit2Health = ParseIntegerConfigByName(root, "bandit2Health", 50);
	ModConfig.bandit2BHealth = ParseIntegerConfigByName(root, "bandit2BHealth", 50);
	ModConfig.skidmanHealth = ParseIntegerConfigByName(root, "skidmanHealth", 100);
	ModConfig.xianLordHealth = ParseIntegerConfigByName(root, "xianLordHealth", 100);
	ModConfig.warriorHealth = ParseIntegerConfigByName(root, "warriorHealth", 80);
	ModConfig.dragonHealth = ParseIntegerConfigByName(root, "dragonHealth", 300);
	ModConfig.giantYetiHealth = ParseIntegerConfigByName(root, "giantYetiHealth", 200);
	ModConfig.dinoHealth = ParseIntegerConfigByName(root, "dinoHealth", 100);
	ModConfig.laraIgnoreMonkIfNotAngry = ParseBooleanConfigByName(root, "laraignoremonkifnotangry");
	ModConfig.makeMercenaryAttackLaraFirst = ParseBooleanConfigByName(root, "mercenaryattacklaradirectly");
	ModConfig.makeMonkAttackLaraFirst = ParseBooleanConfigByName(root, "monksattacklaradirectly");

	ParseSemitransConfiguration(GetJsonField(root, json_object, "semitransparent", NULL));
	ParseReflectConfiguration(GetJsonField(root, json_object, "reflective", NULL));
	return true;
}

static bool ParseModConfiguration(char* levelName, json_value* root) {
	if (root == NULL || root->type != json_object) {
		return false;
	}
	// parsing default configs
	ParseDefaultLevelConfiguration(GetJsonField(root, json_object, "default", NULL));

	// parsing level specific configs
	json_value* levels = GetJsonField(root, json_array, "levels", NULL);
	if (levels) ParseLevelConfiguration(GetJsonObjectByStringField(levels, "filename", levelName, false, NULL));
	return true;
}

void UnloadModConfiguration() {
	if (ModConfig.semitrans.animtex) {
		free(ModConfig.semitrans.animtex);
		ModConfig.semitrans.animtex = NULL;
	}
	FreePolyfilterNodes(&ModConfig.semitrans.rooms);
	FreePolyfilterNodes(&ModConfig.semitrans.statics);
	FreePolyfilterNodes(&ModConfig.reflect.statics);
	for (DWORD i = 0; i < ARRAY_SIZE(ModConfig.semitrans.objects); ++i) {
		FreePolyfilterNodes(&ModConfig.semitrans.objects[i]);
		FreePolyfilterNodes(&ModConfig.reflect.objects[i]);
	}
	memset(&ModConfig, 0, sizeof(ModConfig));
}

bool LoadModConfiguration(LPCTSTR levelFilePath) {
	UnloadModConfiguration();
	if (!PathFileExists(MOD_CONFIG_NAME)) {
		return false;
	}
	char levelName[256] = { 0 };
	strncpy(levelName, PathFindFileName(levelFilePath), sizeof(levelName) - 1);
	char* ext = PathFindExtension(levelName);
	if (ext != NULL) *ext = 0;

	DWORD bytesRead = 0;
	HANDLE hFile = CreateFile(MOD_CONFIG_NAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return false;
	}
	DWORD cfgSize = GetFileSize(hFile, NULL);
	void* cfgData = malloc(cfgSize);
	ReadFile(hFile, cfgData, cfgSize, &bytesRead, NULL);
	CloseHandle(hFile);

	json_value* json = json_parse((const json_char*)cfgData, cfgSize);
	if (json != NULL) {
		ModConfig.isLoaded = ParseModConfiguration(levelName, json);
	}
	json_value_free(json);
	free(cfgData);
	return ModConfig.isLoaded;
}
#endif // FEATURE_MOD_CONFIG