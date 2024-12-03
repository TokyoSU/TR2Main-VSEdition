/*
 * Copyright (c) 2017-2018 Michael Chaban. All rights reserved.
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

#include "precompiled.h"
#include "game/hair.h"
#include "3dsystem/3d_gen.h"
#include "3dsystem/math_tbls.h"
#include "3dsystem/phd_math.h"
#include "game/control.h"
#include "game/draw.h"
#include "specific/game.h"
#include "global/vars.h"

int SmokeWindX;
int SmokeWindZ;

void InitializeHair() {
	PHD_3DPOS* hairpos = &HairPos[0];
	PHD_VECTOR* hairvel = NULL;
	int* bone = &AnimBones[Objects[ID_LARA_HAIR].boneIndex];
	SkipHairPhysics = TRUE;
	hairpos->rotY = 0;
	hairpos->rotX = -PHD_90;
	for (int i = 1; i < 7; ++i) {
		hairpos = &HairPos[i];
		hairpos->x = bone[4 * i - 3];
		hairpos->y = bone[4 * i - 2];
		hairpos->z = bone[4 * i - 1];
		hairpos->rotX = -PHD_90;
		hairpos->rotY = 0;
		hairpos->rotZ = 0;
		hairvel = &HairVelocity[i];
		hairvel->x = 0;
		hairvel->y = 0;
		hairvel->z = 0;
	}
}

void HairControl(BOOL isCutscene) {
	SPHERE_INFO spheres[5] = {};
	UINT16* rotation;
	int* bone, x, y, z, i, j, water, height, random, dx, dy, dz, sum, distance;
	short* frame, *mesh, roomID;
	static long wind = 0;
	static long wind_angle = 2048;
	static long dwind_angle = 2048;

	if (Lara.hit_direction >= 0) {
		switch (Lara.hit_direction) {
		case 1:
			frame = &Anims[127].framePtr[Lara.hit_frame * (Anims[127].interpolation >> 8)];
			break;
		case 2:
			frame = &Anims[126].framePtr[Lara.hit_frame * (Anims[126].interpolation >> 8)];
			break;
		case 3:
			frame = &Anims[128].framePtr[Lara.hit_frame * (Anims[128].interpolation >> 8)];
			break;
		default:
			frame = &Anims[125].framePtr[Lara.hit_frame * (Anims[125].interpolation >> 8)];
			break;
		}
	}
	else {
		frame = GetBestFrame(LaraItem);
	}

	phd_PushUnitMatrix();
	PhdMatrixPtr->_03 = LaraItem->pos.x << W2V_SHIFT;
	PhdMatrixPtr->_13 = LaraItem->pos.y << W2V_SHIFT;
	PhdMatrixPtr->_23 = LaraItem->pos.z << W2V_SHIFT;
	phd_RotYXZ(LaraItem->pos.rotY, LaraItem->pos.rotX, LaraItem->pos.rotZ);
	rotation = (UINT16*)&frame[9];
	bone = &AnimBones[Objects[ID_LARA].boneIndex];
	phd_TranslateRel(frame[6], frame[7], frame[8]);
	phd_RotYXZsuperpack(&rotation, 0);
	phd_PushMatrix();
	mesh = Lara.mesh_ptrs[0];
	phd_TranslateRel(mesh[0], mesh[1], mesh[2]);
	spheres[0].x = PhdMatrixPtr->_03 >> W2V_SHIFT;
	spheres[0].y = PhdMatrixPtr->_13 >> W2V_SHIFT;
	spheres[0].z = PhdMatrixPtr->_23 >> W2V_SHIFT;
	phd_PopMatrix();
	spheres[0].radius = mesh[3];
	phd_TranslateRel(bone[25], bone[26], bone[27]);
	if (Lara.weapon_item != -1 &&
		Lara.gun_type == LGT_M16 &&
		(!Items[Lara.weapon_item].currentAnimState ||
			Items[Lara.weapon_item].currentAnimState == 2 ||
			Items[Lara.weapon_item].currentAnimState == 4))
	{
		rotation = (UINT16*)&Lara.right_arm.frame_base[Lara.right_arm.frame_number * (Anims[Lara.right_arm.anim_number].interpolation >> 8) + 9];
		phd_RotYXZsuperpack(&rotation, 7);
	}
	else {
		phd_RotYXZsuperpack(&rotation, 6);
	}
	phd_RotYXZ(Lara.torso_y_rot, Lara.torso_x_rot, Lara.torso_z_rot);
	phd_PushMatrix();
	mesh = Lara.mesh_ptrs[7];
	phd_TranslateRel(mesh[0], mesh[1], mesh[2]);
	spheres[1].x = PhdMatrixPtr->_03 >> W2V_SHIFT;
	spheres[1].y = PhdMatrixPtr->_13 >> W2V_SHIFT;
	spheres[1].z = PhdMatrixPtr->_23 >> W2V_SHIFT;
	spheres[1].radius = mesh[3];
	phd_PopMatrix();
	phd_PushMatrix();
	phd_TranslateRel(bone[29], bone[30], bone[31]);
	phd_RotYXZsuperpack(&rotation, 0);
	mesh = Lara.mesh_ptrs[8];
	phd_TranslateRel(mesh[0], mesh[1], mesh[2]);
	spheres[3].y = PhdMatrixPtr->_13 >> W2V_SHIFT;
	spheres[3].x = PhdMatrixPtr->_03 >> W2V_SHIFT;
	spheres[3].z = PhdMatrixPtr->_23 >> W2V_SHIFT;
	phd_PopMatrix();
	spheres[3].radius = 3 * mesh[3] / 2;
	phd_PushMatrix();
	phd_TranslateRel(bone[41], bone[42], bone[43]);
	phd_RotYXZsuperpack(&rotation, 2);
	phd_TranslateRel(mesh[0], mesh[1], mesh[2]);
	spheres[4].y = PhdMatrixPtr->_13 >> W2V_SHIFT;
	spheres[4].x = PhdMatrixPtr->_03 >> W2V_SHIFT;
	spheres[4].z = PhdMatrixPtr->_23 >> W2V_SHIFT;
	phd_PopMatrix();
	spheres[4].radius = 3 * mesh[3] / 2;
	phd_TranslateRel(bone[53], bone[54], bone[55]);
	phd_RotYXZsuperpack(&rotation, 2);
	phd_RotYXZ(Lara.head_y_rot, Lara.head_x_rot, Lara.head_z_rot);
	phd_PushMatrix();
	mesh = Lara.mesh_ptrs[14];
	phd_TranslateRel(mesh[0], mesh[1], mesh[2]);
	spheres[2].x = PhdMatrixPtr->_03 >> W2V_SHIFT;
	spheres[2].y = PhdMatrixPtr->_13 >> W2V_SHIFT;
	spheres[2].z = PhdMatrixPtr->_23 >> W2V_SHIFT;
	spheres[2].radius = mesh[3];
	phd_PopMatrix();
	phd_TranslateRel(0, -23, -55);
	x = PhdMatrixPtr->_03 >> W2V_SHIFT;
	y = PhdMatrixPtr->_13 >> W2V_SHIFT;
	z = PhdMatrixPtr->_23 >> W2V_SHIFT;
	phd_PopMatrix();
	bone = &AnimBones[Objects[ID_LARA_HAIR].boneIndex];
	if (SkipHairPhysics) {
		HairPos[0].z = z;
		SkipHairPhysics = FALSE;
		HairPos[0].x = x;
		HairPos[0].y = y;
		for (i = 1; i < 7; ++i) {
			phd_PushUnitMatrix();
			PhdMatrixPtr->_03 = HairPos[i - 1].x << W2V_SHIFT;
			PhdMatrixPtr->_13 = HairPos[i - 1].y << W2V_SHIFT;
			PhdMatrixPtr->_23 = HairPos[i - 1].z << W2V_SHIFT;
			phd_RotYXZ(HairPos[i - 1].rotY, HairPos[i - 1].rotX, 0);
			phd_TranslateRel(bone[4 * i - 3], bone[4 * i - 2], bone[4 * i - 1]);
			HairPos[i].x = PhdMatrixPtr->_03 >> W2V_SHIFT;
			HairPos[i].y = PhdMatrixPtr->_13 >> W2V_SHIFT;
			HairPos[i].z = PhdMatrixPtr->_23 >> W2V_SHIFT;
			phd_PopMatrix();
		}
		wind = 0;
	}
	else {
		HairPos[0].x = x;
		HairPos[0].y = y;
		HairPos[0].z = z;
		roomID = LaraItem->roomNumber;
		if (isCutscene) {
			water = NO_HEIGHT;
		}
		else {
			water = GetWaterHeight(LaraItem->pos.x + (frame[0] + frame[1]) / 2,
				LaraItem->pos.y + (frame[2] + frame[3]) / 2,
				LaraItem->pos.z + (frame[4] + frame[5]) / 2,
				roomID);
		}
		height = GetHeight(GetFloor(x, y, z, &roomID), x, y, z);
		if (height < y)
			height = LaraItem->floor;
		if (CHK_ANY(Rooms[roomID].flags, ROOM_OUTSIDE)) {
			wind += (GetRandomControl() & 7) - 3;
			if (wind <= -2)
				wind++;
			else if (wind >= 9)
				wind--;

			dwind_angle = (dwind_angle + (((GetRandomControl() & 0x3F) - 32) << 1)) & 0x1FFE;
			if (dwind_angle < 1024)
				dwind_angle += (1024 - dwind_angle) << 1;
			else if (dwind_angle > 3072)
				dwind_angle -= (dwind_angle - 3072) << 1;

			wind_angle = (wind_angle + ((dwind_angle - wind_angle) >> 3)) & 0x1FFE;
			SmokeWindX = (wind * rcossin_tbl[wind_angle]) >> 12;
			SmokeWindZ = (wind * rcossin_tbl[wind_angle + 1]) >> 12;
		}
		else {
			wind = 0;
		}
		for (i = 1; i < 7; ++i) {
			HairVelocity[0].z = HairPos[i].z;
			HairVelocity[0].x = HairPos[i].x;
			HairVelocity[0].y = HairPos[i].y;
			HairPos[i].x += 3 * HairVelocity[i].x / 4;
			HairPos[i].y += 3 * HairVelocity[i].y / 4;
			HairPos[i].z += 3 * HairVelocity[i].z / 4;
			switch (Lara.water_status) {
			case LWS_AboveWater:
				HairPos[i].y += 10;
				if (water != NO_HEIGHT && HairPos[i].y > water) {
					HairPos[i].y = water;
				}
				else {
					if (HairPos[i].y <= height) {
						if (CHK_ANY(Rooms[roomID].flags, ROOM_OUTSIDE)) {
							HairPos[i].x += SmokeWindX;
							HairPos[i].z += SmokeWindZ;
						}
					}
					else {
						HairPos[i].y = height;
					}
				}
				break;
			case LWS_Underwater:
			case LWS_Surface:
			case LWS_Wade:
				if (HairPos[i].y < water) {
					HairPos[i].y = water;
				}
				else {
					if (HairPos[i].y > height)
						HairPos[i].y = height;
				}
				break;
			}
			for (j = 0; j < 5; ++j) {
				dx = HairPos[i].x - spheres[j].x;
				dy = HairPos[i].y - spheres[j].y;
				dz = HairPos[i].z - spheres[j].z;
				sum = SQR(dx) + SQR(dy) + SQR(dz);
				if (sum < SQR(spheres[j].radius)) {
					distance = phd_sqrt(sum);
					if (!distance)
						distance = 1;
					HairPos[i].x = spheres[j].x + dx * spheres[j].radius / distance;
					HairPos[i].y = spheres[j].y + dy * spheres[j].radius / distance;
					HairPos[i].z = spheres[j].z + dz * spheres[j].radius / distance;
				}
			}
			HairPos[i - 1].rotY = phd_atan(HairPos[i].z - HairPos[i - 1].z, HairPos[i].x - HairPos[i - 1].x);
			HairPos[i - 1].rotX = -phd_atan(phd_sqrt(SQR(HairPos[i].z - HairPos[i - 1].z) + SQR(HairPos[i].x - HairPos[i - 1].x)), HairPos[i].y - HairPos[i - 1].y);
			phd_PushUnitMatrix();
			PhdMatrixPtr->_03 = HairPos[i - 1].x << W2V_SHIFT;
			PhdMatrixPtr->_13 = HairPos[i - 1].y << W2V_SHIFT;
			PhdMatrixPtr->_23 = HairPos[i - 1].z << W2V_SHIFT;
			phd_RotYXZ(HairPos[i - 1].rotY, HairPos[i - 1].rotX, 0);
			if (i == 6) {
				phd_TranslateRel(bone[17], bone[18], bone[19]);
			}
			else {
				phd_TranslateRel(bone[4 * i - 3], bone[4 * i - 2], bone[4 * i - 1]);
			}
			HairPos[i].x = PhdMatrixPtr->_03 >> W2V_SHIFT;
			HairPos[i].y = PhdMatrixPtr->_13 >> W2V_SHIFT;
			HairPos[i].z = PhdMatrixPtr->_23 >> W2V_SHIFT;
			phd_PopMatrix();
			HairVelocity[i].x = HairPos[i].x - HairVelocity[0].x;
			HairVelocity[i].y = HairPos[i].y - HairVelocity[0].y;
			HairVelocity[i].z = HairPos[i].z - HairVelocity[0].z;
		}
	}
}

void DrawHair() {
	short** mesh = &MeshPtr[Objects[ID_LARA_HAIR].meshIndex];
	for (int i = 0; i < 6; ++i) {
		phd_PushMatrix();
		phd_TranslateAbs(HairPos[i].x, HairPos[i].y, HairPos[i].z);
		phd_RotY(HairPos[i].rotY);
		phd_RotX(HairPos[i].rotX);
		phd_PutPolygons(mesh[i], 1);
		phd_PopMatrix();
	}
}

 /*
  * Inject function
  */
void Inject_Hair() {
	INJECT(0x00420E80, InitializeHair);
	INJECT(0x00420F00, HairControl);
	INJECT(0x00421900, DrawHair);
}