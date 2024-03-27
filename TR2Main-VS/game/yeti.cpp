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
#include "game/yeti.h"
#include "game/box.h"
#include "game/effects.h"
#include "specific/game.h"
#include "modding/mod_utils.h"
#include "global/vars.h"

enum YETI_STATES
{
	YETI_RUN = 1,
	YETI_STOP,
	YETI_WALK,
	YETI_ATTACK1,
	YETI_ATTACK2,
	YETI_ATTACK3,
	YETI_WAIT1,
	YETI_DEATH,
	YETI_WAIT2,
	YETI_CLIMB2,
	YETI_CLIMB3,
	YETI_CLIMB4,
	YETI_FALL4,
	YETI_KILL
};

enum BIRDY_STATES
{
	BIRDY_WAIT1 = 1,
	BIRDY_WALK,
	BIRDY_AIM1, 
	BIRDY_PUNCH1,
	BIRDY_AIM2,
	BIRDY_PUNCH2,
	BIRDY_PUNCHR,
	BIRDY_WAIT2,
	BIRDY_DEATH, 
	BIRDY_AIM3,
	BIRDY_PUNCH3
};

#define YETI_DEATH_ANIM 31
#define YETI_KILL_ANIM 36
#define YETI_CLIMB2_ANIM 34
#define YETI_CLIMB3_ANIM 33
#define YETI_CLIMB4_ANIM 32
#define YETI_FALL4_ANIM 35
#define YETI_ATTACK1_DAMAGE 100
#define YETI_ATTACK2_DAMAGE 150
#define YETI_ATTACK3_DAMAGE 200
#define YETI_ATTACK1_DAMAGE_TO_OTHER 1
#define YETI_ATTACK2_DAMAGE_TO_OTHER 1
#define YETI_ATTACK3_DAMAGE_TO_OTHER 2
#define YETI_TOUCHR MESH_BITS(10)
#define YETI_TOUCHL MESH_BITS(13)
#define BIRDY_ATTACK1_RANGE SQR(WALL_SIZE)
#define BIRDY_ATTACK2_RANGE SQR(WALL_SIZE*2)
#define BIRDY_DAMAGE 200
#define BIRDY_DAMAGE_TO_OTHER 4
#define BIRDY_DEATH_ANIM 20
#define BIRDY_TOUCHL MESH_BITS(19)
#define BIRDY_TOUCHR MESH_BITS(22)

BITE_INFO YetiBiteHandR = { 12, 101, 19, 10 };
BITE_INFO YetiBiteHandL = { 12, 101, 19, 13 };
BITE_INFO BirdyBiteHandL = { 0, 224, 0, 19 };
BITE_INFO BirdyBiteHandR = { 0, 224, 0, 22 };

