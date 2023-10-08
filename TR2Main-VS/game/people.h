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

#ifndef PEOPLE_H_INCLUDED
#define PEOPLE_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
BOOL Targetable(ITEM_INFO* item, AI_INFO* info); // 0x00435EB0
// 0x00435F40: ControlGlow
// 0x00435F80: ControlGunShot
short GunShot(int x, int y, int z, short speed, short rotY, short roomNumber); // 0x00435FD0
short GunHit(int x, int y, int z, short speed, short rotY, short roomNumber); // 0x00436040
short GunMiss(int x, int y, int z, short speed, short rotY, short roomNumber); // 0x00436100
BOOL ShotTargetNew(ITEM_INFO* item, AI_INFO* AI, const BITE_INFO* bite, short angle, int damageLara, int damageOther); // NOTE, TODO: Will remplace the original below.
BOOL ShotTarget(ITEM_INFO* item, AI_INFO* AI, const BITE_INFO* bite, short angle, int damage); // 0x004361B0
#define InitialiseCult1 ((void(__cdecl*)(short)) 0x00436380)
#define Cult1Control ((void(__cdecl*)(short)) 0x004363D0)
#define InitialiseCult3 ((void(__cdecl*)(short)) 0x00436800)
#define Cult3Control ((void(__cdecl*)(short)) 0x00436850)
#define Worker1Control ((void(__cdecl*)(short)) 0x00436DC0)
#define Worker2Control ((void(__cdecl*)(short)) 0x004371C0)
void BanditControl(short itemID); // 0x00437620
void Bandit2Control(short itemID); // 0x00437960
void WinstonControl(short itemID); // 0x00437DA0

#endif // PEOPLE_H_INCLUDED
