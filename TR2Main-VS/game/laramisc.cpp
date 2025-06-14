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
#include "game/laramisc.h"
#include "3dsystem/phd_math.h"
#include "game/collide.h"
#include "game/control.h"
#include "game/effects.h"
#include "game/lara.h"
#include "game/larasurf.h"
#include "game/laraswim.h"
#include "game/invfunc.h"
#include "game/items.h"
#include "game/larafire.h"
#include "game/sound.h"
#include "specific/sndpc.h"
#include "specific/init_sound_xaudio.h"
#include "global/vars.h"

#if defined(FEATURE_MOD_CONFIG)
#include "modding/mod_utils.h"
#endif

void LaraControl() {
	COLL_INFO coll;
	ITEM_INFO* item = LaraItem;

#ifdef FEATURE_CHEAT
	if (CHK_ANY(GF_GameFlow.flags, GFF_EnableCheatCode | GFF_DozyCheatEnabled)) {
		// Recover health + get inventory stuff
		if (CHK_ANY(InputStatus, IN_STUFFCHEAT)) {
			LaraCheatGetStuff();
			item->hitPoints = 1000;
		}
		// Enable dozy cheat (flying with full health). It is allowed for boat, but not for skidoo
		if (!Lara.extra_anim && (Lara.skidoo < 0 || Items[Lara.skidoo].objectID == ID_BOAT)
			&& Lara.water_status != LWS_Cheat && CHK_ANY(InputStatus, IN_DOZYCHEAT))
		{
			if (Lara.water_status != LWS_Underwater || item->hitPoints <= 0) {
				item->pos.y -= 128;
				item->animNumber = 87;
				item->frameNumber = Anims[item->animNumber].frameBase;
				item->currentAnimState = AS_SWIM;
				item->goalAnimState = AS_SWIM;
				item->gravity = 0;
				item->pos.rotX = ANGLE(0);
				item->fallSpeed = 30;
				Lara.torso_x_rot = Lara.torso_y_rot = 0;
				Lara.head_x_rot = Lara.head_y_rot = 0;
				if (item->hitPoints <= 0 && Lara.gun_type != LGT_Flare) {
					BYTE backupGunType = SaveGame.start[CurrentLevel].gunType;
					SaveGame.start[CurrentLevel].gunType = (BYTE)Lara.request_gun_type;
					LaraInitialiseMeshes(CurrentLevel);
					SaveGame.start[CurrentLevel].gunType = backupGunType;
					Lara.gun_status = LGS_Armless;
					Lara.target = NULL;
					memset(&Lara.left_arm, 0, sizeof(Lara.left_arm));
					memset(&Lara.right_arm, 0, sizeof(Lara.right_arm));
					if (Lara.weapon_item >= 0) {
						KillItem(Lara.weapon_item);
						Lara.weapon_item = -1;
					}
				}
			}
			Lara.water_status = LWS_Cheat;
			Lara.skidoo = -1; // get off from vehicle
			Lara.spaz_effect_count = 0;
			Lara.spaz_effect = NULL;
			Lara.hit_frame = 0;
			Lara.hit_direction = -1;
#if defined(FEATURE_MOD_CONFIG)
			Lara.air = Mod.underwater.maxAir;
#else
			Lara.air = LARA_AIR_MAX;
#endif
			Lara.death_count = 0;
			Lara.mesh_effects = 0x7FFF; // Lara has golden skin
		}
	}
#endif // FEATURE_CHEAT

	BOOL isRoomUnderwater = CHK_ANY(Rooms[item->roomNumber].flags, ROOM_UNDERWATER);
	int depth = GetWaterDepth(item->pos.x, item->pos.y, item->pos.z, item->roomNumber);
	int height = GetWaterHeight(item->pos.x, item->pos.y, item->pos.z, item->roomNumber);
	int water_surface_dist = (height == NO_HEIGHT) ? NO_HEIGHT : item->pos.y - height;

	Lara.water_surface_dist = -water_surface_dist;
	WadeSplash(item, height);

	if (Lara.skidoo == -1 && !Lara.extra_anim) {
		switch (Lara.water_status) {
		case LWS_AboveWater:
			if (water_surface_dist != NO_HEIGHT && water_surface_dist >= 384) {
				if (depth <= (730 - 256)) {
					if (water_surface_dist > 384) {
						Lara.water_status = LWS_Wade;
						if (!item->gravity) {
							item->goalAnimState = AS_STOP;
						}
					}
				}
				else if (isRoomUnderwater) {
#if defined(FEATURE_MOD_CONFIG)
					Lara.air = Mod.underwater.maxAir;
#else
					Lara.air = LARA_AIR_MAX;
#endif
					Lara.water_status = LWS_Underwater;
					item->gravity = 0;
					item->pos.y += 100;
					UpdateLaraRoom(item, 0);
					StopSoundEffect(30);
					switch (item->currentAnimState) {
					case AS_SWANDIVE:
						item->pos.rotX = -ANGLE(45);
						item->goalAnimState = AS_DIVE;
						AnimateLara(item);
						item->fallSpeed *= 2;
						break;
					case AS_FASTDIVE:
						item->pos.rotX = -ANGLE(85);
						item->goalAnimState = AS_DIVE;
						AnimateLara(item);
						item->fallSpeed *= 2;
						break;
					default:
						item->pos.rotX = -ANGLE(45);
						item->animNumber = 112;
						item->frameNumber = Anims[item->animNumber].frameBase;
						item->currentAnimState = AS_DIVE;
						item->goalAnimState = AS_SWIM;
						item->fallSpeed = item->fallSpeed * 3 / 2;
						break;
					}
					Lara.torso_x_rot = Lara.torso_y_rot = 0;
					Lara.head_x_rot = Lara.head_y_rot = 0;
					Splash(item);
				}
			}
			break;
		case LWS_Wade:
			Camera.targetElevation = -ANGLE(22);
			if (water_surface_dist >= 384) {
				if (water_surface_dist > 730) {
					Lara.water_status = LWS_Surface;
					item->pos.y += 1 - water_surface_dist;
					switch (item->currentAnimState) {
					case AS_BACK:
						item->currentAnimState = AS_SURFBACK;
						item->animNumber = 140;
						item->frameNumber = Anims[item->animNumber].frameBase;
						break;
					case AS_STEPRIGHT:
						item->currentAnimState = AS_SURFRIGHT;
						item->animNumber = 144;
						item->frameNumber = Anims[item->animNumber].frameBase;
						break;
					case AS_STEPLEFT:
						item->currentAnimState = AS_SURFLEFT;
						item->animNumber = 143;
						item->frameNumber = Anims[item->animNumber].frameBase;
						break;
					default:
						item->currentAnimState = AS_SURFSWIM;
						item->animNumber = 116;
						item->frameNumber = Anims[item->animNumber].frameBase;
						break;
					}
					item->gravity = 0;
					item->goalAnimState = item->currentAnimState;
					item->fallSpeed = 0;
					Lara.dive_count = 0;
					item->pos.rotX = item->pos.rotZ = 0;
					Lara.torso_x_rot = Lara.torso_y_rot = 0;
					Lara.head_x_rot = Lara.head_y_rot = 0;
					UpdateLaraRoom(item, -381);
				}
			}
			else {
				Lara.water_status = LWS_AboveWater;
				if (item->currentAnimState == AS_WADE)
					item->goalAnimState = AS_RUN;
			}
			break;
		case LWS_Surface:
			if (!isRoomUnderwater) {
				if (water_surface_dist <= 384) {
					Lara.water_status = LWS_AboveWater;
					item->animNumber = 34;
					item->frameNumber = Anims[item->animNumber].frameBase;
					item->goalAnimState = AS_FORWARDJUMP;
					item->currentAnimState = AS_FORWARDJUMP;
					item->gravity = 1;
					item->speed = item->fallSpeed / 4;
				}
				else {
					Lara.water_status = LWS_Wade;
					item->animNumber = 103;
					item->currentAnimState = AS_STOP;
					item->frameNumber = Anims[item->animNumber].frameBase;
					item->goalAnimState = AS_WADE;
					AnimateItem(item);
				}
				item->fallSpeed = 0;
				item->pos.rotX = item->pos.rotZ = 0;
				Lara.torso_x_rot = Lara.torso_y_rot = 0;
				Lara.head_x_rot = Lara.head_y_rot = 0;
			}
			break;
		case LWS_Underwater:
			if (!isRoomUnderwater) {
				if (depth == NO_HEIGHT || ABS(water_surface_dist) >= 256) {
					Lara.water_status = LWS_AboveWater;
					item->animNumber = 34;
					item->frameNumber = Anims[item->animNumber].frameBase;
					item->goalAnimState = AS_FORWARDJUMP;
					item->currentAnimState = AS_FORWARDJUMP;
					item->speed = item->fallSpeed / 4;
					item->gravity = 1;
					item->fallSpeed = 0;
					item->pos.rotX = item->pos.rotZ = 0;
					Lara.torso_x_rot = Lara.torso_y_rot = 0;
					Lara.head_x_rot = Lara.head_y_rot = 0;
				}
				else {
					Lara.water_status = LWS_Surface;
					item->pos.y += 1 - water_surface_dist;
					item->animNumber = 114;
					item->frameNumber = Anims[item->animNumber].frameBase;
					item->goalAnimState = AS_SURFTREAD;
					item->currentAnimState = AS_SURFTREAD;
					item->fallSpeed = 0;
					Lara.dive_count = 11;
					item->pos.rotX = item->pos.rotZ = 0;
					Lara.torso_x_rot = Lara.torso_y_rot = 0;
					Lara.head_x_rot = Lara.head_y_rot = 0;
					UpdateLaraRoom(item, -0x17D);
					PlaySoundEffect(36, &item->pos, SFX_ALWAYS);
				}
			}
			break;
		default:
			break;
		}
	}

	auto& curCamRoom = Rooms[Camera.pos.roomNumber];
	if (curCamRoom.reverbType != REVERB_NONE)
		S_SetReverbType((AUDIO_REVERB_TYPE)curCamRoom.reverbType);
	else
		S_SetReverbType(REVERB_NONE);

	if (item->hitPoints > 0) {
		if (GF_NoFloor && item->pos.y >= GF_NoFloor) {
			item->hitPoints = -1;
			Lara.death_count = 9 * FRAMES_PER_SECOND; // let's skip 9 seconds to death
		}
	}
	else {
		item->hitPoints = -1;
		if (!Lara.death_count) {
			S_CDStop();
		}
		++Lara.death_count;
		if (CHK_ANY(item->flags, IFL_ONESHOT)) { // Nothing left from Lara?
			++Lara.death_count; // In this case death timer ticks twice faster
			return;
		}
	}

	switch (Lara.water_status) {
	case LWS_AboveWater:
	case LWS_Wade:
#if defined(FEATURE_MOD_CONFIG)
		Lara.air = Mod.underwater.maxAir;
#else
		Lara.air = LARA_AIR_MAX;
#endif
		LaraAboveWater(item, &coll);
		break;
	case LWS_Underwater:
#if defined(FEATURE_MOD_CONFIG)
		if (!Mod.underwater.unlimitedAir)
			Lara.air--;
#else
		Lara.air--;
#endif
		if (item->hitPoints >= 0 && Lara.air < 0) {
			Lara.air = NO_AIR;
#if defined(FEATURE_MOD_CONFIG)
			item->hitPoints -= Mod.underwater.noAirDamagePerTick;
#else
			item->hitPoints -= LARA_NO_AIR_DAMAGE_PER_TICK;
#endif
		}
		LaraUnderWater(item, &coll);
		break;
	case LWS_Surface:
		if (item->hitPoints >= 0) {
#if defined(FEATURE_MOD_CONFIG)
			Lara.air += Mod.underwater.restoreAirPerTick;
			CLAMPG(Lara.air, Mod.underwater.maxAir)
#else
			Lara.air += LARA_SURFACE_AIR_REGEN_COUNT;
			CLAMPG(Lara.air, LARA_AIR_MAX)
#endif
		}
		LaraSurface(item, &coll);
		break;
#ifdef FEATURE_CHEAT
	case LWS_Cheat:
		if (CHK_ANY(GF_GameFlow.flags, GFF_EnableCheatCode | GFF_DozyCheatEnabled)) {
			// Update Dozy state just in case
			item->hitPoints = 1000;
			Lara.death_count = 0; // NOTE: if died already, forget about death
			LaraUnderWater(item, &coll);
			// Return Lara to normal state if Walk is pressed without Look
			if (!Lara.extra_anim && CHK_ANY(InputStatus, IN_SLOW) && !CHK_ANY(InputStatus, IN_LOOK)) {
				if (isRoomUnderwater || (water_surface_dist != NO_HEIGHT && water_surface_dist > 0)) {
					Lara.water_status = LWS_Underwater;
				}
				else {
					Lara.water_status = LWS_AboveWater;
					item->animNumber = 11;
					item->frameNumber = Anims[item->animNumber].frameBase;
					item->pos.rotX = item->pos.rotZ = 0;
					Lara.torso_x_rot = Lara.torso_y_rot = 0;
					Lara.head_x_rot = Lara.head_y_rot = 0;
				}
				Lara.mesh_effects = 0;
			}
		}
		break;
#endif // FEATURE_CHEAT
	default:
		break;
	}

	SaveGame.statistics.distance += phd_sqrt(SQR(item->pos.z - Lara.last_pos.z) + SQR(item->pos.y - Lara.last_pos.y) + SQR(item->pos.x - Lara.last_pos.x));
	Lara.last_pos.x = item->pos.x;
	Lara.last_pos.y = item->pos.y;
	Lara.last_pos.z = item->pos.z;
}