void GiantYetiControl(short itemID)
{
	if (!CreatureActive(itemID))
		return;

	ITEM_INFO* item = &Items[itemID];
	CREATURE_INFO* birdy = GetCreatureInfo(item);
	AI_INFO AI = {};
	short angle = 0, head = 0;

	if (item->hitPoints <= 0)
	{
		if (item->currentAnimState != BIRDY_DEATH)
			SetAnimation(item, BIRDY_DEATH_ANIM, BIRDY_DEATH);

		if (item->frameNumber == Anims[item->animNumber].frameEnd && !IsModDisableGiantYetiNextLevelOnDeath())
			IsLevelComplete = TRUE;
	}
	else
	{
		CreatureAIInfo(item, &AI);
		if (AI.ahead)
			head = AI.angle;
		CreatureMood(item, &AI, TRUE);
		angle = CreatureTurn(item, birdy->maximum_turn);

		switch (item->currentAnimState)
		{
		case BIRDY_WAIT1:
			birdy->maximum_turn = 0;
			if (AI.ahead && AI.distance < BIRDY_ATTACK1_RANGE)
			{
				if (GetRandomControl() >= 0x4000)
					item->goalAnimState = BIRDY_AIM3;
				else
					item->goalAnimState = BIRDY_AIM1;
			}
			else
			{
				if ((birdy->mood != MOOD_BORED && birdy->mood != MOOD_STALK) || !AI.ahead)
					item->goalAnimState = BIRDY_WALK;
				else
					item->goalAnimState = BIRDY_WAIT2;
			}
			break;
		case BIRDY_WAIT2:
			if (birdy->mood != MOOD_BORED || !AI.ahead)
				item->goalAnimState = BIRDY_WAIT1;
			break;
		case BIRDY_WALK:
			birdy->maximum_turn = 728;
			if (AI.ahead && AI.distance < BIRDY_ATTACK2_RANGE)
				item->goalAnimState = BIRDY_AIM2;
			else if ((birdy->mood == MOOD_BORED || birdy->mood == MOOD_STALK) && AI.ahead)
				item->goalAnimState = BIRDY_WAIT1;
			break;
		case BIRDY_AIM1:
			birdy->flags = 0;
			if (AI.ahead && AI.distance < BIRDY_ATTACK1_RANGE)
				item->goalAnimState = BIRDY_PUNCH1;
			else
				item->goalAnimState = BIRDY_WAIT1;
			break;
		case BIRDY_AIM2:
			birdy->flags = 0;
			if (AI.ahead && AI.distance < BIRDY_ATTACK2_RANGE)
				item->goalAnimState = BIRDY_PUNCH2;
			else
				item->goalAnimState = BIRDY_WALK;
			break;
		case BIRDY_AIM3:
			birdy->flags = 0;
			if (AI.ahead && AI.distance < BIRDY_ATTACK1_RANGE)
				item->goalAnimState = BIRDY_PUNCH3;
			else
				item->goalAnimState = BIRDY_WAIT1;
			break;
		case BIRDY_PUNCH1:
		case BIRDY_PUNCH2:
		case BIRDY_PUNCHR:
		case BIRDY_PUNCH3:
			if (birdy->enemy == LaraItem)
			{
				if (!(birdy->flags & 1) && CHK_ANY(item->touchBits, BIRDY_TOUCHL))
				{
					DamageTarget(item, birdy->enemy, &BirdyBiteHandL, BIRDY_DAMAGE);
					birdy->flags |= 1;
				}
				if (!(birdy->flags & 2) && CHK_ANY(item->touchBits, BIRDY_TOUCHR))
				{
					DamageTarget(item, birdy->enemy, &BirdyBiteHandR, BIRDY_DAMAGE);
					birdy->flags |= 2;
				}
			}
			else if (!(birdy->flags & 1) && IsCreatureNearTarget(item, birdy->enemy))
			{
				DamageTarget(item, birdy->enemy, item->currentAnimState == BIRDY_PUNCHR ? &BirdyBiteHandR : &BirdyBiteHandL, BIRDY_DAMAGE_TO_OTHER);
				birdy->flags |= 1;
			}
			break;
		}
	}

	CreatureHead(item, head);
	CreatureAnimation(itemID, angle, 0);
}

