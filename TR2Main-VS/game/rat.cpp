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
#include "game/rat.h"
#include "game/box.h"
#include "specific/game.h"
#include "global/vars.h"

constexpr auto MOUSE_DAMAGE = 20;
constexpr auto MOUSE_DIE_ANIM = 9;
// Head, torso and all front legs.
constexpr auto MOUSE_TOUCH = MESH_BITS(1) | MESH_BITS(2) | MESH_BITS(3) | MESH_BITS(4) | MESH_BITS(5) | MESH_BITS(6);
constexpr auto MOUSE_WAIT1_CHANCE = 0x500;
constexpr auto MOUSE_WAIT2_CHANCE = (MOUSE_WAIT1_CHANCE + 0x500);
constexpr auto MOUSE_RUN_TURN = ANGLE(6);
constexpr auto MOUSE_ATTACK_RANGE = SQR(BLOCK(1) / 3);

enum MouseState {
	// No state 0
	MOUSE_RUN = 1,
	MOUSE_STOP,
	MOUSE_WAIT1,
	MOUSE_WAIT2,
	MOUSE_ATTACK,
	MOUSE_DEATH
};

static const BITE_INFO MouseBite = { 0, 0, 57, 2 };

void MouseControl(short itemNumber)
{
	if (!CreatureActive(itemNumber))
		return;

	ITEM_INFO* item = &Items[itemNumber];
	CREATURE_INFO* mouse = GetCreatureInfo(item);
	short head = 0, angle = 0;

	if (item->hitPoints <= 0)
	{
		if (item->currentAnimState != MOUSE_DEATH)
			SetAnimation(item, MOUSE_DIE_ANIM, MOUSE_DEATH);
	}
	else
	{
		AI_INFO info;
		CreatureAIInfo(item, &info);
		if (info.ahead)
			head = info.angle;
		CreatureMood(item, &info, FALSE);
		angle = CreatureTurn(item, MOUSE_RUN_TURN);

		switch (item->currentAnimState)
		{
		case MOUSE_WAIT2:
			if (mouse->mood == MOOD_BORED || mouse->mood == MOOD_STALK)
			{
				int random = GetRandomControl();
				if (random < MOUSE_WAIT1_CHANCE)
					item->requiredAnimState = MOUSE_WAIT1;
				else if (random > MOUSE_WAIT2_CHANCE)
					item->requiredAnimState = MOUSE_RUN;
			}
			else if (info.distance < MOUSE_ATTACK_RANGE)
				item->requiredAnimState = MOUSE_ATTACK;
			else
				item->requiredAnimState = MOUSE_RUN;

			if (item->requiredAnimState)
				item->goalAnimState = MOUSE_STOP;
			break;

		case MOUSE_STOP:
			mouse->maximumTurn = 0;
			if (item->requiredAnimState)
				item->goalAnimState = item->requiredAnimState;
			break;

		case MOUSE_RUN:
			mouse->maximumTurn = MOUSE_RUN_TURN;

			if (mouse->mood == MOOD_BORED || mouse->mood == MOOD_STALK)
			{
				int random = GetRandomControl();
				if (random < MOUSE_WAIT1_CHANCE)
				{
					item->requiredAnimState = MOUSE_WAIT1;
					item->goalAnimState = MOUSE_STOP;
				}
				else if (random < MOUSE_WAIT2_CHANCE)
					item->goalAnimState = MOUSE_STOP;
			}
			else if (info.ahead && info.distance < MOUSE_ATTACK_RANGE)
				item->goalAnimState = MOUSE_STOP;
			break;

		case MOUSE_ATTACK:
			if (!item->requiredAnimState)
			{
				if (DamageLaraOrEnemy(item, mouse->enemy, &MouseBite, MOUSE_DAMAGE, 0, item->touchBits & MOUSE_TOUCH))
					item->requiredAnimState = MOUSE_STOP;
			}
			break;

		case MOUSE_WAIT1:
			if (GetRandomControl() < MOUSE_WAIT1_CHANCE)
				item->goalAnimState = MOUSE_STOP;
			break;
		}
	}

	CreatureHead(item, head);
	CreatureAnimation(itemNumber, angle, 0);
}

 /*
  * Inject function
  */
void Inject_Rat() {
	INJECT(0x00438FA0, MouseControl);
}
