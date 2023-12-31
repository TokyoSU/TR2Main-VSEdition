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

#ifndef BOX_H_INCLUDED
#define BOX_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
#define InitialiseCreature ((void(__cdecl*)(short)) 0x0040E190)
#define CreatureActive ((BOOL(__cdecl*)(short)) 0x0040E1C0)
void CreatureAIInfo(ITEM_INFO* item, AI_INFO* AI); // 0x0040E210
//0x0040E470:		SearchLOT
//0x0040E670:		UpdateLOT
//0x0040E6E0:		TargetBox
//0x0040E780:		StalkBox
//0x0040E880:		EscapeBox
//0x0040E930:		ValidBox
#define CreatureMood ((void(__cdecl*)(ITEM_INFO*, AI_INFO*, BOOL)) 0x0040E9E0)
#define CalculateTarget ((int(__cdecl*)(PHD_VECTOR*, ITEM_INFO*, LOT_INFO*)) 0x0040EE50)
//0x0040F2B0:		CreatureCreature
//0x0040F3B0:		BadFloor
void CreatureDie(short itemID, BOOL explode); // 0x0040F440
#define CreatureAnimation ((int(__cdecl*)(short, short, short)) 0x0040F500)
#define CreatureTurn ((short(__cdecl*)(ITEM_INFO *, short)) 0x0040FDD0)
#define CreatureTilt ((void(__cdecl*)(ITEM_INFO *, short)) 0x0040FEB0)
#define CreatureHead ((void(__cdecl*)(ITEM_INFO *, short)) 0x0040FEF0)
#define CreatureNeck ((void(__cdecl*)(ITEM_INFO *, short)) 0x0040FF40)
//0x0040FF90:		CreatureFloat
//0x00410040:		CreatureUnderwater
#define CreatureEffect ((short(__cdecl*)(ITEM_INFO *, const BITE_INFO *, short(__cdecl*)(int, int, int, short, short, short))) 0x00410090)
#define CreatureVault ((int(__cdecl*)(short,short,int,int)) 0x004100F0)
void CreatureKill(ITEM_INFO* item, int killAnim, int killState, int laraKillState); // 0x00410230
void GetBaddieTarget(short creatureIdx, BOOL isMonk); // 0x004103A0
bool IsCreatureNearTarget(ITEM_INFO* item, ITEM_INFO* enemy, int distance = CLICK(2));
// NOTE: Only for hand to hand. (Not exist in the original game)
bool DamageTarget(ITEM_INFO* item, ITEM_INFO* enemy, const BITE_INFO* bite, int damage);
// NOTE: Only for hand to hand. (Not exist in the original game)
bool DamageLaraOrEnemy(ITEM_INFO* item, ITEM_INFO* enemy, const BITE_INFO* bite, int damageLara, int damageEnemy, bool touchBitsLara);
// NOTE: Force animation. (Not exist in the original game)
void SetAnimation(ITEM_INFO* item, int animID, int stateID, int frameID = 0);

#endif // BOX_H_INCLUDED
