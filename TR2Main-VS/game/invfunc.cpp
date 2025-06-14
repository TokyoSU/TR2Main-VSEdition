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

#include "precompiled.h"
#include "game/invfunc.h"
#include "3dsystem/3d_gen.h"
#include "game/health.h"
#include "game/items.h"
#include "game/text.h"
#include "game/secrets.h"
#include "specific/frontend.h"
#include "global/vars.h"

#if defined(FEATURE_MOD_CONFIG)
#include "modding/mod_utils.h"
#endif

 // 0=name
 // 1=count
 // 2=description
TEXT_STR_INFO* InvItemText[3];

 // X coordinates relative to the center of the screen
 // Y coordinates relative to the bottom of the screen
#define ITEMNAME_X_POS		(0)
#define ITEMNAME_Y_POS		(-16)
#define ITEMCOUNT_X_POS		(64)
#define ITEMCOUNT_Y_POS		(-56)

void InitColours() {
	InvColours[ICLR_Black] = S_COLOUR(0x00, 0x00, 0x00);
	InvColours[ICLR_Gray] = S_COLOUR(0x40, 0x40, 0x40);
	InvColours[ICLR_White] = S_COLOUR(0xFF, 0xFF, 0xFF);
	InvColours[ICLR_Red] = S_COLOUR(0xFF, 0x00, 0x00);
	InvColours[ICLR_Orange] = S_COLOUR(0xFF, 0x80, 0x00);
	InvColours[ICLR_Yellow] = S_COLOUR(0xFF, 0xFF, 0x00);
	InvColours[ICLR_DarkGreen] = S_COLOUR(0x00, 0x80, 0x00);
	InvColours[ICLR_Green] = S_COLOUR(0x00, 0xFF, 0x00);
	InvColours[ICLR_Cyan] = S_COLOUR(0x00, 0xFF, 0xFF);
	InvColours[ICLR_Blue] = S_COLOUR(0x00, 0x00, 0xFF);
	InvColours[ICLR_Magenta] = S_COLOUR(0xFF, 0x00, 0xFF);
}

void RingIsOpen(RING_INFO* ring) {
	GAME_STRING_ID strID;

	if (InventoryMode == INV_TitleMode)
		return;

	if (InvRingText == NULL) {
		switch (ring->type) {
		case RING_Main:
			strID = GSI_Heading_Inventory;
			break;

		case RING_Option:
			strID = (InventoryMode == INV_DeathMode) ? GSI_Heading_GameOver : GSI_Heading_Option;
			break;

		case RING_Keys:
			strID = GSI_Heading_Items;
			break;

		default:
			return;
		}
		InvRingText = T_Print(0, 26, 0, GF_GameStringTable[strID]);
		T_CentreH(InvRingText, 1);
	}

	if (InventoryMode == INV_KeysMode || InventoryMode == INV_DeathMode)
		return;

	if (InvUpArrow1 == NULL) {
		if (ring->type == RING_Option || (ring->type == RING_Main && InvKeyObjectsCount != 0)) {
			InvUpArrow1 = T_Print(20, 28, 0, "["); // '[' is Up Arrow Sprite here
			InvUpArrow2 = T_Print(-20, 28, 0, "[");
			T_RightAlign(InvUpArrow2, 1);
		}
	}

	if (InvDownArrow1 == NULL) {
		if (ring->type == RING_Keys || (ring->type == RING_Main && !CHK_ANY(GF_GameFlow.flags, GFF_LockoutOptionRing))) {
			InvDownArrow1 = T_Print(20, -15, 0, "]"); // ']' is Down Arrow Sprite here
			InvDownArrow2 = T_Print(-20, -15, 0, "]");
			T_BottomAlign(InvDownArrow1, 1);
			T_BottomAlign(InvDownArrow2, 1);
			T_RightAlign(InvDownArrow2, 1);
		}
	}
}

void RingIsNotOpen() {
	T_RemovePrint(InvRingText);
	InvRingText = NULL;
	T_RemovePrint(InvUpArrow1);
	InvUpArrow1 = NULL;
	T_RemovePrint(InvUpArrow2);
	InvUpArrow2 = NULL;
	T_RemovePrint(InvDownArrow1);
	InvDownArrow1 = NULL;
	T_RemovePrint(InvDownArrow2);
	InvDownArrow2 = NULL;
}

