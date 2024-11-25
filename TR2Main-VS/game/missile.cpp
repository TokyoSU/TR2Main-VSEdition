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
#include "game/missile.h"
#include "3dsystem/3d_gen.h"
#include "3dsystem/phd_math.h"
#include "game/control.h"
#include "game/draw.h"
#include "game/effects.h"
#include "game/items.h"
#include "game/sound.h"
#include "game/traps.h"
#include "specific/output.h"
#include "specific/game.h"
#include "global/vars.h"

void ControlMissile(short fxID) {
	FX_INFO* fx = &Effects[fxID];

	if (Effects[fxID].objectID == ID_MISSILE_HARPOON && !CHK_ANY(RoomInfo[fx->roomNumber].flags, ROOM_UNDERWATER) && fx->pos.rotX > -12288)
		fx->pos.rotX -= ANGLE(1);

	int speed = fx->speed * phd_cos(fx->pos.rotX) >> W2V_SHIFT;
	fx->pos.y += fx->speed * phd_sin(-fx->pos.rotX) >> W2V_SHIFT;
	fx->pos.z += speed * phd_cos(fx->pos.rotY) >> W2V_SHIFT;
	fx->pos.x += speed * phd_sin(fx->pos.rotY) >> W2V_SHIFT;

	short roomNumber = fx->roomNumber;
	FLOOR_INFO* floor = GetFloor(fx->pos.x, fx->pos.y, fx->pos.z, &roomNumber);
	if (fx->pos.y >= GetHeight(floor, fx->pos.x, fx->pos.y, fx->pos.z)
	|| (fx->pos.y <= GetCeiling(floor, fx->pos.x, fx->pos.y, fx->pos.z)))
	{
		if (fx->objectID == ID_MISSILE_KNIFE || fx->objectID == ID_MISSILE_HARPOON) {
			fx->speed = 0;
			fx->frameNumber = -GetRandomControl() / 11000;
			fx->counter = 6;
			fx->objectID = ID_RICOCHET;
			PlaySoundEffect(258, &fx->pos, 0);
		}
		else if (fx->objectID == ID_MISSILE_FLAME) {
			AddDynamicLight(fx->pos.x, fx->pos.y, fx->pos.z, 14, 11);
			KillEffect(fxID);
		}
		return;
	}

	if (roomNumber != fx->roomNumber) {
		EffectNewRoom(fxID, roomNumber);
	}
	if (fx->objectID == ID_MISSILE_FLAME) {
		if (ItemNearLara(&fx->pos, 350)) {
			LaraItem->hitPoints -= 3;
			LaraItem->hitStatus = 1;
			LaraBurn();
			return;
		}
	}
	else if (ItemNearLara(&fx->pos, 200)) {
		LaraItem->hitStatus = 1;
		fx->pos.rotY = LaraItem->pos.rotY;
		fx->counter = 0;
		fx->speed = LaraItem->speed;
		fx->frameNumber = 0;
		if (fx->objectID == ID_MISSILE_KNIFE || fx->objectID == ID_MISSILE_HARPOON) {
			LaraItem->hitPoints -= 50;
#ifdef FEATURE_CHEAT
			if (Lara.water_status == LWS_Cheat) {
				fx->frameNumber = -GetRandomControl() / 11000;
				fx->counter = 6;
				fx->objectID = ID_RICOCHET;
				PlaySoundEffect(258, &fx->pos, 0);
			}
			else {
				fx->objectID = ID_BLOOD;
				PlaySoundEffect(317, &fx->pos, 0);
			}
#else // FEATURE_CHEAT
			fx->object_number = ID_BLOOD;
			PlaySoundEffect(317, &fx->pos, 0);
#endif // FEATURE_CHEAT
		}
	}

	if (fx->objectID == ID_MISSILE_HARPOON && CHK_ANY(RoomInfo[fx->roomNumber].flags, ROOM_UNDERWATER)) {
		CreateBubble(&fx->pos, fx->roomNumber);
	}
	else if (fx->objectID == ID_MISSILE_FLAME && !fx->counter--) {
		AddDynamicLight(fx->pos.x, fx->pos.y, fx->pos.z, 14, 11);
		PlaySoundEffect(305, &fx->pos, 0);
		KillEffect(fxID);
	}
	else if (fx->objectID == ID_MISSILE_KNIFE) {
		fx->pos.rotZ += ANGLE(30);
	}
}

