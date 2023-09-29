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
#include "game/laraflare.h"
#include "3dsystem/3d_gen.h"
#include "3dsystem/scalespr.h"
#include "game/draw.h"
#include "game/control.h"
#include "game/effects.h"
#include "game/lara.h"
#include "game/sound.h"
#include "specific/game.h"
#include "specific/output.h"
#include "global/vars.h"

#if defined(FEATURE_VIDEOFX_IMPROVED)
extern DWORD AlphaBlendMode;
#endif // FEATURE_VIDEOFX_IMPROVED

#if defined(FEATURE_GAMEPLAY_FIXES)
static bool IsFlareLeftHandUnderwater(PHD_VECTOR* pos, short* roomNumber)
{
	*roomNumber = LaraItem->roomNumber;
	GetFloor(pos->x, pos->y, pos->z, roomNumber);
	return CHK_ANY(RoomInfo[*roomNumber].flags, ROOM_UNDERWATER);
}
#endif

void DoFlareInHand(int flare_age)
{
	PHD_VECTOR pos{};
	pos.x = 11;
	pos.y = 32;
	pos.z = 41;
	GetLaraJointAbsPosition(&pos, LM_HandL);
	Lara.left_arm.flash_gun = DoFlareLight(&pos, flare_age);

	if (Lara.flare_age >= 1800)
	{
		if (Lara.gun_status == LGS_Armless)
			Lara.gun_status = LGS_Undraw;
	}
	else
	{
		Lara.flare_age++;
#if defined(FEATURE_GAMEPLAY_FIXES)
		// NOTE: This fix the bubble appearing through 2 click water room where lara is walking.
		// Also fix sound too, when going on 2 click water, it played it underwater instead of land.
		short roomNumber;
		if (IsFlareLeftHandUnderwater(&pos, &roomNumber))
		{
			PlaySoundEffect(12, &LaraItem->pos, SFX_UNDERWATER);
			if (GetRandomDraw() < 0x4000)
				CreateBubbleVec(&pos, roomNumber);
		}
#else
		if (CHK_ANY(RoomInfo[LaraItem->roomNumber].flags, ROOM_UNDERWATER))
		{
			PlaySoundEffect(12, &LaraItem->pos, SFX_UNDERWATER);
			if (GetRandomDraw() < 0x4000)
				CreateBubbleVec(&pos, LaraItem->roomNumber);
		}
#endif
		else
		{
			PlaySoundEffect(12, &LaraItem->pos, 0);
		}
	}
}

void DrawFlareInAir(ITEM_INFO* item) {
	int rate;
	short* frames[2];
	GetFrames(item, frames, &rate);
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x, item->pos.y, item->pos.z);
	phd_RotYXZ(item->pos.rotY, item->pos.rotX, item->pos.rotZ);
	int clip = S_GetObjectBounds(frames[0]);
	if (clip) {
		CalculateObjectLighting(item, frames[0]);
		phd_PutPolygons(MeshPtr[Objects[ID_FLARE_ITEM].meshIndex], clip);
		if (CHK_ANY((DWORD)item->data, 0x8000)) {
			phd_TranslateRel(-6, 6, 80);
			phd_RotX(-ANGLE(90));
			phd_RotY(GetRandomDraw() * 2);
			S_CalculateStaticLight(0x800);
			phd_PutPolygons(MeshPtr[Objects[ID_FLARE_FIRE].meshIndex], clip);
#if defined(FEATURE_VIDEOFX_IMPROVED)
			if (AlphaBlendMode) {
				int shade = (GetRandomDraw() & 0xFFF) + 0x1000;
				S_DrawSprite(GLOW_FLARE_COLOR | SPR_BLEND_ADD | SPR_TINT | SPR_SHADE | SPR_SEMITRANS, 0, 0, 0, Objects[ID_GLOW].meshIndex, shade, 0);
			}
#endif // FEATURE_VIDEOFX_IMPROVED
		}
	}
	phd_PopMatrix();
}

/*
 * Inject function
 */
void Inject_LaraFlare() {
	//	INJECT(0x0042F840, DoFlareLight);
	INJECT(0x0042F8E0, DoFlareInHand);
	INJECT(0x0042F9C0, DrawFlareInAir);
	//	INJECT(0x0042FAC0, CreateFlare);
	//	INJECT(0x0042FCA0, set_flare_arm);
	//	INJECT(0x0042FCF0, draw_flare);
	//	INJECT(0x0042FE60, undraw_flare);
	//	INJECT(0x00430090, draw_flare_meshes);
	//	INJECT(0x004300B0, undraw_flare_meshes);
	//	INJECT(0x004300D0, ready_flare);
	//	INJECT(0x00430110, FlareControl);
}