void RingNotActive(INVENTORY_ITEM* item) {
	int itemCount;
	char strBuf[64];

	// Keys
	if (InvItemText[0] == NULL) {
		switch (item->objectID) {
		case ID_PASSPORT_OPTION:
			break;

		case ID_PUZZLE_OPTION1:
			InvItemText[0] = T_Print(ITEMNAME_X_POS, ITEMNAME_Y_POS, 0, GF_Puzzle1StringTable[CurrentLevel]);
			break;

		case ID_PUZZLE_OPTION2:
			InvItemText[0] = T_Print(ITEMNAME_X_POS, ITEMNAME_Y_POS, 0, GF_Puzzle2StringTable[CurrentLevel]);
			break;

		case ID_PUZZLE_OPTION3:
			InvItemText[0] = T_Print(ITEMNAME_X_POS, ITEMNAME_Y_POS, 0, GF_Puzzle3StringTable[CurrentLevel]);
			break;

		case ID_PUZZLE_OPTION4:
			InvItemText[0] = T_Print(ITEMNAME_X_POS, ITEMNAME_Y_POS, 0, GF_Puzzle4StringTable[CurrentLevel]);
			break;

		case ID_PICKUP_OPTION1:
			InvItemText[0] = T_Print(ITEMNAME_X_POS, ITEMNAME_Y_POS, 0, GF_Pickup1StringTable[CurrentLevel]);
			break;

		case ID_PICKUP_OPTION2:
			InvItemText[0] = T_Print(ITEMNAME_X_POS, ITEMNAME_Y_POS, 0, GF_Pickup2StringTable[CurrentLevel]);
			break;

		case ID_KEY_OPTION1:
			InvItemText[0] = T_Print(ITEMNAME_X_POS, ITEMNAME_Y_POS, 0, GF_Key1StringTable[CurrentLevel]);
			break;

		case ID_KEY_OPTION2:
			InvItemText[0] = T_Print(ITEMNAME_X_POS, ITEMNAME_Y_POS, 0, GF_Key2StringTable[CurrentLevel]);
			break;

		case ID_KEY_OPTION3:
			InvItemText[0] = T_Print(ITEMNAME_X_POS, ITEMNAME_Y_POS, 0, GF_Key3StringTable[CurrentLevel]);
			break;

		case ID_KEY_OPTION4:
			InvItemText[0] = T_Print(ITEMNAME_X_POS, ITEMNAME_Y_POS, 0, GF_Key4StringTable[CurrentLevel]);
			break;

		default:
			InvItemText[0] = T_Print(ITEMNAME_X_POS, ITEMNAME_Y_POS, 0, item->lpString);
			break;
		}

		if (InvItemText[0] != NULL) {
			T_BottomAlign(InvItemText[0], 1);
			T_CentreH(InvItemText[0], 1);
		}
	}

	// Normal items
	itemCount = Inv_RequestItem((GAME_OBJECT_ID)item->objectID);
	switch (item->objectID) {
	case ID_PISTOL_OPTION:
		return;

	case ID_SHOTGUN_OPTION:
		if (InvItemText[1] != NULL || SaveGame.bonusFlag) {
			return;
		}
		sprintf(strBuf, "%5d", (int)(Lara.shotgun_ammo / SHOTGUN_AMMO_SHOOT));
		break;

	case ID_MAGNUM_OPTION:
		if (InvItemText[1] != NULL || SaveGame.bonusFlag) {
			return;
		}
		sprintf(strBuf, "%5d", (int)Lara.magnum_ammo);
		break;

	case ID_UZI_OPTION:
		if (InvItemText[1] != NULL || SaveGame.bonusFlag) {
			return;
		}
		sprintf(strBuf, "%5d", (int)Lara.uzi_ammo);
		break;

	case ID_HARPOON_OPTION:
		if (InvItemText[1] != NULL || SaveGame.bonusFlag) {
			return;
		}
		sprintf(strBuf, "%5d", (int)Lara.harpoon_ammo);
		break;

	case ID_M16_OPTION:
		if (InvItemText[1] != NULL || SaveGame.bonusFlag) {
			return;
		}
		sprintf(strBuf, "%5d", (int)Lara.m16_ammo);
		break;

	case ID_GRENADE_OPTION:
		if (InvItemText[1] != NULL || SaveGame.bonusFlag) {
			return;
		}
		sprintf(strBuf, "%5d", (int)Lara.grenade_ammo);
		break;

	case ID_PISTOL_AMMO_OPTION:
		return;

	case ID_SHOTGUN_AMMO_OPTION:
		if (InvItemText[1] != NULL || SaveGame.bonusFlag) {
			return;
		}
		sprintf(strBuf, "%d", itemCount * 2);
		break;

	case ID_MAGNUM_AMMO_OPTION:
	case ID_UZI_AMMO_OPTION:
	case ID_HARPOON_AMMO_OPTION:
	case ID_M16_AMMO_OPTION:
		if (InvItemText[1] != NULL || SaveGame.bonusFlag) {
			return;
		}
		sprintf(strBuf, "%d", itemCount * 2);
		break;

	case ID_GRENADE_AMMO_OPTION:
	case ID_FLARES_OPTION:
		if (InvItemText[1] != NULL || SaveGame.bonusFlag) {
			return;
		}
		sprintf(strBuf, "%d", itemCount);
		break;

	case ID_SMALL_MEDIPACK_OPTION:
	case ID_LARGE_MEDIPACK_OPTION:
	case ID_PUZZLE_OPTION1:
	case ID_PUZZLE_OPTION2:
	case ID_PUZZLE_OPTION3:
	case ID_PUZZLE_OPTION4:
	case ID_KEY_OPTION1:
	case ID_KEY_OPTION2:
	case ID_KEY_OPTION3:
	case ID_KEY_OPTION4:
	case ID_PICKUP_OPTION1:
	case ID_PICKUP_OPTION2:
		if (item->objectID == ID_SMALL_MEDIPACK_OPTION || item->objectID == ID_LARGE_MEDIPACK_OPTION)
		{
			HealthBarTimer = 40; // 1.33 seconds
			DrawHealthBar(FlashIt());
		}

		if (InvItemText[1] != NULL || itemCount <= 1) {
			return;
		}

		sprintf(strBuf, "%d", itemCount);
		break;

	default:
		return;
	}

	if (InvItemText[1] == NULL) {
		MakeAmmoString(strBuf);
		InvItemText[1] = T_Print(ITEMCOUNT_X_POS, ITEMCOUNT_Y_POS, 0, strBuf);
		T_BottomAlign(InvItemText[1], true);
		T_CentreH(InvItemText[1], true);
	}

}

