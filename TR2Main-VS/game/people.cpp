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
#include "game/people.h"
#include "3dsystem/phd_math.h"
#include "game/box.h"
#include "game/control.h"
#include "game/effects.h"
#include "game/items.h"
#include "game/larafire.h"
#include "game/sound.h"
#include "game/sphere.h"
#include "specific/game.h"
#include "global/vars.h"

#ifdef FEATURE_VIDEOFX_IMPROVED
extern DWORD AlphaBlendMode;
#endif // FEATURE_VIDEOFX_IMPROVED

#define PEOPLE_TARGET_DISTANCE 0x4000000
#define PEOPLE_HIT_CHANCE 0x2000
#define PEOPLE_TARGETING_SPEED 300

#define BANDIT_DIE_ANIM 14
#define BANDIT_STOP_SHOOTING 2
#define BANDIT_DAMAGE 8
#define BANDIT_DAMAGE_TO_OTHER 2

#define BANDIT2_DIE_ANIM 9
#define BANDIT2_DAMAGE 50
#define BANDIT2_DAMAGE_TO_OTHER 1

typedef enum
{
	BANDIT_STOP = 1,
	BANDIT_WALK,
	BANDIT_RUN,
	BANDIT_AIM1,
	BANDIT_SHOOT1,
	BANDIT_AIM2,
	BANDIT_SHOOT2,
	BANDIT_SHOOT3A,
	BANDIT_SHOOT3B,
	BANDIT_SHOOT4A,
	BANDIT_AIM3,
	BANDIT_AIM4,
	BANDIT_DEATH,
	BANDIT_SHOOT4B
} BANDIT_STATE;

typedef enum
{
	BANDIT2_AIM4 = 1,
	BANDIT2_WAIT,
	BANDIT2_WALK,
	BANDIT2_RUN,
	BANDIT2_AIM1,
	BANDIT2_AIM2,
	BANDIT2_SHOOT1,
	BANDIT2_SHOOT2,
	BANDIT2_SHOOT4A,
	BANDIT2_SHOOT4B,
	BANDIT2_DEATH,
	BANDIT2_AIM5,
	BANDIT2_SHOOT5
} BANDIT2_STATE;

static const BITE_INFO BanditGunBite = { -2, 150, 19, 17 };
static const BITE_INFO Bandit2GunBite = { -1, 230, 9, 17 };

BOOL Targetable(ITEM_INFO* item, AI_INFO* info) {
	GAME_VECTOR source = {}, destination = {};
	CREATURE_INFO* creature = NULL;
	ITEM_INFO* enemy = NULL;

	creature = GetCreatureInfo(item);
	enemy = creature->enemy;
	if (enemy->hitPoints > 0 && info->ahead && info->distance < SQR(8192)) {
		source.x = item->pos.x;
		source.y = item->pos.y - 768;
		source.z = item->pos.z;
		source.roomNumber = item->roomNumber;
		destination.x = enemy->pos.x;
		destination.y = enemy->pos.y - 768;
		destination.z = enemy->pos.z;
		return LOS(&source, &destination);
	}
	return FALSE;
}

BOOL Targetable2(ITEM_INFO* item, AI_INFO* info, int srcHeight, int targetHeight, int range) {
	GAME_VECTOR source = {}, destination = {};
	CREATURE_INFO* creature = NULL;
	ITEM_INFO* enemy = NULL;

	creature = GetCreatureInfo(item);
	enemy = creature->enemy;
	if (enemy->hitPoints > 0 && info->ahead && info->distance < SQR(range)) {
		source.x = item->pos.x;
		source.y = item->pos.y - srcHeight;
		source.z = item->pos.z;
		source.roomNumber = item->roomNumber;
		destination.x = enemy->pos.x;
		destination.y = enemy->pos.y - targetHeight;
		destination.z = enemy->pos.z;
		return LOS(&source, &destination);
	}
	return FALSE;
}

