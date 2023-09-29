/*
 * Copyright (c) 2017-2018 Michael Chaban. All rights reserved.
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
#include "game/spider.h"
#include "game/box.h"
#include "game/effects.h"
#include "game/lot.h"
#include "game/items.h"
#include "game/missile.h"
#include "game/sound.h"
#include "specific/game.h"
#include "global/vars.h"

#define SPIDER_JUMP_ANIM (2)
#define SPIDER_DAMAGE (25)

#define BIG_SPIDER_DEATH_ANIM (2)
#define BIG_SPIDER_DAMAGE (100)

typedef enum
{
	SPIDER_STOP = 1,
	SPIDER_WALK1,
	SPIDER_WALK2,
	SPIDER_ATTACK1,
	SPIDER_ATTACK2,
	SPIDER_ATTACK3,
	SPIDER_DEATH
} SPIDER_STATE;

static const BITE_INFO SpiderBite = { 0, 0, 41, 1 };

void SpiderLeap(short itemID, short angle)
{
	ITEM_INFO* item = &Items[itemID];
	PHD_3DPOS oldPos = item->pos;
	short oldRoomNumber = item->roomNumber;
	CreatureAnimation(itemID, angle, 0);
	if (item->pos.y <= oldPos.y - 384)
	{
		item->pos.x = oldPos.x;
		item->pos.y = oldPos.y;
		item->pos.z = oldPos.z;
		if (item->roomNumber != oldRoomNumber)
			ItemNewRoom(itemID, oldRoomNumber);
		item->animNumber = Objects[item->objectID].animIndex + SPIDER_JUMP_ANIM;
		item->frameNumber = Anims[item->animNumber].frameBase;
		item->currentAnimState = SPIDER_ATTACK2;
		item->goalAnimState = SPIDER_STOP;
		CreatureAnimation(itemID, angle, 0);
	}
}

void SpiderControl(short itemID)
{
	if (!CreatureActive(itemID))
		return;

	ITEM_INFO* item = &Items[itemID];
	CREATURE_INFO* spider = GetCreatureInfo(item);
	if (spider == NULL) return;
	AI_INFO AI{};
	short angle = 0;

	if (item->hitPoints <= 0)
	{
		if (ExplodingDeath(itemID, 0xFFFFFFFF, 0))
		{
			DisableBaddieAI(itemID);
			KillItem(itemID);
			item->status = ITEM_DISABLED;
			PlaySoundEffect(349, &item->pos, 0);
			return;
		}
	}
	else
	{
		CreatureAIInfo(item, &AI);
		CreatureMood(item, &AI, TRUE);
		angle = CreatureTurn(item, 1456);

		switch (item->currentAnimState)
		{
		case SPIDER_STOP:
			spider->flags = 0;

			if (spider->mood != MOOD_BORED)
			{
				if (!AI.ahead || item->touchBits == 0)
				{
					if (spider->mood != MOOD_STALK)
					{
						if (spider->mood == MOOD_ESCAPE || spider->mood == MOOD_ATTACK)
							item->goalAnimState = SPIDER_WALK2;
						else
							item->goalAnimState = SPIDER_WALK1;
					}
				}
				else
				{
					item->goalAnimState = SPIDER_ATTACK1;
				}
			}
			else if (GetRandomControl() < 256)
			{
				item->goalAnimState = SPIDER_WALK1;
			}
			break;
		case SPIDER_WALK1:
			if (spider->mood != MOOD_BORED)
			{
				if (spider->mood == MOOD_ESCAPE || spider->mood == MOOD_ATTACK)
					item->goalAnimState = SPIDER_WALK2;
				else
					item->goalAnimState = SPIDER_WALK1;
			}
			else if (GetRandomControl() < 256)
			{
				item->goalAnimState = SPIDER_STOP;
			}
			break;
		case SPIDER_WALK2:
			if (spider->mood != MOOD_BORED && spider->mood != MOOD_STALK && AI.ahead)
			{
				if (item->touchBits != 0)
					item->goalAnimState = SPIDER_STOP;
				else if (AI.distance < 0xA290)
					item->goalAnimState = SPIDER_ATTACK3;
				else if (GetRandomControl() < 0x1000 && AI.distance < 0x40000)
					item->goalAnimState = SPIDER_ATTACK2;
			}
			else
			{
				item->goalAnimState = SPIDER_WALK1;
			}
			break;
		case SPIDER_ATTACK1:
		case SPIDER_ATTACK2:
		case SPIDER_ATTACK3:
			if (!spider->flags && item->touchBits != 0)
			{
				CreatureEffect(item, &SpiderBite, DoBloodSplat);
				spider->enemy->hitPoints -= SPIDER_DAMAGE;
				spider->enemy->hitStatus = 1;
				spider->flags = 1;
			}
			break;
		}
	}

	SpiderLeap(itemID, angle);
}

void BigSpiderControl(short itemID)
{
	if (!CreatureActive(itemID))
		return;

	ITEM_INFO* item = &Items[itemID];
	CREATURE_INFO* spider = GetCreatureInfo(item);
	if (spider == NULL) return;
	AI_INFO AI{};
	short angle = 0;

	if (item->hitPoints <= 0)
	{
		if (item->currentAnimState != SPIDER_DEATH)
		{
			item->animNumber = Objects[item->objectID].animIndex + BIG_SPIDER_DEATH_ANIM;
			item->frameNumber = Anims[item->animNumber].frameBase;
			item->currentAnimState = SPIDER_DEATH;
			item->goalAnimState = SPIDER_DEATH;
		}
	}
	else
	{
		CreatureAIInfo(item, &AI);
		CreatureMood(item, &AI, TRUE);
		angle = CreatureTurn(item, 728);

		switch (item->currentAnimState)
		{
		case SPIDER_STOP:
			spider->flags = 0;
			if (spider->mood != MOOD_BORED)
			{
				if (AI.ahead && item->touchBits != 0)
				{
					item->goalAnimState = SPIDER_ATTACK1;
				}
				else if (spider->mood != MOOD_STALK)
				{
					if (spider->mood == MOOD_ESCAPE || spider->mood == MOOD_ATTACK)
						item->goalAnimState = SPIDER_WALK2;
				}
				else
				{
					item->goalAnimState = SPIDER_WALK1;
				}
			}
			break;
		case SPIDER_WALK1:
			if (spider->mood != MOOD_BORED)
			{
				if (AI.ahead && item->touchBits != 0)
					item->goalAnimState = SPIDER_STOP;
				else if (spider->mood == MOOD_ESCAPE || spider->mood == MOOD_ATTACK)
					item->goalAnimState = SPIDER_WALK2;
			}
			break;
		case SPIDER_WALK2:
			spider->flags = 0;

			if (AI.ahead && item->touchBits != 0)
				item->goalAnimState = SPIDER_STOP;
			else if (spider->mood == MOOD_BORED || spider->mood == MOOD_STALK)
				item->goalAnimState = SPIDER_WALK1;
			break;
		case SPIDER_ATTACK1:
			if (!spider->flags && item->touchBits != 0)
			{
				CreatureEffect(item, &SpiderBite, DoBloodSplat);
				spider->enemy->hitPoints -= BIG_SPIDER_DAMAGE;
				spider->enemy->hitStatus = 1;
				spider->flags = 1;
			}
			break;
		}
	}

	CreatureAnimation(itemID, angle, 0);
}

 /*
  * Inject function
  */
void Inject_Spider() {
	INJECT(0x00440070, SpiderLeap);
	INJECT(0x00440120, SpiderControl);
	INJECT(0x00440340, BigSpiderControl);
}