void ShootAtLara(FX_INFO* fx)
{
	ShootAtLara2(fx, 0);
}

void ShootAtLara2(FX_INFO* fx, int laraHeight)
{
	int x = LaraItem->pos.x - fx->pos.x;
	int y = (LaraItem->pos.y - laraHeight) - fx->pos.y;
	int z = LaraItem->pos.z - fx->pos.z;
	short* bounds = GetBoundsAccurate(LaraItem);
	y += bounds[3] + (bounds[2] - bounds[3]) * 3 / 4;
	int dist = (int)phd_sqrt(SQR(z) + SQR(x));
	fx->pos.rotX = -phd_atan(dist, y);
	fx->pos.rotY = phd_atan(z, x);
	fx->pos.rotX += (GetRandomControl() - 0x4000) / 0x40;
	fx->pos.rotY += (GetRandomControl() - 0x4000) / 0x40;
}

static void CreateExplodingMesh(ITEM_INFO* item, short meshIndex, short damage)
{
	short fxNum = CreateEffect(item->roomNumber);
	if (fxNum != -1)
	{
		FX_INFO* fx = &Effects[fxNum];
		fx->objectID = ID_BODY_PART;
		fx->pos.x = item->pos.x + (PhdMatrixPtr->_03 >> W2V_SHIFT);
		fx->pos.y = item->pos.y + (PhdMatrixPtr->_13 >> W2V_SHIFT);
		fx->pos.z = item->pos.z + (PhdMatrixPtr->_23 >> W2V_SHIFT);
		fx->roomNumber = item->roomNumber;
		fx->pos.rotY = 2 * GetRandomControl() + 0x8000;
		fx->speed = GetRandomControl() >> 8;
		fx->fallspeed = -GetRandomControl() >> 8;
		fx->counter = damage;
		fx->frameNumber = meshIndex;
		fx->shade = LsAdder - 0x300;
	}
}

BOOL ExplodingDeath(short itemNumber, DWORD meshBits, short damage)
{
	ITEM_INFO* item = &Items[itemNumber];
	OBJECT_INFO* obj = &Objects[item->objectID];
	PhdMatrixPtr->_23 = 0;
	S_CalculateLight(item->pos.x, item->pos.y, item->pos.z, item->roomNumber);
	short* bounds = GetBestFrame(item);
	phd_PushUnitMatrix();
	PhdMatrixPtr->_03 = 0;
	PhdMatrixPtr->_13 = 0;
	PhdMatrixPtr->_23 = 0;
	phd_RotYXZ(item->pos.rotY, item->pos.rotX, item->pos.rotZ);
	phd_TranslateRel(*(bounds + 6), *(bounds + 7), *(bounds + 8));
	USHORT* rot = (USHORT*)(bounds + 9);
	phd_RotYXZsuperpack(&rot, 0);
	int* bone = &AnimBones[obj->boneIndex];
	int meshIndex = 1;
	short* extra_rotations = (short*)item->data;
	if (MESHBITS(meshBits, meshIndex) && MESHBITS(item->meshBits, meshIndex))
	{
		CreateExplodingMesh(item, obj->meshIndex, damage);
		REMOVE_MESHBITS(item->meshBits, meshIndex);
	}
	if (obj->nMeshes > 1)
	{
		for (int i = 1; i < obj->nMeshes - 1; i++, bone += 4)
		{
			int poppush = *(bone + 0);
			if (poppush & 1)
				phd_PopMatrix();
			if (poppush & 2)
				phd_PushMatrix();
			phd_TranslateRel(*(bone + 1), *(bone + 2), *(bone + 3));
			phd_RotYXZsuperpack(&rot, 0);
			if (poppush & 0x1C)
			{
				if (poppush & 8)
					phd_RotY(*(extra_rotations)++);
				if (poppush & 4)
					phd_RotY(*(extra_rotations)++);
				if (poppush & 16)
					phd_RotY(*(extra_rotations)++);
			}
			meshIndex++;
			if (MESHBITS(meshBits, meshIndex) && MESHBITS(item->meshBits, meshIndex))
			{
				CreateExplodingMesh(item, obj->meshIndex + meshIndex, damage);
				REMOVE_MESHBITS(item->meshBits, meshIndex);
			}
		}
	}
	phd_PopMatrix();
	return (item->meshBits & (0x7FFFFFFF >> (31 - obj->nMeshes))) == 0;
}

