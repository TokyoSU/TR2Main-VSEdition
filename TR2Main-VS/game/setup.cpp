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
#include "game/box.h"
#include "game/bird.h"
#include "game/collide.h"
#include "game/diver.h"
#include "game/dog.h"
#include "game/dragon.h"
#include "game/draw.h"
#include "game/eel.h"
#include "game/enemies.h"
#include "game/hair.h"
#include "game/laramisc.h"
#include "game/moveblock.h"
#include "game/people.h"
#include "game/rat.h"
#include "game/shark.h"
#include "game/skidoo.h"
#include "game/spider.h"
#include "game/wolf.h"
#include "game/yeti.h"
#include "specific/winmain.h"
#include "global/vars.h"

#if defined(FEATURE_MOD_CONFIG)
#include "modding/mod_utils.h"
#endif

#if defined(FEATURE_GOLD)
extern bool IsGold();
#endif

void InitialiseLevelFlags() {
	memset(&SaveGame.statistics, 0, sizeof(STATISTICS_INFO));
}

void InitialiseObjects() {
	OBJECT_INFO* obj = NULL;
	for (int i = 0; i < ID_NUMBER_OBJECTS; ++i) {
		obj = &Objects[i];
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
		if (!Objects[ID_BARTOLI].loaded) {
			S_ExitSystem("FATAL: DRAGON_FRONT requires BARTOLI and DRAGON_BACK,\n- BARTOLI need to be placed and triggered instead.\n- DRAGON_BONE_FRONT and DRAGON_BONE_BACK is required, also SPHERE_OF_DOOM 1 to 3.");
		}
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
		if (!Objects[ID_BARTOLI].loaded) {
			S_ExitSystem("FATAL: DRAGON_BACK requires BARTOLI and DRAGON_FRONT,\n- BARTOLI need to be placed and triggered instead.\n- DRAGON_BONE_FRONT and DRAGON_BONE_BACK is required, also SPHERE_OF_DOOM 1 to 3.");
		}
		obj->control = DragonControl;
		obj->collision = DragonCollision;
		obj->radius = 341;
		obj->save_position = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}
	obj = &Objects[ID_BARTOLI];
	if (obj->loaded) {
		if (!Objects[ID_DRAGON_BACK].loaded || !Objects[ID_DRAGON_FRONT].loaded) {
			S_ExitSystem("FATAL: BARTOLI requires DRAGON_BACK and DRAGON_FRONT,\n- BARTOLI need to be placed and triggered.\n- DRAGON_BONE_FRONT and DRAGON_BONE_BACK is required, also SPHERE_OF_DOOM 1 to 3.");
		}
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

/*
 * Inject function
 */
void Inject_Setup() {
	//INJECT(0x0043A330, InitialiseLevel);
	//INJECT(0x0043A490, InitialiseGameFlags);
	INJECT(0x0043A500, InitialiseLevelFlags);
	INJECT(0x0043A530, BaddyObjects);
	//INJECT(0x0043B570, TrapObjects);
	//INJECT(0x0043BB70, ObjectObjects);
	INJECT(0x0043C7C0, InitialiseObjects);
	//INJECT(0x0043C830, GetCarriedItems);
}