short GunShot(int x, int y, int z, short speed, short rotY, short roomNumber) {
#ifdef FEATURE_VIDEOFX_IMPROVED
	if (AlphaBlendMode) {
		short fx_id = CreateEffect(roomNumber);
		if (fx_id >= 0) {
			FX_INFO* fx = &Effects[fx_id];
			fx->pos.x = x;
			fx->pos.y = y;
			fx->pos.z = z;
			fx->roomNumber = roomNumber;
			fx->counter = 4;
			fx->speed = 0x400;
			fx->frameNumber = 0x200; // this is sprite scale
			fx->fallspeed = 0;
			fx->objectID = ID_GLOW;
			fx->shade = 0x800;
			// NOTE: Core's hacky way to store the sprite flags in the rotation fields
			DWORD flags = GLOW_GUNSHOT_COLOR | SPR_BLEND_ADD | SPR_TINT | SPR_SHADE | SPR_SCALE | SPR_SEMITRANS | SPR_ABS;
			fx->pos.rotX = (UINT16)flags;
			fx->pos.rotY = (UINT16)(flags >> 16);
		}
	}
#endif // FEATURE_VIDEOFX_IMPROVED
	short fx_id = CreateEffect(roomNumber);
	if (fx_id >= 0) {
		FX_INFO* fx = &Effects[fx_id];
		fx->pos.x = x;
		fx->pos.y = y;
		fx->pos.z = z;
		fx->roomNumber = roomNumber;
		fx->pos.rotX = 0;
		fx->pos.rotY = rotY;
		fx->pos.rotZ = 0;
		fx->counter = 3;
		fx->frameNumber = 0;
		fx->objectID = ID_GUN_FLASH;
		fx->shade = 0x1000;
	}
	return fx_id;
}

short GunHit(int x, int y, int z, short speed, short rotY, short roomNumber) {
	PHD_VECTOR pos = { 0, 0, 0 };
	GetJointAbsPosition(LaraItem, &pos, GetRandomControl() * 25 / 0x7FFF);
#ifdef FEATURE_CHEAT
	if (Lara.water_status == LWS_Cheat) {
		short fxID = CreateEffect(roomNumber);
		if (fxID >= 0) {
			FX_INFO* fx = &Effects[fxID];
			fx->pos.x = pos.x;
			fx->pos.y = pos.y;
			fx->pos.z = pos.z;
			fx->counter = 4;
			fx->objectID = ID_RICOCHET;
			fx->pos.rotY = LaraItem->pos.rotY;
			fx->speed = LaraItem->speed;
			fx->frameNumber = -3 * GetRandomDraw() / 0x8000;
		}
		PlaySoundEffect(10, &LaraItem->pos, 0);
	}
	else {
		DoBloodSplat(pos.x, pos.y, pos.z, LaraItem->speed, LaraItem->pos.rotY, LaraItem->roomNumber);
		PlaySoundEffect(50, &LaraItem->pos, 0);
	}
#else // FEATURE_CHEAT
	DoBloodSplat(pos.x, pos.y, pos.z, LaraItem->speed, LaraItem->pos.rotY, LaraItem->roomNumber);
	PlaySoundEffect(50, &LaraItem->pos, 0);
#endif // FEATURE_CHEAT
	return GunShot(x, y, z, speed, rotY, roomNumber);
}

short GunMiss(int x, int y, int z, short speed, short rotY, short roomNumber) {
	GAME_VECTOR pos;
	pos.x = LaraItem->pos.x + (GetRandomDraw() - 0x4000) * 0x200 / 0x7FFF;
	pos.y = LaraItem->floor;
	pos.z = LaraItem->pos.z + (GetRandomDraw() - 0x4000) * 0x200 / 0x7FFF;
	pos.roomNumber = LaraItem->roomNumber;
	Richochet(&pos);
	return GunShot(x, y, z, speed, rotY, roomNumber);
}