void RingActive() {
	T_RemovePrint(InvItemText[0]);
	InvItemText[0] = NULL;
	T_RemovePrint(InvItemText[1]);
	InvItemText[1] = NULL;
}

void RingExamineSelected(INVENTORY_ITEM* item) {
#if defined(FEATURE_MOD_CONFIG)
	if (InvItemText[2] == NULL) {
		CUST_INVENTORY_ITEM invItem;
		if (GetCustomItemFromObjectID(item->objectID, invItem))
		{
			if (invItem.canExamine && invItem.message.size() > 0)
			{
				InvItemText[2] = T_Print(invItem.message_pos_x, ITEMCOUNT_Y_POS + invItem.message_pos_y, 0, invItem.message.c_str());
				T_BottomAlign(InvItemText[2], true);
				T_CentreH(InvItemText[2], true);
			}
		}
	}
#endif
}

void RingExamineNotSelected() {
	T_RemovePrint(InvItemText[2]);
	InvItemText[2] = NULL;
}

BOOL Inv_AddItem(GAME_OBJECT_ID objNumber, int secretType) {
	int i, j, count;
	int found = 0;
	GAME_OBJECT_ID optionID = Inv_GetItemOption(objNumber);

	for (i = 0; i < InvMainObjectsCount; ++i) {
		if (InvMainList[i]->objectID == optionID) {
			found = 1;
			break;
		}
	}

	for (j = 0; j < InvKeyObjectsCount; ++j) {
		if (InvKeysList[j]->objectID == optionID) {
			found = 2;
			break;
		}
	}

	if (found == 1) {
		if (objNumber == ID_FLARES_ITEM)
			InvMainQtys[i] += FLARE_AMMO_CLIPS;
		else
			InvMainQtys[i] += 1;

		return TRUE;
	}

	if (found == 2) {
		++InvKeysQtys[j];
		return TRUE;
	}

	if (IsSecret(objNumber)) {
		return AddSecret(secretType) != FALSE;
	}

	switch (objNumber) {
	case ID_COMPASS_ITEM:
	case ID_COMPASS_OPTION:
		Inv_InsertItem(&InvCompassOption);
		return TRUE;

	case ID_PISTOL_ITEM:
	case ID_PISTOL_OPTION:
		Inv_InsertItem(&InvPistolOption);
		if (Lara.last_gun_type == LGT_Unarmed)
			Lara.last_gun_type = LGT_Pistols;
		return TRUE;

	case ID_SHOTGUN_ITEM:
	case ID_SHOTGUN_OPTION:
		count = Inv_RequestItem(ID_SHOTGUN_AMMO_ITEM);
		for (i = 0; i < count; ++i) {
			Inv_RemoveItem(ID_SHOTGUN_AMMO_ITEM);
			Lara.shotgun_ammo += SHOTGUN_AMMO_CLIPS;
		}
		Lara.shotgun_ammo += SHOTGUN_AMMO_CLIPS;
		Inv_InsertItem(&InvShotgunOption);
		if (Lara.last_gun_type == LGT_Unarmed)
			Lara.last_gun_type = LGT_Shotgun;
		if (Lara.back_gun == ID_LARA)
			Lara.back_gun = ID_LARA_SHOTGUN;
		GlobalItemReplace(ID_SHOTGUN_ITEM, ID_SHOTGUN_AMMO_ITEM);
		return FALSE;

	case ID_MAGNUM_ITEM:
	case ID_MAGNUM_OPTION:
		count = Inv_RequestItem(ID_MAGNUM_AMMO_ITEM);
		for (i = 0; i < count; ++i) {
			Inv_RemoveItem(ID_MAGNUM_AMMO_ITEM);
			Lara.magnum_ammo += AUTOPISTOLS_AMMO_CLIPS;
		}
		Lara.magnum_ammo += AUTOPISTOLS_AMMO_CLIPS;
		Inv_InsertItem(&InvMagnumOption);
		GlobalItemReplace(ID_MAGNUM_ITEM, ID_MAGNUM_AMMO_ITEM);
		return FALSE;

	case ID_UZI_ITEM:
	case ID_UZI_OPTION:
		count = Inv_RequestItem(ID_UZI_AMMO_ITEM);
		for (i = 0; i < count; ++i) {
			Inv_RemoveItem(ID_UZI_AMMO_ITEM);
			Lara.uzi_ammo += UZIS_AMMO_CLIPS;
		}
		Lara.uzi_ammo += UZIS_AMMO_CLIPS;
		Inv_InsertItem(&InvUziOption);
		GlobalItemReplace(ID_UZI_ITEM, ID_UZI_AMMO_ITEM);
		return FALSE;

	case ID_HARPOON_ITEM:
	case ID_HARPOON_OPTION:
		count = Inv_RequestItem(ID_HARPOON_AMMO_ITEM);
		for (i = 0; i < count; ++i) {
			Inv_RemoveItem(ID_HARPOON_AMMO_ITEM);
			Lara.harpoon_ammo += HARPOON_AMMO_CLIPS;
		}
		Lara.harpoon_ammo += HARPOON_AMMO_CLIPS;
		Inv_InsertItem(&InvHarpoonOption);
		GlobalItemReplace(ID_HARPOON_ITEM, ID_HARPOON_AMMO_ITEM);
		return FALSE;

	case ID_M16_ITEM:
	case ID_M16_OPTION:
		count = Inv_RequestItem(ID_M16_AMMO_ITEM);
		for (i = 0; i < count; ++i) {
			Inv_RemoveItem(ID_M16_AMMO_ITEM);
			Lara.m16_ammo += M16_AMMO_CLIPS;
		}
		Lara.m16_ammo += M16_AMMO_CLIPS;
		Inv_InsertItem(&InvM16Option);
		GlobalItemReplace(ID_M16_ITEM, ID_M16_AMMO_ITEM);
		return FALSE;

	case ID_GRENADE_ITEM:
	case ID_GRENADE_OPTION:
		count = Inv_RequestItem(ID_GRENADE_AMMO_ITEM);
		for (i = 0; i < count; ++i) {
			Inv_RemoveItem(ID_GRENADE_AMMO_ITEM);
			Lara.grenade_ammo += GRENADE_AMMO_CLIPS;
		}
		Lara.grenade_ammo += GRENADE_AMMO_CLIPS;
		Inv_InsertItem(&InvGrenadeOption);
		GlobalItemReplace(ID_GRENADE_ITEM, ID_GRENADE_AMMO_ITEM);
		return FALSE;

	case ID_SHOTGUN_AMMO_ITEM:
	case ID_SHOTGUN_AMMO_OPTION:
		if (Inv_RequestItem(ID_SHOTGUN_ITEM) != 0)
			Lara.shotgun_ammo += SHOTGUN_AMMO_CLIPS;
		else
			Inv_InsertItem(&InvShotgunAmmoOption);
		return FALSE;

	case ID_MAGNUM_AMMO_ITEM:
	case ID_MAGNUM_AMMO_OPTION:
		if (Inv_RequestItem(ID_MAGNUM_ITEM) != 0)
			Lara.magnum_ammo += AUTOPISTOLS_AMMO_CLIPS;
		else
			Inv_InsertItem(&InvMagnumAmmoOption);
		return FALSE;

	case ID_UZI_AMMO_ITEM:
	case ID_UZI_AMMO_OPTION:
		if (Inv_RequestItem(ID_UZI_ITEM) != 0)
			Lara.uzi_ammo += UZIS_AMMO_CLIPS;
		else
			Inv_InsertItem(&InvUziAmmoOption);
		return FALSE;

	case ID_HARPOON_AMMO_ITEM:
	case ID_HARPOON_AMMO_OPTION:
		if (Inv_RequestItem(ID_HARPOON_ITEM) != 0)
			Lara.harpoon_ammo += HARPOON_AMMO_CLIPS;
		else
			Inv_InsertItem(&InvHarpoonAmmoOption);
		return FALSE;

	case ID_M16_AMMO_ITEM:
	case ID_M16_AMMO_OPTION:
		if (Inv_RequestItem(ID_M16_ITEM) != 0)
			Lara.m16_ammo += M16_AMMO_CLIPS;
		else
			Inv_InsertItem(&InvM16AmmoOption);
		return FALSE;

	case ID_GRENADE_AMMO_ITEM:
	case ID_GRENADE_AMMO_OPTION:
		if (Inv_RequestItem(ID_GRENADE_ITEM) != 0)
			Lara.grenade_ammo += GRENADE_AMMO_CLIPS;
		else
			Inv_InsertItem(&InvGrenadeAmmoOption);
		return FALSE;

	case ID_FLARES_ITEM:
	case ID_FLARES_OPTION:
		Inv_InsertItem(&InvFlareOption);
		for (i = 0; i < FLARE_AMMO_CLIPS; ++i) {
			Inv_AddItem(ID_FLARE_ITEM);
		}
		return TRUE;

	case ID_FLARE_ITEM:
		Inv_InsertItem(&InvFlareOption);
		return TRUE;

	case ID_SMALL_MEDIPACK_ITEM:
	case ID_SMALL_MEDIPACK_OPTION:
		Inv_InsertItem(&InvSmallMedipackOption);
		return TRUE;

	case ID_LARGE_MEDIPACK_ITEM:
	case ID_LARGE_MEDIPACK_OPTION:
		Inv_InsertItem(&InvLargeMedipackOption);
		return TRUE;

	case ID_PICKUP_ITEM1:
	case ID_PICKUP_OPTION1:
		Inv_InsertItem(&InvPickup1Option);
		return TRUE;

	case ID_PICKUP_ITEM2:
	case ID_PICKUP_OPTION2:
		Inv_InsertItem(&InvPickup2Option);
		return TRUE;

	case ID_PUZZLE_ITEM1:
	case ID_PUZZLE_OPTION1:
		Inv_InsertItem(&InvPuzzle1Option);
		return TRUE;

	case ID_PUZZLE_ITEM2:
	case ID_PUZZLE_OPTION2:
		Inv_InsertItem(&InvPuzzle2Option);
		return TRUE;

	case ID_PUZZLE_ITEM3:
	case ID_PUZZLE_OPTION3:
		Inv_InsertItem(&InvPuzzle3Option);
		return TRUE;

	case ID_PUZZLE_ITEM4:
	case ID_PUZZLE_OPTION4:
		Inv_InsertItem(&InvPuzzle4Option);
		return TRUE;

	case ID_KEY_ITEM1:
	case ID_KEY_OPTION1:
		Inv_InsertItem(&InvKey1Option);
		return TRUE;

	case ID_KEY_ITEM2:
	case ID_KEY_OPTION2:
		Inv_InsertItem(&InvKey2Option);
		return TRUE;

	case ID_KEY_ITEM3:
	case ID_KEY_OPTION3:
		Inv_InsertItem(&InvKey3Option);
		return TRUE;

	case ID_KEY_ITEM4:
	case ID_KEY_OPTION4:
		Inv_InsertItem(&InvKey4Option);
		return TRUE;

	default:
		break;
	}
	return FALSE;
}

