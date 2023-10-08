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
#include "game/laraflare.h"
#include "3dsystem/3d_gen.h"
#include "3dsystem/phd_math.h"
#include "3dsystem/scalespr.h"
#include "game/control.h"
#include "game/draw.h"
#include "game/effects.h"
#include "game/invfunc.h"
#include "game/items.h"
#include "game/lara.h"
#include "game/larafire.h"
#include "game/sound.h"
#include "specific/game.h"
#include "specific/output.h"
#include "global/vars.h"

#if defined(FEATURE_VIDEOFX_IMPROVED)
extern DWORD AlphaBlendMode;
#endif // FEATURE_VIDEOFX_IMPROVED

#if defined(FEATURE_GAMEPLAY_FIXES)
static bool IsFlareLeftHandUnderwater(PHD_VECTOR* pos, short* roomNumber)
{
	*roomNumber = LaraItem->roomNumber;
	GetFloor(pos->x, pos->y, pos->z, roomNumber);
	return CHK_ANY(RoomInfo[*roomNumber].flags, ROOM_UNDERWATER);
}
#endif

int DoFlareLight(PHD_VECTOR* pos, int flareAge) {
	BOOL light = TRUE;
	if (flareAge < 1800) {
		int random = GetRandomDraw();
		int x = pos->x + (random & 15);
		if (flareAge < 30) {
			AddDynamicLight(x, pos->y, pos->z, (flareAge - 30) / 5 + 12, 11);
		}
		else {
			if (flareAge < 1740) {
				AddDynamicLight(x, pos->y, pos->z, 12, 11);
			}
			else {
				if (random > 8192) {
					AddDynamicLight(x, pos->y, pos->z, 12 - (random & 3), 11);
				}
				else {
					AddDynamicLight(x, pos->y, pos->z, 12, 5);
					light = FALSE;
				}
			}
		}
	}
	else {
		light = FALSE;
	}
	return light;
}

void DoFlareInHand(int flare_age)
{
	PHD_VECTOR pos{};
	pos.x = 11;
	pos.y = 32;
	pos.z = 41;
	GetLaraJointAbsPosition(&pos, LM_HandL);
	Lara.left_arm.flash_gun = DoFlareLight(&pos, flare_age);

	if (Lara.flare_age >= 1800)
	{
		if (Lara.gun_status == LGS_Armless)
			Lara.gun_status = LGS_Undraw;
	}
	else
	{
		Lara.flare_age++;
#if defined(FEATURE_GAMEPLAY_FIXES)
		// NOTE: This fix the bubble appearing through 2 click water room where lara is walking.
		// Also fix sound too, when going on 2 click water, it played it underwater instead of land.
		short roomNumber;
		if (IsFlareLeftHandUnderwater(&pos, &roomNumber))
		{
			PlaySoundEffect(12, &LaraItem->pos, SFX_UNDERWATER);
			if (GetRandomDraw() < 0x4000)
				CreateBubbleVec(&pos, roomNumber);
		}
#else
		if (CHK_ANY(RoomInfo[LaraItem->roomNumber].flags, ROOM_UNDERWATER))
		{
			PlaySoundEffect(12, &LaraItem->pos, SFX_UNDERWATER);
			if (GetRandomDraw() < 0x4000)
				CreateBubbleVec(&pos, LaraItem->roomNumber);
		}
#endif
		else
		{
			PlaySoundEffect(12, &LaraItem->pos, 0);
		}
	}
}

