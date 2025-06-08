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
#include "game/pickup.h"
#include "game/collide.h"
#include "game/gameflow.h"
#include "game/health.h"
#include "game/larafire.h"
#include "game/laramisc.h"
#include "game/invfunc.h"
#include "game/laraflare.h"
#include "game/items.h"
#include "game/secrets.h"
#include "global/vars.h"

#if defined(FEATURE_MOD_CONFIG)
#include "modding/mod_utils.h"
#endif

void PickUpCollision(short itemNumber, ITEM_INFO* laraitem, COLL_INFO* coll)
{
	ITEM_INFO* item = &Items[itemNumber];
	short oldrotX = item->pos.rotX;
	short oldrotY = item->pos.rotY;
	short oldrotZ = item->pos.rotZ;
	item->pos.rotY = laraitem->pos.rotY;
	item->pos.rotZ = 0;

	if (Lara.water_status == LWS_AboveWater || Lara.water_status == LWS_Wade)
	{
		item->pos.rotX = 0;
		if (!TestLaraPosition(PickUpBounds, item, laraitem))
		{
			item->pos.rotX = oldrotX;
			item->pos.rotY = oldrotY;
			item->pos.rotZ = oldrotZ;
			return;
		}

		if (laraitem->currentAnimState == AS_PICKUP)
		{
			if (laraitem->frameNumber == (Anims[135].frameBase + 42) && item->objectID != ID_FLARE_ITEM)
			{
				AddDisplayPickup(item->objectID, GetSecretType(item));
				Inv_AddItem((GAME_OBJECT_ID)item->objectID, GetSecretType(item));

				// Give items when all secrets is found, ID_SECRET4 not included !
				if (IsSecret(item->objectID) && CHK_ALL(SaveGame.statistics.secrets, 1 | 2 | 4))
					GF_ModifyInventory(CurrentLevel, TRUE);

				item->status = ITEM_INVISIBLE;
				RemoveDrawnItem(itemNumber);
			}
		}
		else if (laraitem->currentAnimState == AS_FLAREPICKUP)
		{
			if (laraitem->frameNumber == (Anims[204].frameBase + 58) && item->objectID == ID_FLARE_ITEM && Lara.gun_type != LGT_Flare)
			{
				Lara.request_gun_type = LGT_Flare;
				Lara.gun_type = LGT_Flare;
				InitialiseNewWeapon();
				Lara.gun_status = LGS_Special;
				Lara.flare_age = (short)int(item->data) & 0x7FFF;
				KillItem(itemNumber);
			}
			return;
		}
		else if (CHK_ANY(InputStatus, IN_ACTION) && !laraitem->gravity && laraitem->currentAnimState == AS_STOP && Lara.gun_status == LGS_Armless && !(Lara.gun_type == LGT_Flare && item->objectID == ID_FLARE_ITEM))
		{
			AlignLaraPosition(&PickUpPosition, item, laraitem);
			Lara.gun_status = LGS_HandBusy;

			if (item->objectID == ID_FLARE_ITEM)
			{
				laraitem->goalAnimState = AS_FLAREPICKUP;
				do
					AnimateLara(laraitem);
				while (laraitem->currentAnimState != AS_FLAREPICKUP);
			}
			else
			{
				laraitem->goalAnimState = AS_PICKUP;
				do
					AnimateLara(laraitem);
				while (laraitem->currentAnimState != AS_PICKUP);
			}

			laraitem->goalAnimState = AS_STOP;
		}
	}
	else if (Lara.water_status == LWS_Underwater)
	{
		item->pos.rotX = -4550;
		if (!TestLaraPosition(PickUpBoundsUW, item, laraitem))
		{
			item->pos.rotX = oldrotX;
			item->pos.rotY = oldrotY;
			item->pos.rotZ = oldrotZ;
			return;
		}

		if (laraitem->currentAnimState == AS_PICKUP)
		{
			if (laraitem->frameNumber == (Anims[130].frameBase + 10) && item->objectID != ID_FLARE_ITEM)
			{
				AddDisplayPickup(item->objectID, GetSecretType(item));
				Inv_AddItem((GAME_OBJECT_ID)item->objectID, GetSecretType(item));

				// Give items when all secrets is found, ID_SECRET4 not included !
				if (IsSecret(item->objectID) && CHK_ALL(SaveGame.statistics.secrets, 1 | 2 | 4))
					GF_ModifyInventory(CurrentLevel, TRUE);

				item->status = ITEM_INVISIBLE;
				RemoveDrawnItem(itemNumber);
			}
		}
		else if (laraitem->currentAnimState == AS_FLAREPICKUP)
		{
			if (laraitem->frameNumber == (Anims[206].frameBase + 20) && item->objectID == ID_FLARE_ITEM && Lara.gun_type != LGT_Flare)
			{
				Lara.request_gun_type = LGT_Flare;
				Lara.gun_type = LGT_Flare;
				InitialiseNewWeapon();
				Lara.gun_status = LGS_Special;
				Lara.flare_age = (short)int(item->data) & 0x7FFF;
				draw_flare_meshes();
				KillItem(itemNumber);
			}
			return;
		}
		else if (CHK_ANY(InputStatus, IN_ACTION) && laraitem->currentAnimState == AS_TREAD && Lara.gun_status == LGS_Armless && (Lara.gun_type != LGT_Flare || item->objectID != ID_FLARE_ITEM))
		{
			if (!MoveLaraPosition(&PickUpPositionUW, item, laraitem))
				return;

			if (item->objectID == ID_FLARE_ITEM)
			{
				laraitem->animNumber = 206;
				laraitem->currentAnimState = AS_FLAREPICKUP;
				laraitem->frameNumber = Anims[laraitem->animNumber].frameBase;
				laraitem->fallSpeed = 0;
			}
			else
			{
				laraitem->goalAnimState = AS_PICKUP;
				do
					AnimateLara(laraitem);
				while (laraitem->currentAnimState != AS_PICKUP);
			}

			laraitem->goalAnimState = AS_TREAD;
			return;
		}
	}

	item->pos.rotX = oldrotX;
	item->pos.rotY = oldrotY;
	item->pos.rotZ = oldrotZ;
}

 /*
  * Inject function
  */
void Inject_Pickup() {
	INJECT(0x00437F20, PickUpCollision);
	//INJECT(0x004383A0, SwitchCollision);
	//INJECT(0x004385B0, SwitchCollision2);
	//INJECT(0x004386B0, DetonatorCollision);
	//INJECT(0x004388F0, KeyHoleCollision);
	//INJECT(0x00438B30, PuzzleHoleCollision);
	//INJECT(0x00438DF0, SwitchControl);
	//INJECT(0x00438E30, SwitchTrigger);
	//INJECT(0x00438EF0, KeyTrigger);
	//INJECT(0x00438F30, PickupTrigger);
	//INJECT(0x00438F70, SecretControl);
}