BOOL ShotTargetNew(ITEM_INFO* item, AI_INFO* AI, const BITE_INFO* bite, short angle, int damageLara, int damageOther)
{
	CREATURE_INFO* creature = GetCreatureInfo(item);
	if (creature == NULL) return FALSE;
	ITEM_INFO* target = creature->enemy;
	if (target == NULL) return FALSE;
	GAME_VECTOR startPos{}, targetPos{};
	BOOL isHit = FALSE, isTargetable = FALSE;
	int distance = 0, random = 0;
	short fxIdx = -1, smashIdx = -1;


	if (AI->distance > PEOPLE_TARGET_DISTANCE || !Targetable(item, AI))
	{
		isHit = FALSE;
		isTargetable = FALSE;
	}
	else
	{
		distance = (phd_sin(AI->enemy_facing) * target->speed >> W2V_SHIFT) * PEOPLE_TARGET_DISTANCE / PEOPLE_TARGETING_SPEED;
		distance = AI->distance + SQR(distance);
		if (distance > PEOPLE_TARGET_DISTANCE)
		{
			isHit = FALSE;
		}
		else
		{
			random = (PEOPLE_TARGET_DISTANCE - AI->distance) / (PEOPLE_TARGET_DISTANCE / 0x5000) + PEOPLE_HIT_CHANCE;
			isHit = GetRandomControl() < random;
		}
		isTargetable = TRUE;
	}

	if (target == LaraItem)
	{
		if (isTargetable)
		{
			fxIdx = CreatureEffect(item, bite, GunHit);
			target->hitPoints -= damageLara;
			target->hitStatus = 1;
		}
		else if (isHit)
		{
			fxIdx = CreatureEffect(item, bite, GunMiss);
		}
	}
	else
	{
		if (isTargetable)
		{
			fxIdx = CreatureEffect(item, bite, GunShot);
			target->hitPoints -= damageOther;
			target->hitStatus = 1;
		}
		else if (isHit)
		{
			fxIdx = CreatureEffect(item, bite, GunMiss);
		}
	}

	if (fxIdx != -1)
		Effects[fxIdx].pos.rotY += angle;

	startPos.x = item->pos.x;
	startPos.y = item->pos.y - 768;
	startPos.z = item->pos.z;
	startPos.roomNumber = item->roomNumber;
	targetPos.x = target->pos.x;
	targetPos.y = target->pos.y - 768;
	targetPos.z = target->pos.z;
	smashIdx = ObjectOnLOS(&startPos, &targetPos);
	if (smashIdx != -1)
		SmashItem(smashIdx, 0);
	return isHit;
}

BOOL ShotTarget(ITEM_INFO* item, AI_INFO* AI, const BITE_INFO* bite, short angle, int damage)
{
	return ShotTargetNew(item, AI, bite, angle, damage, damage / 10);
}

