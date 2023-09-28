/*
 * Copyright (c) 2017-2021 Michael Chaban. All rights reserved.
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
			if (Objects[fx->object_number].control) {
				Objects[fx->object_number].control(id);
			}
			next = fx->next_active;
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

/*#define END_BIT 0x80
void TestTriggers(short* data, BOOL isHeavy)
{
	ITEM_INFO* item = NULL, *camera_item = NULL;
	OBJECT_VECTOR* fixed_cam = NULL;
	int quadrant = 0, value = 0, type = 0, flags = 0, timer = 0, flip = 0, flip_enabled = 0, effect = -1, switch_off = 0;
	short trigger = 0;

	if (!isHeavy)
		Lara.climb_status = 0;

	if (data == NULL)
	{
		LogWarn("Failed to test triggers, data is NULL");
		return;
	}

	switch (*data)
	{
	case LAVA_TYPE:
		LogDebug("Lava trigger");
		if (!isHeavy && (LaraItem->pos.y == LaraItem->floor || Lara.water_status != LWS_AboveWater))
			LavaBurn(LaraItem);
		if (*(data + 1) & END_BIT)
			return;
		++data;
		break;
	case CLIMB_TYPE:
		LogDebug("Climb trigger");
		if (!isHeavy)
		{
			quadrant = 1 << (((LaraItem->pos.rotY + 0x2000) >> 14) + 8);
			if (quadrant & *data)
				Lara.climb_status = 1;
		}
		if (*(data + 1) & END_BIT)
			return;
		++data;
		break;
	}

	type = (*(data++) >> 8) & 0x3F;
	flags = *(data++);
	timer = flags & 0xFF;

	if (Camera.type != CAM_Heavy)
		RefreshCamera(type, data);

	if (!isHeavy)
	{
		switch (type)
		{
		case SWITCH:
			value = *(data++) & 0x3FF;
			if (!SwitchTrigger(value, timer))
				return;
			LogDebug("Switch trigger");
			switch_off = (Items[value].currentAnimState == 1);
			break;
		case PAD:
		case ANTIPAD:
			if (LaraItem->pos.y != LaraItem->floor)
				return;
			LogDebug("Pad/Antipad trigger");
			break;
		case KEY:
			LogDebug("Key trigger");
			break;
		case PICKUP:
			LogDebug("Pickup trigger");
			break;
		case COMBAT:
			LogDebug("Combat trigger");
			break;
		case HEAVY:
		case DUMMY:
			return;
		}
	}
	else if (type != HEAVY)
		return;

	do
	{
		trigger = *(data++);
		value = trigger & 0x3FF;

		switch ((trigger >> 10) & 0xF)
		{
		case TO_OBJECT:
			LogDebug("Object trigger");
			if (value < 0 || value > NUMBER_ITEMS) // NOTE: not exist in the og.
			{
				LogWarn("Failed to call TO_OBJECT trigger, value returned was less than 0 or more than 1024 !");
				break;
			}
			item = &Items[value];
			if (item->flags & IFL_INVISIBLE)
				break;

			item->timer = timer;
			if (timer != 1)
				item->timer *= 30;

			switch (type)
			{
			case SWITCH:
				item->flags ^= (flags & IFL_CODEBITS);
				//if (flags & IFL_INVISIBLE)
				//	item->flags |= IFL_INVISIBLE;
				break;
			case ANTIPAD:
			case ANTITRIGGER:
				item->flags &= ~(IFL_CODEBITS | IFL_REVERSE);
				//if (flags & IFL_INVISIBLE)
				//	item->flags |= IFL_INVISIBLE;
				break;
			default:
				if (flags & IFL_CODEBITS)
					item->flags |= (flags & IFL_CODEBITS);
				break;
			}

			if ((item->flags & IFL_CODEBITS) == IFL_CODEBITS)
				break;

			if (flags & 0x100)
				item->flags |= 1;

			if (!item->active)
			{
				if (Objects[item->objectID].intelligent)
				{
					switch (item->status)
					{
					case ITEM_ACTIVE:
						item->touchBits = 0;
						item->status = ITEM_ACTIVE;
						AddActiveItem(value);
						EnableBaddieAI(value, TRUE);
						break;
					case ITEM_INVISIBLE:
						item->touchBits = 0;
						if (EnableBaddieAI(value, FALSE))
							item->status = ITEM_ACTIVE;
						else
							item->status = ITEM_INVISIBLE;
						AddActiveItem(value);
						break;
					}
				}
				else
				{
					item->touchBits = 0;
					item->status = ITEM_ACTIVE;
					AddActiveItem(value);
				}
			}

			break;
		case TO_CAMERA:
			LogDebug("Camera trigger");

			break;
		case TO_TARGET:
			LogDebug("Target trigger");
			if (value < 0 || value > NUMBER_ITEMS) // NOTE: not exist in the og.
			{
				LogWarn("Failed to call TO_TARGET trigger, value returned was less than 0 or more than 1024 !");
				break;
			}
			camera_item = &Items[value];
			break;
		case TO_SINK:
			LogDebug("Sink trigger");
			fixed_cam = &Camera.fixed[value];
			if (Lara.creature == NULL)
				EnableBaddieAI(Lara.item_number, TRUE);
			Lara.creature->LOT.target.x = fixed_cam->x;
			Lara.creature->LOT.target.y = fixed_cam->y;
			Lara.creature->LOT.target.z = fixed_cam->z;
			Lara.creature->LOT.required_box = fixed_cam->flags;
			Lara.current_active = 6 * fixed_cam->data;
			break;
		case TO_FLIPMAP:
			LogDebug("Flipmap trigger");
			flip_enabled = 1;

			break;
		case TO_FLIPON:
			LogDebug("Flipon trigger");
			flip_enabled = 1;
			if ((FlipMaps[value] & IFL_CODEBITS) == IFL_CODEBITS && !FlipStatus)
				flip = 1;
			break;
		case TO_FLIPOFF:
			LogDebug("Flipoff trigger");
			flip_enabled = 1;
			if ((FlipMaps[value] & IFL_CODEBITS) == IFL_CODEBITS && FlipStatus)
				flip = 1;
			break;
		case TO_FLIPEFFECT:
			LogDebug("Flipeffect trigger");
			effect = value;
			break;
		case TO_FINISH:
			LogDebug("Finish trigger");
			IsLevelComplete = TRUE;
			break;
		case TO_CD:
			LogDebug("CD trigger");
			TriggerCDTrack(value, flags, type);
			break;
		case TO_BODYBAG:
			LogDebug("Bodybag trigger");
			//ClearBodyBag();
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
}*/

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
		if (CHK_ANY(flags, IFL_INVISIBLE)) {
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
	//	INJECT(0x004146C0, AnimateItem);
	//	INJECT(0x00414A30, GetChange);
	//	INJECT(0x00414AE0, TranslateItem);
	//	INJECT(0x00414B40, GetFloor);
	//	INJECT(0x00414CE0, GetWaterHeight);
	//	INJECT(0x00414E50, GetHeight);
	//	INJECT(0x004150D0, RefreshCamera);
	//INJECT(0x004151C0, TestTriggers); (WIP)
	//	INJECT(0x004158A0, TriggerActive);
	//	INJECT(0x00415900, GetCeiling);
	//	INJECT(0x00415B60, GetDoor);
	//	INJECT(0x00415BB0, LOS);
	//	INJECT(0x00415C50, zLOS);
	//	INJECT(0x00415F40, xLOS);
	//	INJECT(0x00416230, ClipTarget);
	//	INJECT(0x00416310, ObjectOnLOS);
	//	INJECT(0x00416610, FlipMap);
	//	INJECT(0x004166D0, RemoveRoomFlipItems);
	//	INJECT(0x00416770, AddRoomFlipItems);
	INJECT(0x004167D0, TriggerCDTrack);
	INJECT(0x00416800, TriggerNormalCDTrack);
}