static bool CheckIfAnimCommandIsValid(USHORT type, bool checkWaterWithNoHeight = true)
{
	switch (type)
	{
	case SFX_LANDANDWATER:
		return true;
	case SFX_LANDONLY:
		return Lara.water_surface_dist >= 0 || Lara.water_surface_dist == NO_HEIGHT;
	case SFX_WATERONLY:
		if (checkWaterWithNoHeight)
			return Lara.water_surface_dist < 0 && Lara.water_surface_dist != NO_HEIGHT;
		else
			return Lara.water_surface_dist < 0;
		break;
	}
	return false;
}

void AnimateLara(ITEM_INFO* item)
{
	ANIM_STRUCT* anim = NULL;
	USHORT type = 0, num = 0;
	short* command = NULL;

	item->frameNumber++;
	anim = &Anims[item->animNumber];
	if (anim->numberChanges > 0 && GetChange(item, anim))
	{
		anim = &Anims[item->animNumber];
		item->currentAnimState = anim->currentAnimState;
	}

	if (item->frameNumber > anim->frameEnd)
	{
		if (anim->numberCommands > 0)
		{
			command = &AnimCommands[anim->commandIndex];
			for (int i = anim->numberCommands; i > 0; i--)
			{
				switch (*command++)
				{
				case 1:
					TranslateItem(item, command[0], command[1], command[2]);
					command += 3;
					break;
				case 2:
					item->fallSpeed = *command++;
					item->speed = *command++;
					item->gravity = TRUE;
					if (Lara.calc_fallspeed)
					{
						item->fallSpeed = Lara.calc_fallspeed;
						Lara.calc_fallspeed = 0;
					}
					break;
				case 3:
					if (Lara.gun_status != LGS_Special)
						Lara.gun_status = LGS_Armless;
					break;
				case 5:
				case 6:
					command += 2;
					break;
				}
			}
		}
		item->animNumber = anim->jumpAnimNum;
		item->frameNumber = anim->jumpFrameNum;
		anim = &Anims[item->animNumber];
		item->currentAnimState = anim->currentAnimState;
	}

	if (anim->numberCommands > 0)
	{
		command = &AnimCommands[anim->commandIndex];
		for (int i = anim->numberCommands; i > 0; i--)
		{
			switch (*command++)
			{
			case 1:
				command += 3;
				break;
			case 2:
				command += 2;
				break;
			case 5:
				if (item->frameNumber == command[0])
				{
					type = command[1] & 0xC000;
					if (CheckIfAnimCommandIsValid(type, true))
					{
						num = command[1] & 0x3FFF;
						if (type == SFX_WATERONLY &&
							Lara.water_status != LWS_Surface &&
							Lara.water_status != LWS_Wade &&
							Lara.water_status != LWS_Underwater)
							CreateSplash(item->pos.x, item->pos.y, item->pos.z, item->roomNumber);
						PlaySoundEffect(num, &item->pos, SFX_ALWAYS);
					}
				}
				command += 2;
				break;
			case 6:
				if (item->frameNumber == command[0])
				{
					type = command[1] & 0xC000;
					if (CheckIfAnimCommandIsValid(type, false))
					{
						num = command[1] & 0x3FFF;
						EffectFunctions[num](item);
					}
				}
				command += 2;
				break;
			}
		}
	}

	if (item->gravity)
	{
		int velocity = anim->velocity + anim->acceleration * (item->frameNumber - anim->frameBase - 1);
		item->speed -= velocity >> 16;
		velocity += anim->acceleration;
		item->speed += velocity >> 16;
		item->fallSpeed += (item->fallSpeed >= 128) ? 1 : 6;
		item->pos.y += item->fallSpeed;
	}
	else
	{
		int velocity = anim->velocity;
		if (anim->acceleration)
			velocity += anim->acceleration * (item->frameNumber - anim->frameBase);
		item->speed = velocity >> 16;
	}

	item->pos.x += item->speed * phd_sin(Lara.move_angle) >> W2V_SHIFT;
	item->pos.z += item->speed * phd_cos(Lara.move_angle) >> W2V_SHIFT;
}

