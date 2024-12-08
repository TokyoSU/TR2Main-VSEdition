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

#ifndef LARA_FIRE_H_INCLUDED
#define LARA_FIRE_H_INCLUDED

#include "global/types.h"

extern bool IsMonkAngry;

 /*
  * Function list
  */
void LaraGun(); // 0x0042E740
int CheckForHoldingState(int state); // 0x0042ECB0
void InitialiseNewWeapon(); // 0x0042ECF0
void LaraTargetInfo(WEAPON_INFO* weapon); // 0x0042EE30
void LaraGetNewTarget(WEAPON_INFO* weapon); // 0x0042EFD0
void find_target_point(ITEM_INFO* item, GAME_VECTOR* target); // 0x0042F1F0
void AimWeapon(WEAPON_INFO* weapon, LARA_ARM* arm); // 0x0042F2A0
int FireWeapon(int weaponType, ITEM_INFO* target, ITEM_INFO* src, short* angles); // 0x0042F370
void HitTarget(ITEM_INFO* item, GAME_VECTOR* dest, int damage); // 0x0042F6E0
void SmashItem(short itemNumber, int weaponType); // 0x0042F780
int WeaponObject(int weaponType); // 0x0042F7E0

#endif // LARA_FIRE_H_INCLUDED
