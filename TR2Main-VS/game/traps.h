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
void ControlSpikeWall(short itemID); // 0x004411C0
void ControlCeilingSpikes(short itemID); // 0x00441300
void HookControl(short itemID); // 0x00441420

//	0x004414B0:		PropellerControl

void SpinningBlade(short itemID); // 0x00441640
void IcicleControl(short itemID); // 0x004417C0
void InitialiseBlade(short itemID); // 0x004418C0
void BladeControl(short itemID); // 0x00441900
void InitialiseKillerStatue(short itemID); // 0x004419A0
void KillerStatueControl(short itemID); // 0x004419F0

//	0x00441B00:		SpringBoardControl
//	0x00441BE0:		InitialiseRollingBall
//	0x00441C20:		RollingBallControl
//	0x00441F70:		RollingBallCollision
//	0x004421C0:		SpikeCollision
//	0x00442320:		TrapDoorControl
//	0x00442370:		TrapDoorFloor
//	0x004423B0:		TrapDoorCeiling
//	0x004423F0:		OnTrapDoor

void Pendulum(short itemID); // 0x004424A0

//	0x004425B0:		FallingBlock
//	0x004426C0:		FallingBlockFloor
//	0x00442700:		FallingBlockCeiling

void TeethTrap(short itemID); // 0x00442750
void FallingCeiling(short itemID); // 0x00442810
void DartEmitterControl(short itemID); // 0x004428F0
void DartsControl(short itemID); // 0x00442A30
void DartEffectControl(short fxID); // 0x00442B90
void FlameEmitterControl(short item_id); // 0x00442BE0
void FlameControl(short fx_id); // 0x00442C70
void LaraBurn(); // 0x00442DE0
void LavaBurn(ITEM_INFO* item); // 0x00442E30

//	0x00442F20:		LavaSpray
//	0x00442FF0:		ControlLavaBlob

#endif // TRAPS_H_INCLUDED