void UseItem(short itemNumber) {
	if (itemNumber <= ID_NONE || itemNumber >= ID_NUMBER_OBJECTS)
		return;

	switch (itemNumber) {
	case ID_PISTOL_ITEM:
	case ID_PISTOL_OPTION:
		Lara.request_gun_type = LGT_Pistols;
		break;

	case ID_MAGNUM_ITEM:
	case ID_MAGNUM_OPTION:
		Lara.request_gun_type = LGT_Magnums;
		break;

	case ID_UZI_ITEM:
	case ID_UZI_OPTION:
		Lara.request_gun_type = LGT_Uzis;
		break;

	case ID_SHOTGUN_ITEM:
	case ID_SHOTGUN_OPTION:
		Lara.request_gun_type = LGT_Shotgun;
		break;

	case ID_HARPOON_ITEM:
	case ID_HARPOON_OPTION:
		Lara.request_gun_type = LGT_Harpoon;
		break;

	case ID_M16_ITEM:
	case ID_M16_OPTION:
		Lara.request_gun_type = LGT_M16;
		break;

	case ID_GRENADE_ITEM:
	case ID_GRENADE_OPTION:
		Lara.request_gun_type = LGT_Grenade;
		break;

	case ID_FLARES_ITEM:
	case ID_FLARES_OPTION:
		Lara.request_gun_type = LGT_Flare;
		break;

	case ID_SMALL_MEDIPACK_ITEM:
	case ID_SMALL_MEDIPACK_OPTION:
		if (LaraItem->hitPoints > 0 && LaraItem->hitPoints < 1000) {
			LaraItem->hitPoints += 500;
			if (LaraItem->hitPoints > 1000)
				LaraItem->hitPoints = 1000;
			Inv_RemoveItem(ID_SMALL_MEDIPACK_ITEM);
			PlaySoundEffect(116, NULL, 2);
			++SaveGame.statistics.mediPacks;
		}
		break;

	case ID_LARGE_MEDIPACK_ITEM:
	case ID_LARGE_MEDIPACK_OPTION:
		if (LaraItem->hitPoints > 0 && LaraItem->hitPoints < 1000) {
			LaraItem->hitPoints += 1000;
			if (LaraItem->hitPoints > 1000)
				LaraItem->hitPoints = 1000;
			Inv_RemoveItem(ID_LARGE_MEDIPACK_ITEM);
			PlaySoundEffect(116, NULL, 2);
			SaveGame.statistics.mediPacks += 2;
		}
		break;
	}
}