void Inv_InsertItem(INVENTORY_ITEM* item) {
	int i, j;

	if (item->invPos < 100) {
		for (i = 0; i < InvMainObjectsCount; ++i) {
			if (InvMainList[i]->invPos > item->invPos) {
				for (j = InvMainObjectsCount; j >= i; --j) {
					InvMainList[j + 1] = InvMainList[j];
					InvMainQtys[j + 1] = InvMainQtys[j];
				}
				break;
			}
		}
		InvMainList[i] = item;
		InvMainQtys[i] = 1;
		++InvMainObjectsCount;
	}
	else {
		for (i = 0; i < InvKeyObjectsCount; ++i) {
			if (InvKeysList[i]->invPos > item->invPos) {
				for (j = InvKeyObjectsCount; j >= i; --j) {
					InvKeysList[j + 1] = InvKeysList[j];
					InvKeysQtys[j + 1] = InvKeysQtys[j];
				}
				break;
			}
		}
		InvKeysList[i] = item;
		InvKeysQtys[i] = 1;
		++InvKeyObjectsCount;
	}
}

int Inv_RequestItem(GAME_OBJECT_ID itemNumber) {
	int i;
	int optionID = Inv_GetItemOption(itemNumber);

	for (i = 0; i < InvMainObjectsCount; ++i)
		if (InvMainList[i]->objectID == optionID)
			return InvMainQtys[i];

	for (i = 0; i < InvKeyObjectsCount; ++i)
		if (InvKeysList[i]->objectID == optionID)
			return InvKeysQtys[i];

	return 0;
}

