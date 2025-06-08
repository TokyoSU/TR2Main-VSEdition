#include "precompiled.h"
#include "game/secrets.h"

#if defined(FEATURE_MOD_CONFIG)
#include "modding/mod_utils.h"
#endif

bool IsSecret(int objNumber)
{
#if defined(FEATURE_MOD_CONFIG)
    if (Mod.useNewVersion && Mod.newVersion >= 1)
        return objNumber == ID_SECRET2_OR_SECRET;
#endif
    return objNumber == ID_SECRET1_OR_SECRET_SPRITE || objNumber == ID_SECRET2_OR_SECRET || objNumber == ID_SECRET3;
}

BOOL AddSecret(int secretType)
{
	if (secretType != -1)
	{
		switch (secretType)
		{
		case 0:
			SaveGame.statistics.secrets |= 1;
			break;
		case 1:
			SaveGame.statistics.secrets |= 2;
			break;
		case 2:
			SaveGame.statistics.secrets |= 4;
			break;
		case 3:
			SaveGame.statistics.secrets |= 8;
			break;
		}
		return TRUE;
	}
	return FALSE;
}

int GetSecretType(ITEM_INFO* item)
{
#if defined(FEATURE_MOD_CONFIG)
    if (Mod.useNewVersion && Mod.newVersion >= 1)
		return item->objectID == ID_SECRET2_OR_SECRET ? item->ocb : -1; // Can go to secret 3 !
#endif
	switch (item->objectID)
	{
	case ID_SECRET1_OR_SECRET_SPRITE:
		return 0;
	case ID_SECRET2_OR_SECRET:
		return 1;
	case ID_SECRET3:
		return 2;
	}
	return -1;
}

int GetSecretMesh(int objNumber, int secretType)
{
#if defined(FEATURE_MOD_CONFIG)
	if (Mod.useNewVersion && Mod.newVersion >= 1)
		return Objects[objNumber].meshIndex + secretType; // Can go to secret 3 !
#endif
	return Objects[objNumber].meshIndex;
}