void LaraCheatGetStuff() {
	// NOTE: there is no availability checks in the original code
	if (Objects[ID_PISTOL_OPTION].loaded) {
		if (!Inv_RequestItem(ID_PISTOL_ITEM)) {
			Inv_AddItem(ID_PISTOL_ITEM);
		}
	}
	if (Objects[ID_MAGNUM_OPTION].loaded) {
		if (!Inv_RequestItem(ID_MAGNUM_ITEM)) {
			Inv_AddItem(ID_MAGNUM_ITEM);
		}
		Lara.magnum_ammo = WEAPON_UNLIMITED;
	}
	if (Objects[ID_UZI_OPTION].loaded) {
		if (!Inv_RequestItem(ID_UZI_ITEM)) {
			Inv_AddItem(ID_UZI_ITEM);
		}
		Lara.uzi_ammo = WEAPON_UNLIMITED;
	}
	if (Objects[ID_SHOTGUN_OPTION].loaded) {
		if (!Inv_RequestItem(ID_SHOTGUN_ITEM)) {
			Inv_AddItem(ID_SHOTGUN_ITEM);
		}
		Lara.shotgun_ammo = WEAPON_UNLIMITED;
	}
	if (Objects[ID_HARPOON_OPTION].loaded) {
		if (!Inv_RequestItem(ID_HARPOON_ITEM)) {
			Inv_AddItem(ID_HARPOON_ITEM);
		}
		Lara.harpoon_ammo = WEAPON_UNLIMITED;
	}
	if (Objects[ID_M16_OPTION].loaded) {
		if (!Inv_RequestItem(ID_M16_ITEM)) {
			Inv_AddItem(ID_M16_ITEM);
		}
		Lara.m16_ammo = WEAPON_UNLIMITED;
	}
	if (Objects[ID_GRENADE_OPTION].loaded) {
		if (!Inv_RequestItem(ID_GRENADE_ITEM)) {
			Inv_AddItem(ID_GRENADE_ITEM);
		}
		Lara.grenade_ammo = WEAPON_UNLIMITED;
	}

	for (int i = 0; i < 10; ++i) {
		// NOTE: there are no limits in the original code, but it works wrong without limits
		if (Objects[ID_FLARES_OPTION].loaded && Inv_RequestItem(ID_FLARE_ITEM) < 240) {
			Inv_AddItem(ID_FLARES_ITEM);
		}
		if (Objects[ID_SMALL_MEDIPACK_OPTION].loaded && Inv_RequestItem(ID_SMALL_MEDIPACK_ITEM) < 240) {
			Inv_AddItem(ID_SMALL_MEDIPACK_ITEM);
		}
		if (Objects[ID_LARGE_MEDIPACK_OPTION].loaded && Inv_RequestItem(ID_LARGE_MEDIPACK_ITEM) < 240) {
			Inv_AddItem(ID_LARGE_MEDIPACK_ITEM);
		}
	}

#ifdef FEATURE_CHEAT
	// NOTE: there is no cheat for keys/puzzles in the original code
	if (Objects[ID_KEY_OPTION1].loaded && !Inv_RequestItem(ID_KEY_ITEM1)) {
		Inv_AddItem(ID_KEY_ITEM1);
	}
	if (Objects[ID_KEY_OPTION2].loaded && !Inv_RequestItem(ID_KEY_ITEM2)) {
		Inv_AddItem(ID_KEY_ITEM2);
	}
	if (Objects[ID_KEY_OPTION3].loaded && !Inv_RequestItem(ID_KEY_ITEM3)) {
		Inv_AddItem(ID_KEY_ITEM3);
	}
	if (Objects[ID_KEY_OPTION4].loaded && !Inv_RequestItem(ID_KEY_ITEM4)) {
		Inv_AddItem(ID_KEY_ITEM4);
	}
	if (Objects[ID_PUZZLE_OPTION1].loaded && !Inv_RequestItem(ID_PUZZLE_ITEM1)) {
		Inv_AddItem(ID_PUZZLE_ITEM1);
	}
	if (Objects[ID_PUZZLE_OPTION2].loaded && !Inv_RequestItem(ID_PUZZLE_ITEM2)) {
		Inv_AddItem(ID_PUZZLE_ITEM2);
	}
	if (Objects[ID_PUZZLE_OPTION3].loaded && !Inv_RequestItem(ID_PUZZLE_ITEM3)) {
		Inv_AddItem(ID_PUZZLE_ITEM3);
	}
	if (Objects[ID_PUZZLE_OPTION4].loaded && !Inv_RequestItem(ID_PUZZLE_ITEM4)) {
		Inv_AddItem(ID_PUZZLE_ITEM4);
	}
	if (Objects[ID_PICKUP_OPTION1].loaded && !Inv_RequestItem(ID_PICKUP_ITEM1)) {
		Inv_AddItem(ID_PICKUP_ITEM1);
	}
	if (Objects[ID_PICKUP_OPTION2].loaded && !Inv_RequestItem(ID_PICKUP_ITEM2)) {
		Inv_AddItem(ID_PICKUP_ITEM2);
	}
#endif // FEATURE_CHEAT
}

