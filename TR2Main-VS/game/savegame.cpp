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

#include "precompiled.h"
#include "game/savegame.h"
#include "game/control.h"
#include "game/dragon.h"
#include "game/invfunc.h"
#include "game/items.h"
#include "game/larafire.h"
#include "game/laramisc.h"
#include "game/moveblock.h"
#include "game/lot.h"
#include "game/pickup.h"
#include "game/skidoo.h"
#include "game/traps.h"
#include "specific/winmain.h"
#include "global/vars.h"

#ifdef FEATURE_MOD_CONFIG
#include "modding/mod_utils.h"
#endif

#define SGF_CREATURE (0x80)

SAVEGAME_INFO SaveGame = {};
BYTE* SG_Point = NULL;
DWORD SG_Count = 0;

void InitialiseStartInfo() {
#if 0 // NOTE: this original check is removed, because it breaks game+ logic in case of any level selection
	// skip initialise if bonus game started
	if (SaveGame.bonusFlag)
		return;
#endif

	for (int i = 0; i < 24; ++i) {
		START_INFO* start = &SaveGame.start[i];
		ModifyStartInfo(i);
		start->available = 0; // make level not available
		memset(&start->statistics, 0, sizeof(STATISTICS_INFO));
	}

	SaveGame.start[0].available = TRUE; // make assault available
	SaveGame.start[1].available = TRUE; // make new game available
}

void ModifyStartInfo(int levelIdx) {
	START_INFO* start = &SaveGame.start[levelIdx];
	start->has_pistols = TRUE; // Lara has pistols
	start->gunType = LGT_Pistols; // current weapon is pistols
	start->pistolAmmo = 1000; // infinite pistols ammo

	switch (levelIdx) {
	case 0: // Assault (Lara's Home)
		start->available = TRUE; // make level available
		start->has_pistols = FALSE; // Lara has no weapons
		start->has_autopistol = FALSE;
		start->has_uzis = FALSE;
		start->has_shotgun = FALSE;
		start->has_m16 = FALSE;
		start->has_grenade = FALSE;
		start->has_harpoon = FALSE;
		start->gunType = LGT_Unarmed; // Lara doesn't have weapons
		start->gunStatus = LGS_Armless; // Lara has no weapons in hands

		start->pistolAmmo = 0;
		start->autopistolAmmo = 0;
		start->uziAmmo = 0;
		start->shotgunAmmo = 0;
		start->m16Ammo = 0;
		start->grenadeAmmo = 0;
		start->harpoonAmmo = 0;

		start->flares = 0;
		start->smallMedipacks = 0;
		start->largeMedipacks = 0;
		break;

	case 1: // Regular New Game
		start->available = TRUE; // make level available
		start->gunStatus = LGS_Armless; // Lara has no weapons in hands
#if defined(FEATURE_MOD_CONFIG)
		start->has_pistols = Mod.pistolAtStart; // Lara has pistols and shotgun
		start->has_shotgun = Mod.shotgunAtStart;
		start->has_autopistol = Mod.autoPistolAtStart;
		start->has_uzis = Mod.uzisAtStart;
		start->has_m16 = Mod.m16AtStart;
		start->has_grenade = Mod.grenadeAtStart;
		start->has_harpoon = Mod.harpoonAtStart;
		start->shotgunAmmo = Mod.shotgunAmmoAtStart * (SHOTGUN_AMMO_CLIPS / 2);
		start->autopistolAmmo = Mod.autoPistolAmmoAtStart;
		start->uziAmmo = Mod.uzisAmmoAtStart;
		start->m16Ammo = Mod.m16AmmoAtStart;
		start->grenadeAmmo = Mod.grenadeAmmoAtStart;
		start->harpoonAmmo = Mod.harpoonAmmoAtStart;
		start->flares = Mod.flareAtStart;
		start->smallMedipacks = Mod.smallMediAtStart;
		start->largeMedipacks = Mod.bigMediAtStart;
#else
		start->has_pistols = TRUE; // Lara has pistols and shotgun
		start->has_magnums = FALSE;
		start->has_uzis = FALSE;
		start->has_shotgun = TRUE;
		start->has_m16 = FALSE;
		start->has_grenade = FALSE;
		start->has_harpoon = FALSE;
		start->magnumAmmo = 0;
		start->uziAmmo = 0;
		start->shotgunAmmo = 12;
		start->m16Ammo = 0;
		start->grenadeAmmo = 0;
		start->harpoonAmmo = 0;
		start->flares = 2;
		start->smallMedipacks = 1;
		start->largeMedipacks = 1;
#endif
		break;

		// NOTE: there was no 'default' in the original game, so new game with level selection was broken
	default: // New Game from any other level
		if (SaveGame.start[levelIdx - 1].statistics.timer) break; // no reset if it's not a new game
		start->available = TRUE; // make level available
		start->has_pistols = TRUE; // Lara has just pistols
		start->has_autopistol = FALSE;
		start->has_uzis = FALSE;
		start->has_shotgun = FALSE;
		start->has_m16 = FALSE;
		start->has_grenade = FALSE;
		start->has_harpoon = FALSE;
		start->gunStatus = LGS_Armless; // Lara has no weapons in hands

		start->autopistolAmmo = 0;
		start->uziAmmo = 0;
		start->shotgunAmmo = 0;
		start->m16Ammo = 0;
		start->grenadeAmmo = 0;
		start->harpoonAmmo = 0;

		start->flares = 0;
		start->smallMedipacks = 0;
		start->largeMedipacks = 0;
		break;
	}

	// Bonus game activated and level is not Assault
	if (SaveGame.bonusFlag && levelIdx != 0) {
		start->available = TRUE; // make level available
		start->has_pistols = TRUE; // Lara has all weapons
		start->has_autopistol = TRUE;
		start->has_uzis = TRUE;
		start->has_shotgun = TRUE;
		start->has_m16 = TRUE;
		start->has_grenade = TRUE;
		start->has_harpoon = TRUE;
		start->gunType = LGT_Grenade; // current weapon is grenade launcher

		start->uziAmmo = WEAPON_UNLIMITED;
		start->autopistolAmmo = WEAPON_UNLIMITED;
		start->shotgunAmmo = WEAPON_UNLIMITED;
		start->m16Ammo = WEAPON_UNLIMITED;
		start->grenadeAmmo = WEAPON_UNLIMITED;
		start->harpoonAmmo = WEAPON_UNLIMITED;

		start->flares = 255;
	}
}