void YetiControl(short itemID)
{
	if (!CreatureActive(itemID))
		return;

	ITEM_INFO* item = &Items[itemID];
	CREATURE_INFO* yeti = GetCreatureInfo(item);
	OBJECT_INFO* obj = &Objects[item->objectID];
	AI_INFO AI = {};
	int random = 0;
	short head = 0, neck = 0, angle = 0;
	bool isTargetAlive = yeti->enemy != NULL && yeti->enemy->hitPoints > 0;

	if (item->hitPoints <= 0)
	{
		if (item->currentAnimState != YETI_DEATH)
			SetAnimation(item, YETI_DEATH_ANIM, YETI_DEATH);
	}
	else
	{
		CreatureAIInfo(item, &AI);
		if (AI.ahead)
			neck = AI.angle;
		CreatureMood(item, &AI, TRUE);
		angle = CreatureTurn(item, yeti->maximum_turn);

		switch (item->currentAnimState)
		{
		case YETI_STOP:
			yeti->flags = 0;
			yeti->maximum_turn = 0;

			if (yeti->mood == MOOD_ESCAPE)
			{
				item->goalAnimState = YETI_RUN;
				break;
			}

			random = GetRandomControl();
			if (item->requiredAnimState != 0)
			{
				item->goalAnimState = item->requiredAnimState;
			}
			else if (yeti->mood != MOOD_BORED)
			{
				if (AI.ahead && AI.distance < 0x40000 && GetRandomControl() < 0x4000)
					item->goalAnimState = YETI_ATTACK1;
				else if (AI.ahead && AI.distance < 0x718E4)
					item->goalAnimState = YETI_ATTACK2;
				else if (yeti->mood == MOOD_STALK)
					item->goalAnimState = YETI_WALK;
				else
					item->goalAnimState = YETI_RUN;
			}
			else if (random < 256 || !isTargetAlive)
				item->goalAnimState = YETI_WAIT1;
			else if (random >= 512 && random < 768)
				item->goalAnimState = YETI_WAIT2;
			else
				item->goalAnimState = YETI_WALK;
			break;
		case YETI_WAIT1:
			if (yeti->mood == MOOD_ESCAPE || item->hitStatus)
			{
				item->goalAnimState = YETI_STOP;
				break;
			}

			random = GetRandomControl();
			if (yeti->mood != MOOD_BORED && GetRandomControl() < 512)
			{
				item->goalAnimState = YETI_STOP;
			}
			else if (random < 256 || !isTargetAlive)
			{
				item->goalAnimState = YETI_WAIT2;
			}
			else if (random >= 512 && random < 768)
			{
				item->goalAnimState = YETI_STOP;
				item->requiredAnimState = YETI_WALK;
			}
			else
			{
				item->goalAnimState = YETI_STOP;
			}
			break;
		case YETI_WAIT2:
			if (yeti->mood == MOOD_ESCAPE || item->hitStatus)
			{
				item->goalAnimState = YETI_STOP;
				break;
			}

			random = GetRandomControl();
			if (yeti->mood != MOOD_BORED && GetRandomControl() < 512)
			{
				item->goalAnimState = YETI_STOP;
			}
			else if (random < 256 || !isTargetAlive)
			{
				item->goalAnimState = YETI_WAIT1;
			}
			else if (random >= 512 && random < 768)
			{
				item->goalAnimState = YETI_STOP;
				item->requiredAnimState = YETI_WALK;
			}
			else
			{
				item->goalAnimState = YETI_STOP;
			}
			break;
		case YETI_WALK:
			yeti->maximum_turn = 728;

			random = GetRandomControl();
			if (yeti->mood == MOOD_ESCAPE)
			{
				item->goalAnimState = YETI_RUN;
			}
			else if (yeti->mood != MOOD_BORED)
			{
				if (yeti->mood == MOOD_ATTACK)
				{
					if (AI.ahead && AI.distance < 0x718E4)
						item->goalAnimState = YETI_STOP;
					else if (AI.distance > 0x400000)
						item->goalAnimState = YETI_RUN;
				}
			}
			else if (random < 256 || !isTargetAlive)
			{
				item->goalAnimState = YETI_STOP;
				item->requiredAnimState = YETI_WAIT1;
			}
			else if (random >= 512 && random < 768)
			{
				item->goalAnimState = YETI_STOP;
				item->requiredAnimState = YETI_WAIT2;
			}
			else
			{
				item->goalAnimState = YETI_STOP;
			}
			break;
		case YETI_RUN:
			yeti->flags = 0;
			yeti->maximum_turn = 1092;

			if (yeti->mood == MOOD_ESCAPE)
				break;

			if (yeti->mood != MOOD_BORED)
			{
				if (AI.ahead && AI.distance < 0x718E4)
					item->goalAnimState = YETI_STOP;
				else if (AI.ahead && AI.distance < 0x400000)
					item->goalAnimState = YETI_ATTACK3;
				else if (yeti->mood == MOOD_STALK)
					item->goalAnimState = YETI_WALK;
			}
			else
			{
				item->goalAnimState = YETI_WALK;
			}

			break;
		case YETI_ATTACK1:
			head = neck;
			neck = 0;

			if (yeti->enemy == LaraItem)
			{
				if (!(yeti->flags & 2) && CHK_ANY(item->touchBits, YETI_TOUCHR))
				{
					DamageTarget(item, yeti->enemy, &YetiBiteHandR, YETI_ATTACK1_DAMAGE);
					yeti->flags |= 2;
				}
			}
			else if (!(yeti->flags & 1) && IsCreatureNearTarget(item, yeti->enemy))
			{
				DamageTarget(item, yeti->enemy, &YetiBiteHandL, YETI_ATTACK1_DAMAGE_TO_OTHER);
				yeti->flags |= 1;
			}
			break;
		case YETI_ATTACK2:
			head = neck;
			neck = 0;

			if (yeti->enemy == LaraItem)
			{
				// NOTE: This animation deal damage with two hand, doing /2 will just redo the old damage she take.
				// The original game didn't damage lara with both hand separated, it checked for both hand at the same time, dealing YETI_ATTACK2_DAMAGE 1 time only (full damage).
				// Now it check for both and deal only damage if the left or right hand touch the target (or both).

				if (!(yeti->flags & 1) && CHK_ANY(item->touchBits, YETI_TOUCHL))
				{
					DamageTarget(item, yeti->enemy, &YetiBiteHandL, YETI_ATTACK2_DAMAGE/2);
					yeti->flags |= 1;
				}

				if (!(yeti->flags & 2) && CHK_ANY(item->touchBits, YETI_TOUCHR))
				{
					DamageTarget(item, yeti->enemy, &YetiBiteHandR, YETI_ATTACK2_DAMAGE/2);
					yeti->flags |= 2;
				}
			}
			else if (!(yeti->flags & 1) && IsCreatureNearTarget(item, yeti->enemy))
			{
				DamageTarget(item, yeti->enemy, &YetiBiteHandL, YETI_ATTACK2_DAMAGE_TO_OTHER);
				yeti->flags |= 1;
			}
			break;
		case YETI_ATTACK3:
			head = neck;
			neck = 0;

			if (yeti->enemy == LaraItem)
			{
				if (!(yeti->flags & 1) && CHK_ANY(item->touchBits, YETI_TOUCHL))
				{
					DamageTarget(item, yeti->enemy, &YetiBiteHandL, YETI_ATTACK3_DAMAGE);
					yeti->flags |= 1;
				}

				if (!(yeti->flags & 2) && CHK_ANY(item->touchBits, YETI_TOUCHR))
				{
					DamageTarget(item, yeti->enemy, &YetiBiteHandR, YETI_ATTACK3_DAMAGE);
					yeti->flags |= 2;
				}
			}
			else if (!(yeti->flags & 1) && IsCreatureNearTarget(item, yeti->enemy))
			{
				DamageTarget(item, yeti->enemy, &YetiBiteHandL, YETI_ATTACK3_DAMAGE_TO_OTHER);
				yeti->flags |= 1;
			}

			break;
		}
	}

	if (isTargetAlive && yeti->enemy->hitPoints <= 0 && yeti->enemy->objectID == ID_LARA)
	{
		CreatureKill(item, YETI_KILL_ANIM, YETI_KILL, AS_STOP);
		return;
	}
	
	CreatureNeck(item, neck);
	CreatureHead(item, head);

	if (item->currentAnimState < YETI_CLIMB2)
	{
		switch (CreatureVault(itemID, angle, 2, 300))
		{
		case 2:
			SetAnimation(item, YETI_CLIMB2_ANIM, YETI_CLIMB2);
			break;
		case 3:
			SetAnimation(item, YETI_CLIMB3_ANIM, YETI_CLIMB3);
			break;
		case 4:
			SetAnimation(item, YETI_CLIMB4_ANIM, YETI_CLIMB4);
			break;
		case -4:
			SetAnimation(item, YETI_FALL4_ANIM, YETI_FALL4);
			break;
		}
	}
	else
	{
		CreatureAnimation(itemID, angle, 0);
	}
}

 /*
  * Inject function
  */
void Inject_Yeti() {
	INJECT(0x00443100, GiantYetiControl);
	INJECT(0x00443400, YetiControl);
}
