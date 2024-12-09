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

#include "precompiled.h"
#include "game/lara2gun.h"
#include "3dsystem/phd_math.h"
#include "game/draw.h"
#include "game/larafire.h"
#include "game/sound.h"
#include "global/vars.h"

#ifdef FEATURE_INPUT_IMPROVED
#include "modding/joy_output.h"
#endif // FEATURE_INPUT_IMPROVED

#define AIMGUNS_F	0						// 0-4
#define DRAW1_F		5                       // 5-12
#define DRAW2_F		13                      // 13-23
#define RECOIL_F	24                      // 24-32

void set_pistol_arm(LARA_ARM* arm, int frame) {
	OBJECT_INFO* obj = &Objects[ID_LARA_PISTOLS];
	short anim;
	if (frame < DRAW1_F)
		anim = obj->animIndex;
	else if (frame < DRAW2_F)
		anim = obj->animIndex + 1;
	else if (frame < RECOIL_F)
		anim = obj->animIndex + 2;
	else
		anim = obj->animIndex + 3;
	arm->anim_number = anim;
	arm->frame_number = frame;
	arm->frame_base = Anims[anim].framePtr;
}

void draw_pistols(int weaponType)
{
	short frm = Lara.left_arm.frame_number;
	frm++;

	if (frm < DRAW1_F || frm > (RECOIL_F - 1))
	{
		frm = DRAW1_F;
	}
	else if (frm == DRAW2_F)
	{
		draw_pistol_meshes(weaponType);
		PlaySoundEffect(6, &LaraItem->pos, 0);
	}
	else if (frm == (RECOIL_F - 1))
	{
		ready_pistols(weaponType);
		frm = AIMGUNS_F;
	}

	set_pistol_arm(&Lara.right_arm, frm);
	set_pistol_arm(&Lara.left_arm, frm);
}

void undraw_pistols(int weaponType)
{
	short frmL = Lara.left_arm.frame_number;
	if (frmL >= RECOIL_F)
	{
		frmL = DRAW1_F - 1;
	}
	else if (frmL > 0 && frmL < DRAW1_F)
	{
		Lara.left_arm.x_rot -= Lara.left_arm.x_rot / frmL;
		Lara.left_arm.y_rot -= Lara.left_arm.y_rot / frmL;
		frmL--;
	}
	else if (frmL == 0)
	{
		Lara.left_arm.x_rot = Lara.left_arm.y_rot = Lara.left_arm.z_rot = 0;
		frmL = RECOIL_F - 1;
	}
	else if (frmL > DRAW1_F && frmL < RECOIL_F)
	{
		frmL--;
		if (frmL == DRAW2_F - 1)
		{
			undraw_pistol_mesh_left(weaponType);
			PlaySoundEffect(7, &LaraItem->pos, 0);
		}
	}
	set_pistol_arm(&Lara.left_arm, frmL);

	short frmR = Lara.right_arm.frame_number;
	if (frmR >= RECOIL_F)
	{
		frmR = DRAW1_F - 1;
	}
	else if (frmR > 0 && frmR < DRAW1_F)
	{
		Lara.right_arm.x_rot -= Lara.right_arm.x_rot / frmR;
		Lara.right_arm.y_rot -= Lara.right_arm.y_rot / frmR;
		frmR--;
	}
	else if (frmR == 0)
	{
		Lara.right_arm.x_rot = Lara.right_arm.y_rot = Lara.right_arm.z_rot = 0;
		frmR = RECOIL_F - 1;
	}
	else if (frmR > DRAW1_F && frmR < RECOIL_F)
	{
		frmR--;
		if (frmR == DRAW2_F - 1)
		{
			undraw_pistol_mesh_right(weaponType);
			PlaySoundEffect(7, &LaraItem->pos, 0);
		}
	}
	set_pistol_arm(&Lara.right_arm, frmR);

	if (frmL == DRAW1_F && frmR == DRAW1_F)
	{
		Lara.gun_status = LGS_Armless;
		Lara.right_arm.frame_number = Lara.left_arm.frame_number = 0;
		Lara.target = NULL;
		Lara.left_arm.lock = Lara.right_arm.lock = FALSE;
	}

	if (!CHK_ANY(InputStatus, IN_LOOK))
	{
		Lara.torso_y_rot = Lara.head_y_rot = (Lara.left_arm.y_rot + Lara.right_arm.y_rot) / 4;
		Lara.torso_x_rot = Lara.head_x_rot = (Lara.left_arm.x_rot + Lara.right_arm.x_rot) / 4;
	}
}

