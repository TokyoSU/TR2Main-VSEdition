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
#include "game/setup.h"
#include "game/bear.h"
#include "game/boat.h"
#include "game/box.h"
#include "game/bird.h"
#include "game/cinema.h"
#include "game/collide.h"
#include "game/diver.h"
#include "game/dog.h"
#include "game/dragon.h"
#include "game/draw.h"
#include "game/eel.h"
#include "game/enemies.h"
#include "game/gameflow.h"
#include "game/items.h"
#include "game/invfunc.h"
#include "game/text.h"
#include "game/health.h"
#include "game/hair.h"
#include "game/lara1gun.h"
#include "game/laramisc.h"
#include "game/laraflare.h"
#include "game/moveblock.h"
#include "game/people.h"
#include "game/rat.h"
#include "game/shark.h"
#include "game/skidoo.h"
#include "game/spider.h"
#include "game/wolf.h"
#include "game/lot.h"
#include "game/objects.h"
#include "game/dragon.h"
#include "game/yeti.h"
#include "game/effects.h"
#include "game/traps.h"
#include "game/pickup.h"
#include "game/missile.h"
#include "game/sound.h"
#include "game/savegame.h"
#include "specific/sndpc.h"
#include "specific/file.h"
#include "specific/output.h"
#include "specific/init.h"
#include "specific/winmain.h"
#include "global/vars.h"

#if defined(FEATURE_MOD_CONFIG)
#include "modding/mod_utils.h"
#endif

#if defined(FEATURE_GOLD)
extern bool IsGold();
#endif

BOOL InitialiseLevel(int levelIndex, GF_LEVEL_TYPE type)
{
	if (type != GFL_TITLE && type != GFL_CUTSCENE)
		CurrentLevel = levelIndex;

	IsDemoLevelType = type == GFL_DEMO;
	InitialiseGameFlags();
	Lara.item_number = -1;
	IsTitleLoaded = FALSE;

	BOOL result;
	if (type)
	{
		if (type == GFL_SAVED || type != GFL_CUTSCENE)
			result = S_LoadLevelFile(GF_LevelFilesStringTable[levelIndex], levelIndex, type);
		else
			result = S_LoadLevelFile(GF_CutsFilesStringTable[levelIndex], levelIndex, GFL_CUTSCENE);
	}
	else
	{
		result = S_LoadLevelFile(GF_TitleFilesStringTable[0], levelIndex, GFL_TITLE);
	}

	if (result)
	{
		if (Lara.item_number != -1)
			InitialiseLara(type);
		if (type == GFL_NORMAL || type == GFL_SAVED || type == GFL_DEMO)
			GetCarriedItems();
		InitialiseFXArray();
		InitialiseLOTarray();
		InitColours();
		T_InitPrint();
		InitialisePickUpDisplay();
		S_InitialiseScreen(type);
		HealthBarTimer = 100;
		SOUND_Stop();
		if (type == GFL_SAVED)
			ExtractSaveGameInfo();
		else if (type == GFL_NORMAL)
			GF_ModifyInventory(CurrentLevel, 0);

		if (Objects[ID_FINAL_LEVEL_COUNTER].loaded)
			InitialiseFinalLevel();

		if (type == GFL_NORMAL || type == GFL_SAVED || type == GFL_DEMO)
		{
			if (TrackIDs[0])
				S_CDPlay(TrackIDs[0], TRUE);
		}

		IsAssaultTimerActive = FALSE;
		IsAssaultTimerDisplay = FALSE;
		Camera.underwater = FALSE;

		result = TRUE;
	}
	return result;
}

void InitialiseLevelFlags() {
	memset(&SaveGame.statistics, 0, sizeof(STATISTICS_INFO));
}

void InitialiseObjects() {
	for (int i = 0; i < ID_NUMBER_OBJECTS; ++i) {
		OBJECT_INFO* obj = &Objects[i];
		obj->initialise = NULL;
		obj->collision = NULL;
		obj->control = NULL;
		obj->ceiling = NULL;
		obj->floor = NULL;
		obj->drawRoutine = DrawAnimatingItem;
		obj->hitPoints = HP_DONT_TARGET;
		obj->pivotLength = 0;
		obj->radius = 10;
		obj->shadowSize = 0;
		obj->intelligent = FALSE;
		obj->water_creature = FALSE;
		obj->save_position = FALSE;
		obj->save_hitpoints = FALSE;
		obj->save_flags = FALSE;
		obj->save_anim = FALSE;
	}
	BaddyObjects();
	TrapObjects();
	ObjectObjects();
	InitialiseHair();
}

void BaddyObjects() {
	OBJECT_INFO* obj = NULL;

	// Lara object is mandatory
	obj = &Objects[ID_LARA];
	obj->initialise = InitialiseLaraLoad;
	obj->shadowSize = 160;
	obj->hitPoints = 1000;
	obj->drawRoutine = DrawDummyItem;
	obj->save_position = 1;
	obj->save_hitpoints = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;
	obj->control = ControlLaraExtra;

	// Other objects are optional
	obj = &Objects[ID_DOG];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = DogControl;
		obj->shadowSize = 128;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.dog;
#else
		obj->hitPoints = 10;
#endif
		obj->pivotLength = 300;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->radius = 341;
		AnimBones[obj->boneIndex + 19 * 4] |= 0x08;
	}
	obj = &Objects[ID_MOUSE];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = MouseControl;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.mouse;
