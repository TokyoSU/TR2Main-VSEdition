#pragma once
#include "global/types.h"
#include "global/vars.h"

struct ModConfig
{
	ModConfig() {
		Initialize();
	}
	~ModConfig() {
		Release();
	}

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

	bool isDartEffectOpaque = false;
	bool isFlameOpaque = false;
	bool isLavaFountainOpaque = false;
	bool isDragonFlameOpaque = false;
	bool isSphereOfDoom1Opaque = false;
	bool isSphereOfDoom2Opaque = false;
	bool isBloodOpaque = false;
	bool isExplosionOpaque = false;
	bool isTwinkleOpaque = false;
	bool isSplashOpaque = false;
	bool isWaterSpriteOpaque = false;
	bool isHotLiquidOpaque = false;
	bool isRainOpaque = false;
	bool isSnowOpaque = true;

	short shotgunAmmoAtStart = 2;
	short uzisAmmoAtStart = 0;
	short autoPistolAmmoAtStart = 0;
	short m16AmmoAtStart = 0;
	short grenadeAmmoAtStart = 0;
	short harpoonAmmoAtStart = 0;

	short flareAtStart = 2;
	short smallMediAtStart = 1;
	short bigMediAtStart = 1;

	DWORD rainSplashColor = RGB_MAKE(255, 255, 255);
	short rainSpriteScale = 1024;
	short rainDensity = 0;
	short rainSplashSize = 128;
	short rainDamageRange = CLICK(1);
	BYTE rainDamage = 20;
	bool rainEnabled = false;
	bool rainSplashEnabled = true;
	bool rainDoDamageOnHit = false;

	short snowSpriteScale = 1024;
	short snowDensity = 0;
	bool snowEnabled = false;

	std::string levelLoadingPix;
	std::string titleLoadingPixLanguage;
	std::string titleLoadingPix;
	std::string titleLoadingPixGold;

	bool isUIColorLoaded = false;

	CUST_UNDERWATER_INFO underwater;
	D3DCOLOR waterColor;
	CUST_INVENTORY_ITEM invItemList[MAX_ITEM_IN_INVENTORY];
	LARA_BAR_CONFIG laraBar;
	BAR_CONFIG enemyBar;
	ENEMY_HEALTH_INFO enemyHealth;
	SEMITRANS_CONFIG semitrans;
	REFLECT_CONFIG reflect;

	void Initialize();
	void Release();
	bool LoadJson(LPCSTR filePath);
};

extern bool GetCustomItemFromObjectID(int objectID, CUST_INVENTORY_ITEM& invItem);

extern void LoadHealthBarConfig(Value& data, BAR_CONFIG* result);
extern void LoadAirBarConfig(Value& data, BAR_CONFIG* result);
extern void LoadCustomInventoryItems(Value& data);
extern void LoadLevelConfig(Value& data);
extern void LoadSemitransConfig(Value& data, SEMITRANS_CONFIG* semitrans);
extern void LoadPolyfilterConfig(Value& data, LPCSTR name, POLYFILTER_NODE** filterNodes);
extern void LoadReflectConfig(Value& data, REFLECT_CONFIG* reflect);
extern void LoadUIConfig(Value& data, LPCSTR name);
extern void LoadUIConfigDefault();

extern void ParseDefaultConfiguration(Value& data);
extern void ParseLevelConfiguration(Value& data, LPCSTR currentLevel);

extern POLYFILTER* CreatePolyfilterNode(POLYFILTER_NODE** root, int id);
extern void FreePolyfilterNodes(POLYFILTER_NODE** root);
extern bool IsCompatibleFilterObjects(short* ptrObj, POLYFILTER* filter);
extern short* EnumeratePolysSpecificObjects(short* ptrObj, int vtxCount, bool colored, ENUM_POLYS_OBJECTS_CB callback, POLYINDEX* filter, LPVOID param);
extern void EnumeratePolysSpecificRoomFace4(FACE4* ptrObj, int faceCount, bool colored, ENUM_POLYS_FACE4_CB callback, POLYINDEX* filter, LPVOID param);
extern void EnumeratePolysSpecificRoomFace3(FACE3* ptrObj, int faceCount, bool colored, ENUM_POLYS_FACE3_CB callback, POLYINDEX* filter, LPVOID param);
extern bool EnumeratePolysObjects(short* ptrObj, ENUM_POLYS_OBJECTS_CB callback, POLYFILTER* filter, LPVOID param);
extern bool EnumeratePolysRoomFace3(FACE3* ptrObj, int faceCount, ENUM_POLYS_FACE3_CB callback, POLYFILTER* filter, LPVOID param);
extern bool EnumeratePolysRoomFace4(FACE4* ptrObj, int faceCount, ENUM_POLYS_FACE4_CB callback, POLYFILTER* filter, LPVOID param);
extern int ParsePolyString(LPCSTR str, POLYINDEX* lst, DWORD lstLen);
extern int ParsePolyValue(std::string value, POLYINDEX* lst, DWORD lstLen);

extern ModConfig Mod;