void Inv_RemoveAllItems() {
	InvMainObjectsCount = 0;
	InvMainCurrent = 0;
	InvKeyObjectsCount = 0;
	InvKeysCurrent = 0;
}

BOOL Inv_RemoveItem(GAME_OBJECT_ID itemNumber) {
	int i, j;
	GAME_OBJECT_ID optionID = Inv_GetItemOption(itemNumber);

	for (i = 0; i < InvMainObjectsCount; ++i) {
		if (InvMainList[i]->objectID == optionID) {
			if (--InvMainQtys[i] <= 0) {
				--InvMainObjectsCount;

				for (j = i; j < InvMainObjectsCount; ++j) {
					InvMainList[j] = InvMainList[j + 1];
					InvMainQtys[j] = InvMainQtys[j + 1];
				}
			}
			return TRUE;
		}
	}

	for (i = 0; i < InvKeyObjectsCount; ++i) {
		if (InvKeysList[i]->objectID == optionID) {
			if (--InvKeysQtys[i] <= 0) {
				--InvKeyObjectsCount;

				for (j = i; j < InvKeyObjectsCount; ++j) {
					InvKeysList[j] = InvKeysList[j + 1];
					InvKeysQtys[j] = InvKeysQtys[j + 1];
				}
			}
			return TRUE;
		}
	}

	return FALSE;
}