#else
		obj->hitPoints = 4;
#endif
		obj->pivotLength = 50;
		obj->radius = 102;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 3 * 4] |= 0x08;
	}
	obj = &Objects[ID_CULT1];
	if (obj->loaded) {
		obj->initialise = InitialiseCult1;
		obj->collision = CreatureCollision;
		obj->control = Cult1Control;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.cult1;
#else
		obj->hitPoints = 25;
#endif
		obj->pivotLength = 50;
		obj->radius = 102;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 0 * 4] |= 0x08;
	}
	obj = &Objects[ID_CULT1A];
	if (obj->loaded) {
		if (!Objects[ID_CULT1].loaded) {
			S_ExitSystem("FATAL: CULT1A requires CULT1"); // NOTE: there was a typo: CULT1B instead of CULT1A
		}
		obj->frameBase = Objects[ID_CULT1].frameBase;
		obj->animIndex = Objects[ID_CULT1].animIndex;
		obj->initialise = InitialiseCult1;
		obj->control = Cult1Control;
		obj->collision = CreatureCollision;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.cult1A;
#else
		obj->hitPoints = 25;
#endif
		obj->pivotLength = 50;
		obj->radius = 102;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 0 * 4] |= 0x08;
	}
	obj = &Objects[ID_CULT1B];
	if (obj->loaded) {
		if (!Objects[ID_CULT1].loaded) {
			S_ExitSystem("FATAL: CULT1B requires CULT1");
		}
		obj->frameBase = Objects[ID_CULT1].frameBase;
		obj->animIndex = Objects[ID_CULT1].animIndex;
		obj->initialise = InitialiseCult1;
		obj->collision = CreatureCollision;
		obj->control = Cult1Control;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.cult1B;
#else
		obj->hitPoints = 25;
#endif
		obj->pivotLength = 50;
		obj->radius = 102;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 0 * 4] |= 0x08;
	}
	obj = &Objects[ID_CULT2];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = Cult2Control;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.cult2;
#else
		obj->hitPoints = 60;
#endif
		obj->pivotLength = 50;
		obj->radius = 102;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 0 * 4] |= 0x08;
		AnimBones[obj->boneIndex + 8 * 4] |= 0x08;
	}
	obj = &Objects[ID_SHARK];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = SharkControl;
		obj->drawRoutine = DrawUnclippedItem;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.shark;
#else
		obj->hitPoints = 30;
#endif
		obj->pivotLength = 200;
		obj->radius = 341;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->water_creature = 1;
		AnimBones[obj->boneIndex + 9 * 4] |= 0x08;
	}
	obj = &Objects[ID_TIGER];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = TigerControl;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.tiger;
#else
		obj->hitPoints = 20;
#endif
		obj->pivotLength = 200;
		obj->radius = 341;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 21 * 4] |= 0x08;
	}
	obj = &Objects[ID_BARRACUDA];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = BaracuddaControl;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.barracuda;
#else
		obj->hitPoints = 12;
#endif
		obj->pivotLength = 200;
		obj->radius = 204;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->water_creature = 1;
		AnimBones[obj->boneIndex + 6 * 4] |= 0x08;
	}
	obj = &Objects[ID_SPIDER_or_WOLF];
	if (obj->loaded) {
#if defined(FEATURE_GOLD)
		obj->initialise = IsGold() ? InitialiseWolf : InitialiseCreature;
		obj->control = IsGold() ? WolfControl : SpiderControl;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = IsGold() ? Mod.enemyHealth.wolf : Mod.enemyHealth.smallSpider;
#else
		obj->hitPoints = IsGold() ? 10 : 5;
#endif
		obj->radius = IsGold() ? 341 : 102;
#else
		obj->control = SpiderControl;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.smallSpider;
#else
		obj->hitPoints = 5;
#endif
		obj->radius = 102;
#endif
		obj->collision = CreatureCollision;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}
	obj = &Objects[ID_BIG_SPIDER_or_BEAR];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
#if defined(FEATURE_GOLD)
		obj->control = IsGold() ? BearControl : BigSpiderControl;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = IsGold() ? Mod.enemyHealth.bear : Mod.enemyHealth.bigSpider;
#else
		obj->hitPoints = IsGold() ? 30 : 40;
#endif
#else
		obj->control = BigSpiderControl;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.bigSpider;
#else
		obj->hitPoints = 40;
#endif
#endif
		obj->shadowSize = 128;
		obj->radius = 341;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}
	obj = &Objects[ID_YETI];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = YetiControl;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.yeti;
#else
		obj->hitPoints = 30;
#endif
		obj->pivotLength = 100;
		obj->radius = 128;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 6 * 4] |= 0x08;
		AnimBones[obj->boneIndex + 14 * 4] |= 0x08;
	}
	obj = &Objects[ID_JELLY];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = JellyControl;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.jelly;
#else
		obj->hitPoints = 10;
#endif
		obj->shadowSize = 128;
		obj->radius = 102;
		obj->pivotLength = 0;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->water_creature = 1;
	}
	obj = &Objects[ID_DIVER];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = DiverControl;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.diver;