void ControlLaraExtra(short itemNumber) {
	AnimateItem(&Items[itemNumber]);
}

void InitialiseLaraLoad(short itemNumber) {
	Lara.item_number = itemNumber;
	LaraItem = &Items[itemNumber];
}

void InitialiseLara(GF_LEVEL_TYPE levelType)
{
	ITEM_INFO* item = LaraItem;
	item->data = &Lara;
	item->collidable = FALSE;
	item->hitPoints = 1000;

	Lara.calc_fallspeed = 0;
	Lara.climb_status = 0;
	Lara.pose_count = 0;
	Lara.hit_frame = 0;
	Lara.hit_direction = -1;
#if defined(FEATURE_MOD_CONFIG)
	Lara.air = Mod.underwater.maxAir;
#else
	Lara.air = LARA_AIR_MAX;
#endif
	Lara.dive_count = 0;
	Lara.death_count = 0;
	Lara.current_active = 0;
	Lara.spaz_effect_count = 0;
	Lara.flare_age = 0;
	Lara.skidoo = -1;
	Lara.weapon_item = -1;
	Lara.back_gun = 0;
	Lara.flare_frame = 0;
	Lara.flare_control_left = Lara.flare_control_right = 0;
	Lara.extra_anim = 0;
	Lara.look = 1;
	Lara.burn = 0;
	Lara.water_surface_dist = 100;
	Lara.last_pos.x = item->pos.x;
	Lara.last_pos.y = item->pos.y;
	Lara.last_pos.z = item->pos.z;
	Lara.spaz_effect = NULL;
	Lara.mesh_effects = 0;
	Lara.target = NULL;
	Lara.turn_rate = 0;
	Lara.move_angle = 0;
	Lara.head_x_rot = Lara.head_y_rot = Lara.head_z_rot = 0;
	Lara.torso_x_rot = Lara.torso_y_rot = Lara.torso_z_rot = 0;
	Lara.left_arm.flash_gun = Lara.right_arm.flash_gun = 0;
	Lara.left_arm.lock = Lara.right_arm.lock = FALSE;
	Lara.creature = NULL;

	if (levelType == GFL_NORMAL && GF_LaraStartAnim)
	{
		Lara.water_status = LWS_AboveWater;
		Lara.gun_status = LGS_HandBusy;
		item->animNumber = Objects[ID_LARA_EXTRA].animIndex;
		item->frameNumber = Anims[item->animNumber].frameBase;
		item->currentAnimState = EXTRA_BREATH;
		item->goalAnimState = (short)GF_LaraStartAnim;
		AnimateLara(item);
		Lara.extra_anim = 1;

		Camera.type = CAM_Cinematic;
		CineFrameIdx = 0;
		memcpy(&CinematicPos, &item->pos, sizeof(PHD_3DPOS));
	}
	else if (CHK_ANY(Rooms[item->roomNumber].flags, ROOM_UNDERWATER))
	{
		Lara.water_status = LWS_Underwater;
		item->fallSpeed = 0;
		item->goalAnimState = AS_TREAD;
		item->currentAnimState = AS_TREAD;
		item->animNumber = 108;
		item->frameNumber = Anims[item->animNumber].frameBase;

	}
	else
	{
		Lara.water_status = LWS_AboveWater;
		item->goalAnimState = AS_STOP;
		item->currentAnimState = AS_STOP;
		item->animNumber = 103;
		item->frameNumber = Anims[item->animNumber].frameBase;
	}

	if (levelType == GFL_CUTSCENE)
	{
		for (int i = 0; i < 15; i++)
			Lara.mesh_ptrs[i] = &Meshes[Objects[ID_LARA].meshIndex + i];
		Lara.mesh_ptrs[LM_ThighL] = &Meshes[Objects[ID_LARA_PISTOLS].meshIndex + LM_ThighL];
		Lara.mesh_ptrs[LM_ThighR] = &Meshes[Objects[ID_LARA_PISTOLS].meshIndex + LM_ThighR];
		Lara.gun_status = LGS_Armless;
	}
	else
	{
		InitialiseLaraInventory(CurrentLevel);
	}

	DashTimer = 0;
}

