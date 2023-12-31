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
#include "game/demo.h"
#include "game/control.h"
#include "game/gameflow.h"
#include "game/items.h"
#include "game/laramisc.h"
#include "game/setup.h"
#include "game/text.h"
#include "specific/frontend.h"
#include "specific/game.h"
#include "specific/winmain.h"
#include "global/vars.h"

#ifdef FEATURE_HUD_IMPROVED
extern bool PsxBarPosEnabled;
DWORD DemoTextMode = 0;
#endif // FEATURE_HUD_IMPROVED

int DoDemoSequence(int levelID) {
	static int DemoLevelID = 0;

	if (levelID < 0 && !GF_GameFlow.num_Demos)
		return GF_EXIT_TO_TITLE;
	if (levelID < 0) {
		if (DemoLevelID >= GF_GameFlow.num_Demos)
			DemoLevelID = 0;
		++DemoLevelID;
		return GF_DoLevelSequence(GF_DemoLevels[DemoLevelID - 1], GFL_DEMO);
	}
	DemoLevelID = levelID;
	return GF_DoLevelSequence(levelID, GFL_DEMO);
}

int StartDemo(int levelID) {
	static int DemoLevelID = 0;

	if (levelID < 0 && !GF_GameFlow.num_Demos) {
		return GF_EXIT_TO_TITLE;
	}

	if (levelID < 0) {
		if (DemoLevelID >= GF_GameFlow.num_Demos) {
			DemoLevelID = 0;
		}
		levelID = GF_DemoLevels[DemoLevelID++];
	}
	else {
		DemoLevelID = levelID;
	}

	START_INFO* start = &SaveGame.start[levelID];
	START_INFO startBackup = *start;
	start->available = 1;
	start->pistolAmmo = 1000;
	start->gunStatus = LGS_Armless;
	start->gunType = LGT_Pistols;
	SeedRandomDraw(RANDOM_SEED);
	SeedRandomControl(RANDOM_SEED);
	IsTitleLoaded = FALSE;

	if (!InitialiseLevel(levelID, GFL_DEMO)) {
		return GF_EXIT_GAME;
	}

	IsLevelComplete = FALSE;

	if (!IsDemoLoaded) {
		char str[64];
		sprintf(str, "Level '%s' has no demo data!", GF_LevelFilesStringTable[levelID]);
		S_ExitSystem(str);
	}

	LoadLaraDemoPos();
	LaraCheatGetStuff();
	SeedRandomDraw(RANDOM_SEED);
	SeedRandomControl(RANDOM_SEED);

#ifdef FEATURE_HUD_IMPROVED
	TEXT_STR_INFO* bottomText = NULL;
	TEXT_STR_INFO* topText = NULL;
	if (DemoTextMode == 1) {
		bottomText = T_Print(0, -16, 0, GF_SpecificStringTable[SSI_DemoMode]);
	}
	else if (DemoTextMode == 2) {
		bottomText = T_Print(0, -16, 0, "Press any button to quit");
		topText = T_Print(16, PsxBarPosEnabled ? 26 : 32, 0, "DEMO MODE");
	}
#else // FEATURE_HUD_IMPROVED
	// NOTE: here was the bug in the original game, wrong y coordinate and wrong align
	TEXT_STR_INFO* bottomText = T_Print(0, -16, 0, GF_SpecificStringTable[SSI_DemoMode]);
#endif // FEATURE_HUD_IMPROVED
	T_FlashText(bottomText, 1, 20);
	T_BottomAlign(bottomText, 1);
	T_CentreH(bottomText, 1);

	InvDemoMode = TRUE;
	int result = GameLoop(1);
	InvDemoMode = FALSE;

#ifdef FEATURE_HUD_IMPROVED
	T_RemovePrint(topText);
#endif // FEATURE_HUD_IMPROVED
	T_RemovePrint(bottomText);
	S_FadeToBlack();

	*start = startBackup;
	return result;
}

void LoadLaraDemoPos() {
	short roomID;
	LaraItem->pos.x = ((int*)DemoPtr)[0];
	LaraItem->pos.y = ((int*)DemoPtr)[1];
	LaraItem->pos.z = ((int*)DemoPtr)[2];
	LaraItem->pos.rotX = ((int*)DemoPtr)[3];
	LaraItem->pos.rotY = ((int*)DemoPtr)[4];
	LaraItem->pos.rotZ = ((int*)DemoPtr)[5];
	roomID = ((int*)DemoPtr)[6];
	if (LaraItem->roomNumber != roomID)
		ItemNewRoom(Lara.item_number, roomID);
	LaraItem->floor = GetHeight(GetFloor(LaraItem->pos.x, LaraItem->pos.y, LaraItem->pos.z, &roomID), LaraItem->pos.x, LaraItem->pos.y, LaraItem->pos.z);
	Lara.last_gun_type = ((int*)DemoPtr)[7];
	DemoCount += 8;
}

void GetDemoInput() {
	DWORD input;

	if (DemoCount < 9000) {
		input = ((DWORD*)DemoPtr)[DemoCount];
	}
	else {
		input = 0xFFFFFFFF;
	}
	InputStatus = input;
	if (input != 0xFFFFFFFF)
		++DemoCount;
}

/*
 * Inject function
 */
void Inject_Demo() {
	INJECT(0x004168E0, DoDemoSequence);
	INJECT(0x00416940, StartDemo);
	INJECT(0x00416AF0, LoadLaraDemoPos);
	INJECT(0x00416BC0, GetDemoInput);
}