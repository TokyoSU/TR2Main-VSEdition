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

#ifndef EFFECTS_H_INCLUDED
#define EFFECTS_H_INCLUDED

#include "global/types.h"

 /*
  * Function list
  */
int ItemNearLara(PHD_3DPOS* pos, int distance); // 0x0041C4B0
void SoundEffects(); // 0x0041C540
__int16 DoBloodSplat(int x, int y, int z, __int16 speed, __int16 direction, __int16 roomID); // 0x0041C5B0
void DoLotsOfBlood(int x, int y, int z, __int16 speed, __int16 direction, __int16 roomID, int number); // 0x0041C610
void ControlBlood1(__int16 fxID); // 0x0041C6C0
void ControlExplosion1(__int16 fxID); // 0x0041C750
void Richochet(GAME_VECTOR* pos); // 0x0041C7D0
void ControlRichochet1(__int16 fxID); // 0x0041C850
void CreateBubble(PHD_3DPOS* pos, __int16 roomNumber); // 0x0041C880
void LaraBubbles(ITEM_INFO* item); // 0x0041C8F0
void ControlBubble1(__int16 fxID); // 0x0041C970
void Splash(ITEM_INFO* item); // 0x0041CA70
void WadeSplash(ITEM_INFO* item, int height);
void ControlSplash1(__int16 fxID); // 0x0041CB40
void ControlWaterSprite(__int16 fxID); // 0x0041CBC0
void ControlSnowSprite(__int16 fxID); // 0x0041CC70
void ControlHotLiquid(__int16 fxID); // 0x0041CD00
void WaterFall(__int16 itemID); // 0x0041CDE0
void finish_level_effect(ITEM_INFO* item); // 0x0041CF20
void turn180_effect(ITEM_INFO* item); // 0x0041CF30
void floor_shake_effect(ITEM_INFO* item); // 0x0041CF50
void lara_normal_effect(ITEM_INFO* item); // 0x0041CFF0
void BoilerFX(ITEM_INFO* item); // 0x0041D030
void FloodFX(ITEM_INFO* item); // 0x0041D050
void RubbleFX(ITEM_INFO* item); // 0x0041D0E0
void ChandelierFX(ITEM_INFO* item); // 0x0041D110
void ExplosionFX(ITEM_INFO* item); // 0x0041D140
void PistonFX(ITEM_INFO* item); // 0x0041D170
void CurtainFX(ITEM_INFO* item); // 0x0041D190
void StatueFX(ITEM_INFO* item); // 0x0041D1B0
void SetChangeFX(ITEM_INFO* item); // 0x0041D1D0
void ControlDingDong(__int16 itemID); // 0x0041D1F0
void ControlLaraAlarm(__int16 itemID); // 0x0041D230
void ControlAlarmSound(__int16 itemID); // 0x0041D270
void ControlBirdTweeter(__int16 itemID); // 0x0041D2E0
void DoChimeSound(ITEM_INFO* item); // 0x0041D340
void ControlClockChimes(__int16 itemID); // 0x0041D3A0
void SphereOfDoomCollision(__int16 itemID, ITEM_INFO* laraItem, COLL_INFO* coll); // 0x0041D410
void SphereOfDoom(__int16 itemID); // 0x0041D540
void DrawSphereOfDoom(ITEM_INFO* item); // 0x0041D630
void lara_hands_free(ITEM_INFO* item); // 0x0041D760
void flip_map_effect(ITEM_INFO* item); // 0x0041D770
void draw_right_gun(ITEM_INFO* item); // 0x0041D780
void draw_left_gun(ITEM_INFO* item); // 0x0041D7D0

//	----------:		shoot_right_gun
//	----------:		shoot_left_gun

void swap_meshes_with_meshswap1(ITEM_INFO* item); // 0x0041D820
void swap_meshes_with_meshswap2(ITEM_INFO* item); // 0x0041D890
void swap_meshes_with_meshswap3(ITEM_INFO* item); // 0x0041D900
void invisibility_on(ITEM_INFO* item); // 0x0041D9A0
void invisibility_off(ITEM_INFO* item); // 0x0041D9B0
void dynamic_light_on(ITEM_INFO* item); // 0x0041D9D0
void dynamic_light_off(ITEM_INFO* item); // 0x0041D9E0
void reset_hair(ITEM_INFO* item); // 0x0041D9F0
void AssaultStart(ITEM_INFO* item); // 0x0041DA00
void AssaultStop(ITEM_INFO* item); // 0x0041DA30
void AssaultReset(ITEM_INFO* item); // 0x0041DA50
void AssaultFinished(ITEM_INFO* item); // 0x0041DA70

#endif // EFFECTS_H_INCLUDED
