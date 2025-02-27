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

#ifndef ENEMIES_H_INCLUDED
#define ENEMIES_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
short Knife(int x, int y, int z, short speed, short rotY, short roomNum); // 0x0041DB30
void Cult2Control(short itemNumber); // 0x0041DBB0
void MonkControl(short itemNumber); // 0x0041DFE0
void Worker3Control(short itemNumber); // 0x0041E4B0
void DrawXianLord(ITEM_INFO* item); // 0x0041EAC0
void XianDamage(ITEM_INFO* item, CREATURE_INFO* creature, int damage, int damageOther); // 0x0041EEC0
void InitialiseXianLord(short itemNumber); // 0x0041EF70
void XianLordControl(short itemNumber); // 0x0041EFD0
void WarriorSparkleTrail(ITEM_INFO* item); // 0x0041F5B0
void WarriorControl(short itemNumber); // 0x0041F650

#endif // ENEMIES_H_INCLUDED