void ready_pistols(int weaponType)
{
	Lara.gun_status = LGS_Ready;
	Lara.left_arm.x_rot = Lara.left_arm.y_rot = Lara.left_arm.z_rot = 0;
	Lara.right_arm.x_rot = Lara.right_arm.y_rot = Lara.right_arm.z_rot = 0;
	Lara.left_arm.frame_number = Lara.right_arm.frame_number = 0;
	Lara.target = NULL;
	Lara.left_arm.lock = Lara.right_arm.lock = FALSE;
	Lara.left_arm.frame_base = Lara.right_arm.frame_base = Objects[ID_LARA_PISTOLS].frameBase;
}

void draw_pistol_meshes(int weaponType)
{
	int obj = WeaponObject(weaponType);
	Lara.mesh_ptrs[LM_HandL] = MeshPtr[Objects[obj].meshIndex + LM_HandL];
	Lara.mesh_ptrs[LM_HandR] = MeshPtr[Objects[obj].meshIndex + LM_HandR];
	Lara.mesh_ptrs[LM_ThighL] = MeshPtr[Objects[ID_LARA].meshIndex + LM_ThighL];
	Lara.mesh_ptrs[LM_ThighR] = MeshPtr[Objects[ID_LARA].meshIndex + LM_ThighR];
}

void undraw_pistol_mesh_left(int weaponType)
{
	int obj = WeaponObject(weaponType);
	Lara.mesh_ptrs[LM_ThighL] = MeshPtr[Objects[obj].meshIndex + LM_ThighL];
	Lara.mesh_ptrs[LM_HandL] = MeshPtr[Objects[ID_LARA].meshIndex + LM_HandL];
}

void undraw_pistol_mesh_right(int weaponType)
{
	int obj = WeaponObject(weaponType);
	Lara.mesh_ptrs[LM_ThighR] = MeshPtr[Objects[obj].meshIndex + LM_ThighR];
	Lara.mesh_ptrs[LM_HandR] = MeshPtr[Objects[ID_LARA].meshIndex + LM_HandR];
}

void PistolHandler(int weaponType) {
	WEAPON_INFO* weapon = &Weapons[weaponType];

	if (CHK_ANY(InputStatus, IN_ACTION)) {
		LaraTargetInfo(weapon);
	}
	else {
		Lara.target = NULL;
	}

	if (Lara.target == NULL) {
		LaraGetNewTarget(weapon);
	}

	AimWeapon(weapon, &Lara.left_arm);
	AimWeapon(weapon, &Lara.right_arm);

	if (Lara.left_arm.lock && !Lara.right_arm.lock) {
		Lara.head_y_rot = Lara.torso_y_rot = Lara.left_arm.y_rot / 2;
		Lara.head_x_rot = Lara.torso_x_rot = Lara.left_arm.x_rot / 2;
	}
	else if (!Lara.left_arm.lock && Lara.right_arm.lock) {
		Lara.head_y_rot = Lara.torso_y_rot = Lara.right_arm.y_rot / 2;
		Lara.head_x_rot = Lara.torso_x_rot = Lara.right_arm.x_rot / 2;
	}
	else if (Lara.left_arm.lock && Lara.right_arm.lock) {
		Lara.head_y_rot = Lara.torso_y_rot = (Lara.right_arm.y_rot + Lara.left_arm.y_rot) / 4;
		Lara.head_x_rot = Lara.torso_x_rot = (Lara.right_arm.x_rot + Lara.left_arm.x_rot) / 4;
	}

	AnimatePistols(weaponType);

	if (Lara.left_arm.flash_gun || Lara.right_arm.flash_gun) {
		int x = LaraItem->pos.x + (phd_sin(LaraItem->pos.rotY) >> (W2V_SHIFT - 10));
		int y = LaraItem->pos.y - 0x200;
		int z = LaraItem->pos.z + (phd_cos(LaraItem->pos.rotY) >> (W2V_SHIFT - 10));
		AddDynamicLight(x, y, z, 12, 11);
#ifdef FEATURE_INPUT_IMPROVED
		if (weaponType == LGT_Uzis) {
			JoyVibrate(0x400, 0x400, 1, 0x100, 2, false);
		}
		else if (Lara.left_arm.flash_gun && Lara.right_arm.flash_gun) {
			JoyVibrate(0x600, 0x600, 1, 0x300, 2, false);
		}
		else {
			JoyVibrate(0x400, 0x400, 1, 0x100, 2, false);
		}
#endif // FEATURE_INPUT_IMPROVED
	}
}