GAME_OBJECT_ID Inv_GetItemOption(GAME_OBJECT_ID itemNumber) {
	switch (itemNumber) {
		// Weapons
	case ID_PISTOL_ITEM:
	case ID_PISTOL_OPTION:
		return ID_PISTOL_OPTION;
	case ID_SHOTGUN_ITEM:
	case ID_SHOTGUN_OPTION:
		return ID_SHOTGUN_OPTION;
	case ID_MAGNUM_ITEM:
	case ID_MAGNUM_OPTION:
		return ID_MAGNUM_OPTION;
	case ID_UZI_ITEM:
	case ID_UZI_OPTION:
		return ID_UZI_OPTION;
	case ID_HARPOON_ITEM:
	case ID_HARPOON_OPTION:
		return ID_HARPOON_OPTION;
	case ID_M16_ITEM:
	case ID_M16_OPTION:
		return ID_M16_OPTION;
	case ID_GRENADE_ITEM:
	case ID_GRENADE_OPTION:
		return ID_GRENADE_OPTION;
		// Ammo
			// NOTE: It seems here could be Pistol Ammo Option, but it was deleted or commented by CORE
		/*	case ID_PISTOL_AMMO_ITEM :
			case ID_PISTOL_AMMO_OPTION :
				return ID_PISTOL_AMMO_OPTION; */
	case ID_SHOTGUN_AMMO_ITEM:
	case ID_SHOTGUN_AMMO_OPTION:
		return ID_SHOTGUN_AMMO_OPTION;
	case ID_MAGNUM_AMMO_ITEM:
	case ID_MAGNUM_AMMO_OPTION:
		return ID_MAGNUM_AMMO_OPTION;
	case ID_UZI_AMMO_ITEM:
	case ID_UZI_AMMO_OPTION:
		return ID_UZI_AMMO_OPTION;
	case ID_HARPOON_AMMO_ITEM:
	case ID_HARPOON_AMMO_OPTION:
		return ID_HARPOON_AMMO_OPTION;
	case ID_M16_AMMO_ITEM:
	case ID_M16_AMMO_OPTION:
		return ID_M16_AMMO_OPTION;
	case ID_GRENADE_AMMO_ITEM:
	case ID_GRENADE_AMMO_OPTION:
		return ID_GRENADE_AMMO_OPTION;
		// Flares, Medipacks
	case ID_SMALL_MEDIPACK_ITEM:
	case ID_SMALL_MEDIPACK_OPTION:
		return ID_SMALL_MEDIPACK_OPTION;
	case ID_LARGE_MEDIPACK_ITEM:
	case ID_LARGE_MEDIPACK_OPTION:
		return ID_LARGE_MEDIPACK_OPTION;
	case ID_FLARE_ITEM:
	case ID_FLARES_ITEM:
	case ID_FLARES_OPTION:
		return ID_FLARES_OPTION;
		// Puzzles
	case ID_PUZZLE_ITEM1:
	case ID_PUZZLE_OPTION1:
		return ID_PUZZLE_OPTION1;
	case ID_PUZZLE_ITEM2:
	case ID_PUZZLE_OPTION2:
		return ID_PUZZLE_OPTION2;
	case ID_PUZZLE_ITEM3:
	case ID_PUZZLE_OPTION3:
		return ID_PUZZLE_OPTION3;
	case ID_PUZZLE_ITEM4:
	case ID_PUZZLE_OPTION4:
		return ID_PUZZLE_OPTION4;
		// Pickups
	case ID_PICKUP_ITEM1:
	case ID_PICKUP_OPTION1:
		return ID_PICKUP_OPTION1;
	case ID_PICKUP_ITEM2:
	case ID_PICKUP_OPTION2:
		return ID_PICKUP_OPTION2;
		// Keys
	case ID_KEY_ITEM1:
	case ID_KEY_OPTION1:
		return ID_KEY_OPTION1;
	case ID_KEY_ITEM2:
	case ID_KEY_OPTION2:
		return ID_KEY_OPTION2;
	case ID_KEY_ITEM3:
	case ID_KEY_OPTION3:
		return ID_KEY_OPTION3;
	case ID_KEY_ITEM4:
	case ID_KEY_OPTION4:
		return ID_KEY_OPTION4;
	default:
		break;
	}
	return ID_NONE;
}

void RemoveInventoryText() {
	for (int i = 0; i < 2; ++i) {
		T_RemovePrint(InvItemText[i]);
		InvItemText[i] = NULL;
	}
}

void Inv_RingInit(RING_INFO* ring, short type, INVENTORY_ITEM** itemList, short objCount, short currentObj, INV_MOTION_INFO* motionInfo) {
	ring->type = type;
	ring->itemList = itemList;
	ring->objCount = objCount;
	ring->currentObj = currentObj;
	ring->radius = 0;
	ring->angleAdder = PHD_360 / objCount;
	ring->cameraPitch = (InventoryMode == INV_TitleMode) ? 0x400 : 0;
	ring->isRotating = 0;
	ring->rotCount = 0;
	ring->targetObj = 0;
	ring->rotAdder = 0;
	ring->rotAdderL = 0;
	ring->rotAdderR = 0;
	ring->motionInfo = motionInfo;
	ring->camera.x = 0;
	ring->camera.y = -0x600;
	ring->camera.z = 0x380;
	ring->camera.rotX = 0;
	ring->camera.rotY = 0;
	ring->camera.rotZ = 0;
	Inv_RingMotionInit(ring, 32, 0, 1);
	Inv_RingMotionRadius(ring, 0x2B0);
	Inv_RingMotionCameraPos(ring, -0x100);
	Inv_RingMotionRotation(ring, PHD_180, (-PHD_90) - (ring->currentObj * ring->angleAdder));
	ring->ringPos.x = 0;
	ring->ringPos.y = 0;
	ring->ringPos.z = 0;
	ring->ringPos.rotX = 0;
	ring->ringPos.rotZ = 0;
	ring->ringPos.rotY = motionInfo->rotateTarget + PHD_180;
	ring->light.x = -0x600;
	ring->light.y = 0x100;
	ring->light.z = 0x400;
}

void Inv_RingGetView(RING_INFO* ring, PHD_3DPOS* view) {
	VECTOR_ANGLES angles;

	phd_GetVectorAngles((0 - ring->camera.x), (-96 - ring->camera.y), (ring->radius - ring->camera.z), &angles);
	view->x = ring->camera.x;
	view->y = ring->camera.y;
	view->z = ring->camera.z;
	view->rotX = angles.rotX + ring->cameraPitch;
	view->rotY = angles.rotY;
	view->rotZ = 0;
}

void Inv_RingLight(RING_INFO* ring) {
	VECTOR_ANGLES angles;

	LsDivider = 0x6000;
	phd_GetVectorAngles(ring->light.x, ring->light.y, ring->light.z, &angles);
	phd_RotateLight(angles.rotY, angles.rotX);
}