void DrawFlareInAir(ITEM_INFO* item) {
	int rate;
	short* frames[2];
	GetFrames(item, frames, &rate);
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x, item->pos.y, item->pos.z);
	phd_RotYXZ(item->pos.rotY, item->pos.rotX, item->pos.rotZ);
	int clip = S_GetObjectBounds(frames[0]);
	if (clip) {
		CalculateObjectLighting(item, frames[0]);
		phd_PutPolygons(MeshPtr[Objects[ID_FLARE_ITEM].meshIndex], clip);
		if (CHK_ANY((DWORD)item->data, 0x8000)) {
			phd_TranslateRel(-6, 6, 80);
			phd_RotX(-ANGLE(90));
			phd_RotY(GetRandomDraw() * 2);
			S_CalculateStaticLight(0x800);
			phd_PutPolygons(MeshPtr[Objects[ID_FLARE_FIRE].meshIndex], clip);
#if defined(FEATURE_VIDEOFX_IMPROVED)
			if (AlphaBlendMode) {
				int shade = (GetRandomDraw() & 0xFFF) + 0x1000;
				S_DrawSprite(GLOW_FLARE_COLOR | SPR_BLEND_ADD | SPR_TINT | SPR_SHADE | SPR_SEMITRANS, 0, 0, 0, Objects[ID_GLOW].meshIndex, shade, 0);
			}
#endif // FEATURE_VIDEOFX_IMPROVED
		}
	}
	phd_PopMatrix();
}

void CreateFlare(BOOL isFlying) {
	__int16 itemID;
	ITEM_INFO* item;
	PHD_VECTOR pos;

	itemID = CreateItem();
	if (itemID != -1) {
		item = &Items[itemID];
		item->objectID = ID_FLARE_ITEM;
		item->roomNumber = LaraItem->roomNumber;
		pos.x = -16;
		pos.y = 32;
		pos.z = 42;
		GetLaraJointAbsPosition(&pos, 13);
		if (GetHeight(GetFloor(pos.x, pos.y, pos.z, &item->roomNumber), pos.x, pos.y, pos.z) < pos.y) {
			item->pos.x = LaraItem->pos.x;
			item->pos.y = pos.y;
			item->pos.z = LaraItem->pos.z;
			item->pos.rotY = -LaraItem->pos.rotY;
			item->roomNumber = LaraItem->roomNumber;
		}
		else {
			item->pos.x = pos.x;
			item->pos.y = pos.y;
			item->pos.z = pos.z;
			item->pos.rotY = isFlying ? LaraItem->pos.rotY : LaraItem->pos.rotY - PHD_45;
		}
		InitialiseItem(itemID);
		item->pos.rotZ = 0;
		item->pos.rotX = 0;
		item->shade1 = -1;
		if (isFlying) {
			item->speed = LaraItem->speed + 50;
			item->fallSpeed = LaraItem->fallSpeed - 50;
		}
		else {
			item->speed = LaraItem->speed + 10;
			item->fallSpeed = LaraItem->fallSpeed + 50;
		}
		if (DoFlareLight((PHD_VECTOR*)&item->pos, Lara.flare_age)) {
			item->data = (LPVOID)(Lara.flare_age | 0x8000);
		}
		else {
			item->data = (LPVOID)(Lara.flare_age & 0x7FFF);
		}
		AddActiveItem(itemID);
		item->status = ITEM_ACTIVE;
	}
}

void set_flare_arm(int frame) {
	if (frame < 1)
		Lara.left_arm.anim_number = Objects[ID_LARA_FLARE].animIndex;
	else if (frame < 33)
		Lara.left_arm.anim_number = Objects[ID_LARA_FLARE].animIndex + 1;
	else if (frame < 72)
		Lara.left_arm.anim_number = Objects[ID_LARA_FLARE].animIndex + 2;
	else if (frame < 95)
		Lara.left_arm.anim_number = Objects[ID_LARA_FLARE].animIndex + 3;
	else
		Lara.left_arm.anim_number = Objects[ID_LARA_FLARE].animIndex + 4;
	Lara.left_arm.frame_base = Anims[Lara.left_arm.anim_number].framePtr;
}