void InitialiseLaraInventory(int levelID) {
	int i;
	START_INFO* start = &SaveGame.start[levelID];

	Inv_RemoveAllItems();

	if (GF_RemoveWeapons) {
		start->has_pistols = 0;
		start->has_autopistol = 0;
		start->has_uzis = 0;
		start->has_shotgun = 0;
		start->has_m16 = 0;
		start->has_grenade = 0;
		start->has_harpoon = 0;
		start->gunType = LGT_Unarmed;
		start->gunStatus = LGS_Armless;
		GF_RemoveWeapons = 0;
	}

	if (GF_RemoveAmmo) {
		start->m16Ammo = 0;
		start->grenadeAmmo = 0;
		start->harpoonAmmo = 0;
		start->shotgunAmmo = 0;
		start->uziAmmo = 0;
		start->autopistolAmmo = 0;
		start->pistolAmmo = 0;
		start->flares = 0;
		start->largeMedipacks = 0;
		start->smallMedipacks = 0;
		GF_RemoveAmmo = 0;
	}

	// NOTE: additional weapon availability checks not presented in the original game
	if (!Objects[ID_PISTOL_OPTION].loaded) {
		start->has_pistols = 0;
	}
	if (!Objects[ID_UZI_OPTION].loaded) {
		start->has_uzis = 0;
	}
	if (!Objects[ID_MAGNUM_OPTION].loaded) {
		start->has_autopistol = 0;
	}
	if (!Objects[ID_SHOTGUN_OPTION].loaded) {
		start->has_shotgun = 0;
	}
	if (!Objects[ID_GRENADE_OPTION].loaded) {
		start->has_grenade = 0;
	}
	if (!Objects[ID_M16_OPTION].loaded) {
		start->has_m16 = 0;
	}
	if (!Objects[ID_HARPOON_OPTION].loaded) {
		start->has_harpoon = 0;
	}

	// NOTE: additional ammo availability checks not presented in the original game
	if (!start->has_pistols && !Objects[ID_PISTOL_AMMO_OPTION].loaded) {
		start->pistolAmmo = 0;
	}
	if (!start->has_uzis && !Objects[ID_UZI_AMMO_OPTION].loaded) {
		start->uziAmmo = 0;
	}
	if (!start->has_autopistol && !Objects[ID_MAGNUM_AMMO_OPTION].loaded) {
		start->autopistolAmmo = 0;
	}
	if (!start->has_shotgun && !Objects[ID_SHOTGUN_AMMO_OPTION].loaded) {
		start->shotgunAmmo = 0;
	}
	if (!start->has_grenade && !Objects[ID_GRENADE_AMMO_OPTION].loaded) {
		start->grenadeAmmo = 0;
	}
	if (!start->has_m16 && !Objects[ID_M16_AMMO_OPTION].loaded) {
		start->m16Ammo = 0;
	}
	if (!start->has_harpoon && !Objects[ID_HARPOON_AMMO_OPTION].loaded) {
		start->harpoonAmmo = 0;
	}

	// NOTE: additional start gun type availability checks not presented in the original game
	switch (start->gunType) {
		// if the current weapon is absent then fall through
	case LGT_Pistols: if (start->has_pistols) break; // fall through
	case LGT_Magnums: if (start->has_autopistol) break; // fall through
	case LGT_Uzis: if (start->has_uzis) break; // fall through
	case LGT_Shotgun: if (start->has_shotgun) break; // fall through
	case LGT_M16: if (start->has_m16) break; // fall through
	case LGT_Grenade: if (start->has_grenade) break; // fall through
	case LGT_Harpoon: if (start->has_harpoon) break; // fall through
		start->gunType = start->has_pistols ? LGT_Pistols : LGT_Unarmed;
		break;
	default:
		break;
	}

	// Statistics
	Inv_AddItem(ID_COMPASS_ITEM);

	// Pistols
	Lara.pistol_ammo = PISTOLS_AMMO_DEFAULT;
	if (start->has_pistols) {
		Inv_AddItem(ID_PISTOL_ITEM);
	}

	// Magnums
	if (start->has_autopistol) {
		Inv_AddItem(ID_MAGNUM_ITEM);
		Lara.magnum_ammo = start->autopistolAmmo;
		GlobalItemReplace(ID_MAGNUM_ITEM, ID_MAGNUM_AMMO_ITEM);
	}
	else {
		for (i = 0; i < start->autopistolAmmo / AUTOPISTOLS_AMMO_CLIPS; ++i) {
			Inv_AddItem(ID_MAGNUM_AMMO_ITEM);
		}
		Lara.magnum_ammo = 0;
	}

	// Uzis
	if (start->has_uzis) {
		Inv_AddItem(ID_UZI_ITEM);
		Lara.uzi_ammo = start->uziAmmo;
		GlobalItemReplace(ID_UZI_ITEM, ID_UZI_AMMO_ITEM);
	}
	else {
		for (i = 0; i < start->uziAmmo / UZIS_AMMO_CLIPS; ++i) {
			Inv_AddItem(ID_UZI_AMMO_ITEM);
		}
		Lara.uzi_ammo = 0;
	}

	// Shotgun
	if (start->has_shotgun) {
		Inv_AddItem(ID_SHOTGUN_ITEM);
		Lara.shotgun_ammo = start->shotgunAmmo;
		GlobalItemReplace(ID_SHOTGUN_ITEM, ID_SHOTGUN_AMMO_ITEM);
	}
	else {
		for (i = 0; i < start->shotgunAmmo / SHOTGUN_AMMO_CLIPS; ++i) {
			Inv_AddItem(ID_SHOTGUN_AMMO_ITEM);
		}
		Lara.shotgun_ammo = 0;
	}

	// Grenade
	if (start->has_grenade) {
		Inv_AddItem(ID_GRENADE_ITEM);
		Lara.grenade_ammo = start->grenadeAmmo;
		GlobalItemReplace(ID_GRENADE_ITEM, ID_GRENADE_AMMO_ITEM);
	}
	else {
		for (i = 0; i < start->grenadeAmmo / GRENADE_AMMO_CLIPS; ++i) {
			Inv_AddItem(ID_GRENADE_AMMO_ITEM);
		}
		Lara.grenade_ammo = 0;
	}

	// M16
	if (start->has_m16) {
		Inv_AddItem(ID_M16_ITEM);
		Lara.m16_ammo = start->m16Ammo;
		GlobalItemReplace(ID_M16_ITEM, ID_M16_AMMO_ITEM);
	}
	else {
		for (i = 0; i < start->m16Ammo / M16_AMMO_CLIPS; ++i) {
			Inv_AddItem(ID_M16_AMMO_ITEM);
		}
		Lara.m16_ammo = 0;
	}

	// Harpoon
	if (start->has_harpoon) {
		Inv_AddItem(ID_HARPOON_ITEM);
		Lara.harpoon_ammo = start->harpoonAmmo;
		GlobalItemReplace(ID_HARPOON_ITEM, ID_HARPOON_AMMO_ITEM);
	}
	else {
		for (i = 0; i < start->harpoonAmmo / HARPOON_AMMO_CLIPS; ++i) {
			Inv_AddItem(ID_HARPOON_AMMO_ITEM);
		}
		Lara.harpoon_ammo = 0;
	}

	// Flares
	for (i = 0; i < start->flares; ++i) {
		Inv_AddItem(ID_FLARE_ITEM);
	}

	// Small medipacks
	for (i = 0; i < start->smallMedipacks; ++i) {
		Inv_AddItem(ID_SMALL_MEDIPACK_ITEM);
	}

	// Large medipacks
	for (i = 0; i < start->largeMedipacks; ++i) {
		Inv_AddItem(ID_LARGE_MEDIPACK_ITEM);
	}

	Lara.gun_status = LGS_Armless;
	Lara.last_gun_type = start->gunType;
	Lara.gun_type = start->gunType;
	Lara.request_gun_type = start->gunType;
	LaraInitialiseMeshes(levelID);
	InitialiseNewWeapon();
}