#else
		obj->hitPoints = 20;
#endif
		obj->shadowSize = 128;
		obj->radius = 341;
		obj->pivotLength = 50;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->water_creature = 1;
		AnimBones[obj->boneIndex + 10 * 4] |= 0x08;
		AnimBones[obj->boneIndex + 14 * 4] |= 0x10;
	}
	obj = &Objects[ID_WORKER1];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = Worker1Control;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.worker1;
#else
		obj->hitPoints = 25;
#endif
		obj->radius = 102;
		obj->pivotLength = 0;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 4 * 4] |= 0x08;
		AnimBones[obj->boneIndex + 13 * 4] |= 0x08;
	}
	obj = &Objects[ID_WORKER2];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = Worker2Control;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.worker2;
#else
		obj->hitPoints = 20;
#endif
		obj->pivotLength = 0;
		obj->radius = 102;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 4 * 4] |= 0x08;
		AnimBones[obj->boneIndex + 13 * 4] |= 0x08;
	}
	obj = &Objects[ID_WORKER3];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = Worker3Control;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.worker3;
#else
		obj->hitPoints = 27;
#endif
		obj->pivotLength = 0;
		obj->radius = 102;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 0 * 4] |= 0x08;
		AnimBones[obj->boneIndex + 4 * 4] |= 0x08;
	}
	obj = &Objects[ID_WORKER4];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = Worker3Control;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.worker4;
#else
		obj->hitPoints = 27;
#endif
		obj->pivotLength = 0;
		obj->radius = 102;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 0 * 4] |= 0x08;
		AnimBones[obj->boneIndex + 4 * 4] |= 0x08;
	}
	obj = &Objects[ID_WORKER5];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = Worker2Control;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.worker5;
#else
		obj->hitPoints = 20;
#endif
		obj->radius = 102;
		obj->pivotLength = 0;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 4 * 4] |= 0x08;
		AnimBones[obj->boneIndex + 13 * 4] |= 0x08;
	}
	obj = &Objects[ID_CULT3];
	if (obj->loaded) {
		obj->initialise = InitialiseCult3;
		obj->control = Cult3Control;
		obj->collision = CreatureCollision;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.cult3;
#else
		obj->hitPoints = 150;
#endif
		obj->radius = 102;
		obj->pivotLength = 0;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}
	obj = &Objects[ID_MONK1];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->control = MonkControl;
		obj->collision = CreatureCollision;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.monk1;
#else
		obj->hitPoints = 30;
#endif
		obj->radius = 102;
		obj->pivotLength = 0;
#if defined(FEATURE_GOLD)
		obj->shadowSize = IsGold() ? 0 : 128;
#else
		obj->shadowSize = 128;
#endif
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 6 * 4] |= 0x08;
	}
	obj = &Objects[ID_MONK2];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->control = MonkControl;
		obj->collision = CreatureCollision;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.monk2;
#else
		obj->hitPoints = 30;
#endif
		obj->shadowSize = 128;
		obj->radius = 102;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 6 * 4] |= 0x08;
	}
	obj = &Objects[ID_EAGLE];
	if (obj->loaded) {
		obj->initialise = InitialiseBird;
		obj->control = BirdControl;
		obj->collision = CreatureCollision;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.eagle;
#else
		obj->hitPoints = 20;
#endif
		obj->pivotLength = 0;
		obj->radius = 204;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}
	obj = &Objects[ID_CROW];
	if (obj->loaded) {
		obj->initialise = InitialiseBird;
		obj->control = BirdControl;
		obj->collision = CreatureCollision;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.crow;
#else
		obj->hitPoints = 15;
#endif
		obj->radius = 204;
		obj->pivotLength = 0;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}
	obj = &Objects[ID_BIG_EEL];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = BigEelControl;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.bigEel;
#else
		obj->hitPoints = 20;
#endif
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->water_creature = 1;
	}
	obj = &Objects[ID_EEL];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = EelControl;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.eel;
#else
		obj->hitPoints = 5;
#endif
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->water_creature = 1;
	}
	obj = &Objects[ID_BANDIT1];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = BanditControl;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.bandit1;
#else
		obj->hitPoints = 45;
#endif
		obj->radius = 102;
		obj->pivotLength = 0;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 6 * 4] |= 0x08;
		AnimBones[obj->boneIndex + 8 * 4] |= 0x08;
	}
	obj = &Objects[ID_BANDIT2];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = Bandit2Control;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.bandit2;
#else
		obj->hitPoints = 50;
