/*
 * Copyright (c) 2017-2019 Michael Chaban. All rights reserved.
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
#include "game/larafire.h"
#include "3dsystem/3d_gen.h"
#include "game/control.h"
#include "game/effects.h"
#include "game/invfunc.h"
#include "game/items.h"
#include "game/lara1gun.h"
#include "game/lara2gun.h"
#include "game/laraflare.h"
#include "game/lot.h"
#include "game/objects.h"
#include "game/sound.h"
#include "game/sphere.h"
#include "specific/game.h"
#include "global/vars.h"

#if defined(FEATURE_MOD_CONFIG)
#include "modding/mod_utils.h"
#endif

bool IsMonkAngry = false;

void LaraGun() {
	DWORD* ammo;

	if (Lara.left_arm.flash_gun > 0)
		--Lara.left_arm.flash_gun;
	if (Lara.right_arm.flash_gun > 0)
		--Lara.right_arm.flash_gun;
	if (LaraItem->hitPoints <= 0) {
		Lara.gun_status = LGS_Armless;
	}
	else {
		if (Lara.gun_status == LGS_Armless) {
			if (CHK_ANY(InputStatus, IN_DRAW)) {
				Lara.request_gun_type = Lara.last_gun_type;
			}
			else {
				if (CHK_ANY(InputStatus, IN_FLARE)) {
					if (Lara.gun_type == LGT_Flare) {
						Lara.gun_status = LGS_Undraw;
					}
					else {
						if (Inv_RequestItem(ID_FLARES_ITEM))
							Lara.request_gun_type = LGT_Flare;
					}
				}
			}
			if (Lara.request_gun_type != Lara.gun_type || CHK_ANY(InputStatus, IN_DRAW)) {
				if (Lara.request_gun_type != LGT_Flare &&
					(Lara.skidoo != -1 ||
						(Lara.request_gun_type != LGT_Harpoon &&
							Lara.water_status != LWS_AboveWater &&
							(Lara.water_status != LWS_Wade ||
								Lara.water_surface_dist <= -Weapons[Lara.gun_type].gunHeight))))
				{
					Lara.last_gun_type = Lara.request_gun_type;
					if (Lara.gun_type == LGT_Flare) {
						Lara.request_gun_type = LGT_Flare;
					}
					else {
						Lara.gun_type = Lara.request_gun_type;
					}
				}
				else {
					if (Lara.gun_type == LGT_Flare) {
						CreateFlare(FALSE);
						undraw_flare_meshes();
						Lara.flare_control_left = 0;
					}
					Lara.gun_type = Lara.request_gun_type;
					InitialiseNewWeapon();
					Lara.gun_status = LGS_Draw;
					Lara.right_arm.frame_number = 0;
					Lara.left_arm.frame_number = 0;
				}
			}
		}
		else {
			if (Lara.gun_status == LGS_Ready) {
				if (CHK_ANY(InputStatus, IN_FLARE) && Inv_RequestItem(ID_FLARES_ITEM))
					Lara.request_gun_type = LGT_Flare;
				if (CHK_ANY(InputStatus, IN_DRAW) ||
					Lara.request_gun_type != Lara.gun_type ||
					(Lara.gun_type != LGT_Harpoon &&
						Lara.water_status != LWS_AboveWater &&
						(Lara.water_status != LWS_Wade ||
							Lara.water_surface_dist < -Weapons[Lara.gun_type].gunHeight)))
				{
					Lara.gun_status = LGS_Undraw;
				}
			}
		}
	}
	switch (Lara.gun_status) {
	case LGS_Armless:
		if (Lara.gun_type == LGT_Flare) {
			if (Lara.skidoo == -1 && !CheckForHoldingState(LaraItem->currentAnimState)) {
				Lara.flare_control_left = 0;
			}
			else {
				if (!Lara.flare_control_left) {
					Lara.flare_control_left = 1;
					Lara.left_arm.frame_number = 95;
				}
				else {
					if (Lara.left_arm.frame_number) {
						++Lara.left_arm.frame_number;
						if (Lara.left_arm.frame_number == 110)
							Lara.left_arm.frame_number = 0;
					}
				}
			}
			DoFlareInHand(Lara.flare_age);
			set_flare_arm(Lara.left_arm.frame_number);
		}
		break;
	case LGS_HandBusy:
		if (Lara.gun_type == LGT_Flare) {
			Lara.flare_control_left = Lara.skidoo != -1 || CheckForHoldingState(LaraItem->currentAnimState);
			DoFlareInHand(Lara.flare_age);
			set_flare_arm(Lara.left_arm.frame_number);
		}
		break;
	case LGS_Draw:
		if (Lara.gun_type != LGT_Flare && Lara.gun_type != LGT_Unarmed)
			Lara.last_gun_type = Lara.gun_type;
		switch (Lara.gun_type) {
		case LGT_Pistols:
		case LGT_Magnums:
		case LGT_Uzis:
			if (Camera.type != CAM_Cinematic && Camera.type != CAM_Look)
				Camera.type = CAM_Combat;
			draw_pistols(Lara.gun_type);
			break;
		case LGT_Shotgun:
		case LGT_M16:
		case LGT_Grenade:
		case LGT_Harpoon:
			if (Camera.type != CAM_Cinematic && Camera.type != CAM_Look)
				Camera.type = CAM_Combat;
			draw_shotgun(Lara.gun_type);
			break;
		case LGT_Flare:
			draw_flare();
			break;
		default:
			Lara.gun_status = LGS_Armless;
			break;
		}
		break;
	case LGS_Undraw:
		Lara.mesh_ptrs[14] = MeshPtr[Objects[ID_LARA].meshIndex + 14];
		switch (Lara.gun_type) {
		case LGT_Pistols:
		case LGT_Magnums:
		case LGT_Uzis:
			undraw_pistols(Lara.gun_type);
			break;
		case LGT_Shotgun:
		case LGT_M16:
		case LGT_Grenade:
		case LGT_Harpoon:
			undraw_shotgun(Lara.gun_type);
			break;
		case LGT_Flare:
			undraw_flare();
			break;
		}
		break;
	case LGS_Ready:
		if (!Lara.pistol_ammo || !CHK_ANY(InputStatus, IN_ACTION)) {
			Lara.mesh_ptrs[14] = MeshPtr[Objects[ID_LARA].meshIndex + 14];
		}
		else {
			Lara.mesh_ptrs[14] = MeshPtr[Objects[ID_LARA_UZIS].meshIndex + 14];
		}
		if (Camera.type != CAM_Cinematic && Camera.type != CAM_Look)
			Camera.type = CAM_Combat;
		switch (Lara.gun_type) {
		case LGT_Magnums:
			ammo = &Lara.magnum_ammo;
			break;
		case LGT_Uzis:
			ammo = &Lara.uzi_ammo;
			break;
		case LGT_Shotgun:
			ammo = &Lara.shotgun_ammo;
			break;
		case LGT_M16:
			ammo = &Lara.m16_ammo;
			break;
		case LGT_Grenade:
			ammo = &Lara.grenade_ammo;
			break;
		case LGT_Harpoon:
			ammo = &Lara.harpoon_ammo;
			break;
		default:
			ammo = &Lara.pistol_ammo;
			break;
		}
		if (CHK_ANY(InputStatus, IN_ACTION) && *ammo <= 0) {
			*ammo = 0;
			PlaySoundEffect(48, &LaraItem->pos, 0);
			Lara.request_gun_type = Inv_RequestItem(ID_PISTOL_ITEM) ? LGT_Pistols : LGT_Unarmed;
		}
		else {
			switch (Lara.gun_type) {
			case LGT_Pistols:
			case LGT_Magnums:
			case LGT_Uzis:
				PistolHandler(Lara.gun_type);
				break;
			case LGT_Shotgun:
			case LGT_M16:
			case LGT_Grenade:
			case LGT_Harpoon:
				RifleHandler(Lara.gun_type);
				break;
			}
		}
		break;
	case LGS_Special:
		draw_flare();
		break;
	}
}

void LaraGetNewTarget(WEAPON_INFO* weapon)
{
	CREATURE_INFO* creature = NULL;
	ITEM_INFO* targetItem = NULL, *bestTarget = NULL;
	GAME_VECTOR srcPos{}, targetPos{};
	VECTOR_ANGLES angle{};
	int bestDistance = 0x7FFFFFFF, distance = 0, x = 0, y = 0, z = 0;
	short bestAngle = 0x7FFF, angleY = 0;
	srcPos.x = LaraItem->pos.x;
	srcPos.y = LaraItem->pos.y - weapon->gunHeight;
	srcPos.z = LaraItem->pos.z;
	srcPos.roomNumber = LaraItem->roomNumber;

	for (int i = 0; i < MAX_CREATURES; i++)
	{
		creature = &BaddiesSlots[i];
		if (creature->itemID == -1 || Lara.item_number == creature->itemID)
			continue;
		targetItem = &Items[creature->itemID];
		if (targetItem->hitPoints <= 0)
			continue;
#if defined(FEATURE_MOD_CONFIG)
		if (Mod.laraIgnoreMonkIfNotAngry)
		{
			if ((targetItem->objectID == ID_MONK1 || targetItem->objectID == ID_MONK2) && !IsMonkAngry)
				continue;
		}
#endif
		x = targetItem->pos.x - srcPos.x;
		y = targetItem->pos.y - srcPos.y;
		z = targetItem->pos.z - srcPos.z;
		if (ABS(x) > weapon->targetDist ||
			ABS(y) > weapon->targetDist ||
			ABS(z) > weapon->targetDist)
			continue;
		distance = SQR(z) + SQR(y) + SQR(x);
		if (distance < SQR(weapon->targetDist))
		{
			find_target_point(targetItem, &targetPos);
			if (LOS(&srcPos, &targetPos))
			{
				phd_GetVectorAngles(targetPos.x - srcPos.x, targetPos.y - srcPos.y, targetPos.z - srcPos.z, &angle);
				angle.rotY -= LaraItem->pos.rotY + Lara.torso_y_rot;
				angle.rotX -= LaraItem->pos.rotX + Lara.torso_x_rot;

				if ((angle.rotY >= weapon->lockAngles[0])
				&&  (angle.rotY <= weapon->lockAngles[1])
				&&  (angle.rotX >= weapon->lockAngles[2])
				&&  (angle.rotX <= weapon->lockAngles[3]))
				{
					angleY = ABS(angle.rotY);
					if ((angleY < (bestAngle + 0xAAA))
					&&  (distance < bestDistance))
					{
						bestDistance = distance;
						bestAngle = angleY;
						bestTarget = targetItem;
					}
				}

			}
		}
	}

	Lara.target = bestTarget;
	LaraTargetInfo(weapon);
}

int FireWeapon(int weaponType, ITEM_INFO* target, ITEM_INFO* src, short* angles) {
	WEAPON_INFO* weaponInfo = NULL;
	SPHERE_INFO spherelist[33] = {};
	SPHERE_INFO* sphereresult = NULL;
	GAME_VECTOR startPos{}, targetPos{};
	PHD_3DPOS viewPos{};
	PHD_VECTOR gunPos{};
	DWORD* ammoPtr = NULL;
	int sphereCount = 0, bestdistance = 0, best = -1, radius = 0, sphereIdx = 0, hasHit = 0;
	short shatterIdx = -1;

	switch (weaponType) {
	case LGT_Magnums:
		ammoPtr = &Lara.magnum_ammo;
		if (SaveGame.bonusFlag)
			Lara.magnum_ammo = 1000;
		break;
	case LGT_Uzis:
		ammoPtr = &Lara.uzi_ammo;
		if (SaveGame.bonusFlag)
			Lara.uzi_ammo = 1000;
		break;
	case LGT_M16:
		ammoPtr = &Lara.m16_ammo;
		if (SaveGame.bonusFlag)
			Lara.m16_ammo = 1000;
		break;
	case LGT_Shotgun:
		ammoPtr = &Lara.shotgun_ammo;
		if (SaveGame.bonusFlag)
			Lara.shotgun_ammo = 1000;
		break;
	default:
		ammoPtr = &Lara.pistol_ammo;
		Lara.pistol_ammo = 1000;
		break;
	}

	// If no ammo then don't do anything and return !
	if (*ammoPtr <= 0) {
		*ammoPtr = 0;
		return 0;
	}
	
	--*ammoPtr; // Decrease ammo

	weaponInfo = &Weapons[weaponType];
	viewPos.x = src->pos.x;
	viewPos.y = src->pos.y - weaponInfo->gunHeight;
	viewPos.z = src->pos.z;
	viewPos.rotX = angles[1] + (weaponInfo->shotAccuracy * (GetRandomControl() - 0x4000)) / 0x10000;
	viewPos.rotY = angles[0] + (weaponInfo->shotAccuracy * (GetRandomControl() - 0x4000)) / 0x10000;
	viewPos.rotZ = 0;
	phd_GenerateW2V(&viewPos);
	
	sphereCount = GetSpheres(target, spherelist, FALSE);
	best = -1;
	bestdistance = INT_MAX;

	for (sphereIdx = 0; sphereIdx < sphereCount; sphereIdx++) {
		sphereresult = &spherelist[sphereIdx];
		radius = sphereresult->radius;
		if (ABS(sphereresult->x) < radius &&
			ABS(sphereresult->y) < radius &&
			    sphereresult->z  > radius &&
		   (SQR(sphereresult->x) + SQR(sphereresult->y)) < SQR(radius)) {
			if (sphereresult->z - radius < bestdistance) {
				bestdistance = sphereresult->z - radius;
				best = sphereIdx;
			}
		}
	}

	++SaveGame.statistics.shots;

	startPos.x = viewPos.x;
	startPos.y = viewPos.y;
	startPos.z = viewPos.z;
	startPos.roomNumber = src->roomNumber;

	// If target not hit
	if (best < 0) {
		targetPos.x = viewPos.x + ((weaponInfo->targetDist * PhdMatrixPtr->_20) >> W2V_SHIFT);
		targetPos.y = viewPos.y + ((weaponInfo->targetDist * PhdMatrixPtr->_21) >> W2V_SHIFT);
		targetPos.z = viewPos.z + ((weaponInfo->targetDist * PhdMatrixPtr->_22) >> W2V_SHIFT);
		hasHit = LOS(&startPos, &targetPos);
		shatterIdx = ObjectOnLOS(&startPos, &targetPos);
		if (shatterIdx != -1) {
			SmashItem(shatterIdx, weaponType);
		}
		else if (!hasHit) {
			Richochet(&targetPos);
		}
		return -1;
	}
	else
	{
		++SaveGame.statistics.hits;
		targetPos.x = viewPos.x + ((bestdistance * PhdMatrixPtr->_20) >> W2V_SHIFT);
		targetPos.y = viewPos.y + ((bestdistance * PhdMatrixPtr->_21) >> W2V_SHIFT);
		targetPos.z = viewPos.z + ((bestdistance * PhdMatrixPtr->_22) >> W2V_SHIFT);
		shatterIdx = ObjectOnLOS(&startPos, &targetPos);
		if (shatterIdx != -1)
			SmashItem(shatterIdx, weaponType);
		HitTarget(target, &targetPos, weaponInfo->damage);
	}

	return 1;
}

void HitTarget(ITEM_INFO* item, GAME_VECTOR* dest, int damage) {
	if (item->hitPoints > 0 && item->hitPoints <= damage) {
		++SaveGame.statistics.kills;
	}

	item->hitPoints -= damage;
	item->hitStatus = TRUE;

	if (dest != NULL) {
		DoBloodSplat(dest->x, dest->y, dest->z, item->speed, item->pos.rotY, item->roomNumber);
	}

	if (!IsMonkAngry && (item->objectID == ID_MONK1 || item->objectID == ID_MONK2) && item->data != NULL)
	{
		CREATURE_INFO* creature = GetCreatureInfo(item);
		creature->flags += damage;
		if ((creature->flags & 0xFFF) > 10 || creature->mood == MOOD_BORED)
			IsMonkAngry = true;
	}
}

void SmashItem(short itemID, int weaponType) {
	ITEM_INFO* item = &Items[itemID];
	if (item->objectID == ID_WINDOW1 || item->objectID == ID_WINDOW3) {
		SmashWindow(itemID);
	}
	else {
		if (item->objectID == ID_BELL && item->status != ITEM_ACTIVE) {
			item->status = ITEM_ACTIVE;
			AddActiveItem(itemID);
		}
	}
}

 /*
  * Inject function
  */
void Inject_LaraFire() {
	INJECT(0x0042E740, LaraGun);
	//INJECT(0x0042ECB0, CheckForHoldingState);
	//INJECT(0x0042ECF0, InitialiseNewWeapon);
	//INJECT(0x0042EE30, LaraTargetInfo);
	INJECT(0x0042EFD0, LaraGetNewTarget);
	//INJECT(0x0042F1F0, find_target_point);
	//INJECT(0x0042F2A0, AimWeapon);
	INJECT(0x0042F370, FireWeapon);
	INJECT(0x0042F6E0, HitTarget);
	INJECT(0x0042F780, SmashItem);
	//INJECT(0x0042F7E0, WeaponObject);
}
