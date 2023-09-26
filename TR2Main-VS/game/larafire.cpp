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
#include "game/sphere.h"
#include "specific/game.h"
#include "global/vars.h"

int FireWeapon(int weaponType, ITEM_INFO* target, ITEM_INFO* src, short* angles) {
	WEAPON_INFO* weaponInfo = NULL;
	SPHERE spherelist[33] = {};
	SPHERE* sphereresult = NULL;
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
	CREATURE_INFO* creature = NULL;

	if (item->hitPoints > 0 && item->hitPoints <= damage) {
		++SaveGame.statistics.kills;
	}

	item->hitPoints -= damage;
	item->hitStatus = TRUE;

	if (dest != NULL) {
		DoBloodSplat(dest->x, dest->y, dest->z, item->speed, item->pos.rotY, item->roomNumber);
	}

	if (IsMonkAngry == FALSE && (item->objectID == ID_MONK1 || item->objectID == ID_MONK2) && item->data != NULL)
	{
		creature = (CREATURE_INFO*)item->data;
		if ((creature->flags & 0x0FFF) > 10 || creature->mood == MOOD_BORED)
			IsMonkAngry = TRUE;
	}
}

 /*
  * Inject function
  */
void Inject_LaraFire() {
	//	INJECT(0x0042E740, LaraGun);
	//	INJECT(0x0042ECB0, CheckForHoldingState);
	//	INJECT(0x0042ECF0, InitialiseNewWeapon);
	//	INJECT(0x0042EE30, LaraTargetInfo);
	//	INJECT(0x0042EFD0, LaraGetNewTarget);
	//	INJECT(0x0042F1F0, find_target_point);
	//	INJECT(0x0042F2A0, AimWeapon);
	INJECT(0x0042F370, FireWeapon);
	INJECT(0x0042F6E0, HitTarget);
	//	INJECT(0x0042F780, SmashItem);
	//	INJECT(0x0042F7E0, WeaponObject);
}