void LaraInitialiseMeshes(int levelID)
{
	START_INFO* start = &SaveGame.start[levelID];

	for (int i = 0; i < 15; i++)
		Lara.mesh_ptrs[i] = MeshPtr[Objects[ID_LARA].meshIndex + i];

	if (start->gunType != LGT_Unarmed)
	{
		short meshIdx = 0;
		switch (start->gunType)
		{
		case LGT_Magnums:
			meshIdx = Objects[ID_LARA_MAGNUMS].meshIndex;
			break;
		case LGT_Uzis:
			meshIdx = Objects[ID_LARA_UZIS].meshIndex;
			break;
		default:
			meshIdx = Objects[ID_LARA_PISTOLS].meshIndex;
			break;
		}
		Lara.mesh_ptrs[1] = MeshPtr[meshIdx + 1];
		Lara.mesh_ptrs[4] = MeshPtr[meshIdx + 4];
	}

	if (start->gunType == LGT_Flare)
		Lara.mesh_ptrs[13] = MeshPtr[Objects[ID_LARA_FLARE].meshIndex + 13];

	switch (start->gunType)
	{
	case LGT_M16:
		Lara.back_gun = ID_LARA_M16;
		break;
	case LGT_Harpoon:
		Lara.back_gun = ID_LARA_HARPOON;
		break;
	case LGT_Grenade:
		Lara.back_gun = ID_LARA_GRENADE;
		break;
	case LGT_Shotgun:
		Lara.back_gun = ID_LARA_SHOTGUN;
		break;
	default:
		if (start->has_shotgun)
			Lara.back_gun = ID_LARA_SHOTGUN;
		else if (start->has_m16)
			Lara.back_gun = ID_LARA_M16;
		else if (start->has_grenade)
			Lara.back_gun = ID_LARA_GRENADE;
		else if (start->has_harpoon)
			Lara.back_gun = ID_LARA_HARPOON;
		break;
	}
}

/*
 * Inject function
 */
void Inject_LaraMisc() {
	INJECT(0x00430380, LaraControl);
	INJECT(0x00430A10, AnimateLara);
	INJECT(0x00430D10, UseItem);
	INJECT(0x00430ED0, LaraCheatGetStuff);
	INJECT(0x00430F90, ControlLaraExtra);
	INJECT(0x00430FB0, InitialiseLaraLoad);
	INJECT(0x00430FE0, InitialiseLara);
	INJECT(0x004312A0, InitialiseLaraInventory);
	INJECT(0x00431610, LaraInitialiseMeshes);
}