void draw_flare() {
	short frame;

	if (LaraItem->currentAnimState != AS_FLAREPICKUP && LaraItem->currentAnimState != AS_PICKUP) {
		Lara.flare_control_left = 1;
		frame = Lara.left_arm.frame_number + 1;
		if (frame >= 33 && frame <= 94) {
			if (frame == 46) {
				draw_flare_meshes();
				if (!SaveGame.bonusFlag)
					Inv_RemoveItem(ID_FLARES_ITEM);
			}
			else {
				if (frame >= 72 && frame <= 93) {
					if (frame == 72) {
						PlaySoundEffect(11, &LaraItem->pos, CHK_ANY(RoomInfo[LaraItem->roomNumber].flags, ROOM_UNDERWATER) ? SFX_UNDERWATER : 0);
						Lara.flare_age = 0;
					}
					DoFlareInHand(Lara.flare_age);
				}
				else {
					if (frame == 94) {
						ready_flare();
						DoFlareInHand(Lara.flare_age);
						frame = 0;
					}
				}
			}
		}
		else {
			frame = 33;
		}
	}
	else {
		DoFlareInHand(Lara.flare_age);
		Lara.flare_control_left = 0;
		frame = 93;
	}
	Lara.left_arm.frame_number = frame;
	set_flare_arm(Lara.left_arm.frame_number);
}

void undraw_flare() {
	short frame;

	Lara.flare_control_left = 1;
	frame = Lara.left_arm.frame_number;
	if (LaraItem->goalAnimState == AS_STOP && Lara.skidoo == -1) {
		if (LaraItem->animNumber == 103) {
			LaraItem->animNumber = 189;
			Lara.flare_frame = frame + Anims[LaraItem->animNumber].frameBase;
			LaraItem->frameNumber = Lara.flare_frame;
		}
		if (LaraItem->animNumber == 189) {
			Lara.flare_control_left = 0;
			if (Lara.flare_frame >= Anims[189].frameBase + 31) {
				Lara.request_gun_type = Lara.last_gun_type;
				Lara.gun_type = Lara.last_gun_type;
				Lara.gun_status = LGS_Armless;
				InitialiseNewWeapon();
				Lara.target = NULL;
				Lara.right_arm.lock = 0;
				Lara.left_arm.lock = 0;
				LaraItem->animNumber = 11;
				Lara.flare_frame = Anims[LaraItem->animNumber].frameBase;
				LaraItem->frameNumber = Lara.flare_frame;
				LaraItem->currentAnimState = AS_STOP;
				LaraItem->goalAnimState = AS_STOP;
				return;
			}
			++Lara.flare_frame;
		}
	}
	else {
		if (LaraItem->currentAnimState == AS_STOP && Lara.skidoo == -1) {
			LaraItem->animNumber = 11;
			LaraItem->frameNumber = Anims[LaraItem->animNumber].frameBase;
		}
	}
	if (frame) {
		if (frame >= 72 && frame < 95) {
			++frame;
			if (frame == 94)
				frame = 1;
		}
		else {
			if (frame >= 1 && frame < 33) {
				++frame;
				if (frame == 21) {
					CreateFlare(TRUE);
					undraw_flare_meshes();
				}
				else {
					if (frame == 33) {
						frame = 0;
						Lara.request_gun_type = Lara.last_gun_type;
						Lara.gun_type = Lara.last_gun_type;
						Lara.gun_status = LGS_Armless;
						InitialiseNewWeapon();
						Lara.flare_control_left = 0;
						Lara.target = NULL;
						Lara.right_arm.lock = 0;
						Lara.left_arm.lock = 0;
						Lara.flare_frame = 0;
					}
				}
			}
			else {
				if (frame >= 95 && frame < 110) {
					++frame;
					if (frame == 110)
						frame = 1;
				}
			}
		}
	}
	else {
		frame = 1;
	}
	if (frame >= 1 && frame < 21)
		DoFlareInHand(Lara.flare_age);
	Lara.left_arm.frame_number = frame;
	set_flare_arm(Lara.left_arm.frame_number);
}

void draw_flare_meshes() {
	Lara.mesh_ptrs[13] = MeshPtr[Objects[ID_LARA_FLARE].meshIndex + 13];
}

void undraw_flare_meshes() {
	Lara.mesh_ptrs[13] = MeshPtr[Objects[ID_LARA].meshIndex + 13];
}

