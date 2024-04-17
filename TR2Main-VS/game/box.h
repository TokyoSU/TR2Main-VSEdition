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
void InitialiseCreature(short itemNum); // 0x0040E190
int CreatureActive(short itemNum); // 0x0040E1C0
void CreatureAIInfo(ITEM_INFO* item, AI_INFO* AI); // 0x0040E210
//0x0040E470:		SearchLOT
//0x0040E670:		UpdateLOT
void TargetBox(LOT_INFO* LOT, short boxNum); // 0x0040E6E0
#define StalkBox ((int(__cdecl*)(ITEM_INFO*,ITEM_INFO*,short)) 0x0040E780)
#define EscapeBox ((int(__cdecl*)(ITEM_INFO*,ITEM_INFO*,short)) 0x0040E880)
int ValidBox(ITEM_INFO* item, short zoneNum, short boxNum); // 0x0040E930
void CreatureMood(ITEM_INFO* item, AI_INFO* ai, BOOL isViolent); // 0x0040E9E0
//#define CreatureMood ((void(__cdecl*)(ITEM_INFO*, AI_INFO*, BOOL)) 0x0040E9E0)
#define CalculateTarget ((int(__cdecl*)(PHD_VECTOR*, ITEM_INFO*, LOT_INFO*)) 0x0040EE50)
//0x0040F2B0:		CreatureCreature
int BadFloor(int x, int y, int z, int boxHeight, int nextHeight, short roomNum, LOT_INFO* LOT); // 0x0040F3B0
void CreatureDie(short itemID, BOOL explode); // 0x0040F440
#define CreatureAnimation ((int(__cdecl*)(short, short, short)) 0x0040F500)
#define CreatureTurn ((short(__cdecl*)(ITEM_INFO *, short)) 0x0040FDD0)
#define CreatureTilt ((void(__cdecl*)(ITEM_INFO *, short)) 0x0040FEB0)
#define CreatureHead ((void(__cdecl*)(ITEM_INFO *, short)) 0x0040FEF0)
#define CreatureNeck ((void(__cdecl*)(ITEM_INFO *, short)) 0x0040FF40)
#define CreatureFloat ((void(__cdecl*)(short)) 0x0040FF90)
#define CreatureUnderwater ((void(__cdecl*)(ITEM_INFO*, int)) 0x00410040)
#define CreatureEffect ((short(__cdecl*)(ITEM_INFO *, const BITE_INFO *, short(__cdecl*)(int, int, int, short, short, short))) 0x00410090)
#define CreatureVault ((int(__cdecl*)(short,short,int,int)) 0x004100F0)
void CreatureKill(ITEM_INFO* item, int killAnim, int killState, int laraKillState); // 0x00410230
void GetBaddieTarget(short creatureIdx, BOOL isMonk); // 0x004103A0
void CreatureDropItem(ITEM_INFO* item);

bool IsCreatureNearTarget(ITEM_INFO* item, ITEM_INFO* enemy, int distance = CLICK(2));
bool DamageTarget(ITEM_INFO* item, ITEM_INFO* enemy, const BITE_INFO* bite, int damage); // NOTE: Only for hand to hand. (Not exist in the original game)
bool DamageLaraOrEnemy(ITEM_INFO* item, ITEM_INFO* enemy, const BITE_INFO* bite, int damageLara, int damageEnemy, bool touchBitsLara); // NOTE: Only for hand to hand. (Not exist in the original game)
void SetAnimation(ITEM_INFO* item, int animID, int stateID, int frameID = 0); // NOTE: Force animation. (Not exist in the original game)
void SetAnimationWithObject(ITEM_INFO* item, GAME_OBJECT_ID fromObjectIndex, int animID, int stateID, int frameID = 0); // NOTE: Force animation using another object animation. (Not exist in the original game)

#endif // BOX_H_INCLUDED