void CreateStartInfo(int levelID) {
	START_INFO* start = &SaveGame.start[levelID];

	start->available = TRUE;
	start->pistolAmmo = PISTOLS_AMMO_DEFAULT;
	start->has_pistols = Inv_RequestItem(ID_PISTOL_ITEM) ? 1 : 0;

	if (Inv_RequestItem(ID_MAGNUM_ITEM)) {
		start->autopistolAmmo = Lara.magnum_ammo;
		start->has_autopistol = 1;
	}
	else {
		start->autopistolAmmo = AUTOPISTOLS_AMMO_CLIPS * Inv_RequestItem(ID_MAGNUM_AMMO_ITEM);
		start->has_autopistol = 0;
	}

	if (Inv_RequestItem(ID_UZI_ITEM)) {
		start->uziAmmo = Lara.uzi_ammo;
		start->has_uzis = 1;
	}
	else {
		start->uziAmmo = UZIS_AMMO_CLIPS * Inv_RequestItem(ID_UZI_AMMO_ITEM);
		start->has_uzis = 0;
	}

	if (Inv_RequestItem(ID_SHOTGUN_ITEM)) {
		start->shotgunAmmo = Lara.shotgun_ammo;
		start->has_shotgun = 1;
	}
	else {
		start->shotgunAmmo = SHOTGUN_AMMO_CLIPS * Inv_RequestItem(ID_SHOTGUN_AMMO_ITEM);
		start->has_shotgun = 0;
	}

	if (Inv_RequestItem(ID_HARPOON_ITEM)) {
		start->harpoonAmmo = Lara.harpoon_ammo;
		start->has_harpoon = 1;
	}
	else {
		start->harpoonAmmo = HARPOON_AMMO_CLIPS * Inv_RequestItem(ID_HARPOON_AMMO_ITEM);
		start->has_harpoon = 0;
	}

	if (Inv_RequestItem(ID_M16_ITEM)) {
		start->m16Ammo = Lara.m16_ammo;
		start->has_m16 = 1;
	}
	else {
		start->m16Ammo = M16_AMMO_CLIPS * Inv_RequestItem(ID_M16_AMMO_ITEM);
		start->has_m16 = 0;
	}

	if (Inv_RequestItem(ID_GRENADE_ITEM)) {
		start->grenadeAmmo = Lara.grenade_ammo;
		start->has_grenade = 1;
	}
	else {
		start->grenadeAmmo = GRENADE_AMMO_CLIPS * Inv_RequestItem(ID_GRENADE_AMMO_ITEM);
		start->has_grenade = 0;
	}

	start->flares = Inv_RequestItem(ID_FLARE_ITEM);
	start->smallMedipacks = Inv_RequestItem(ID_SMALL_MEDIPACK_ITEM);
	start->largeMedipacks = Inv_RequestItem(ID_LARGE_MEDIPACK_ITEM);

	start->gunType = (Lara.gun_type == LGT_Flare) ? Lara.last_gun_type : Lara.gun_type;
	start->gunStatus = LGS_Armless;
}