void BanditControl(short itemID)
{
	if (!CreatureActive(itemID))
		return;

	ITEM_INFO* item = &Items[itemID];
	CREATURE_INFO* bandit = GetCreatureInfo(item);
	if (bandit == NULL) return; // NOTE: Not exist in the original code.
	AI_INFO AI{};
	short angle = 0, head = 0, tilt = 0, neck = 0;

	if (item->hitPoints <= 0)
	{
		if (item->currentAnimState != BANDIT_DEATH)
		{
			item->animNumber = Objects[item->objectID].animIndex + BANDIT_DIE_ANIM;
			item->frameNumber = Anims[item->animNumber].frameBase;
			item->currentAnimState = BANDIT_DEATH;
			item->goalAnimState = BANDIT_DEATH;
		}
	}
	else
	{
		CreatureAIInfo(item, &AI);
		CreatureMood(item, &AI, FALSE);
		angle = CreatureTurn(item, bandit->maximumTurn);

		switch (item->currentAnimState)
		{
		case BANDIT_STOP:
			bandit->flags = 0;
			bandit->maximumTurn = 0;
			if (AI.ahead)
				neck = AI.angle;
			if (bandit->mood == MOOD_ESCAPE)
			{
				item->goalAnimState = BANDIT_RUN;
			}
			else if (Targetable(item, &AI))
			{
				if (AI.distance > 0x400000)
				{
					item->goalAnimState = BANDIT_WALK;
				}
				else if (GetRandomControl() >= 0x2000)
				{
					if (GetRandomControl() >= 0x4000)
						item->goalAnimState = BANDIT_AIM3;
					else
						item->goalAnimState = BANDIT_SHOOT2;
				}
				else
				{
					item->goalAnimState = BANDIT_SHOOT1;
				}
			}
			else
			{
				if (bandit->mood != MOOD_BORED)
					item->goalAnimState = BANDIT_RUN;
				else if (!AI.ahead)
					item->goalAnimState = BANDIT_WALK;
				else
					item->goalAnimState = BANDIT_STOP;
			}
			break;
		case BANDIT_WALK:
			bandit->flags = 0;
			bandit->maximumTurn = 728;
			if (AI.ahead)
				neck = AI.angle;

			if (bandit->mood == MOOD_ESCAPE)
			{
				item->goalAnimState = BANDIT_RUN;
			}
			else if (Targetable(item, &AI))
			{
				if ((AI.distance <= 0x400000) || (AI.zone_number != AI.enemy_zone))
					item->goalAnimState = BANDIT_STOP;
				else
					item->goalAnimState = BANDIT_AIM4;
			}
			else if (bandit->mood != MOOD_BORED)
			{
				item->goalAnimState = BANDIT_RUN;
			}
			else if (AI.ahead)
			{
				item->goalAnimState = BANDIT_WALK;
			}
			else
			{
				item->goalAnimState = BANDIT_STOP;
			}
			break;
		case BANDIT_RUN:
			bandit->maximumTurn = 1092;
			if (AI.ahead)
				neck = AI.angle;
			tilt = angle >> 2;

			if (bandit->mood != MOOD_ESCAPE)
			{
				if (Targetable(item, &AI))
					item->goalAnimState = BANDIT_STOP;
				else
					item->goalAnimState = BANDIT_WALK;
			}
			break;
		case BANDIT_SHOOT2:
			if (AI.ahead)
				head = AI.angle;

			if (item->frameNumber == Anims[item->animNumber].frameBase + 3)
			{
				if (!ShotTargetNew(item, &AI, &BanditGunBite, head, BANDIT_DAMAGE, BANDIT_DAMAGE_TO_OTHER))
					item->goalAnimState = BANDIT_STOP;
			}
			break;
		case BANDIT_SHOOT1:
		case BANDIT_SHOOT3A:
		case BANDIT_SHOOT3B:
			if (AI.ahead)
				head = AI.angle;

			if (item->frameNumber == Anims[item->animNumber].frameBase)
			{
				if (!ShotTargetNew(item, &AI, &BanditGunBite, head, BANDIT_DAMAGE, BANDIT_DAMAGE_TO_OTHER))
					item->goalAnimState = BANDIT_STOP;
			}
			break;
		case BANDIT_SHOOT4A:
		case BANDIT_SHOOT4B:
			if (AI.ahead)
				head = AI.angle;

			if (item->frameNumber == Anims[item->animNumber].frameBase)
			{
				if (!ShotTargetNew(item, &AI, &BanditGunBite, head, BANDIT_DAMAGE, BANDIT_DAMAGE_TO_OTHER))
					item->goalAnimState = BANDIT_WALK;
			}
			break;

		}
	}

	CreatureTilt(item, tilt);
	CreatureHead(item, head);
	CreatureNeck(item, neck);
	CreatureAnimation(itemID, angle, 0);
}

