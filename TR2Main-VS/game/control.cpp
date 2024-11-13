/*
 * Copyright (c) 2017-2024 Michael Chaban. All rights reserved.
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
#include "game/control.h"
#include "3dsystem/phd_math.h"
#include "game/camera.h"
#include "game/demo.h"
#include "game/effects.h"
#include "game/hair.h"
#include "game/items.h"
#include "game/inventory.h"
#include "game/laramisc.h"
#include "game/traps.h"
#include "game/lot.h"
#include "game/pickup.h"
#include "game/sound.h"
#include "game/savegame.h"
#include "specific/game.h"
#include "specific/input.h"
#include "specific/smain.h"
#include "specific/sndpc.h"
#include "global/vars.h"

#ifdef FEATURE_BACKGROUND_IMPROVED
#include "modding/pause.h"
#endif // FEATURE_BACKGROUND_IMPROVED

#ifdef FEATURE_INPUT_IMPROVED
#include "modding/joy_output.h"
#endif // FEATURE_INPUT_IMPROVED

int ControlPhase(int nTicks, BOOL demoMode) {
	static int tickCount = 0;
	ITEM_INFO* item = NULL;
	FX_INFO* fx = NULL;
	int id = -1;
	int next = -1;
	int result = 0;

	CLAMPG(nTicks, 5 * TICKS_PER_FRAME);
	for (tickCount += nTicks; tickCount > 0; tickCount -= TICKS_PER_FRAME) {
		if (CD_TrackID > 0) {
			S_CDLoop();
		}
		if (!CHK_ANY(GF_GameFlow.flags, GFF_CheatModeCheckDisabled)) {
			CheckCheatMode();
		}
		if (IsLevelComplete) {
			return 1;
		}
		S_UpdateInput();
		if (IsResetFlag) {
			return GF_EXIT_TO_TITLE;
		}
		if (demoMode) {
			if (InputStatus) {
				return GF_GameFlow.onDemo_Interrupt;
			}
			GetDemoInput();
			if (InputStatus == (DWORD)~0) {
				InputStatus = 0;
				return GF_GameFlow.onDemo_End;
			}
		}
		else {
			if (CHK_ANY(GF_GameFlow.flags, GFF_NoInputTimeout)) {
				if (InputStatus) {
					NoInputCounter = 0;
				}
				else if (++NoInputCounter > GF_GameFlow.noInput_Time) {
					return GF_START_DEMO;
				}
			}
		}
		if (OverlayStatus == 2 || Lara.death_count > 10 * 30 || (Lara.death_count > 2 * 30 && InputStatus)) {
			if (demoMode) {
				return GF_GameFlow.onDeath_DemoMode;
			}
			if (CurrentLevel == 0) { // Lara's Home
				return GF_EXIT_TO_TITLE;
			}
			if (OverlayStatus == 2) {
				OverlayStatus = 1;
				result = Display_Inventory(INV_DeathMode);
				if (result) {
					return result;
				}
			}
			else {
				OverlayStatus = 2;
			}
		}
		if (!Lara.death_count && !Lara.extra_anim && (CHK_ANY(InputStatus, IN_OPTION | IN_LOAD | IN_SAVE) || OverlayStatus <= 0)) {
			if (OverlayStatus > 0) {
				if (CHK_ANY(GF_GameFlow.flags, GFF_LoadSaveDisabled)) {
					OverlayStatus = 0;
				}
				else if (CHK_ANY(InputStatus, IN_LOAD)) {
					OverlayStatus = -1;
				}
				else if (CHK_ANY(InputStatus, IN_SAVE)) {
					OverlayStatus = -2;
				}
				else {
					OverlayStatus = 0;
				}
			}
			else {
				if (OverlayStatus == -1) {
					result = Display_Inventory(INV_LoadMode);
				}
				else if (OverlayStatus == -2) {
					result = Display_Inventory(INV_SaveMode);
				}
				else {
					result = Display_Inventory(INV_GameMode);
				}
				OverlayStatus = 1;
				if (result) {
					if (InventoryExtraData[0] != 1) {
						return result;
					}
					if (CurrentLevel == 0) { // Lara's Home
						return 1;
					}
					CreateSaveGameInfo();
					S_SaveGame(&SaveGame, sizeof(SaveGame), InventoryExtraData[1]);
					S_SaveSettings();
				}
			}
		}

#ifdef FEATURE_BACKGROUND_IMPROVED
		if (!Lara.death_count && !Lara.extra_anim && CHK_ANY(InputStatus, IN_PAUSE) && S_Pause()) {
			return 1;
		}
#endif // FEATURE_BACKGROUND_IMPROVED

		DynamicLightCount = 0;

		for (id = NextItemActive; id >= 0; id = next) {
			item = &Items[id];
			// NOTE: there is no IFL_CLEARBODY check in the original code
			if (Objects[item->objectID].control && !CHK_ANY(item->flags, IFL_CLEARBODY)) {
				Objects[item->objectID].control(id);
			}
			next = item->nextActive;
		}

		for (id = NextEffectActive; id >= 0; id = next) {
			fx = &Effects[id];
			if (Objects[fx->objectID].control) {
				Objects[fx->objectID].control(id);
			}
			next = fx->nextActive;
		}

		LaraControl(0);
		HairControl(0);
		CalculateCamera();
		SoundEffects();
		--HealthBarTimer;

		// Update statistics timer for normal levels
		if (CurrentLevel != 0 || IsAssaultTimerActive) {
			++SaveGame.statistics.timer;
		}
	}
#ifdef FEATURE_INPUT_IMPROVED
	UpdateJoyOutput(!IsDemoLevelType);
#endif // FEATURE_INPUT_IMPROVED
	return 0;
}

void AnimateItem(ITEM_INFO* item)
{
	ANIM_STRUCT* anim = NULL;
	USHORT type = 0;
	short num = 0;
	short* command = NULL;

	item->hitStatus = FALSE;
	item->touchBits = 0;
	item->frameNumber++;

	anim = &Anims[item->animNumber];
	if (anim->numberChanges > 0 && GetChange(item, anim))
	{
		anim = &Anims[item->animNumber];
		item->currentAnimState = anim->currentAnimState;
		if (item->requiredAnimState == item->currentAnimState)
			item->requiredAnimState = 0;
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
					break;
				case 4:
					item->status = ITEM_DISABLED;
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
		if (item->currentAnimState != anim->currentAnimState)
		{
			item->currentAnimState = anim->currentAnimState;
			item->goalAnimState = item->currentAnimState;
		}
		if (item->requiredAnimState == item->currentAnimState)
			item->requiredAnimState = 0;
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
					num = command[1] & 0x3FFF;
					if (Objects[item->objectID].water_creature)
					{
						PlaySoundEffect(num, &item->pos, SFX_UNDERWATER);
					}
					else
					{
						if (item->roomNumber != 255)
						{
							short roomNum = item->roomNumber;
							GetFloor(item->pos.x, item->pos.y, item->pos.z, &roomNum);
							if (type == SFX_WATERONLY)
								CreateSplash(item->pos.x, item->pos.y, item->pos.z, item->roomNumber);
						}

						if (item->roomNumber == 255)
						{
							item->pos.x = LaraItem->pos.x;
							item->pos.y = LaraItem->pos.y - 762;
							item->pos.z = LaraItem->pos.z;
							if (item->objectID == ID_LARA_HARPOON)
								PlaySoundEffect(num, &item->pos, SFX_ALWAYS);
							else
								PlaySoundEffect(num, &item->pos, NULL);
						}
						else if (CHK_ANY(RoomInfo[item->roomNumber].flags, ROOM_UNDERWATER))
						{
							if (type == SFX_LANDANDWATER || type == SFX_WATERONLY)
								PlaySoundEffect(num, &item->pos, NULL);
						}
						else if (type == SFX_LANDANDWATER || type == SFX_LANDONLY)
							PlaySoundEffect(num, &item->pos, NULL);
					}
				}
				command += 2;
				break;
			case 6:
				if (item->frameNumber == command[0])
				{
					num = command[1] & 0x3FFF;
					EffectFunctions[num](item);
				}
				command += 2;
				break;
			}
		}
	}

	if (item->gravity)
	{
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

	item->pos.x += item->speed * phd_sin(item->pos.rotY) >> W2V_SHIFT;
	item->pos.z += item->speed * phd_cos(item->pos.rotY) >> W2V_SHIFT;
}

void TestTriggers(short* data, BOOL isHeavy)
{
	CREATURE_INFO* creature = NULL;
	LOT_INFO* LOT = NULL;
	ITEM_INFO* item = NULL, *camera_item = NULL;
	OBJECT_VECTOR* fixed_cam = NULL;
	int value = 0, type = 0, flags = 0, timer = 0, flip = 0, flip_enabled = 0, effect = -1, switch_off = 0;
	USHORT quadrant = 0;
	short trigger = 0, camera_flags = 0, camera_timer = 0;

	if (!isHeavy)
		Lara.climb_status = 0;

	if (data == NULL)
		return;

	if ((*data & DATA_TYPE) == FT_LAVA)
	{
		if (!isHeavy && (LaraItem->pos.y == LaraItem->floor || Lara.water_status != LWS_AboveWater))
			LavaBurn(LaraItem);
		if (*data & END_BIT)
			return;
		++data;
	}

	if ((*data & DATA_TYPE) == FT_CLIMB)
	{
		if (!isHeavy)
		{
			quadrant = USHORT(LaraItem->pos.rotY + 0x2000) / 0x4000;
			if ((1 << (quadrant + 8)) & *data)
				Lara.climb_status = 1;
		}
		if (*data & END_BIT)
			return;
		++data;
	}

	type = (*data >> 8) & 0x3F;
	data++;
	flags = *data++;
	timer = flags & 0xFF;

	if (Camera.type != CAM_Heavy)
		RefreshCamera(type, data);

	if (isHeavy)
	{
		if (type != TT_HEAVY)
			return;
	}
	else
	{
		switch (type)
		{
		case TT_SWITCH:
			value = *(data++) & VALUE_BITS;
			if (!SwitchTrigger(value, timer))
				return;
			switch_off = Items[value].currentAnimState == 1;
			break;
		case TT_PAD:
		case TT_ANTIPAD:
			if (LaraItem->pos.y != LaraItem->floor)
				return;
			break;
		case TT_KEY:
			value = *(data++) & VALUE_BITS;
			if (!KeyTrigger(value))
				return;
			break;
		case TT_PICKUP:
			value = *(data++) & VALUE_BITS;
			if (!PickupTrigger(value))
				return;
			break;
		case TT_COMBAT:
			if (Lara.gun_status != LGS_Ready)
				return;
			break;
		case TT_HEAVY:
		case TT_DUMMY:
			return;
		}
	}

	camera_item = NULL;
	do
	{
		trigger = *(data++);
		value = trigger & VALUE_BITS;

		switch (TRIG_BITS(trigger))
		{
		case TO_OBJECT:
			item = &Items[value];
			if (CHK_ANY(item->flags, IFL_ONESHOT))
				break;

			item->timer = timer;
			if (timer != 1)
				item->timer *= FRAMES_PER_SECOND;

			if (type == TT_SWITCH)
			{
				item->flags ^= (flags & IFL_CODEBITS);
			}
			else if (type == TT_ANTIPAD || type == TT_ANTITRIGGER)
			{
				item->flags &= ~(flags & IFL_CODEBITS);
				if (CHK_ANY(flags, IFL_ONESHOT))
					item->flags |= IFL_ONESHOT;
			}
			else if (CHK_ANY(flags, IFL_CODEBITS))
				item->flags |= (flags & IFL_CODEBITS);

			if (CHK_NOP(item->flags, IFL_CODEBITS))
				break;

			if (CHK_ANY(flags, IFL_ONESHOT))
				item->flags |= IFL_ONESHOT;

			if (!item->active)
			{
				if (Objects[item->objectID].intelligent)
				{
					if (item->status == ITEM_INACTIVE)
					{
						item->touchBits = 0;
						item->status = ITEM_ACTIVE;
						AddActiveItem(value);
						EnableBaddieAI(value, TRUE);
					}
					else if (item->status == ITEM_INVISIBLE)
					{
						item->touchBits = 0;
						if (EnableBaddieAI(value, FALSE))
							item->status = ITEM_ACTIVE;
						else
							item->status = ITEM_INVISIBLE;
						AddActiveItem(value);
					}
				}
				else
				{
					item->touchBits = 0;
					AddActiveItem(value);
					item->status = ITEM_ACTIVE;
				}
			}

			break;
		case TO_CAMERA:
			trigger = *(data++);
			camera_flags = trigger;
			camera_timer = trigger & 0xFF;
			if (CHK_ANY(Camera.fixed[value].flags, IFL_ONESHOT))
				break;

			Camera.number = value;
			if (Camera.type == CAM_Look || Camera.type == CAM_Combat)
				break;
			if (type == TT_COMBAT)
				break;
			if (type == TT_SWITCH && timer && switch_off)
				break;

			if (Camera.number != Camera.last || type == TT_SWITCH)
			{
				Camera.timer = camera_timer * FRAMES_PER_SECOND;
				if (CHK_ANY(camera_flags, IFL_ONESHOT))
					Camera.fixed[value].flags |= IFL_ONESHOT;
				Camera.speed = ((camera_flags & IFL_CODEBITS) >> 6) + 1;
				Camera.type = isHeavy ? CAM_Heavy : CAM_Fixed;
			}

			break;
		case TO_TARGET:
			if (value < 0 || value > NUMBER_ITEMS) // NOTE: not exist in the og.
			{
				LogWarn("Failed to call TO_TARGET trigger, value returned was less than 0 or more than 1024 !");
				break;
			}
			camera_item = &Items[value];
			break;
		case TO_SINK:
			fixed_cam = &Camera.fixed[value];

			if (Lara.creature == NULL)
			{
				if (!EnableBaddieAI(Lara.item_number, TRUE))
					LogWarn("Failed to enable AI for Lara.creature !");
			}

			if (Lara.creature != NULL)
			{
				LOT = &Lara.creature->LOT;
				if (LOT)
				{
					LOT->target.x = fixed_cam->x;
					LOT->target.y = fixed_cam->y;
					LOT->target.z = fixed_cam->z;
					LOT->requiredBox = fixed_cam->flags;
				}
				Lara.current_active = fixed_cam->data * 6;
			}
			else
			{
				LogWarn("Trying to play sink trigger when Lara.creature is NULL !");
			}
			
			break;
		case TO_FLIPMAP:
			flip_enabled = 1;
			if (CHK_ANY(FlipMaps[value], IFL_ONESHOT))
				break;

			if (type == TT_SWITCH)
				FlipMaps[value] ^= (flags & IFL_CODEBITS);
			else if (flags & IFL_CODEBITS)
				FlipMaps[value] |= (flags & IFL_CODEBITS);

			if (CHK_ALL(FlipMaps[value], IFL_CODEBITS))
			{
				if (CHK_ANY(flags, IFL_ONESHOT))
					FlipMaps[value] |= IFL_ONESHOT;
				if (!FlipStatus)
					flip = 1;
			}
			else if (FlipStatus)
				flip = 1;

			break;
		case TO_FLIPON:
			flip_enabled = 1;
			if (CHK_ALL(FlipMaps[value], IFL_CODEBITS) && !FlipStatus)
				flip = 1;
			break;
		case TO_FLIPOFF:
			flip_enabled = 1;
			if (CHK_ALL(FlipMaps[value], IFL_CODEBITS) && FlipStatus)
				flip = 1;
			break;
		case TO_FLIPEFFECT:
			effect = value;
			break;
		case TO_FINISH:
			IsLevelComplete = TRUE;
			break;
		case TO_CD:
			TriggerCDTrack(value, flags, type);
			break;
		case TO_BODYBAG:
			ClearBodyBag();
			break;
		}
	}
	while (!(trigger & END_BIT));

	if (camera_item != NULL && (Camera.type == CAM_Fixed || Camera.type == CAM_Heavy))
		Camera.item = camera_item;

	if (flip)
		FlipMap();

	if (effect != -1 && (flip || !flip_enabled))
	{
		FlipEffect = effect;
		FlipTimer = 0;
	}
}

int LOS(GAME_VECTOR* start, GAME_VECTOR* target) {
	int beginning, ending;

	if (ABS(target->z - start->z) > ABS(target->x - start->x)) {
		beginning = xLOS(start, target);
		ending = zLOS(start, target);
	}
	else {
		beginning = zLOS(start, target);
		ending = xLOS(start, target);
	}
	return ending && ClipTarget(start, target, GetFloor(target->x, target->y, target->z, &target->roomNumber)) && beginning == 1 && ending == 1;
}

int zLOS(GAME_VECTOR* start, GAME_VECTOR* target) {
	int dx, dy, dz, x, y, z;
	short previousID, roomID;
	FLOOR_INFO* floor;

	dz = target->z - start->z;
	if (!dz)
		return 1;
	dx = ((target->x - start->x) << WALL_SHIFT) / dz;
	previousID = start->roomNumber;
	roomID = start->roomNumber;
	LosRooms[0] = start->roomNumber;
	LosRoomsCount = 1;
	dy = ((target->y - start->y) << WALL_SHIFT) / dz;
	if (dz < 0) {
		z = start->z & -0x400;
		x = start->x + ((z - start->z) * dx >> WALL_SHIFT);
		y = start->y + ((z - start->z) * dy >> WALL_SHIFT);
		while (z > target->z) {
			floor = GetFloor(x, y, z, &roomID);
			if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z)) {
				target->x = x;
				target->y = y;
				target->z = z;
				target->roomNumber = roomID;
				return -1;
			}
			if (roomID != previousID) {
				previousID = roomID;
				LosRooms[LosRoomsCount] = roomID;
				++LosRoomsCount;
			}
			floor = GetFloor(x, y, z - 1, &roomID);
			if (y > GetHeight(floor, x, y, z - 1) || y < GetCeiling(floor, x, y, z - 1)) {
				target->x = x;
				target->roomNumber = previousID;
				target->y = y;
				target->z = z;
				return 0;
			}
			z -= 1024;
			x -= dx;
			y -= dy;
		}
	}
	else {
		z = start->z | 0x3FF;
		x = start->x + ((z - start->z) * dx >> WALL_SHIFT);
		y = start->y + ((z - start->z) * dy >> WALL_SHIFT);
		while (z < target->z) {
			floor = GetFloor(x, y, z, &roomID);
			if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z)) {
				target->x = x;
				target->y = y;
				target->z = z;
				target->roomNumber = roomID;
				return -1;
			}
			if (roomID != previousID) {
				previousID = roomID;
				LosRooms[LosRoomsCount] = roomID;
				++LosRoomsCount;
			}
			floor = GetFloor(x, y, z + 1, &roomID);
			if (y > GetHeight(floor, x, y, z + 1) || y < GetCeiling(floor, x, y, z + 1)) {
				target->x = x;
				target->y = y;
				target->z = z;
				target->roomNumber = previousID;
				return 0;
			}
			z += 1024;
			x += dx;
			y += dy;
		}
	}
	target->roomNumber = roomID;
	return 1;
}

int xLOS(GAME_VECTOR* start, GAME_VECTOR* target) {
	int dx, dy, dz, x, y, z;
	short previousID, roomID;
	FLOOR_INFO* floor;

	dx = target->x - start->x;
	if (!dx)
		return 1;
	dy = ((target->y - start->y) << WALL_SHIFT) / dx;
	previousID = start->roomNumber;
	roomID = start->roomNumber;
	LosRooms[0] = start->roomNumber;
	LosRoomsCount = 1;
	dz = ((target->z - start->z) << WALL_SHIFT) / dx;
	if (dx < 0) {
		x = start->x & -0x400;
		y = start->y + ((x - start->x) * dy >> WALL_SHIFT);
		z = start->z + ((x - start->x) * dz >> WALL_SHIFT);
		while (x > target->x) {
			floor = GetFloor(x, y, z, &roomID);
			if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z)) {
				target->x = x;
				target->y = y;
				target->z = z;
				target->roomNumber = roomID;
				return -1;
			}
			if (roomID != previousID) {
				previousID = roomID;
				LosRooms[LosRoomsCount] = roomID;
				++LosRoomsCount;
			}
			floor = GetFloor(x - 1, y, z, &roomID);
			if (y > GetHeight(floor, x - 1, y, z) || y < GetCeiling(floor, x - 1, y, z)) {
				target->x = x;
				target->roomNumber = previousID;
				target->y = y;
				target->z = z;
				return 0;
			}
			x -= 1024;
			y -= dy;
			z -= dz;
		}
	}
	else {
		x = start->x | 0x3FF;
		y = start->y + ((x - start->x) * dy >> WALL_SHIFT);
		z = start->z + ((x - start->x) * dz >> WALL_SHIFT);
		while (x < target->x) {
			floor = GetFloor(x, y, z, &roomID);
			if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z)) {
				target->z = z;
				target->y = y;
				target->x = x;
				target->roomNumber = roomID;
				return -1;
			}
			if (roomID != previousID) {
				previousID = roomID;
				LosRooms[LosRoomsCount] = roomID;
				++LosRoomsCount;
			}
			floor = GetFloor(x + 1, y, z, &roomID);
			if (y > GetHeight(floor, x + 1, y, z) || y < GetCeiling(floor, x + 1, y, z)) {
				target->x = x;
				target->y = y;
				target->z = z;
				target->roomNumber = previousID;
				return 0;
			}
			x += 1024;
			y += dy;
			z += dz;
		}
	}
	target->roomNumber = roomID;
	return 1;
}

int ClipTarget(GAME_VECTOR* start, GAME_VECTOR* target, FLOOR_INFO* floor) {
	int dx, dy, dz, height, ceiling;

	dx = target->x - start->x;
	dy = target->y - start->y;
	dz = target->z - start->z;
	height = GetHeight(floor, target->x, target->y, target->z);
	if (target->y > height && start->y < height) {
		target->y = height;
		target->x = start->x + (target->y - start->y) * dx / dy;
		target->z = start->z + (target->y - start->y) * dz / dy;
		return 0;
	}
	ceiling = GetCeiling(floor, target->x, target->y, target->z);
	if (target->y < ceiling && start->y > ceiling) {
		target->y = ceiling;
		target->x = start->x + (target->y - start->y) * dx / dy;
		target->z = start->z + (target->y - start->y) * dz / dy;
		return 0;
	}
	return 1;
}

void TriggerCDTrack(short value, UINT16 flags, short type) {
	if (value > 1 && value < 64) {
		TriggerNormalCDTrack(value, flags, type);
	}
}

void TriggerNormalCDTrack(short value, UINT16 flags, short type) {
	if (type != 2) {
		UINT16 codebits = flags & IFL_CODEBITS;
		if (CHK_ANY(codebits, CD_Flags[value])) {
			return;
		}
		if (CHK_ANY(flags, IFL_ONESHOT)) {
			CD_Flags[value] |= codebits;
		}
	}

	if (value == CD_TrackID) {
		UINT8 timer = CD_Flags[value] & 0xFF;
		if (timer) {
			if (!--timer) {
				CD_TrackID = -1;
				S_CDPlay(value, FALSE);
			}
			CD_Flags[value] = (CD_Flags[value] & ~0xFF) | timer;
		}
	}
	else {
		UINT8 timer = flags & 0xFF;
		if (timer) {
			CD_TrackID = value;
			CD_Flags[value] = (CD_Flags[value] & ~0xFF) | ((timer * 30) & 0xFF);
		}
		else {
			S_CDPlay(value, FALSE);
		}
	}
}

/*
 * Inject function
 */
void Inject_Control() {
	INJECT(0x00414370, ControlPhase);
	INJECT(0x004146C0, AnimateItem);
	//INJECT(0x00414A30, GetChange);
	//INJECT(0x00414AE0, TranslateItem);
	//INJECT(0x00414B40, GetFloor);
	//INJECT(0x00414CE0, GetWaterHeight);
	//INJECT(0x00414E50, GetHeight);
	//INJECT(0x004150D0, RefreshCamera);
	INJECT(0x004151C0, TestTriggers);
	//INJECT(0x004158A0, TriggerActive);
	//INJECT(0x00415900, GetCeiling);
	//INJECT(0x00415B60, GetDoor);
	INJECT(0x00415BB0, LOS);
	INJECT(0x00415C50, zLOS);
	INJECT(0x00415F40, xLOS);
	INJECT(0x00416230, ClipTarget);
	//INJECT(0x00416310, ObjectOnLOS);
	//INJECT(0x00416610, FlipMap);
	//INJECT(0x004166D0, RemoveRoomFlipItems);
	//INJECT(0x00416770, AddRoomFlipItems);
	INJECT(0x004167D0, TriggerCDTrack);
	INJECT(0x00416800, TriggerNormalCDTrack);
}