void CreateSaveGameInfo() {
	ITEM_INFO* item = NULL;
	SaveGame.currentLevel = CurrentLevel;
	CreateStartInfo(CurrentLevel);

	SaveGame.numPickup[0] = Inv_RequestItem(ID_PICKUP_ITEM1);
	SaveGame.numPickup[1] = Inv_RequestItem(ID_PICKUP_ITEM2);
	SaveGame.numPuzzle[0] = Inv_RequestItem(ID_PUZZLE_ITEM1);
	SaveGame.numPuzzle[1] = Inv_RequestItem(ID_PUZZLE_ITEM2);
	SaveGame.numPuzzle[2] = Inv_RequestItem(ID_PUZZLE_ITEM3);
	SaveGame.numPuzzle[3] = Inv_RequestItem(ID_PUZZLE_ITEM4);
	SaveGame.numKey[0] = Inv_RequestItem(ID_KEY_ITEM1);
	SaveGame.numKey[1] = Inv_RequestItem(ID_KEY_ITEM2);
	SaveGame.numKey[2] = Inv_RequestItem(ID_KEY_ITEM3);
	SaveGame.numKey[3] = Inv_RequestItem(ID_KEY_ITEM4);

	ResetSG();
	memset(SaveGame.buffer, 0, sizeof(SaveGame.buffer));

	WriteSG(&FlipStatus, sizeof(FlipStatus));
	for (DWORD i = 0; i < ARRAY_SIZE(FlipMaps); ++i) {
		char flip = FlipMaps[i] >> 8;
		WriteSG(&flip, sizeof(flip));
	}

	WriteSG(CDFlags, sizeof(CDFlags));

	for (DWORD i = 0; i < CameraCount; ++i) {
		WriteSG(&Camera.fixed[i].flags, sizeof(Camera.fixed[i].flags));
	}

	for (int i = 0; i < LevelItemCount; ++i) {
		item = &Items[i];
		OBJECT_INFO* obj = &Objects[item->objectID];
		if (obj->save_position) {
			WriteSG(&item->pos, sizeof(item->pos));
			WriteSG(&item->roomNumber, sizeof(item->roomNumber));
			WriteSG(&item->speed, sizeof(item->speed));
			WriteSG(&item->fallSpeed, sizeof(item->fallSpeed));
		}
		if (obj->save_anim) {
			WriteSG(&item->currentAnimState, sizeof(item->currentAnimState));
			WriteSG(&item->goalAnimState, sizeof(item->goalAnimState));
			WriteSG(&item->requiredAnimState, sizeof(item->requiredAnimState));
			WriteSG(&item->animNumber, sizeof(item->animNumber));
			WriteSG(&item->frameNumber, sizeof(item->frameNumber));
		}
		if (obj->save_hitpoints) {
			WriteSG(&item->hitPoints, sizeof(item->hitPoints));
		}
		if (obj->save_flags) {
			UINT16 flags = item->flags | item->active | (item->status << 1) | (item->gravity << 3) | (item->collidable << 4);
			bool is_creature = obj->intelligent && item->data != NULL;
			WriteSG(&is_creature, sizeof(is_creature));
			WriteSG(&flags, sizeof(flags));
			if (obj->intelligent) {
				WriteSG(&item->carriedItem, sizeof(item->carriedItem));
			}
			WriteSG(&item->timer, sizeof(item->timer));
			if (is_creature) {
				CREATURE_INFO* creature = (CREATURE_INFO*)item->data;
				WriteSG(&creature->headRotation, sizeof(creature->headRotation));
				WriteSG(&creature->neckRotation, sizeof(creature->neckRotation));
				WriteSG(&creature->maximumTurn, sizeof(creature->maximumTurn));
				WriteSG(&creature->flags, sizeof(creature->flags));
				WriteSG(&creature->mood, sizeof(creature->mood));
			}
		}
		switch (item->objectID) {
		case ID_BOAT:
			WriteSG(item->data, sizeof(BOAT_INFO));
			break;
		case ID_SKIDOO_FAST:
			WriteSG(item->data, sizeof(SKIDOO_INFO));
			break;
		case ID_LIFT:
			WriteSG(item->data, sizeof(int) * 2);
			break;
		default:
			break;
		}
	}

	for (DWORD i = 0; i < ARRAY_SIZE(Lara.mesh_ptrs); ++i) {
		Lara.mesh_ptrs[i] = (short*)((long)Lara.mesh_ptrs[i] - (long)Meshes);
	}
	Lara.left_arm.frame_base = (short*)((long)Lara.left_arm.frame_base - (long)AnimFrames);
	Lara.right_arm.frame_base = (short*)((long)Lara.right_arm.frame_base - (long)AnimFrames);

	WriteSG(&Lara, sizeof(Lara));

	for (DWORD i = 0; i < ARRAY_SIZE(Lara.mesh_ptrs); ++i) {
		Lara.mesh_ptrs[i] = (short*)((long)Lara.mesh_ptrs[i] + (long)Meshes);
	}
	Lara.left_arm.frame_base = (short*)((long)Lara.left_arm.frame_base + (long)AnimFrames);
	Lara.right_arm.frame_base = (short*)((long)Lara.right_arm.frame_base + (long)AnimFrames);

	if (Lara.weapon_item >= 0) {
		item = &Items[Lara.weapon_item];
		WriteSG(&item->objectID, sizeof(item->objectID));
		WriteSG(&item->animNumber, sizeof(item->animNumber));
		WriteSG(&item->frameNumber, sizeof(item->frameNumber));
		WriteSG(&item->currentAnimState, sizeof(item->currentAnimState));
		WriteSG(&item->goalAnimState, sizeof(item->goalAnimState));
	}

	WriteSG(&FlipEffect, sizeof(FlipEffect));
	WriteSG(&FlipTimer, sizeof(FlipTimer));
	WriteSG(&IsMonkAngry, sizeof(IsMonkAngry));

	int numFlares = 0;
	for (int i = LevelItemCount; i < NUMBER_ITEMS; ++i) {
		item = &Items[i];
		if (item->active && item->objectID == ID_FLARE_ITEM) {
			++numFlares;
		}
	}

	WriteSG(&numFlares, sizeof(numFlares));

	for (int i = LevelItemCount; i < NUMBER_ITEMS; ++i) {
		item = &Items[i];
		if (item->active && item->objectID == ID_FLARE_ITEM) {
			WriteSG(&item->pos, sizeof(item->pos));
			WriteSG(&item->roomNumber, sizeof(item->roomNumber));
			WriteSG(&item->speed, sizeof(item->speed));
			WriteSG(&item->fallSpeed, sizeof(item->fallSpeed));
			int flareAge = (int)item->data;
			WriteSG(&flareAge, sizeof(flareAge));
		}
	}
}