void AnimatePistols(int gunType) {
	WEAPON_INFO* weapon;
	static BOOL IsLeftUzi = FALSE;
	static BOOL IsRightUzi = FALSE;
	BOOL right;
	short frame, angles[2];

	right = FALSE;
	frame = Lara.right_arm.frame_number;
	weapon = &Weapons[gunType];
	if (!Lara.right_arm.lock && (!CHK_ANY(InputStatus, IN_ACTION) || Lara.target)) {
		if (frame >= 24) {
			frame = 4;
		}
		else {
			if (frame > 0 && frame <= 4)
				--frame;
		}
		if (IsRightUzi) {
			PlaySoundEffect(weapon->sampleNum + 1, &LaraItem->pos, 0);
			IsRightUzi = FALSE;
		}
	}
	else {
		if (frame >= 0 && frame < 4) {
			++frame;
		}
		else {
			if (frame == 4) {
				if (CHK_ANY(InputStatus, IN_ACTION)) {
					angles[0] = LaraItem->pos.rotY + Lara.right_arm.y_rot;
					angles[1] = Lara.right_arm.x_rot;
					if (FireWeapon(gunType, Lara.target, LaraItem, angles)) {
						Lara.right_arm.flash_gun = weapon->flashTime;
						PlaySoundEffect(weapon->sampleNum, &LaraItem->pos, 0);
						right = TRUE;
						if (gunType == LGT_Uzis)
							IsRightUzi = TRUE;
					}
					frame = 24;
				}
				else {
					if (IsRightUzi) {
						PlaySoundEffect(weapon->sampleNum + 1, &LaraItem->pos, 0);
						IsRightUzi = FALSE;
					}
				}
			}
			else {
				if (frame >= 24) {
					if (gunType == LGT_Uzis) {
						PlaySoundEffect(weapon->sampleNum, &LaraItem->pos, 0);
						IsRightUzi = TRUE;
					}
					++frame;
					if (frame == weapon->recoilFrame + 24)
						frame = 4;
				}
			}
		}
	}
	set_pistol_arm(&Lara.right_arm, frame);
	frame = Lara.left_arm.frame_number;
	if (!Lara.left_arm.lock && (!CHK_ANY(InputStatus, IN_ACTION) || Lara.target)) {
		if (frame >= 24) {
			frame = 4;
		}
		else {
			if (frame > 0 && frame <= 4)
				--frame;
		}
		if (IsLeftUzi) {
			PlaySoundEffect(weapon->sampleNum + 1, &LaraItem->pos, 0);
			IsLeftUzi = FALSE;
		}
	}
	else {
		if (frame >= 0 && frame < 4) {
			++frame;
		}
		else {
			if (frame == 4) {
				if (CHK_ANY(InputStatus, IN_ACTION)) {
					angles[0] = LaraItem->pos.rotY + Lara.left_arm.y_rot;
					angles[1] = Lara.left_arm.x_rot;
					if (FireWeapon(gunType, Lara.target, LaraItem, angles)) {
						Lara.left_arm.flash_gun = weapon->flashTime;
						if (!right)
							PlaySoundEffect(weapon->sampleNum, &LaraItem->pos, 0);
						if (gunType == LGT_Uzis)
							IsLeftUzi = TRUE;
					}
					frame = 24;
				}
				else {
					if (IsLeftUzi) {
						PlaySoundEffect(weapon->sampleNum + 1, &LaraItem->pos, 0);
						IsLeftUzi = FALSE;
					}
				}
			}
			else {
				if (frame >= 24) {
					if (gunType == LGT_Uzis) {
						PlaySoundEffect(weapon->sampleNum, &LaraItem->pos, 0);
						IsLeftUzi = TRUE;
					}
					++frame;
					if (frame == weapon->recoilFrame + 24)
						frame = 4;
				}
			}
		}
	}
	set_pistol_arm(&Lara.left_arm, frame);
}

/*
 * Inject function
 */
void Inject_Lara2Gun() {
	INJECT(0x0042D000, set_pistol_arm);
	INJECT(0x0042D050, draw_pistols);
	INJECT(0x0042D0D0, undraw_pistols);
	INJECT(0x0042D300, ready_pistols);
	INJECT(0x0042D360, draw_pistol_meshes);
	INJECT(0x0042D3B0, undraw_pistol_mesh_left);
	INJECT(0x0042D3F0, undraw_pistol_mesh_right);
	INJECT(0x0042D430, PistolHandler);
	INJECT(0x0042D5C0, AnimatePistols);
}