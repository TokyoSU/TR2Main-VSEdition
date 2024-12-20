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
#include "game/skidoo.h"
#include "3dsystem/3d_gen.h"
#include "3dsystem/phd_math.h"
#include "game/box.h"
#include "game/collide.h"
#include "game/draw.h"
#include "game/effects.h"
#include "game/items.h"
#include "game/larafire.h"
#include "game/missile.h"
#include "game/people.h"
#include "game/sound.h"
#include "specific/game.h"
#include "specific/output.h"
#include "global/vars.h"

#ifdef FEATURE_INPUT_IMPROVED
#include "modding/joy_output.h"
#endif // FEATURE_INPUT_IMPROVED

static const BITE_INFO SkidooLeftGun = { 219, -71, 550, 0 };
static const BITE_INFO SkidooRightGun = { -235, -71, 550, 0 };

void SkidooBaddieCollision(ITEM_INFO* skidoo)
{
	short roomList[20], roomCount;
	memset(roomList, 0, sizeof(roomList));
	roomList[0] = skidoo->roomNumber;
	roomCount = 1;

	ROOM_INFO* room = &Rooms[skidoo->roomNumber];
	if (room->doors != NULL && room->doors->wCount > 0)
	{
		for (int i = 0; i < room->doors->wCount; ++i) {
			DOOR_INFO* door = &room->doors->door[i];
			if (door->room != NO_ROOM)
				roomList[roomCount++] = door->room;
		}
	}

	for (int i = 0; i < roomCount; i++)
	{
		short targetItemNumber = Rooms[roomList[i]].itemNumber;
		while (targetItemNumber != -1)
		{
			ITEM_INFO* targetItem = &Items[targetItemNumber];
			if (targetItem->collidable && targetItem->status != ITEM_INVISIBLE && targetItem != LaraItem && targetItem != skidoo)
			{
				OBJECT_INFO* obj = &Objects[targetItem->objectID];
				if (obj->collision && (obj->intelligent || targetItem->objectID == ID_ROLLING_BALL2))
				{
					int x = skidoo->pos.x - targetItem->pos.x;
					int y = skidoo->pos.y - targetItem->pos.y;
					int z = skidoo->pos.z - targetItem->pos.z;
					if (x > -BLOCK(2) && x < BLOCK(2) &&
						z > -BLOCK(2) && z < BLOCK(2) &&
						y > -BLOCK(2) && y < BLOCK(2))
					{
						if (TestBoundsCollide(targetItem, skidoo, 500))
						{
							if (targetItem->objectID == ID_SKIDOO_ARMED)
							{
								SkidmanPush(targetItem, skidoo, 500);
							}
							else if (targetItem->objectID == ID_ROLLING_BALL2)
							{
								if (targetItem->currentAnimState == 1)
								{
									LaraItem->hitPoints -= 100;
									LaraItem->hitStatus = TRUE;
								}
							}
							else
							{
								DoLotsOfBlood(targetItem->pos.x, targetItem->pos.y - CLICK(1), targetItem->pos.z, skidoo->speed, skidoo->pos.rotY, targetItem->roomNumber, 3);
								targetItem->hitPoints = 0;
							}
						}
					}
				}
			}
			targetItemNumber = targetItem->nextItem;
		}
	}
}

void DoSnowEffect(ITEM_INFO* item) {
	short fxID;
	FX_INFO* fx;
	int displacement;

	fxID = CreateEffect(item->roomNumber);
	if (fxID != -1) {
		fx = &Effects[fxID];
		displacement = 260 * (GetRandomDraw() - 16384) >> 14;
		fx->pos.x = item->pos.x - ((500 * phd_sin(item->pos.rotY) + displacement * phd_cos(item->pos.rotY)) >> W2V_SHIFT);
		fx->pos.y = item->pos.y + (500 * phd_sin(item->pos.rotX) >> W2V_SHIFT);
		fx->pos.z = item->pos.z - ((500 * phd_cos(item->pos.rotY) - displacement * phd_sin(item->pos.rotY)) >> W2V_SHIFT);
		fx->roomNumber = item->roomNumber;
		fx->frameNumber = 0;
		fx->objectID = ID_SNOW_SPRITE;
		fx->speed = 0;
		if (item->speed < 64) {
			fx->fallspeed = (ABS(item->speed) - 64) * GetRandomDraw() >> 15;
		}
		else {
			fx->fallspeed = 0;
		}
		PhdMatrixPtr->_23 = 0;
		S_CalculateLight(fx->pos.x, fx->pos.y, fx->pos.z, fx->roomNumber);
		fx->shade = LsAdder - 512;
		CLAMPL(fx->shade, 0);
	}
}