void ExtractSaveGameInfo() {
	ITEM_INFO* item = NULL;
	InitialiseLaraInventory(CurrentLevel);

	for (int i = 0; i < 2; ++i) {
		GAME_OBJECT_ID id[] = { ID_PICKUP_ITEM1, ID_PICKUP_ITEM2 };
		for (int j = 0; j < SaveGame.numPickup[i]; ++j) {
			Inv_AddItem(id[i]);
		}
	}
	for (int i = 0; i < 4; ++i) {
		GAME_OBJECT_ID id[] = { ID_PUZZLE_ITEM1, ID_PUZZLE_ITEM2, ID_PUZZLE_ITEM3, ID_PUZZLE_ITEM4 };
		for (int j = 0; j < SaveGame.numPuzzle[i]; ++j) {
			Inv_AddItem(id[i]);
		}
	}
	for (int i = 0; i < 4; ++i) {
		GAME_OBJECT_ID id[] = { ID_KEY_ITEM1, ID_KEY_ITEM2, ID_KEY_ITEM3, ID_KEY_ITEM4 };
		for (int j = 0; j < SaveGame.numKey[i]; ++j) {
			Inv_AddItem(id[i]);
		}
	}

	ResetSG();

	int flipStatus;
	ReadSG(&flipStatus, sizeof(flipStatus));
	if (flipStatus) FlipMap();
	for (DWORD i = 0; i < ARRAY_SIZE(FlipMaps); ++i) {
		char flip;
		ReadSG(&flip, sizeof(flip));
		FlipMaps[i] = flip << 8;
	}

	ReadSG(CDFlags, sizeof(CDFlags));

	for (DWORD i = 0; i < CameraCount; ++i) {
		ReadSG(&Camera.fixed[i].flags, sizeof(Camera.fixed[i].flags));
	}

	for (int i = 0; i < LevelItemCount; ++i) {
		item = &Items[i];
		OBJECT_INFO* obj = &Objects[item->objectID];

		if (obj->control == MovableBlock) {
			AlterFloorHeight(item, 0x400);
		}
		if (obj->save_position) {
			short roomNumber;
			ReadSG(&item->pos, sizeof(item->pos));
			ReadSG(&roomNumber, sizeof(roomNumber));
			ReadSG(&item->speed, sizeof(item->speed));
			ReadSG(&item->fallSpeed, sizeof(item->fallSpeed));
			if (item->roomNumber != roomNumber) {
				ItemNewRoom(i, roomNumber);
			}
			if (obj->shadowSize) {
				FLOOR_INFO* floor = GetFloor(item->pos.x, item->pos.y, item->pos.z, &roomNumber);
				item->floor = GetHeight(floor, item->pos.x, item->pos.y, item->pos.z);
			}
		}
		if (obj->save_anim) {
			ReadSG(&item->currentAnimState, sizeof(item->currentAnimState));
			ReadSG(&item->goalAnimState, sizeof(item->goalAnimState));
			ReadSG(&item->requiredAnimState, sizeof(item->requiredAnimState));
			ReadSG(&item->animNumber, sizeof(item->animNumber));
			ReadSG(&item->frameNumber, sizeof(item->frameNumber));
		}
		if (obj->save_hitpoints) {
			ReadSG(&item->hitPoints, sizeof(item->hitPoints));
		}
		if (obj->save_flags) {
			bool is_creature = false;
			ReadSG(&is_creature, sizeof(is_creature));
			ReadSG(&item->flags, sizeof(item->flags));
			if (obj->intelligent) {
				ReadSG(&item->carriedItem, sizeof(item->carriedItem));
			}
			ReadSG(&item->timer, sizeof(item->timer));

			if (CHK_ANY(item->flags, IFL_CLEARBODY)) {
				KillItem(i);
				item->status = ITEM_DISABLED;
			}
			else {
				if (CHK_ANY(item->flags, 0x01) && !item->active) {
					AddActiveItem(i);
				}
				item->status = (item->flags >> 1) & 3;
				if (CHK_ANY(item->flags, 0x08)) {
					item->gravity = 1;
				}
				if (!CHK_ANY(item->flags, 0x10)) {
					item->collidable = 0;
				}
			}

			if (is_creature) {
				EnableBaddieAI(i, 1);
				CREATURE_INFO* creature = (CREATURE_INFO*)item->data;
				if (creature != NULL) {
					ReadSG(&creature->headRotation, sizeof(creature->headRotation));
					ReadSG(&creature->neckRotation, sizeof(creature->neckRotation));
					ReadSG(&creature->maximumTurn, sizeof(creature->maximumTurn));
					ReadSG(&creature->flags, sizeof(creature->flags));
					ReadSG(&creature->mood, sizeof(creature->mood));
				}
				else {
					SG_Point += sizeof(creature->headRotation) +
						        sizeof(creature->neckRotation) +
						        sizeof(creature->maximumTurn) +
						        sizeof(creature->flags) +
						        sizeof(creature->mood);
				}
			}
			else if (obj->intelligent) {
				item->data = NULL;
				if (item->clear_body && item->hitPoints <= 0 && !CHK_ANY(item->flags, IFL_CLEARBODY)) {
					item->nextActive = PrevItemActive;
					PrevItemActive = i;
				}
			}
			item->flags &= ~0xFF;

			if (obj->collision == PuzzleHoleCollision && (item->status == ITEM_ACTIVE || item->status == ITEM_DISABLED)) {
				item->objectID += ID_PUZZLE_DONE1 - ID_PUZZLE_HOLE1;
			}
			if (obj->collision == PickUpCollision && item->status == ITEM_DISABLED) {
				RemoveDrawnItem(i);
			}
			if ((item->objectID == ID_WINDOW1 || item->objectID == ID_WINDOW2 || item->objectID == ID_WINDOW3 || item->objectID == ID_WINDOW4) && CHK_ANY(item->flags, IFL_ONESHOT)) {
				item->meshBits = 0x100;
			}
			if (item->objectID == ID_MINE && CHK_ANY(item->flags, IFL_ONESHOT)) {
				item->meshBits = 1;
			}
		}

		if (obj->control == MovableBlock && item->status == ITEM_INACTIVE) {
			AlterFloorHeight(item, -1024);
		}

		switch (item->objectID) {
		case ID_BOAT:
			ReadSG(item->data, sizeof(BOAT_INFO));
			break;
		case ID_SKIDOO_FAST:
			ReadSG(item->data, sizeof(SKIDOO_INFO));
			break;
		case ID_LIFT:
			ReadSG(item->data, sizeof(int) * 2);
			break;
		default:
			break;
		}
		if (item->objectID == ID_SKIDMAN && item->status == ITEM_DISABLED) {
			Items[(int)item->data].objectID = ID_SKIDOO_FAST;
			InitialiseSkidoo((int)item->data);
		}

		if (item->objectID == ID_DRAGON_FRONT && item->status == ITEM_DISABLED) {
			item->pos.y -= 1010;
			DragonBones(i);
			item->pos.y += 1010;
		}
	}

	ReadSG(&Lara, sizeof(Lara));
	Lara.creature = NULL;
	Lara.spaz_effect = NULL;
	Lara.target = NULL;

	for (DWORD i = 0; i < ARRAY_SIZE(Lara.mesh_ptrs); ++i) {
		Lara.mesh_ptrs[i] = (short*)((long)Lara.mesh_ptrs[i] + (long)Meshes);
	}
	Lara.left_arm.frame_base = (short*)((long)Lara.left_arm.frame_base + (long)AnimFrames);
	Lara.right_arm.frame_base = (short*)((long)Lara.right_arm.frame_base + (long)AnimFrames);

	if (Lara.weapon_item >= 0) {
		Lara.weapon_item = CreateItem();
		item = &Items[Lara.weapon_item];
		ReadSG(&item->objectID, sizeof(item->objectID));
		ReadSG(&item->animNumber, sizeof(item->animNumber));
		ReadSG(&item->frameNumber, sizeof(item->frameNumber));
		ReadSG(&item->currentAnimState, sizeof(item->currentAnimState));
		ReadSG(&item->goalAnimState, sizeof(item->goalAnimState));
		item->status = ITEM_ACTIVE;
		item->roomNumber = NO_ROOM;
	}

	if (Lara.burn) {
		Lara.burn = 0;
		LaraBurn();
	}

	ReadSG(&FlipEffect, sizeof(FlipEffect));
	ReadSG(&FlipTimer, sizeof(FlipTimer));
	ReadSG(&IsMonkAngry, sizeof(IsMonkAngry));

	int numFlares = 0;
	ReadSG(&numFlares, sizeof(numFlares));
	for (int i = 0; i < numFlares; ++i) {
		short itemNumber = CreateItem();
		item = &Items[itemNumber];
		item->objectID = ID_FLARE_ITEM;
		ReadSG(&item->pos, sizeof(item->pos));
		ReadSG(&item->roomNumber, sizeof(item->roomNumber));
		ReadSG(&item->speed, sizeof(item->speed));
		ReadSG(&item->fallSpeed, sizeof(item->fallSpeed));
		InitialiseItem(itemNumber);
		AddActiveItem(itemNumber);
		int flareAge;
		ReadSG(&flareAge, sizeof(flareAge));
		item->data = (LPVOID)flareAge;
	}
}

void ResetSG() {
	SG_Count = 0;
	SG_Point = SaveGame.buffer;
}

void WriteSG(void* ptr, int len) {
	SG_Count += len;
	if (SG_Count >= sizeof(SaveGame.buffer)) {
		S_ExitSystem("FATAL: Savegame is too big to fit in buffer");
	}
	memcpy(SG_Point, ptr, len);
	SG_Point += len;
}

void ReadSG(void* ptr, int len) {
	SG_Count += len;
	memcpy(ptr, SG_Point, len);
	SG_Point += len;
}

/*
 * Inject function
 */
void Inject_SaveGame() {
	INJECT(0x00439190, InitialiseStartInfo);
	INJECT(0x00439200, ModifyStartInfo);
	INJECT(0x004392E0, CreateStartInfo);
	INJECT(0x004394F0, CreateSaveGameInfo);
	INJECT(0x00439A20, ExtractSaveGameInfo);
	INJECT(0x0043A280, ResetSG);
	INJECT(0x0043A2A0, WriteSG);
	INJECT(0x0043A2F0, ReadSG);
}