#pragma once
#include "global/types.h"

#if defined(FEATURE_MOD_CONFIG)
class ModConfig
{
public:
	ModConfig() {
		Initialize();
	}
	~ModConfig() {
		Release();
	}
	void Initialize();
	void Release();
public:
	bool LoadJson(LPCSTR filePath);
public:
	bool isLoaded = false;
	bool isBarefoot = false;
	bool disableGiantYetiNextLevelOnDeath = false;
	bool laraIgnoreMonkIfNotAngry = true;
	bool makeMonkAttackLaraFirst = false;
	bool makeMercenaryAttackLaraFirst = false;
	bool enemyBarEnabled = true;
	bool makeYetiExplodeOnDeath = false;

	bool pistolAtStart = true;
	bool shotgunAtStart = true;
	bool uzisAtStart = false;
	bool autoPistolAtStart = false;
	bool m16AtStart = false;
	bool grenadeAtStart = false;
	bool harpoonAtStart = false;

	short shotgunAmmoAtStart = 2;
	short uzisAmmoAtStart = 0;
	short autoPistolAmmoAtStart = 0;
	short m16AmmoAtStart = 0;
	short grenadeAmmoAtStart = 0;
	short harpoonAmmoAtStart = 0;

	short flareAtStart = 2;
	short smallMediAtStart = 1;
	short bigMediAtStart = 1;

	D3DCOLOR waterColor;
	bool loadingPixFound = false;
	char loadingPix[256];

	LARA_BAR_CONFIG laraBar;
	BAR_CONFIG enemyBar;
	ENEMY_HEALTH_INFO enemyHealth;
	SEMITRANS_CONFIG semitrans;
	REFLECT_CONFIG reflect;
private: // Loader for data inside json:
	void LoadHealthBarConfig(Value& data, BAR_CONFIG* result);
	void LoadAirBarConfig(Value& data, BAR_CONFIG* result);
	void LoadLevelConfig(Value& data);
	void LoadSemitransConfig(Value& data, SEMITRANS_CONFIG* semitrans);
	void LoadPolyfilterConfig(Value& data, LPCSTR name, POLYFILTER_NODE** filterNodes);
	void LoadReflectConfig(Value& data, REFLECT_CONFIG* reflect);
private:
	void ParseDefaultConfiguration(Value& data);
	void ParseLevelConfiguration(Value& data, LPCSTR currentLevel);

public: // Utilities.
	POLYFILTER* CreatePolyfilterNode(POLYFILTER_NODE** root, int id);
	void FreePolyfilterNodes(POLYFILTER_NODE** root);
	bool IsCompatibleFilterObjects(short* ptrObj, POLYFILTER* filter);
	short* EnumeratePolysSpecificObjects(short* ptrObj, int vtxCount, bool colored, ENUM_POLYS_OBJECTS_CB callback, POLYINDEX* filter, LPVOID param);
	void EnumeratePolysSpecificRoomFace4(FACE4* ptrObj, int faceCount, bool colored, ENUM_POLYS_FACE4_CB callback, POLYINDEX* filter, LPVOID param);
	void EnumeratePolysSpecificRoomFace3(FACE3* ptrObj, int faceCount, bool colored, ENUM_POLYS_FACE3_CB callback, POLYINDEX* filter, LPVOID param);
	bool EnumeratePolysObjects(short* ptrObj, ENUM_POLYS_OBJECTS_CB callback, POLYFILTER* filter, LPVOID param);
	bool EnumeratePolysRoomFace3(FACE3* ptrObj, int faceCount, ENUM_POLYS_FACE3_CB callback, POLYFILTER* filter, LPVOID param);
	bool EnumeratePolysRoomFace4(FACE4* ptrObj, int faceCount, ENUM_POLYS_FACE4_CB callback, POLYFILTER* filter, LPVOID param);
	int ParsePolyString(LPCSTR str, POLYINDEX* lst, DWORD lstLen);
	int ParsePolyValue(LPCSTR value, POLYINDEX* lst, DWORD lstLen);
};

extern ModConfig Mod;
#endif