void ControlBodyPart(short fxNum)
{
	FX_INFO* fx = &Effects[fxNum];
	fx->pos.rotX += ANGLE(5);
	fx->pos.rotZ += ANGLE(10);
	fx->fallspeed += 6;
	fx->pos.x += fx->speed * phd_sin(fx->pos.rotY) >> W2V_SHIFT;
	fx->pos.y += fx->fallspeed;
	fx->pos.z += fx->speed * phd_cos(fx->pos.rotY) >> W2V_SHIFT;

	short newRoom = fx->roomNumber;
	FLOOR_INFO* floor = GetFloor(fx->pos.x, fx->pos.y, fx->pos.z, &newRoom);
	if (!(RoomInfo[fx->roomNumber].flags & ROOM_UNDERWATER))
	{
		if (RoomInfo[newRoom].flags & ROOM_UNDERWATER)
		{
			short fxNum = CreateEffect(fx->roomNumber);
			if (fxNum != -1)
			{
				FX_INFO* fxWater = &Effects[fxNum];
				fxWater->pos.x = fx->pos.x;
				fxWater->pos.y = fx->pos.y;
				fxWater->pos.z = fx->pos.z;
				fxWater->pos.rotY = 0;
				fxWater->speed = 0;
				fxWater->frameNumber = 0;
				fxWater->objectID = ID_SPLASH;
			}
		}
	}

	int ceiling = GetCeiling(floor, fx->pos.x, fx->pos.y, fx->pos.z);
	if (fx->pos.y < ceiling)
	{
		fx->pos.y = ceiling;
		fx->fallspeed = -fx->fallspeed; // invert direction !
	}

	int height = GetHeight(floor, fx->pos.x, fx->pos.y, fx->pos.z);
	if (fx->pos.y < height)
	{
		if (ItemNearLara(&fx->pos, 2 * fx->counter))
		{
			LaraItem->hitPoints -= fx->counter;
			LaraItem->hitStatus = TRUE;
			if (fx->counter != 0)
			{
				fx->speed = 0;
				fx->frameNumber = 0;
				fx->counter = 0;
				fx->objectID = ID_EXPLOSION;
				PlaySoundEffect(105, &fx->pos, 0);
				Lara.spaz_effect_count = 5;
				Lara.spaz_effect = fx;
			}
			else
			{
				KillEffect(fxNum);
			}
		}
		if (newRoom != fx->roomNumber)
			EffectNewRoom(fxNum, newRoom);
	}
	else if (fx->counter)
	{
		fx->speed = 0;
		fx->frameNumber = 0;
		fx->counter = 0;
		fx->objectID = ID_EXPLOSION;
		PlaySoundEffect(105, &fx->pos, 0);
	}
	else
	{
		KillEffect(fxNum);
	}
}

/*
 * Inject function
 */
void Inject_Missile() {
	INJECT(0x00433090, ControlMissile);
	INJECT(0x00433360, ShootAtLara);
	INJECT(0x00433410, ExplodingDeath);
	INJECT(0x004337A0, ControlBodyPart);
}