void Inv_RingCalcAdders(RING_INFO* ring, short rotDuration) {
	ring->angleAdder = PHD_360 / ring->objCount;
	ring->rotAdderL = ring->angleAdder / rotDuration;
	ring->rotAdderR = -ring->rotAdderL;
}

void Inv_RingDoMotions(RING_INFO* ring) {
	INVENTORY_ITEM* item;
	INV_MOTION_INFO* mi = ring->motionInfo;

	if (mi->framesCount != 0) {
		ring->radius += mi->radiusRate;
		ring->camera.y += mi->cameraRate_y;
		ring->ringPos.rotY += mi->rotateRate;
		ring->cameraPitch += mi->cameraRate_pitch;

		item = ring->itemList[ring->currentObj];
		item->xRotPt += mi->itemRate_xRotPt;
		item->yRotSel += mi->itemRate_xRot;
		item->yTrans += mi->itemRate_yTrans;
		item->zTrans += mi->itemRate_zTrans;

		if (--mi->framesCount == 0) {
			mi->status = mi->statusTarget;

			if (mi->radiusRate != 0) {
				mi->radiusRate = 0;
				ring->radius = mi->radiusTarget;
			}
			if (mi->cameraRate_y != 0) {
				mi->cameraRate_y = 0;
				ring->camera.y = mi->cameraTarget_y;
			}
			if (mi->rotateRate != 0) {
				mi->rotateRate = 0;
				ring->ringPos.rotY = mi->rotateTarget;
			}
			if (mi->itemRate_xRotPt != 0) {
				mi->itemRate_xRotPt = 0;
				item->xRotPt = mi->itemTarget_xRotPt;
			}
			if (mi->itemRate_xRot != 0) {
				mi->itemRate_xRot = 0;
				item->yRotSel = mi->itemTarget_xRot;
			}
			if (mi->itemRate_yTrans != 0) {
				mi->itemRate_yTrans = 0;
				item->yTrans = mi->itemTarget_yTrans;
			}
			if (mi->itemRate_zTrans != 0) {
				mi->itemRate_zTrans = 0;
				item->zTrans = mi->itemTarget_zTrans;
			}
			if (mi->cameraRate_pitch != 0) {
				mi->cameraRate_pitch = 0;
				ring->cameraPitch = mi->cameraTarget_pitch;
			}
		}
	}

	if (ring->isRotating != 0) {
		ring->ringPos.rotY += ring->rotAdder;
		if (--ring->rotCount == 0) {
			ring->currentObj = ring->targetObj;
			ring->ringPos.rotY = (-PHD_90) - (ring->currentObj * ring->angleAdder);
			ring->isRotating = 0;
		}
	}
}

void Inv_RingRotateLeft(RING_INFO* ring) {
	ring->isRotating = 1;
	ring->targetObj = ring->currentObj - 1;
	if (ring->targetObj < 0) {
		ring->targetObj = ring->objCount - 1;
	}
	ring->rotCount = 24;
	ring->rotAdder = ring->rotAdderL;
}

void Inv_RingRotateRight(RING_INFO* ring) {
	ring->isRotating = 1;
	ring->targetObj = ring->currentObj + 1;
	if (ring->targetObj >= ring->objCount) {
		ring->targetObj = 0;
	}
	ring->rotCount = 24;
	ring->rotAdder = ring->rotAdderR;
}

void Inv_RingMotionInit(RING_INFO* ring, short framesCount, short status, short statusTarget) {
	INV_MOTION_INFO* mi = ring->motionInfo;

	mi->framesCount = framesCount;
	mi->status = status;
	mi->statusTarget = statusTarget;
	mi->radiusTarget = 0;
	mi->radiusRate = 0;
	mi->cameraTarget_y = 0;
	mi->cameraRate_y = 0;
	mi->cameraTarget_pitch = 0;
	mi->cameraRate_pitch = 0;
	mi->rotateTarget = 0;
	mi->rotateRate = 0;
	mi->itemTarget_xRotPt = 0;
	mi->itemRate_xRotPt = 0;
	mi->itemTarget_xRot = 0;
	mi->itemRate_xRot = 0;
	mi->itemTarget_yTrans = 0;
	mi->itemRate_yTrans = 0;
	mi->itemTarget_zTrans = 0;
	mi->itemRate_zTrans = 0;
	mi->misc = 0;
}

void Inv_RingMotionSetup(RING_INFO* ring, short status, short statusTarget, short framesCount) {
	INV_MOTION_INFO* mi = ring->motionInfo;

	mi->framesCount = framesCount;
	mi->statusTarget = statusTarget;
	mi->status = status;
	mi->radiusRate = 0;
	mi->cameraRate_y = 0;
}

void Inv_RingMotionRadius(RING_INFO* ring, short target) {
	INV_MOTION_INFO* mi = ring->motionInfo;

	mi->radiusTarget = target;
	mi->radiusRate = (target - ring->radius) / mi->framesCount;
}

void Inv_RingMotionRotation(RING_INFO* ring, short rotation, short target) {
	INV_MOTION_INFO* mi = ring->motionInfo;

	mi->rotateTarget = target;
	mi->rotateRate = rotation / mi->framesCount;
}