void Bandit2Control(short itemID)
{
	if (!CreatureActive(itemID))
		return;

	ITEM_INFO* item = &Items[itemID];
	CREATURE_INFO* bandit = GetCreatureInfo(item);
	if (bandit == NULL) return; // NOTE: Not exist in the original code.
	AI_INFO AI{};
	short angle = 0, head = 0, tilt = 0, neck = 0;

	if (item->hitPoints <= 0)
	{
		if (item->currentAnimState != BANDIT2_DEATH)
		{
			item->animNumber = Objects[item->objectID].animIndex + BANDIT2_DIE_ANIM;
			item->frameNumber = Anims[item->animNumber].frameBase;
			item->currentAnimState = BANDIT2_DEATH;
			item->goalAnimState = BANDIT2_DEATH;
		}
	}
	else
	{
		CreatureAIInfo(item, &AI);
		CreatureMood(item, &AI, TRUE);
		angle = CreatureTurn(item, bandit->maximumTurn);

		switch (item->currentAnimState)
		{
		case BANDIT2_WAIT:
			bandit->maximumTurn = 0;
			if (AI.ahead)
				neck = AI.angle;

			if (bandit->mood == MOOD_ESCAPE)
			{
				item->goalAnimState = BANDIT2_RUN;
			}
			else if (Targetable(item, &AI))
			{
				if (AI.distance <= 0x400000 || GetRandomControl() >= 0x4000)
				{
					if (GetRandomControl() >= 0x2000)
					{
						if (GetRandomControl() >= 20480)
							item->goalAnimState = BANDIT2_AIM5;
						else
							item->goalAnimState = BANDIT2_SHOOT2;
					}
					else
					{
						item->goalAnimState = BANDIT2_SHOOT1;
					}
				}
				else
				{
					item->goalAnimState = BANDIT2_WALK;
				}
			}
			else if (bandit->mood != MOOD_BORED)
				item->goalAnimState = BANDIT2_WALK;
			else if (!AI.ahead || GetRandomControl() < 256)
				item->goalAnimState = BANDIT2_WALK;
			break;
		case BANDIT2_WALK:
			bandit->maximumTurn = 728;
			if (AI.ahead)
				neck = AI.angle;

			if (bandit->mood == MOOD_ESCAPE)
			{
				item->goalAnimState = BANDIT2_RUN;
			}
			else if (Targetable(item, &AI))
			{
				if ((AI.distance < 0x400000) || (GetRandomControl() < 1024))
					item->goalAnimState = BANDIT2_WAIT;
				else
					item->goalAnimState = BANDIT2_AIM4;
			}
			else if (bandit->mood != MOOD_BORED)
			{
				item->goalAnimState = BANDIT2_RUN;
			}
			else if (AI.ahead && GetRandomControl() < 1024)
			{
				item->goalAnimState = BANDIT2_WAIT;
			}
			break;
		case BANDIT2_RUN:
			bandit->maximumTurn = 1092;
			if (AI.ahead)
				neck = AI.angle;
			tilt = angle >> 2;

			if (bandit->mood != MOOD_ESCAPE && (bandit->mood == MOOD_BORED || Targetable(item, &AI)))
				item->goalAnimState = BANDIT2_WAIT;
			break;
		case BANDIT2_AIM4:
			bandit->flags = 0;
			if (AI.ahead)
				head = AI.angle;
			if (Targetable(item, &AI))
				item->goalAnimState = BANDIT2_SHOOT4A;
			else
				item->goalAnimState = BANDIT2_WALK;
			break;
		case BANDIT2_AIM1:
		case BANDIT2_AIM2:
			bandit->flags = 0;
			if (AI.ahead)
				head = AI.angle;

			if (Targetable(item, &AI))
				item->goalAnimState = item->currentAnimState == BANDIT2_AIM1 ? BANDIT2_SHOOT1 : BANDIT2_SHOOT2;
			else
				item->goalAnimState = BANDIT2_WAIT;
			break;
		case BANDIT2_AIM5:
			bandit->flags = 0;
			if (AI.ahead)
				head = AI.angle;

			if (Targetable(item, &AI))
				item->goalAnimState = BANDIT2_SHOOT5;
			else
				item->goalAnimState = BANDIT2_WAIT;
			break;
		case BANDIT2_SHOOT1:
		case BANDIT2_SHOOT2:
			if (AI.ahead)
				head = AI.angle;

			if (item->frameNumber == Anims[item->animNumber].frameBase + 3)
			{
				if (!ShotTargetNew(item, &AI, &Bandit2GunBite, head, BANDIT2_DAMAGE, BANDIT2_DAMAGE_TO_OTHER) || GetRandomControl() < 0x2000)
					item->goalAnimState = BANDIT2_WAIT;
			}
			break;
		case BANDIT2_SHOOT5:
			if (AI.ahead)
				head = AI.angle;

			if (item->frameNumber == Anims[item->animNumber].frameBase)
			{
				if (!ShotTargetNew(item, &AI, &Bandit2GunBite, head, BANDIT2_DAMAGE, BANDIT2_DAMAGE_TO_OTHER) || GetRandomControl() < 0x2000)
					item->goalAnimState = BANDIT2_WAIT;
			}
			break;
		case BANDIT2_SHOOT4A:
			if (AI.ahead)
				head = AI.angle;

			if (item->frameNumber == Anims[item->animNumber].frameBase + 1)
			{
				if (!ShotTargetNew(item, &AI, &Bandit2GunBite, head, BANDIT2_DAMAGE, BANDIT2_DAMAGE_TO_OTHER))
					item->goalAnimState = BANDIT2_WALK;
			}
			break;
		case BANDIT2_SHOOT4B:
			if (AI.ahead)
				head = AI.angle;

			if (item->frameNumber == Anims[item->animNumber].frameBase + 1)
			{
				if (!ShotTargetNew(item, &AI, &Bandit2GunBite, head, BANDIT2_DAMAGE, BANDIT2_DAMAGE_TO_OTHER))
					item->goalAnimState = BANDIT2_WALK;
			}
			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureHead(item, head);
	CreatureNeck(item, neck);
	CreatureAnimation(itemID, angle, 0);
}

void WinstonControl(short itemID) {
	if (!CreatureActive(itemID))
		return;

	ITEM_INFO* item = &Items[itemID];
	CREATURE_INFO* winston = (CREATURE_INFO*)item->data;
	AI_INFO info;
	CreatureAIInfo(item, &info);
	CreatureMood(item, &info, TRUE);
	short angle = CreatureTurn(item, winston->maximumTurn);
	if (item->currentAnimState == 1) {
		if ((info.distance > SQR(1536) || !info.ahead) && item->goalAnimState != 2) {
			item->goalAnimState = 2;
			PlaySoundEffect(345, &item->pos, 0);
		}
	}
	else {
		if (info.distance <= SQR(1536)) {
			if (info.ahead) {
				item->goalAnimState = 1;
				if (CHK_ANY(winston->flags, 1))
					--winston->flags;
			}
			else {
				if (!CHK_ANY(winston->flags, 1)) {
					PlaySoundEffect(344, &item->pos, 0);
					PlaySoundEffect(347, &item->pos, 0);
					winston->flags |= 1;
				}
			}
		}
	}
	if (item->touchBits && !CHK_ANY(winston->flags, 2)) {
		PlaySoundEffect(346, &item->pos, 0);
		PlaySoundEffect(347, &item->pos, 0);
		winston->flags |= 2;
	}
	else {
		if (!item->touchBits && CHK_ANY(winston->flags, 2))
			winston->flags -= 2;
	}
	if (GetRandomDraw() < 256)
		PlaySoundEffect(347, &item->pos, 0);
	CreatureAnimation(itemID, angle, 0);
}

/*
 * Inject function
 */
void Inject_People() {
	INJECT(0x00435EB0, Targetable);
	//INJECT(0x00435F40, ControlGlow);
	//INJECT(0x00435F80, ControlGunShot);
	INJECT(0x00435FD0, GunShot);
	INJECT(0x00436040, GunHit);
	INJECT(0x00436100, GunMiss);
	INJECT(0x004361B0, ShotTarget);
	//INJECT(0x00436380, InitialiseCult1);
	//INJECT(0x004363D0, Cult1Control);
	//INJECT(0x00436800, InitialiseCult3);
	//INJECT(0x00436850, Cult3Control);
	//INJECT(0x00436DC0, Worker1Control);
	//INJECT(0x004371C0, Worker2Control);
	INJECT(0x00437620, BanditControl);
	INJECT(0x00437960, Bandit2Control);
	INJECT(0x00437DA0, WinstonControl);
}