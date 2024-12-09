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
short DoBloodSplat(int x, int y, int z, short speed, short direction, short roomID); // 0x0041C5B0
void DoLotsOfBlood(int x, int y, int z, short speed, short direction, short roomID, int number); // 0x0041C610
void ControlBlood1(short fxID); // 0x0041C6C0
void ControlExplosion1(short fxID); // 0x0041C750
void Richochet(GAME_VECTOR* pos); // 0x0041C7D0
void ControlRichochet1(short fxID); // 0x0041C850
void CreateBubbleVec(PHD_VECTOR* pos, short roomNumber); // New argument, avoiding useless cast. TODO: remove 'Vec' when passing to EXE.
void CreateBubble(PHD_3DPOS* pos, short roomNumber); // 0x0041C880
void CreateExplosion(PHD_3DPOS* pos, short roomNumber, int yOffset = 0); // NOTE: New function to create explosion easily...
void LaraBubbles(ITEM_INFO* item); // 0x0041C8F0
void ControlBubble1(short fxID); // 0x0041C970
void Splash(ITEM_INFO* item); // 0x0041CA70
// TODO: Change to 'Splash' when going to EXE !
// NOTE: Not exist in the original code, new argument.
void Splash2(ITEM_INFO* item, int waterHeight);
void CreateRainSpash(DWORD color, int x, int y, int z, short scale, short roomNumber);
void CreateSplash(int x, int y, int z, short roomNumber);
void WadeSplash(ITEM_INFO* item, int height);
void ControlSplash1(short fxID); // 0x0041CB40
void ControlWaterSprite(short fxID); // 0x0041CBC0
void ControlSnowSprite(short fxID); // 0x0041CC70
void ControlHotLiquid(short fxID); // 0x0041CD00
void WaterFall(short itemNumber); // 0x0041CDE0
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
void ControlDingDong(short itemNumber); // 0x0041D1F0
void ControlLaraAlarm(short itemNumber); // 0x0041D230
void ControlAlarmSound(short itemNumber); // 0x0041D270
void ControlBirdTweeter(short itemNumber); // 0x0041D2E0
void DoChimeSound(ITEM_INFO* item); // 0x0041D340
void ControlClockChimes(short itemNumber); // 0x0041D3A0
void SphereOfDoomCollision(short itemNumber, ITEM_INFO* laraItem, COLL_INFO* coll); // 0x0041D410
void SphereOfDoom(short itemNumber); // 0x0041D540
void DrawSphereOfDoom(ITEM_INFO* item); // 0x0041D630
void lara_hands_free(ITEM_INFO* item); // 0x0041D760
void flip_map_effect(ITEM_INFO* item); // 0x0041D770
void draw_right_gun(ITEM_INFO* item); // 0x0041D780
void draw_left_gun(ITEM_INFO* item); // 0x0041D7D0

//----------:		shoot_right_gun
//----------:		shoot_left_gun

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
