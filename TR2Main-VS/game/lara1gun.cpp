/*
 * Copyright (c) 2017-2020 Michael Chaban. All rights reserved.
 * Copyright (c) 2020 ChocolateFan <asasas9500@gmail.com>
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
#include "game/lara1gun.h"
#include "3dsystem/phd_math.h"
#include "game/box.h"
#include "game/control.h"
#include "game/draw.h"
#include "game/items.h"
#include "game/effects.h"
#include "game/lara.h"
#include "game/larafire.h"
#include "game/objects.h"
#include "game/sound.h"
#include "specific/game.h"
#include "global/vars.h"

#ifdef FEATURE_INPUT_IMPROVED
#include "modding/joy_output.h"
#endif // FEATURE_INPUT_IMPROVED

#ifdef FEATURE_GAMEPLAY_FIXES
bool IsRunningM16fix = true;
#endif // FEATURE_GAMEPLAY_FIXES

enum WEAPON_STATE {
	WS_AIM,
	WS_DRAW,
	WS_RECOIL,
	WS_UNDRAW,
	WS_UNAIM,
	WS_RELOAD,
	WS_UAIM,
	WS_UUNAIM,
	WS_URECOIL,
	WS_SURF_UNDRAW
};

void draw_shotgun_meshes(int weaponType)
{
	Lara.back_gun = 0;
	Lara.mesh_ptrs[10] = MeshPtr[Objects[WeaponObject(weaponType)].meshIndex + 10];
}

void undraw_shotgun_meshes(int weaponType)
{
	Lara.back_gun = WeaponObject(weaponType);
	Lara.mesh_ptrs[10] = MeshPtr[Objects[ID_LARA].meshIndex + 10];
}

void ready_shotgun(int weaponType)
{
	Lara.gun_status = LGS_Ready;
	Lara.left_arm.z_rot = 0;
	Lara.left_arm.y_rot = 0;
	Lara.left_arm.x_rot = 0;
	Lara.right_arm.z_rot = 0;
	Lara.right_arm.y_rot = 0;
	Lara.right_arm.x_rot = 0;
	Lara.right_arm.frame_number = 0;
	Lara.left_arm.frame_number = 0;
	Lara.right_arm.lock = 0;
	Lara.left_arm.lock = 0;
	Lara.target = 0;
	Lara.right_arm.frame_base = Objects[WeaponObject(weaponType)].frameBase;
	Lara.left_arm.frame_base = Lara.right_arm.frame_base;
}

void RifleHandler(int weaponType) {
	WEAPON_INFO* weapon = &Weapons[weaponType];

	if (CHK_ANY(InputStatus, IN_ACTION)) {
		LaraTargetInfo(&Weapons[weaponType]);
	}
	else {
		Lara.target = NULL;
	}

	if (!Lara.target) {
		LaraGetNewTarget(weapon);
	}

	AimWeapon(weapon, &Lara.left_arm);

	if (Lara.left_arm.lock) {
		Lara.torso_x_rot = Lara.left_arm.x_rot;
		Lara.torso_y_rot = Lara.left_arm.y_rot;
		Lara.head_x_rot = Lara.head_y_rot = 0;
	}

	AnimateShotgun(weaponType);

	if (Lara.right_arm.flash_gun && (weaponType == LGT_Shotgun || weaponType == LGT_M16)) {
		int x = LaraItem->pos.x + (phd_sin(LaraItem->pos.rotY) >> (W2V_SHIFT - 10));
		int y = LaraItem->pos.y - 0x200;
		int z = LaraItem->pos.z + (phd_cos(LaraItem->pos.rotY) >> (W2V_SHIFT - 10));
		AddDynamicLight(x, y, z, 12, 11);
	}
}

void FireShotgun() {
	WEAPON_INFO* weapon = &Weapons[LGT_Shotgun];
	short base[2]{}, angles[2]{};

	base[0] = Lara.left_arm.y_rot + LaraItem->pos.rotY;
	base[1] = Lara.left_arm.x_rot;
	for (int i = 0; i < 6; ++i) {
		angles[0] = base[0] + ANGLE(20) * (GetRandomControl() - PHD_ONE / 4) / PHD_ONE;
		angles[1] = base[1] + ANGLE(20) * (GetRandomControl() - PHD_ONE / 4) / PHD_ONE;
		if (FireWeapon(LGT_Shotgun, Lara.target, LaraItem, angles)) {
			Lara.right_arm.flash_gun = weapon->flashTime;
			PlaySoundEffect(weapon->sampleNum, &LaraItem->pos, 0);
#ifdef FEATURE_INPUT_IMPROVED
			JoyVibrate(0x2000, 0x2000, 2, 0x800, 5, false);
#endif // FEATURE_INPUT_IMPROVED
		}
	}
}

void FireM16(BOOL isRunning) {
	WEAPON_INFO* weapon = &Weapons[LGT_M16];
	short angles[2]{};

	angles[0] = Lara.left_arm.y_rot + LaraItem->pos.rotY;
	angles[1] = Lara.left_arm.x_rot;

	// NOTE: There was a bug in the original game - ID_LARA_M16 instead of LGT_M16
#ifdef FEATURE_GAMEPLAY_FIXES
	if (IsRunningM16fix && isRunning) {
		weapon->shotAccuracy = ANGLE(12);
		weapon->damage = 1;
	}
	else {
		weapon->shotAccuracy = ANGLE(4);
		weapon->damage = 3;
	}
#endif // FEATURE_GAMEPLAY_FIXES

	if (FireWeapon(LGT_M16, Lara.target, LaraItem, angles)) {
		Lara.right_arm.flash_gun = weapon->flashTime;
#ifdef FEATURE_INPUT_IMPROVED
		JoyVibrate(0x400, 0x400, 2, 0x80, 4, false);
#endif // FEATURE_INPUT_IMPROVED
	}
}

void FireHarpoon() {
	GAME_VECTOR pos{};
	if (Lara.harpoon_ammo <= 0) return;
	short itemNumber = CreateItem();
	if (itemNumber < 0) return;

	ITEM_INFO* item = &Items[itemNumber];
	item->objectID = ID_HARPOON_BOLT;
	item->roomNumber = LaraItem->roomNumber;
	pos.x = -2;
	pos.y = 373;
	pos.z = 77;
	GetLaraJointAbsPosition((PHD_VECTOR*)&pos, 10);
	item->pos.x = pos.x;
	item->pos.y = pos.y;
	item->pos.z = pos.z;
	InitialiseItem(itemNumber);
	if (Lara.target) {
		find_target_point(Lara.target, &pos);
		item->pos.rotY = phd_atan(pos.z - item->pos.z, pos.x - item->pos.x);
		int distance = phd_sqrt(SQR(pos.x - item->pos.x) + SQR(pos.z - item->pos.z));
		item->pos.rotX = -phd_atan(distance, pos.y - item->pos.y);
	}
	else {
		item->pos.rotX = Lara.left_arm.x_rot + LaraItem->pos.rotX;
		item->pos.rotY = Lara.left_arm.y_rot + LaraItem->pos.rotY;
	}
	item->pos.rotZ = 0;
	item->fallSpeed = -150 * phd_sin(item->pos.rotX) >> W2V_SHIFT;
	item->speed = 150 * phd_cos(item->pos.rotX) >> W2V_SHIFT;
	AddActiveItem(itemNumber);
	if (!SaveGame.bonusFlag) {
		--Lara.harpoon_ammo;
	}
	++SaveGame.statistics.shots;
#ifdef FEATURE_INPUT_IMPROVED
	JoyVibrate(0xC00, 0xC00, 2, 0x400, 4, false);
#endif // FEATURE_INPUT_IMPROVED
}

void ControlHarpoonBolt(short itemNumber)
{
	FLOOR_INFO* floor = NULL;
	ITEM_INFO* item = NULL, *target = NULL;
	PHD_VECTOR pos{}, pos2{}, oldPos{};
	int sin = 0, cos = 0, rX = 0, rY = 0, sX = 0, rZ = 0, sZ = 0;
	short* bounds = NULL;
	short roomNumber = 0, targetID = 0;

	item = &Items[itemNumber];
	oldPos.x = item->pos.x;
	oldPos.y = item->pos.y;
	oldPos.z = item->pos.z;
	
	if (!CHK_ANY(Rooms[item->roomNumber].flags, ROOM_UNDERWATER))
		item->fallSpeed += 3;

	item->pos.x += item->speed * phd_sin(item->pos.rotY) >> W2V_SHIFT;
	item->pos.y += item->fallSpeed;
	item->pos.z += item->speed * phd_cos(item->pos.rotY) >> W2V_SHIFT;

	roomNumber = item->roomNumber;
	floor = GetFloor(item->pos.x, item->pos.y, item->pos.z, &roomNumber);
	item->floor = GetHeight(floor, item->pos.x, item->pos.y, item->pos.z);
	if (item->roomNumber != roomNumber)
		ItemNewRoom(itemNumber, roomNumber);
	if (CHK_ANY(Rooms[item->roomNumber].flags, ROOM_UNDERWATER))
		CreateBubble(&item->pos, item->roomNumber);

	// Hit wall, ceiling or floor !
	if (item->pos.y >= item->floor || item->pos.y <= GetCeiling(floor, item->pos.x, item->pos.y, item->pos.z))
	{
		KillItem(itemNumber);
		return;
	}

	for (targetID = Rooms[item->roomNumber].itemNumber; targetID != -1; targetID = target->nextItem)
	{
		target = &Items[targetID];

		if (target == LaraItem)
			continue;

		if ((target->objectID == ID_WINDOW1 || target->objectID == ID_WINDOW3) || (item->status != ITEM_INVISIBLE && Objects[target->objectID].collision != NULL)) {
			bounds = GetBestFrame(target);
			if (item->pos.y < (target->pos.y + bounds[2]) || item->pos.y > (target->pos.y + bounds[3]))
				continue;

			sin = phd_sin(target->pos.rotY);
			cos = phd_cos(target->pos.rotY);
			pos.x = item->pos.x - target->pos.x;
			pos.z = item->pos.z - target->pos.z;
			pos2.x = oldPos.x - target->pos.x;
			pos2.z = oldPos.z - target->pos.z;

			rX = ((cos * pos.x) - (sin * pos.z)) >> W2V_SHIFT;
			sX = ((cos * pos2.x) - (sin * pos2.z)) >> W2V_SHIFT;
			if ((rX < bounds[0] && sX < bounds[0]) || (rX > bounds[1] && sX > bounds[1]))
				continue;

			rZ = ((cos * pos.z) + (sin * pos.x)) >> W2V_SHIFT;
			sZ = ((cos * pos2.z) + (sin * pos2.x)) >> W2V_SHIFT;
			if ((rZ < bounds[4] && sZ < bounds[4]) || (rZ > bounds[5] && sZ > bounds[5]))
				continue;

			if (target->objectID == ID_WINDOW1 || target->objectID == ID_WINDOW3) {
				SmashWindow(targetID);
			}
			else {
				if (Objects[target->objectID].intelligent) {
					DoLotsOfBlood(item->pos.x, item->pos.y, item->pos.z, 0, 0, item->roomNumber, CHK_ANY(Rooms[item->roomNumber].flags, ROOM_UNDERWATER) ? 5 : 1);
					HitTarget(target, 0, Weapons[LGT_Harpoon].damage);
					++SaveGame.statistics.hits;
				}
				KillItem(itemNumber);
				return;
			}
		}
	}
}

void FireRocket() {
	short itemNumber;
	ITEM_INFO* item;
	PHD_VECTOR pos{};

	if (Lara.grenade_ammo > 0) {
		itemNumber = CreateItem();
		if (itemNumber != -1) {
			item = &Items[itemNumber];
			item->objectID = ID_ROCKET;
			item->roomNumber = LaraItem->roomNumber;
			pos.x = -2;
			pos.y = 373;
			pos.z = 77;
			GetLaraJointAbsPosition(&pos, 10);
			item->pos.x = pos.x;
			item->pos.y = pos.y;
			item->pos.z = pos.z;
			InitialiseItem(itemNumber);
			item->pos.rotX = LaraItem->pos.rotX + Lara.left_arm.x_rot;
			item->pos.rotZ = 0;
			item->speed = 200;
			item->fallSpeed = 0;
			item->pos.rotY = LaraItem->pos.rotY + Lara.left_arm.y_rot;
			AddActiveItem(itemNumber);
			if (!SaveGame.bonusFlag)
				--Lara.grenade_ammo;
			++SaveGame.statistics.shots;
#ifdef FEATURE_INPUT_IMPROVED
			JoyVibrate(0x1000, 0x1000, 2, 0x400, 4, false);
#endif // FEATURE_INPUT_IMPROVED
		}
	}
}

void ControlRocket(short itemNumber) {
	ITEM_INFO* item, * link;
	int oldX, oldY, oldZ, displacement, c, s, r, oldR;
	short room, linkID, * frame, fxID;
	FLOOR_INFO* floor;
	BOOL collision;
	FX_INFO* fx;

	item = &Items[itemNumber];
	oldX = item->pos.x;
	oldY = item->pos.y;
	oldZ = item->pos.z;
	if (item->speed < 190)
		++item->fallSpeed;
	--item->speed;
	item->pos.y += item->fallSpeed - (item->speed * phd_sin(item->pos.rotX) >> W2V_SHIFT);
	item->pos.z += phd_cos(item->pos.rotY) * (item->speed * phd_cos(item->pos.rotX) >> W2V_SHIFT) >> W2V_SHIFT;
	item->pos.x += phd_sin(item->pos.rotY) * (item->speed * phd_cos(item->pos.rotX) >> W2V_SHIFT) >> W2V_SHIFT;
	room = item->roomNumber;
	floor = GetFloor(item->pos.x, item->pos.y, item->pos.z, &room);
	item->floor = GetHeight(floor, item->pos.x, item->pos.y, item->pos.z);
	if (item->roomNumber != room)
		ItemNewRoom(itemNumber, room);
	if (item->pos.y < item->floor && item->pos.y > GetCeiling(floor, item->pos.x, item->pos.y, item->pos.z)) {
		collision = FALSE;
		displacement = 0;
	}
	else {
		displacement = 512;
		collision = TRUE;
	}
	for (linkID = Rooms[item->roomNumber].itemNumber; linkID != -1; linkID = link->nextItem) {
		link = &Items[linkID];
		if (link != LaraItem && link->collidable && ((link->objectID == ID_WINDOW1 || link->objectID == ID_WINDOW3) || (Objects[link->objectID].intelligent && link->status != ITEM_INVISIBLE && Objects[link->objectID].collision)))
		{
			frame = GetBestFrame(link);
			if (item->pos.y + displacement >= link->pos.y + frame[2] && item->pos.y - displacement <= link->pos.y + frame[3]) {
				c = phd_cos(link->pos.rotY);
				s = phd_sin(link->pos.rotY);
				r = (c * (item->pos.x - link->pos.x) - s * (item->pos.z - link->pos.z)) >> W2V_SHIFT;
				oldR = (c * (oldX - link->pos.x) - s * (oldZ - link->pos.z)) >> W2V_SHIFT;
				if ((r + displacement >= frame[0] || oldR + displacement >= frame[0]) &&
					(r - displacement <= frame[1] || oldR - displacement <= frame[1]))
				{
					r = (s * (item->pos.x - link->pos.x) + c * (item->pos.z - link->pos.z)) >> W2V_SHIFT;
					oldR = (s * (oldX - link->pos.x) + c * (oldZ - link->pos.z)) >> W2V_SHIFT;
					if ((r + displacement >= frame[4] || oldR + displacement >= frame[4]) &&
						(r - displacement <= frame[5] || oldR - displacement <= frame[5]))
					{
						if (link->objectID == ID_WINDOW1 || link->objectID == ID_WINDOW3) {
							SmashWindow(linkID);
						}
						else {
							if (link->status == ITEM_ACTIVE) {
								HitTarget(link, NULL, 30);
								++SaveGame.statistics.hits;
								if (link->hitPoints <= 0) {
									++SaveGame.statistics.kills;
									if (link->objectID != ID_DRAGON_FRONT && link->objectID != ID_GIANT_YETI)
										CreatureDie(linkID, TRUE);
								}
							}
							collision = TRUE;
						}
					}
				}
			}
		}
	}
	if (collision) {
		fxID = CreateEffect(item->roomNumber);
		if (fxID != -1) {
			fx = &Effects[fxID];
			fx->pos.x = oldX;
			fx->pos.y = oldY;
			fx->pos.z = oldZ;
			fx->speed = 0;
			fx->frameNumber = 0;
			fx->counter = 0;
			fx->objectID = ID_EXPLOSION;
		}
		PlaySoundEffect(105, NULL, 0);
		KillItem(itemNumber);
#ifdef FEATURE_INPUT_IMPROVED
		JoyRumbleExplode(oldX, oldY, oldZ, 0x1400, true);
#endif // FEATURE_INPUT_IMPROVED
	}
}

void draw_shotgun(int weaponType)
{
	ITEM_INFO* item = NULL;
	if (Lara.weapon_item == -1)
	{
		Lara.weapon_item = CreateItem();
		if (Lara.weapon_item == -1)
		{
			LogDebug("Failed to create weapon_item for the back weapon (torso)");
			return;
		}
		item = &Items[Lara.weapon_item];
		item->objectID = WeaponObject(weaponType);
		short animIdx = Objects[item->objectID].animIndex;
		item->animNumber = weaponType == LGT_Grenade ? animIdx : animIdx + 1;
		item->frameNumber = Anims[item->animNumber].frameBase;
		item->goalAnimState = 1;
		item->currentAnimState = 1;
		item->status = ITEM_ACTIVE;
		item->roomNumber = NO_ROOM;
		Lara.right_arm.frame_base = Objects[item->objectID].frameBase;
		Lara.left_arm.frame_base = Lara.right_arm.frame_base;
	}
	else
	{
		item = &Items[Lara.weapon_item];
	}
	AnimateItem(item);
	if (item->currentAnimState != 0 && item->currentAnimState != 6)
	{
		if ((item->frameNumber - Anims[item->animNumber].frameBase) == 10)
			draw_shotgun_meshes(weaponType);
		else
			item->goalAnimState = 6;
	}
	else
	{
		ready_shotgun(weaponType);
	}
	Lara.right_arm.frame_base = Anims[item->animNumber].framePtr;
	Lara.left_arm.frame_base = Lara.right_arm.frame_base;
	Lara.right_arm.frame_number = item->frameNumber - Anims[item->animNumber].frameBase;
	Lara.left_arm.frame_number = Lara.right_arm.frame_number;
	Lara.right_arm.anim_number = item->animNumber;
	Lara.left_arm.anim_number = Lara.right_arm.anim_number;
}

void undraw_shotgun(int weaponType)
{
	if (Lara.weapon_item == -1)
	{
		LogDebug("Failed to undraw back weapon, it not exist !");
		return;
	}
	ITEM_INFO* item = &Items[Lara.weapon_item];
	if (Lara.water_status == LWS_Surface)
		item->goalAnimState = 9;
	else
		item->goalAnimState = 3;
	AnimateItem(item);
	if (item->status == ITEM_DISABLED)
	{
		Lara.gun_status = LGS_Armless;
		Lara.target = NULL;
		Lara.right_arm.lock = FALSE;
		Lara.left_arm.lock = FALSE;
		KillItem(Lara.weapon_item);
		Lara.weapon_item = -1;
		Lara.right_arm.frame_number = 0;
		Lara.left_arm.frame_number = 0;
	}
	else if (item->currentAnimState == 3 && (item->frameNumber - Anims[item->animNumber].frameBase) == 21)
	{
		undraw_shotgun_meshes(weaponType);
	}
	Lara.right_arm.frame_base = Anims[item->animNumber].framePtr;
	Lara.left_arm.frame_base = Lara.right_arm.frame_base;
	Lara.right_arm.frame_number = item->frameNumber - Anims[item->animNumber].frameBase;
	Lara.left_arm.frame_number = Lara.right_arm.frame_number;
	Lara.right_arm.anim_number = item->animNumber;
	Lara.left_arm.anim_number = Lara.right_arm.anim_number;
}

void AnimateShotgun(int weaponType)
{
	static BOOL m16_firing = FALSE, harpoon_fired = FALSE;
	ITEM_INFO* item = NULL;
	BOOL running = FALSE;

	item = &Items[Lara.weapon_item];
	running = (weaponType == LGT_M16 && LaraItem->speed != 0);

	switch (item->currentAnimState)
	{
	case WS_AIM: // Aim
		m16_firing = FALSE;

		if (harpoon_fired)
		{
			item->goalAnimState = WS_RELOAD;
			harpoon_fired = FALSE;
		}
		else if (Lara.water_status == LWS_Underwater || running)
			item->goalAnimState = WS_UAIM;
		else if ((CHK_ANY(InputStatus, IN_ACTION) && Lara.target == NULL) || Lara.left_arm.lock)
			item->goalAnimState = WS_RECOIL;
		else
			item->goalAnimState = WS_UNAIM;

		break;
	case WS_RECOIL:
		if (item->frameNumber == Anims[item->animNumber].frameBase)
		{
			item->goalAnimState = WS_UNAIM;
			if (Lara.water_status != LWS_Underwater && !running && !harpoon_fired)
			{
				if (CHK_ANY(InputStatus, IN_ACTION) && (Lara.target == NULL || Lara.left_arm.lock))
				{
					switch (weaponType)
					{
					case LGT_Harpoon:
						FireHarpoon();
						if (!(Lara.harpoon_ammo & 3))
							harpoon_fired = TRUE;
						break;
					case LGT_M16:
						FireM16(FALSE);
						PlaySoundEffect(78, &LaraItem->pos, 0);
						m16_firing = TRUE;
						break;
					case LGT_Grenade:
						FireRocket();
						break;
					case LGT_Shotgun:
						FireShotgun();
						break;
					}
					item->goalAnimState = WS_RECOIL;
				}
				else if (Lara.left_arm.lock)
				{
					item->goalAnimState = WS_AIM;
				}
			}
			if (item->goalAnimState != WS_RECOIL && m16_firing)
			{
				PlaySoundEffect(104, &LaraItem->pos, 0);
				m16_firing = FALSE;
			}
		}
		else if (m16_firing)
			PlaySoundEffect(78, &LaraItem->pos, 0);
		else if (weaponType == LGT_Shotgun && !CHK_ANY(InputStatus, IN_ACTION) && !Lara.left_arm.lock)
			item->goalAnimState = WS_UNAIM;

		break;
	case WS_UAIM:
		m16_firing = FALSE;

		if (harpoon_fired)
		{
			item->goalAnimState = WS_RELOAD;
			harpoon_fired = FALSE;
		}
		else if (Lara.water_status != LWS_Underwater && !running)
			item->goalAnimState = WS_AIM;
		else if ((CHK_ANY(InputStatus, IN_ACTION) && Lara.target == NULL) || Lara.left_arm.lock)
			item->goalAnimState = WS_URECOIL;
		else
			item->goalAnimState = WS_UUNAIM;
		break;
	case WS_URECOIL:
		if ((item->frameNumber - Anims[item->animNumber].frameBase) == 0)
		{
			item->goalAnimState = WS_UUNAIM;
			if ((Lara.water_status == LWS_Underwater || running) && !harpoon_fired)
			{
				if (CHK_ANY(InputStatus, IN_ACTION) && (Lara.target == NULL || Lara.left_arm.lock))
				{
					if (weaponType == LGT_Harpoon)
					{
						FireHarpoon();
						if (!(Lara.harpoon_ammo & 3))
							harpoon_fired = TRUE;
					}
					else
					{
						FireM16(TRUE);
					}
					item->goalAnimState = WS_URECOIL;
				}
				else if (Lara.left_arm.lock)
				{
					item->goalAnimState = WS_UAIM;
				}
			}
		}
		if (weaponType == LGT_M16 && item->goalAnimState == WS_URECOIL)
			PlaySoundEffect(78, &LaraItem->pos, 0);
		break;

	}

	AnimateItem(item);
	Lara.right_arm.frame_base = Anims[item->animNumber].framePtr;
	Lara.left_arm.frame_base = Lara.right_arm.frame_base;
	Lara.right_arm.frame_number = item->frameNumber - Anims[item->animNumber].frameBase;
	Lara.left_arm.frame_number = Lara.right_arm.frame_number;
	Lara.right_arm.anim_number = item->animNumber;
	Lara.left_arm.anim_number = Lara.right_arm.anim_number;
}

/*
 * Inject function
 */
void Inject_Lara1Gun() {
	INJECT(0x0042BC90, draw_shotgun_meshes);
	INJECT(0x0042BCD0, undraw_shotgun_meshes);
	INJECT(0x0042BD00, ready_shotgun);
	INJECT(0x0042BD70, RifleHandler);
	INJECT(0x0042BE70, FireShotgun);
	INJECT(0x0042BF70, FireM16);
	INJECT(0x0042BFF0, FireHarpoon);
	INJECT(0x0042C180, ControlHarpoonBolt);
	INJECT(0x0042C4D0, FireRocket);
	INJECT(0x0042C5C0, ControlRocket);
	INJECT(0x0042C9D0, draw_shotgun);
	INJECT(0x0042CB40, undraw_shotgun);
	INJECT(0x0042CC50, AnimateShotgun);
}