void SkidooExplode(ITEM_INFO* item) {
	short fxID;
	FX_INFO* fx;

	fxID = CreateEffect(item->roomNumber);
	if (fxID != -1) {
		fx = &Effects[fxID];
		fx->pos.x = item->pos.x;
		fx->pos.y = item->pos.y;
		fx->pos.z = item->pos.z;
		fx->speed = 0;
		fx->frameNumber = 0;
		fx->counter = 0;
		fx->objectID = ID_EXPLOSION;
	}
	ExplodingDeath(Lara.skidoo, ~3, 0);
	PlaySoundEffect(105, NULL, 0);
	Lara.skidoo = -1;
#ifdef FEATURE_INPUT_IMPROVED
	JoyRumbleExplode(LaraItem->pos.x, LaraItem->pos.y, LaraItem->pos.z, 0x1400, true);
#endif // FEATURE_INPUT_IMPROVED
}

void SkidooGuns() {
	WEAPON_INFO* weapon = &Weapons[LGT_Skidoo];
	LaraGetNewTarget(weapon);
	AimWeapon(weapon, &Lara.right_arm);

	if (CHK_ANY(InputStatus, IN_ACTION)) {
		short angles[2];
		angles[0] = Lara.right_arm.y_rot + LaraItem->pos.rotY;
		angles[1] = Lara.right_arm.x_rot;
		if (FireWeapon(LGT_Skidoo, Lara.target, LaraItem, angles)) {
			Lara.right_arm.flash_gun = weapon->flashTime;
			PlaySoundEffect(weapon->sampleNum, &LaraItem->pos, 0);
			int x = LaraItem->pos.x + (phd_sin(LaraItem->pos.rotY) >> (W2V_SHIFT - 10));
			int y = LaraItem->pos.y - 0x200;
			int z = LaraItem->pos.z + (phd_cos(LaraItem->pos.rotY) >> (W2V_SHIFT - 10));
			AddDynamicLight(x, y, z, 12, 11);
			CreatureEffect(&Items[Lara.skidoo], &SkidooLeftGun, GunShot);
			CreatureEffect(&Items[Lara.skidoo], &SkidooRightGun, GunShot);
#ifdef FEATURE_INPUT_IMPROVED
			JoyVibrate(0x400, 0x400, 2, 0x80, 4, false);
#endif // FEATURE_INPUT_IMPROVED
		}
	}
}

