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

#ifndef TRAPS_H_INCLUDED
#define TRAPS_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */

void MineControl(short mineID); // 0x00440FC0
void ControlSpikeWall(short itemNumber); // 0x004411C0
void ControlCeilingSpikes(short itemNumber); // 0x00441300
void HookControl(short itemNumber); // 0x00441420
#define PropellerControl ((void(__cdecl*)(short)) 0x004414B0)
void SpinningBlade(short itemNumber); // 0x00441640
void InitialiseIcicle(short itemNumber);
void IcicleControl(short itemNumber); // 0x004417C0
void InitialiseBlade(short itemNumber); // 0x004418C0
void BladeControl(short itemNumber); // 0x00441900
void InitialiseKillerStatue(short itemNumber); // 0x004419A0
void KillerStatueControl(short itemNumber); // 0x004419F0
void SpringBoardControl(short itemNumber); // 0x00441B00
void InitialiseRollingBall(short itemNumber); // 0x00441BE0
void RollingBallControl(short itemNumber); // 0x00441C20
void RollingBallCollision(short itemNumber, ITEM_INFO* laraItem, COLL_INFO* coll); // 0x00441F70
#define SpikeCollision ((void(__cdecl*)(short,ITEM_INFO*,COLL_INFO*)) 0x004421C0)
#define TrapDoorControl ((void(__cdecl*)(short)) 0x00442320)
#define TrapDoorFloor ((void(__cdecl*)(ITEM_INFO*,int,int,int,int*)) 0x00442370)
#define TrapDoorCeiling ((void(__cdecl*)(ITEM_INFO*,int,int,int,int*)) 0x004423B0)
#define OnTrapDoor ((int(__cdecl*)(ITEM_INFO*,int,int)) 0x004423F0)
void Pendulum(short itemNumber); // 0x004424A0
#define FallingBlock ((void(__cdecl*)(short)) 0x004425B0)
#define FallingBlockFloor ((void(__cdecl*)(ITEM_INFO*,int,int,int,int*)) 0x004426C0)
#define FallingBlockCeiling ((void(__cdecl*)(ITEM_INFO*,int,int,int,int*)) 0x00442700)
void TeethTrap(short itemNumber); // 0x00442750
void FallingCeiling(short itemNumber); // 0x00442810
void DartEmitterControl(short itemNumber); // 0x004428F0
void DartsControl(short itemNumber); // 0x00442A30
void DartEffectControl(short fxID); // 0x00442B90
void FlameEmitterControl(short item_id); // 0x00442BE0
void FlameControl(short fx_id); // 0x00442C70
void LaraBurn(); // 0x00442DE0
void LavaBurn(ITEM_INFO* item); // 0x00442E30
void LavaSpray(short itemNumber); // 0x00442F20
void ControlLavaBlob(short fxNum); // 0x00442FF0
void InitializeCandleFlameEmitter(short itemNumber);
void CandleFlameEmitterControl(short itemNumber);
void CandleEmitterSpriteControl(short fxNum);

#endif // TRAPS_H_INCLUDED