#endif
		obj->radius = 102;
		obj->pivotLength = 0;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 6 * 4] |= 0x08;
		AnimBones[obj->boneIndex + 8 * 4] |= 0x08;
	}
	obj = &Objects[ID_BANDIT2B];
	if (obj->loaded) {
		if (!Objects[ID_BANDIT2].loaded) {
			S_ExitSystem("FATAL: BANDIT2B requires BANDIT2");
		}
		obj->frameBase = Objects[ID_BANDIT2].frameBase;
		obj->animIndex = Objects[ID_BANDIT2].animIndex;
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = Bandit2Control;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.bandit2B;
#else
		obj->hitPoints = 50;
#endif
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->radius = 102;
		obj->pivotLength = 0;
		AnimBones[obj->boneIndex + 6 * 4] |= 0x08;
		AnimBones[obj->boneIndex + 8 * 4] |= 0x08;
	}
	obj = &Objects[ID_SKIDOO_ARMED];
	if (obj->loaded) {
		if (!Objects[ID_SKIDMAN].loaded) {
			S_ExitSystem("FATAL: SKIDOO_ARMED requires SKIDMAN, Also SKIDMAN need to be placed and triggered instead.");
		}
		obj->collision = SkidmanCollision;
		obj->drawRoutine = DrawSkidoo;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.skidman;
#else
		obj->hitPoints = 100;
#endif
		obj->radius = 341;
		obj->pivotLength = 0;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}
	obj = &Objects[ID_SKIDMAN];
	if (obj->loaded) {
		obj->initialise = InitialiseSkidman;
		obj->control = SkidManControl;
		obj->hitPoints = 1;
		obj->save_position = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}
	obj = &Objects[ID_XIAN_LORD];
	if (obj->loaded) {
		if (!Objects[ID_CHINESE2].loaded) {
			S_ExitSystem("FATAL: XianLord requires CHINESE2 (statue) mesh object.");
		}
		obj->initialise = InitialiseXianLord;
		obj->collision = CreatureCollision;
		obj->control = XianLordControl;
		obj->drawRoutine = DrawXianLord;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.xianLord;
#else
		obj->hitPoints = 100;
#endif
		obj->pivotLength = 0;
		obj->radius = 204;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 6 * 4] |= 0x08;
		AnimBones[obj->boneIndex + 12 * 4] |= 0x08;
	}
	obj = &Objects[ID_WARRIOR];
	if (obj->loaded) {
		if (!Objects[ID_CHINESE4].loaded) {
			S_ExitSystem("FATAL: Warrior requires CHINESE4 (statue) mesh object.");
		}
		obj->initialise = InitialiseXianLord;
		obj->collision = CreatureCollision;
		obj->control = WarriorControl;
		obj->drawRoutine = DrawXianLord;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.warrior;
#else
		obj->hitPoints = 80;
#endif
		obj->pivotLength = 0;
		obj->radius = 204;
		obj->shadowSize = 128;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 6 * 4] |= 0x08;
		AnimBones[obj->boneIndex + 16 * 4] |= 0x08;
	}
	obj = &Objects[ID_DRAGON_FRONT];
	if (obj->loaded) {
		//if (!Objects[ID_BARTOLI].loaded) {
		//	S_ExitSystem("FATAL: DRAGON_FRONT requires BARTOLI and DRAGON_BACK,\n- BARTOLI need to be placed and triggered instead.\n- DRAGON_BONE_FRONT and DRAGON_BONE_BACK is required, also SPHERE_OF_DOOM 1 to 3.");
		//}
		obj->initialise = InitialiseCreature;
		obj->collision = DragonCollision;
		obj->control = DragonControl;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.dragon;
#else
		obj->hitPoints = 300;
#endif
		obj->radius = 341;
		obj->pivotLength = 300;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 10 * 4] |= 0x10;
	}
	obj = &Objects[ID_DRAGON_BACK];
	if (obj->loaded) {
		//if (!Objects[ID_BARTOLI].loaded) {
		//	S_ExitSystem("FATAL: DRAGON_BACK requires BARTOLI and DRAGON_FRONT,\n- BARTOLI need to be placed and triggered instead.\n- DRAGON_BONE_FRONT and DRAGON_BONE_BACK is required, also SPHERE_OF_DOOM 1 to 3.");
		//}
		obj->control = DragonControl;
		obj->collision = DragonCollision;
		obj->radius = 341;
		obj->save_position = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}
	obj = &Objects[ID_BARTOLI];
	if (obj->loaded) {
		//if (!Objects[ID_DRAGON_BACK].loaded || !Objects[ID_DRAGON_FRONT].loaded) {
		//	S_ExitSystem("FATAL: BARTOLI requires DRAGON_BACK and DRAGON_FRONT,\n- BARTOLI need to be placed and triggered.\n- DRAGON_BONE_FRONT and DRAGON_BONE_BACK is required, also SPHERE_OF_DOOM 1 to 3.");
		//}
		obj->initialise = InitialiseBartoli;
		obj->control = BartoliControl;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}
	obj = &Objects[ID_GIANT_YETI];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = GiantYetiControl;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.giantYeti;
#else
		obj->hitPoints = 200;
#endif
		obj->radius = 341;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 14 * 4] |= 0x08;
	}
	obj = &Objects[ID_DINO];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = CreatureCollision;
		obj->control = DinoControl;
#if defined(FEATURE_MOD_CONFIG)
		obj->hitPoints = Mod.enemyHealth.dino;
#else
		obj->hitPoints = 100;
#endif
		obj->shadowSize = 64;
		obj->radius = 341;
		obj->pivotLength = 1800;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		AnimBones[obj->boneIndex + 10 * 4] |= 0x08;
		AnimBones[obj->boneIndex + 11 * 4] |= 0x08;
	}
	obj = &Objects[ID_WINSTON];
	if (obj->loaded) {
		obj->initialise = InitialiseCreature;
		obj->collision = ObjectCollision;
		obj->control = WinstonControl;
		obj->hitPoints = HP_DONT_TARGET;
		obj->shadowSize = 64;
		obj->radius = 102;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}
}