void ready_flare() {
	Lara.gun_status = LGS_Armless;
	Lara.left_arm.z_rot = 0;
	Lara.left_arm.y_rot = 0;
	Lara.left_arm.x_rot = 0;
	Lara.right_arm.z_rot = 0;
	Lara.right_arm.y_rot = 0;
	Lara.right_arm.x_rot = 0;
	Lara.right_arm.lock = 0;
	Lara.left_arm.lock = 0;
	Lara.target = NULL;
}

void FlareControl(short itemID) {
	FLOOR_INFO* floor;
	ITEM_INFO* item;
	int x, y, z, height, ceiling, age;
	short roomID;

	item = &Items[itemID];
	if (item->fallSpeed) {
		item->pos.rotX += 3 * PHD_DEGREE;
		item->pos.rotZ += 5 * PHD_DEGREE;
	}
	else {
		item->pos.rotX = 0;
		item->pos.rotZ = 0;
	}
	x = item->pos.x;
	z = item->pos.z;
	y = item->pos.y;
	item->pos.z += phd_cos(item->pos.rotY) * item->speed >> W2V_SHIFT;
	item->pos.x += phd_sin(item->pos.rotY) * item->speed >> W2V_SHIFT;
	if (CHK_ANY(RoomInfo[item->roomNumber].flags, ROOM_UNDERWATER)) {
		item->fallSpeed += (5 - item->fallSpeed) / 2;
		item->speed += (5 - item->speed) / 2;
	}
	else {
		item->fallSpeed += 6;
	}
	item->pos.y += item->fallSpeed;
	roomID = item->roomNumber;
	floor = GetFloor(item->pos.x, item->pos.y, item->pos.z, &roomID);
	height = GetHeight(floor, item->pos.x, item->pos.y, item->pos.z);
	if (item->pos.y >= height) {
		if (y > height) {
			item->pos.rotY += PHD_180;
			item->pos.y = y;
			item->speed /= 2;
			item->pos.x = x;
			item->pos.z = z;
			roomID = item->roomNumber;
		}
		else {
			if (item->fallSpeed > 40) {
				item->fallSpeed = 40 - item->fallSpeed;
				CLAMPL(item->fallSpeed, -100);
			}
			else {
				item->speed -= 3;
				item->fallSpeed = 0;
				CLAMPL(item->speed, 0);
			}
			item->pos.y = height;
		}
	}
	else {
		ceiling = GetCeiling(floor, item->pos.x, item->pos.y, item->pos.z);
		if (item->pos.y < ceiling) {
			item->fallSpeed = -item->fallSpeed;
			item->pos.y = ceiling;
		}
	}
	if (roomID != item->roomNumber)
		ItemNewRoom(itemID, roomID);
	age = (int)item->data & 0x7FFF;
	if (age < 1800 || item->fallSpeed || item->speed) {
		if (age < 1800)
			++age;
		if (DoFlareLight((PHD_VECTOR*)&item->pos, age)) {
			age |= 0x8000;
			if (CHK_ANY(RoomInfo[item->roomNumber].flags, ROOM_UNDERWATER)) {
				PlaySoundEffect(12, &item->pos, SFX_UNDERWATER);
				if (GetRandomDraw() < 16384)
					CreateBubble(&item->pos, item->roomNumber);
			}
			else {
				PlaySoundEffect(12, &item->pos, 0);
			}
		}
		item->data = (LPVOID)age;
	}
	else {
		KillItem(itemID);
	}
}

/*
 * Inject function
 */
void Inject_LaraFlare() {
	INJECT(0x0042F840, DoFlareLight);
	INJECT(0x0042F8E0, DoFlareInHand);
	INJECT(0x0042F9C0, DrawFlareInAir);
	INJECT(0x0042FAC0, CreateFlare);
	INJECT(0x0042FCA0, set_flare_arm);
	INJECT(0x0042FCF0, draw_flare);
	INJECT(0x0042FE60, undraw_flare);
	INJECT(0x00430090, draw_flare_meshes);
	INJECT(0x004300B0, undraw_flare_meshes);
	INJECT(0x004300D0, ready_flare);
	INJECT(0x00430110, FlareControl);
}