void Inv_RingMotionCameraPos(RING_INFO* ring, short target) {
	INV_MOTION_INFO* mi = ring->motionInfo;

	mi->cameraTarget_y = target;
	mi->cameraRate_y = (target - ring->camera.y) / mi->framesCount;
}

void Inv_RingMotionCameraPitch(RING_INFO* ring, short target) {
	INV_MOTION_INFO* mi = ring->motionInfo;

	mi->cameraTarget_pitch = target;
	mi->cameraRate_pitch = target / mi->framesCount;
}

void Inv_RingMotionItemSelect(RING_INFO* ring, INVENTORY_ITEM* item) {
	INV_MOTION_INFO* mi = ring->motionInfo;

	mi->itemTarget_xRotPt = item->xRotPtSel;
	mi->itemRate_xRotPt = item->xRotPtSel / mi->framesCount;
	mi->itemTarget_xRot = item->xRotSel;
	mi->itemRate_xRot = (item->xRotSel - item->xRot) / mi->framesCount;
	mi->itemTarget_yTrans = item->yTransSel;
	mi->itemRate_yTrans = item->yTransSel / mi->framesCount;
	mi->itemTarget_zTrans = item->zTransSel;
	mi->itemRate_zTrans = item->zTransSel / mi->framesCount;
}

void Inv_RingMotionItemDeselect(RING_INFO* ring, INVENTORY_ITEM* item) {
	INV_MOTION_INFO* mi = ring->motionInfo;

	mi->itemTarget_xRotPt = 0;
	mi->itemRate_xRotPt = -(item->xRotPtSel / mi->framesCount);
	mi->itemTarget_xRot = item->xRot;
	mi->itemRate_xRot = (item->xRot - item->xRotSel) / mi->framesCount;
	mi->itemTarget_yTrans = 0;
	mi->itemRate_yTrans = -(item->yTransSel / mi->framesCount);
	mi->itemTarget_zTrans = 0;
	mi->itemRate_zTrans = -(item->zTransSel / mi->framesCount);
}

INVENTORY_ITEM* Inv_GetItemFromIndex(int index)
{
	switch (index)
	{
	case 0: return &InvCompassOption;
	case 1: return &InvPistolOption;
	case 2: return &InvFlareOption;
	case 3: return &InvShotgunOption;
	case 4: return &InvMagnumOption;
	case 5: return &InvUziOption;
	case 6: return &InvHarpoonOption;
	case 7: return &InvM16Option;
	case 8: return &InvGrenadeOption;
	case 9: return &InvPistolAmmoOption;
	case 10: return &InvShotgunAmmoOption;
	case 11: return &InvMagnumAmmoOption;
	case 12: return &InvUziAmmoOption;
	case 13: return &InvHarpoonAmmoOption;
	case 14: return &InvM16AmmoOption;
	case 15: return &InvGrenadeAmmoOption;
	case 16: return &InvSmallMedipackOption;
	case 17: return &InvLargeMedipackOption;
	case 18: return &InvPickup1Option;
	case 19: return &InvPickup2Option;
	case 20: return &InvPuzzle1Option;
	case 21: return &InvPuzzle2Option;
	case 22: return &InvPuzzle3Option;
	case 23: return &InvPuzzle4Option;
	case 24: return &InvKey1Option;
	case 25: return &InvKey2Option;
	case 26: return &InvKey3Option;
	case 27: return &InvKey4Option;
	case 28: return &InvPassportOption;
	case 29: return &InvDetailOption;
	case 30: return &InvSoundOption;
	case 31: return &InvControlOption;
	case 32: return &InvPhotoOption;
	}
	return nullptr;
}

/*
 * Inject function
 */
void Inject_InvFunc() {
	INJECT(0x00423B10, InitColours);
	INJECT(0x00423C20, RingIsOpen);
	INJECT(0x00423D90, RingIsNotOpen);
	INJECT(0x00423E20, RingNotActive);
	INJECT(0x00424290, RingActive);
	INJECT(0x004242D0, Inv_AddItem);
	INJECT(0x00424AE0, Inv_InsertItem);
	INJECT(0x00424C10, Inv_RequestItem);
	INJECT(0x00424C90, Inv_RemoveAllItems);
	INJECT(0x00424CB0, Inv_RemoveItem);
	INJECT(0x00424DC0, Inv_GetItemOption);
	INJECT(0x00424FB0, RemoveInventoryText);
	INJECT(0x00424FE0, Inv_RingInit);
	INJECT(0x004250F0, Inv_RingGetView);
	INJECT(0x00425150, Inv_RingLight);
	INJECT(0x00425190, Inv_RingCalcAdders);
	INJECT(0x004251C0, Inv_RingDoMotions);
	INJECT(0x00425300, Inv_RingRotateLeft);
	INJECT(0x00425330, Inv_RingRotateRight);
	INJECT(0x00425360, Inv_RingMotionInit);
	INJECT(0x004253D0, Inv_RingMotionSetup);
	INJECT(0x00425400, Inv_RingMotionRadius);
	INJECT(0x00425430, Inv_RingMotionRotation);
	INJECT(0x00425460, Inv_RingMotionCameraPos);
	INJECT(0x00425490, Inv_RingMotionCameraPitch);
	INJECT(0x004254B0, Inv_RingMotionItemSelect);
	INJECT(0x00425510, Inv_RingMotionItemDeselect);
}