void TrapObjects()
{
	OBJECT_INFO* obj = NULL;

	obj = &Objects[ID_GONDOLA];
	obj->control = GondolaControl;
	obj->collision = ObjectCollision;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;

	obj = &Objects[ID_CEILING_SPIKES];
	obj->control = ControlCeilingSpikes;
	obj->collision = TrapCollision;
	obj->save_flags = TRUE;
	obj->save_position = TRUE;

	obj = &Objects[ID_COPTER];
	obj->control = CopterControl;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;

	obj = &Objects[ID_MINI_COPTER];
	obj->control = MiniCopterControl;
	obj->save_position = TRUE;
	obj->save_flags = TRUE;

	obj = &Objects[ID_HOOK];
	obj->control = HookControl;
	obj->collision = CreatureCollision;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;

	obj = &Objects[ID_GENERAL];
	obj->control = GeneralControl;
	obj->collision = ObjectCollision;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;
	obj->water_creature = TRUE;

	obj = &Objects[ID_DYING_MONK];
	obj->initialise = InitialiseDyingMonk;
	obj->control = DyingMonk;
	obj->collision = ObjectCollision;
	obj->save_flags = TRUE;

	obj = &Objects[ID_MINE];
	obj->control = MineControl;
	obj->collision = ObjectCollision;
	obj->save_flags = TRUE;

	obj = &Objects[ID_DEATH_SLIDE];
	obj->initialise = InitialiseRollingBall; // TODO: change it to have this defined function...
	obj->control = ControlDeathSlide;
	obj->collision = DeathSlideCollision;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;
	obj->save_position = TRUE;

	for (int i = ID_PROPELLER1; i <= ID_PROPELLER4; i++)
	{
		obj = &Objects[i];
		obj->control = PropellerControl;
		obj->collision = ObjectCollision;
		obj->save_anim = TRUE;
		obj->save_flags = TRUE;
	}

	obj = &Objects[ID_SPIKE_WALL];
	obj->control = ControlSpikeWall;
	obj->collision = ObjectCollision;
	obj->save_flags = TRUE;
	obj->save_position = TRUE;

	obj = &Objects[ID_SPINNING_BLADE];
	obj->initialise = InitialiseKillerStatue; // TODO: change it to have this defined function...
	obj->control = SpinningBlade;
	obj->collision = ObjectCollision;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;
	obj->save_position = TRUE;

	obj = &Objects[ID_KILLER_STATUE];
	obj->initialise = InitialiseKillerStatue;
	obj->control = KillerStatueControl;
	obj->collision = TrapCollision;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;

	for (int i = ID_FALLING_BLOCK1; i <= ID_FALLING_BLOCK3; i++)
	{
		obj = &Objects[i];
		obj->control = FallingBlock;
		obj->floor = FallingBlockFloor;
		obj->ceiling = FallingBlockCeiling;
		obj->save_anim = TRUE;
		obj->save_flags = TRUE;
		obj->save_position = TRUE;
	}

	obj = &Objects[ID_ICICLE];
	obj->control = IcicleControl;
	obj->collision = TrapCollision;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;
	obj->save_position = TRUE;

	obj = &Objects[ID_BLADE];
	obj->initialise = InitialiseBlade;
	obj->control = BladeControl;
	obj->collision = TrapCollision;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;

	obj = &Objects[ID_SPRING_BOARD];
	obj->control = SpringBoardControl;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;

	obj = &Objects[ID_SPIKES];
	obj->collision = SpikeCollision;

	obj = &Objects[ID_PENDULUM1];
	obj->control = Pendulum;
	obj->collision = ObjectCollision;
	obj->shadowSize = 128;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;

	obj = &Objects[ID_PENDULUM2];
	obj->control = Pendulum;
	obj->collision = ObjectCollision;
	obj->shadowSize = 128;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;

	obj = &Objects[ID_TEETH_TRAP];
	obj->collision = TrapCollision;
	obj->control = TeethTrap;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;

	for (int i = ID_MOVABLE_BLOCK1; i <= ID_MOVABLE_BLOCK4; i++)
	{
		obj = &Objects[i];
		obj->initialise = InitialiseMovingBlock;
		obj->control = MovableBlock;
		obj->collision = MovableBlockCollision;
		obj->drawRoutine = DrawMovableBlock;
		obj->save_anim = TRUE;
		obj->save_flags = TRUE;
		obj->save_position = TRUE;
	}

	obj = &Objects[ID_ROLLING_BALL1];
	obj->initialise = InitialiseRollingBall;
	obj->control = RollingBallControl;
	obj->collision = RollingBallCollision;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;
	obj->save_position = TRUE;

	obj = &Objects[ID_ROLLING_BALL2];
	obj->initialise = InitialiseRollingBall;
	obj->control = RollingBallControl;
	obj->collision = RollingBallCollision;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;
	obj->save_position = TRUE;

	obj = &Objects[ID_ROLLING_BALL3];
	obj->initialise = InitialiseRollingBall;
	obj->control = RollingBallControl;
	obj->collision = RollingBallCollision;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;
	obj->save_position = TRUE;

	obj = &Objects[ID_DART_EMITTER];
	obj->control = DartEmitterControl;
	obj->save_flags = TRUE;

	obj = &Objects[ID_FLAME_EMITTER];
	obj->control = FlameEmitterControl;
	obj->drawRoutine = DrawDummyItem;
	obj->save_flags = TRUE;
	
	obj = &Objects[ID_DART_EFFECT];
	obj->control = DartEffectControl;
	obj->drawRoutine = DrawSpriteItem;
#if defined(FEATURE_MOD_CONFIG)
	obj->semi_transparent = !Mod.isDartEffectOpaque;
#else
	obj->semi_transparent = TRUE;
#endif

	obj = &Objects[ID_FLAME];
	obj->control = FlameControl;
#if defined(FEATURE_MOD_CONFIG)
	obj->semi_transparent = !Mod.isFlameOpaque;
#else
	obj->semi_transparent = TRUE;
#endif

	obj = &Objects[ID_DARTS];
	obj->control = DartsControl;
	obj->collision = ObjectCollision;
	obj->shadowSize = 128;

	obj = &Objects[ID_LAVA_EMITTER];
	obj->control = LavaSpray;
	obj->drawRoutine = DrawDummyItem;
	obj->save_flags = TRUE;

	obj = &Objects[ID_LAVA];
	obj->control = ControlLavaBlob;
#if defined(FEATURE_MOD_CONFIG)
	obj->semi_transparent = !Mod.isLavaFountainOpaque;
#else
	obj->semi_transparent = TRUE;
#endif
}

