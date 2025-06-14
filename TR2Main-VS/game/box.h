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

enum BaddieTargetType
{
	BTT_ToBandit = 0x1,
	BTT_ToMonk = 0x2,
	BTT_ToCult = 0x4,
	BTT_ToTrex = 0x8,
	BTT_ToWorker = 0x10
};

enum class DamageTargetFlags
{
	None = 0,
	DisableBlood = 1,
	DoLotsOfBloods = 2
};

typedef short(CREATURE_EFFECT_CALLBACK)(int x, int y, int z, short speed, short rotY, short roomNumber);

 /*
  * Function list
  */
void InitialiseCreature(short itemNumber); // 0x0040E190
int CreatureActive(short itemNumber); // 0x0040E1C0
void CreatureAIInfo(ITEM_INFO* item, AI_INFO* AI); // 0x0040E210
int SearchLOT(LOT_INFO* LOT, int expansion); // 0x0040E470
//#define SearchLOT ((int(__cdecl*)(LOT_INFO*,int)) 0x0040E470)
#define UpdateLOT ((int(__cdecl*)(LOT_INFO*,int)) 0x0040E670)
void TargetBox(LOT_INFO* LOT, UINT16 boxNum); // 0x0040E6E0
#define StalkBox ((int(__cdecl*)(ITEM_INFO*,ITEM_INFO*,short)) 0x0040E780)
#define EscapeBox ((int(__cdecl*)(ITEM_INFO*,ITEM_INFO*,short)) 0x0040E880)
int ValidBox(ITEM_INFO* item, UINT16 zoneNumber, UINT16 boxNumber); // 0x0040E930
void CreatureMood(ITEM_INFO* item, AI_INFO* ai, BOOL isViolent); // 0x0040E9E0
#define CalculateTarget ((int(__cdecl*)(PHD_VECTOR*, ITEM_INFO*, LOT_INFO*)) 0x0040EE50)
int CreatureCreature(short itemNumber); // 0x0040F2B0
int BadFloor(int x, int y, int z, int boxHeight, int nextHeight, short roomNumber, LOT_INFO* LOT); // 0x0040F3B0
void CreatureDie(short itemNumber, BOOL explode); // 0x0040F440
int CreatureAnimation(short itemNumber, short angle, short tilt); // 0x0040F500
short CreatureTurn(ITEM_INFO* item, short maximumTurn); // 0x0040FDD0
#define CreatureTilt ((void(__cdecl*)(ITEM_INFO*, short)) 0x0040FEB0)
#define CreatureHead ((void(__cdecl*)(ITEM_INFO*, short)) 0x0040FEF0)
#define CreatureNeck ((void(__cdecl*)(ITEM_INFO*, short)) 0x0040FF40)
#define CreatureFloat ((void(__cdecl*)(short)) 0x0040FF90)
#define CreatureUnderwater ((void(__cdecl*)(ITEM_INFO*, int)) 0x00410040)
#define CreatureEffect ((short(__cdecl*)(ITEM_INFO*, const BITE_INFO*, CREATURE_EFFECT_CALLBACK)) 0x00410090)
int CreatureVault(short itemNumber, short angle, int vault, int shift); // 0x004100F0
void CreatureKill(ITEM_INFO* item, int killAnim, int killState, int laraKillState); // 0x00410230
void GetBaddieTarget(short creatureIdx, BaddieTargetType type); // 0x004103A0

/// Not from TR2:
void CreatureDropItem(ITEM_INFO* item);
bool IsCreatureNearTarget(ITEM_INFO* item, ITEM_INFO* enemy, int distance = CLICK(2));
bool DamageTarget(ITEM_INFO* item, ITEM_INFO* enemy, const BITE_INFO* bite, int damage, DamageTargetFlags flags = DamageTargetFlags::None); // NOTE: Only for hand to hand. (Not exist in the original game)
bool DamageLaraOrEnemy(ITEM_INFO* item, ITEM_INFO* enemy, const BITE_INFO* bite, int damageLara, int damageEnemy, bool touchBitsLara, int distance = CLICK(2), DamageTargetFlags flags = DamageTargetFlags::None); // NOTE: Only for hand to hand. (Not exist in the original game)
void SetAnimation(ITEM_INFO* item, int animID, int stateID, int frameID = 0); // NOTE: Force animation. (Not exist in the original game)
void SetAnimationWithObject(ITEM_INFO* item, GAME_OBJECT_ID fromObjectIndex, int animID, int stateID, int frameID = 0); // NOTE: Force animation using another object animation. (Not exist in the original game)

#endif // BOX_H_INCLUDED
