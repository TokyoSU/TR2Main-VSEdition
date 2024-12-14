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
#include "game/eel.h"
#include "3dsystem/phd_math.h"
#include "game/control.h"
#include "game/box.h"
#include "game/effects.h"
#include "specific/init.h"
#include "global/vars.h"

enum BigEelState {
	BIGEEL_EMPTY,
	BIGEEL_ATTACK,
	BIGEEL_STOP,
	BIGEEL_DEATH
};

#define BIGEEL_DAMAGE 500
#define BIGEEL_ANGLE ANGLE(10)
#define BIGEEL_RANGE BLOCK(6)
#define BIGEEL_MOVE (BLOCK(1)/10)
#define BIGEEL_LENGTH (BLOCK(5)/2)
#define BIGEEL_SLIDE (BIGEEL_RANGE - BIGEEL_LENGTH)
#define BIGEEL_DIE_ANIM 2
#define BIGEEL_TOUCH 0x180

#define EEL_DAMAGE 50
#define EEL_ANGLE ANGLE(25)
#define EEL_RANGE BLOCK(2)
#define EEL_MOVE (BLOCK(1)/10)
#define EEL_TURN (ANGLE(2)/3)
#define EEL_LENGTH (BLOCK(1)/2)
#define EEL_SLIDE (EEL_RANGE - EEL_LENGTH)
#define EEL_DIE_ANIM 3

static const BITE_INFO BigEelBite = { 7, 157, 333, 7 };

void BigEelControl(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];
	int pos = (int)item->data;
	item->pos.x -= pos * phd_sin(item->pos.rotY) >> W2V_SHIFT;
	item->pos.z -= pos * phd_cos(item->pos.rotY) >> W2V_SHIFT;

	if (item->hitPoints <= 0)
	{
		if (pos < BIGEEL_SLIDE)
			pos += BIGEEL_MOVE;
		if (item->currentAnimState != BIGEEL_DEATH)
			SetAnimation(item, BIGEEL_DIE_ANIM, BIGEEL_DEATH);
	}
	else
	{
		int z = LaraItem->pos.z - item->pos.z;
		int x = LaraItem->pos.x - item->pos.x;
		short angle = phd_atan(z, x) - item->pos.rotY;
		int distance = phd_sqrt(SQR(z) + SQR(x));

		switch (item->currentAnimState)
		{
		case BIGEEL_STOP:
			if (pos > 0)
				pos -= BIGEEL_MOVE;
			if (distance <= BIGEEL_RANGE && ABS(angle) < BIGEEL_ANGLE)
				item->goalAnimState = BIGEEL_ATTACK;
			break;

		case BIGEEL_ATTACK:
			if (pos < (distance - BIGEEL_LENGTH))
				pos += BIGEEL_MOVE;

			if (!item->requiredAnimState && (item->touchBits & BIGEEL_TOUCH))
			{
				LaraItem->hitStatus = TRUE;
				LaraItem->hitPoints -= BIGEEL_DAMAGE;
				CreatureEffect(item, &BigEelBite, DoBloodSplat);
				item->requiredAnimState = BIGEEL_STOP;
			}
			break;
		}
	}

	item->pos.x += pos * phd_sin(item->pos.rotY) >> W2V_SHIFT;
	item->pos.z += pos * phd_cos(item->pos.rotY) >> W2V_SHIFT;
	item->data = LPVOID(pos);

	AnimateItem(item);
}

struct EEL_INFO
{
	short originalRotY;
	int pos;
};

static EEL_INFO* GetEelInfo(ITEM_INFO* item)
{
	return (EEL_INFO*)item->data;
}

void InitializeEel(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];
	InitialiseCreature(itemNumber);
	item->data = game_malloc(sizeof(EEL_INFO), GBUF_TempAlloc);
	EEL_INFO* eel = GetEelInfo(item);
	eel->originalRotY = item->pos.rotY;
	eel->pos = 0;
}

void EelControl(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];
	EEL_INFO* eel = (EEL_INFO*)item->data;

	item->pos.x -= eel->pos * phd_sin(item->pos.rotY) >> W2V_SHIFT;
	item->pos.z -= eel->pos * phd_cos(item->pos.rotY) >> W2V_SHIFT;
	
	if (item->hitPoints <= 0)
	{
		if (eel->pos < EEL_SLIDE)
			eel->pos += EEL_MOVE;
		if (item->currentAnimState != BIGEEL_DEATH)
			SetAnimation(item, EEL_DIE_ANIM, BIGEEL_DEATH);
	}
	else
	{
		short absRotY = ABS(eel->originalRotY);
		int z = LaraItem->pos.z - item->pos.z;
		int x = LaraItem->pos.x - item->pos.x;
		short angleToTarget = phd_atan(z, x); // Angle to target
		short deltaAngle = angleToTarget - item->pos.rotY; // Difference to current rotation
		short rotationDelta = 0;
		int distance = phd_sqrt(SQR(z) + SQR(x));

		// Normalize deltaAngle to range -16384 to 16384
		if (deltaAngle > 16384)
			deltaAngle -= 32768;
		else if (deltaAngle < -16384)
			deltaAngle += 32768;

		switch (item->currentAnimState)
		{
		case BIGEEL_STOP:
			if (eel->pos > 0)
				eel->pos -= EEL_MOVE;

			// Gradually return to original rotation
			rotationDelta = eel->originalRotY - item->pos.rotY;
			if (rotationDelta > 16384)
				rotationDelta -= 32768;
			else if (rotationDelta < -16384)
				rotationDelta += 32768;

			if (rotationDelta > 0)
				item->pos.rotY += EEL_TURN;
			else if (rotationDelta < 0)
				item->pos.rotY -= EEL_TURN;

			// Ensure the rotation doesn't overshoot the original rotation
			if (ABS(rotationDelta) < EEL_TURN)
				item->pos.rotY = eel->originalRotY;

			if (distance <= EEL_RANGE && ABS(deltaAngle) < EEL_ANGLE)
				item->goalAnimState = BIGEEL_ATTACK;
			break;

		case BIGEEL_ATTACK:
			if (eel->pos < (distance - EEL_LENGTH))
				eel->pos += EEL_MOVE;

			// Gradually turn to face Lara
			if (deltaAngle > 0)
				item->pos.rotY += EEL_TURN;
			else if (deltaAngle < 0)
				item->pos.rotY -= EEL_TURN;

			// Ensure the rotation doesn't overshoot the target angle
			if (ABS(deltaAngle) < EEL_TURN)
				item->pos.rotY = angleToTarget;

			if (!item->requiredAnimState && (item->touchBits & BIGEEL_TOUCH))
			{
				LaraItem->hitStatus = TRUE;
				LaraItem->hitPoints -= EEL_DAMAGE;
				CreatureEffect(item, &BigEelBite, DoBloodSplat);
				item->requiredAnimState = BIGEEL_STOP;
			}
			break;
		}
	}

	item->pos.x += eel->pos * phd_sin(item->pos.rotY) >> W2V_SHIFT;
	item->pos.z += eel->pos * phd_cos(item->pos.rotY) >> W2V_SHIFT;

	AnimateItem(item);
}

 /*
  * Inject function
  */
void Inject_Eel() {
	INJECT(0x0041C120, BigEelControl);
	INJECT(0x0041C2C0, EelControl);
}