void DrawSkidoo(ITEM_INFO* item) {
	short* frames[2];
	int rate = 0;
	UINT16 flags = 0;
	OBJECT_INFO* obj;

	int frac = GetFrames(item, frames, &rate);
	if (item->data) {
		flags = *(UINT16*)item->data;
	}
	if (CHK_ANY(flags, 4)) {
		obj = &Objects[ID_SKIDOO_ARMED];
	}
	else {
		obj = &Objects[item->objectID];
	}

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x, item->pos.y, item->pos.z);
	phd_RotYXZ(item->pos.rotY, item->pos.rotX, item->pos.rotZ);
	int clip = S_GetObjectBounds(frames[0]);

	if (clip) {
		CalculateObjectLighting(item, frames[0]);

		short** track = 0;
		short** meshPtr = &MeshPtr[obj->meshIndex];
		int* bonePtr = &AnimBones[obj->boneIndex];
		if ((flags & 3) == 1) {
			track = &MeshPtr[Objects[ID_SKIDOO_LARA].meshIndex + 1];
		}
		else if ((flags & 3) == 2) {
			track = &MeshPtr[Objects[ID_SKIDOO_LARA].meshIndex + 7];
		}

		if (frac) {
			UINT16* rot1 = (UINT16*)&frames[0][9];
			UINT16* rot2 = (UINT16*)&frames[1][9];
			InitInterpolate(frac, rate);
			phd_TranslateRel_ID(frames[0][6], frames[0][7], frames[0][8], frames[1][6], frames[1][7], frames[1][8]);
			phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
#ifdef FEATURE_VIDEOFX_IMPROVED
			SetMeshReflectState(CHK_ANY(flags, 4) ? (int)ID_SKIDOO_ARMED : item->objectID, 0);
#endif // FEATURE_VIDEOFX_IMPROVED
			phd_PutPolygons_I(meshPtr[0], clip);
#ifdef FEATURE_VIDEOFX_IMPROVED
			ClearMeshReflectState();
#endif // FEATURE_VIDEOFX_IMPROVED

			for (int i = 1; i < obj->nMeshes; ++i) {
				DWORD state = *bonePtr;
				if (CHK_ANY(state, 1)) {
					phd_PopMatrix_I();
				}
				if (CHK_ANY(state, 2)) {
					phd_PushMatrix_I();
				}
				phd_TranslateRel_I(bonePtr[1], bonePtr[2], bonePtr[3]);
				phd_RotYXZsuperpack_I(&rot1, &rot2, 0);
				if (track) {
#ifdef FEATURE_VIDEOFX_IMPROVED
					SetMeshReflectState(ID_SKIDOO_LARA, (flags & 3) == 1 ? 1 : 7);
#endif // FEATURE_VIDEOFX_IMPROVED
					phd_PutPolygons_I(*track, clip);
#ifdef FEATURE_VIDEOFX_IMPROVED
					ClearMeshReflectState();
#endif // FEATURE_VIDEOFX_IMPROVED
					track = NULL;
				}
				else {
#ifdef FEATURE_VIDEOFX_IMPROVED
					SetMeshReflectState(CHK_ANY(flags, 4) ? (int)ID_SKIDOO_ARMED : item->objectID, i);
#endif // FEATURE_VIDEOFX_IMPROVED
					phd_PutPolygons_I(meshPtr[i], clip);
#ifdef FEATURE_VIDEOFX_IMPROVED
					ClearMeshReflectState();
#endif // FEATURE_VIDEOFX_IMPROVED
				}
				bonePtr += 4;
			}
		}
		else {
			UINT16* rot = (UINT16*)&frames[0][9];
			phd_TranslateRel(frames[0][6], frames[0][7], frames[0][8]);
			phd_RotYXZsuperpack(&rot, 0);
#ifdef FEATURE_VIDEOFX_IMPROVED
			SetMeshReflectState(CHK_ANY(flags, 4) ? (int)ID_SKIDOO_ARMED : item->objectID, 0);
#endif // FEATURE_VIDEOFX_IMPROVED
			phd_PutPolygons(meshPtr[0], clip);
#ifdef FEATURE_VIDEOFX_IMPROVED
			ClearMeshReflectState();
#endif // FEATURE_VIDEOFX_IMPROVED

			for (int i = 1; i < obj->nMeshes; ++i) {
				DWORD state = *bonePtr;
				if (CHK_ANY(state, 1)) {
					phd_PopMatrix();
				}
				if (CHK_ANY(state, 2)) {
					phd_PushMatrix();
				}
				phd_TranslateRel(bonePtr[1], bonePtr[2], bonePtr[3]);
				phd_RotYXZsuperpack(&rot, 0);
				if (track) {
#ifdef FEATURE_VIDEOFX_IMPROVED
					SetMeshReflectState(ID_SKIDOO_LARA, (flags & 3) == 1 ? 1 : 7);
#endif // FEATURE_VIDEOFX_IMPROVED
					phd_PutPolygons(*track, clip);
#ifdef FEATURE_VIDEOFX_IMPROVED
					ClearMeshReflectState();
#endif // FEATURE_VIDEOFX_IMPROVED
					track = NULL;
				}
				else {
#ifdef FEATURE_VIDEOFX_IMPROVED
					SetMeshReflectState(CHK_ANY(flags, 4) ? (int)ID_SKIDOO_ARMED : item->objectID, i);
#endif // FEATURE_VIDEOFX_IMPROVED
					phd_PutPolygons(meshPtr[i], clip);
#ifdef FEATURE_VIDEOFX_IMPROVED
					ClearMeshReflectState();
#endif // FEATURE_VIDEOFX_IMPROVED
				}
				bonePtr += 4;
			}
		}
	}
	phd_PopMatrix();
}

/*
 * Inject function
 */
void Inject_Skidoo() {
	//INJECT(0x0043CEE0, InitialiseSkidoo);
	//INJECT(0x0043CF20, SkidooCheckGeton);
	//INJECT(0x0043D010, SkidooCollision);
	INJECT(0x0043D110, SkidooBaddieCollision);
	//INJECT(0x0043D310, TestHeight);
	//INJECT(0x0043D3D0, DoShift);
	//INJECT(0x0043D650, DoDynamics);
	//INJECT(0x0043D6B0, GetCollisionAnim);
	INJECT(0x0043D740, DoSnowEffect);
	//INJECT(0x0043D880, SkidooDynamics);
	//INJECT(0x0043DD20, SkidooUserControl);
	//INJECT(0x0043DEE0, SkidooCheckGetOffOK);
	//INJECT(0x0043DFF0, SkidooAnimation);
	INJECT(0x0043E2D0, SkidooExplode);
	//INJECT(0x0043E350, SkidooCheckGetOff);
	INJECT(0x0043E590, SkidooGuns);
	//INJECT(0x0043E6B0, SkidooControl);
	INJECT(0x0043EB10, DrawSkidoo);
	//INJECT(0x0043EDF0, InitialiseSkidman);
	//INJECT(0x0043EE80, SkidManControl);
	//INJECT(0x0043F280, SkidmanPush);
	//INJECT(0x0043F3A0, SkidmanCollision);
}