void ObjectObjects()
{
	OBJECT_INFO* obj = NULL;

	obj = &Objects[ID_ROCKET];
	obj->control = ControlRocket;
	obj->save_position = TRUE;

	obj = &Objects[ID_HARPOON_BOLT];
	obj->control = ControlHarpoonBolt;
	obj->save_position = TRUE;

	obj = &Objects[ID_MISSILE_KNIFE];
	obj->control = ControlMissile;
	obj->save_position = TRUE;

	obj = &Objects[ID_MISSILE_HARPOON];
	obj->control = ControlMissile;
	obj->save_position = TRUE;

	obj = &Objects[ID_MISSILE_FLAME];
	obj->control = ControlMissile;
#if defined(FEATURE_MOD_CONFIG)
	obj->semi_transparent = !Mod.isDragonFlameOpaque;
#else
	obj->semi_transparent = TRUE;
#endif

	obj = &Objects[ID_SPHERE_OF_DOOM1];
	obj->collision = SphereOfDoomCollision;
	obj->control = SphereOfDoom;
	obj->drawRoutine = DrawSphereOfDoom;
	obj->save_position = TRUE;
	obj->save_flags = TRUE;
#if defined(FEATURE_MOD_CONFIG)
	obj->semi_transparent = !Mod.isSphereOfDoom1Opaque;
#else
	obj->semi_transparent = TRUE;
#endif

	obj = &Objects[ID_SPHERE_OF_DOOM2];
	obj->collision = SphereOfDoomCollision;
	obj->control = SphereOfDoom;
	obj->drawRoutine = DrawSphereOfDoom;
	obj->save_position = TRUE;
	obj->save_flags = TRUE;
#if defined(FEATURE_MOD_CONFIG)
	obj->semi_transparent = !Mod.isSphereOfDoom2Opaque;
#else
	obj->semi_transparent = TRUE;
#endif

	obj = &Objects[ID_SPHERE_OF_DOOM3];
	obj->collision = SphereOfDoomCollision;
	obj->control = SphereOfDoom;
	obj->drawRoutine = DrawSphereOfDoom;
	obj->save_position = TRUE;
	obj->save_flags = TRUE;

	obj = &Objects[ID_SKIDOO_FAST];
	obj->initialise = InitialiseSkidoo;
	obj->collision = SkidooCollision;
	obj->control = NULL; // Used in LaraControl() instead.
	obj->drawRoutine = DrawSkidoo;
	obj->save_position = TRUE;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;

	obj = &Objects[ID_BOAT];
	obj->initialise = InitialiseBoat;
	obj->collision = BoatCollision;
	obj->control = BoatControl;
	obj->save_position = TRUE;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;

	obj = &Objects[ID_BIG_BOWL];
	obj->control = BigBowlControl;
	obj->save_flags = TRUE;
	obj->save_anim = TRUE;

	obj = &Objects[ID_BELL];
	obj->control = BellControl;
	obj->collision = ObjectCollision;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;

	obj = &Objects[ID_CAMERA_TARGET];
	obj->drawRoutine = DrawDummyItem;

	obj = &Objects[ID_FLARE_ITEM];
	obj->control = FlareControl;
	obj->collision = PickUpCollision;
	obj->drawRoutine = DrawFlareInAir;
	obj->save_flags = TRUE;
	obj->save_position = TRUE;

	obj = &Objects[ID_WINDOW1];
	obj->initialise = InitialiseWindow;
	obj->collision = ObjectCollision;
	obj->control = WindowControl;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;

	obj = &Objects[ID_WINDOW2];
	obj->initialise = InitialiseWindow;
	obj->collision = ObjectCollision;
	obj->control = SmashIceControl;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;

	// NOTE: WINDOW3 and WINDOW4 was not used, now its like that: 3 is breakable (like 1), 4 with jump (like 2).
	obj = &Objects[ID_WINDOW3];
	obj->initialise = InitialiseWindow;
	obj->collision = ObjectCollision;
	obj->control = WindowControl;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;

	obj = &Objects[ID_WINDOW4];
	obj->initialise = InitialiseWindow;
	obj->collision = ObjectCollision;
	obj->control = SmashIceControl;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;

	obj = &Objects[ID_LIFT];
	obj->initialise = InitialiseLift;
	obj->control = LiftControl;
	obj->floor = LiftFloor;
	obj->ceiling = LiftCeiling;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;
	obj->save_position = TRUE;

	obj = &Objects[ID_BRIDGE_FLAT];
	obj->floor = BridgeFlatFloor;
	obj->ceiling = BridgeFlatCeiling;

	obj = &Objects[ID_BRIDGE_TILT1];
	obj->floor = BridgeTilt1Floor;
	obj->ceiling = BridgeTilt1Ceiling;

	obj = &Objects[ID_BRIDGE_TILT2];
	obj->floor = BridgeTilt2Floor;
	obj->ceiling = BridgeTilt2Ceiling;

	obj = &Objects[ID_DRAW_BRIDGE];
	if (obj->loaded)
	{
		obj->control = GeneralControl;
		obj->collision = DrawBridgeCollision;
		obj->floor = DrawBridgeFloor;
		obj->ceiling = DrawBridgeCeiling;
		obj->save_anim = TRUE;
		obj->save_flags = TRUE;
	}

	for (int i = ID_SWITCH_TYPE1; i <= ID_SWITCH_TYPE2; i++)
	{
		obj = &Objects[i];
		obj->control = SwitchControl;
		obj->collision = SwitchCollision;
		obj->save_anim = TRUE;
		obj->save_flags = TRUE;
	}

	for (int i = ID_SWITCH_TYPE3; i <= ID_SWITCH_TYPE5; i++)
	{
		obj = &Objects[i];
		obj->control = SwitchControl;
		obj->collision = i == ID_SWITCH_TYPE5 ? SwitchCollision2 : SwitchCollision;
		obj->save_anim = TRUE;
		obj->save_flags = TRUE;
	}

	for (int i = ID_DOOR_TYPE1; i <= ID_DOOR_TYPE8; i++)
	{
		obj = &Objects[i];
		obj->initialise = InitialiseDoor;
		obj->control = DoorControl;
		obj->collision = DoorCollision;
		obj->drawRoutine = DrawUnclippedItem;
		obj->save_anim = TRUE;
		obj->save_flags = TRUE;
	}

	for (int i = ID_TRAPDOOR_TYPE1; i <= ID_TRAPDOOR_TYPE3; i++)
	{
		obj = &Objects[i];
		obj->control = TrapDoorControl;
		obj->floor = TrapDoorFloor;
		obj->ceiling = TrapDoorCeiling;
		obj->save_anim = TRUE;
		obj->save_flags = TRUE;
	}

	for (int i = ID_PISTOL_ITEM; i <= ID_FLARES_ITEM; i++)
	{
		obj = &Objects[i];
		obj->collision = PickUpCollision;
		obj->drawRoutine = DrawSpriteItem;
		obj->save_position = TRUE;
		obj->save_flags = TRUE;
	}

	for (int i = ID_PUZZLE_ITEM1; i <= ID_PUZZLE_ITEM4; i++)
	{
		obj = &Objects[i];
		obj->collision = PickUpCollision;
		obj->drawRoutine = DrawSpriteItem;
		obj->save_position = TRUE;
		obj->save_flags = TRUE;
	}

	for (int i = ID_SECRET1; i <= ID_SECRET3; i++)
	{
		obj = &Objects[i];
		obj->collision = PickUpCollision;
		obj->drawRoutine = DrawSpriteItem;
		obj->save_position = TRUE;
		obj->save_flags = TRUE;
	}

	for (int i = ID_KEY_ITEM1; i <= ID_KEY_ITEM4; i++)
	{
		obj = &Objects[i];
		obj->collision = PickUpCollision;
		obj->drawRoutine = DrawSpriteItem;
		obj->save_position = TRUE;
		obj->save_flags = TRUE;
	}

	for (int i = ID_PICKUP_ITEM1; i <= ID_PICKUP_ITEM2; i++)
	{
		obj = &Objects[i];
		obj->collision = PickUpCollision;
		obj->drawRoutine = DrawSpriteItem;
		obj->save_position = TRUE;
		obj->save_flags = TRUE;
	}

	for (int i = ID_KEY_HOLE1; i <= ID_KEY_HOLE4; i++)
	{
		obj = &Objects[i];
		obj->collision = KeyHoleCollision;
		obj->save_flags = TRUE;
	}

	for (int i = ID_PUZZLE_HOLE1; i <= ID_PUZZLE_HOLE4; i++)
	{
		obj = &Objects[i];
		obj->collision = PuzzleHoleCollision;
		obj->save_flags = TRUE;
	}

	for (int i = ID_PUZZLE_DONE1; i <= ID_PUZZLE_DONE4; i++)
	{
		obj = &Objects[i];
		obj->save_flags = TRUE;
	}

	for (int i = ID_DETONATOR1; i <= ID_DETONATOR2; i++)
	{
		obj = &Objects[i];
		obj->control = DetonatorControl;
		obj->collision = DetonatorCollision;
		obj->save_anim = TRUE;
		obj->save_flags = TRUE;
	}

	obj = &Objects[ID_BODY_PART];
	obj->loaded = TRUE;
	obj->nMeshes = 0;
	obj->control = ControlBodyPart;

	obj = &Objects[ID_ALARM_SOUND];
	obj->control = ControlAlarmSound;
	obj->save_flags = TRUE;

	obj = &Objects[ID_LARA_ALARM];
	obj->control = ControlLaraAlarm;
	obj->save_flags = TRUE;

	for (int i = ID_PLAYER1; i <= ID_PLAYER10; i++)
	{
		obj = &Objects[i];
		obj->initialise = InitialiseGenPlayer;
		obj->control = ControlCinematicPlayer;
		obj->hitPoints = 1;
	}

	obj = &Objects[ID_BLOOD];
	obj->control = ControlBlood1;
#if defined(FEATURE_MOD_CONFIG)
	obj->semi_transparent = !Mod.isBloodOpaque;
#else
	obj->semi_transparent = TRUE;
#endif

	obj = &Objects[ID_EXPLOSION];
	obj->control = ControlExplosion1;
#if defined(FEATURE_MOD_CONFIG)
	obj->semi_transparent = !Mod.isExplosionOpaque;
#else
	obj->semi_transparent = TRUE;
#endif

	obj = &Objects[ID_RICOCHET];
	obj->control = ControlRichochet1;

	obj = &Objects[ID_TWINKLE];
	obj->control = ControlTwinkle;
#if defined(FEATURE_MOD_CONFIG)
	obj->semi_transparent = !Mod.isTwinkleOpaque;
#else
	obj->semi_transparent = TRUE;
#endif

	obj = &Objects[ID_SPLASH];
	obj->control = ControlSplash1;
#if defined(FEATURE_MOD_CONFIG)
	obj->semi_transparent = !Mod.isSplashOpaque;
#else
	obj->semi_transparent = TRUE;
#endif

	obj = &Objects[ID_SNOW_SPRITE];
	obj->control = ControlSnowSprite;

	obj = &Objects[ID_WATER_SPRITE];
	obj->control = ControlWaterSprite;
#if defined(FEATURE_MOD_CONFIG)
	obj->semi_transparent = !Mod.isWaterSpriteOpaque;
#else
	obj->semi_transparent = TRUE;
#endif

	obj = &Objects[ID_WATERFALL];
	obj->control = WaterFall;
	obj->drawRoutine = DrawDummyItem;

	obj = &Objects[ID_GUN_FLASH];
	obj->control = ControlGunShot;

	obj = &Objects[ID_GLOW];
	obj->control = ControlGlow;

	obj = &Objects[ID_HOT_LIQUID];
	obj->control = ControlHotLiquid;
#if defined(FEATURE_MOD_CONFIG)
	obj->semi_transparent = !Mod.isHotLiquidOpaque;
#else
	obj->semi_transparent = TRUE;
#endif

	obj = &Objects[ID_BIRD_TWEETER1];
	obj->control = ControlBirdTweeter;
	obj->drawRoutine = DrawDummyItem;

	obj = &Objects[ID_BIRD_TWEETER2];
	obj->control = ControlBirdTweeter;
	obj->drawRoutine = DrawDummyItem;

	obj = &Objects[ID_DING_DONG];
	obj->control = ControlDingDong;
	obj->drawRoutine = DrawDummyItem;

	obj = &Objects[ID_CLOCK_CHIMES];
	obj->control = ControlClockChimes;
	obj->drawRoutine = DrawDummyItem;
	obj->save_flags = TRUE;

	obj = &Objects[ID_FINAL_LEVEL_COUNTER];
	obj->control = FinalLevelCounter;
	obj->drawRoutine = DrawDummyItem;
	obj->save_flags = TRUE;

	obj = &Objects[ID_CUT_SHOTGUN];
	obj->control = ControlCutShotgun;
	obj->save_anim = TRUE;
	obj->save_flags = TRUE;

	obj = &Objects[ID_EARTHQUAKE];
	obj->control = EarthQuake;
	obj->drawRoutine = DrawDummyItem;
	obj->save_flags = TRUE;
}

/*
 * Inject function
 */
void Inject_Setup() {
	INJECT(0x0043A330, InitialiseLevel);
	//INJECT(0x0043A490, InitialiseGameFlags);
	INJECT(0x0043A500, InitialiseLevelFlags);
	INJECT(0x0043A530, BaddyObjects);
	INJECT(0x0043B570, TrapObjects);
	INJECT(0x0043BB70, ObjectObjects);
	INJECT(0x0043C7C0, InitialiseObjects);
	//INJECT(0x0043C830, GetCarriedItems);
}