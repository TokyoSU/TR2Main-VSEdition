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
#include "game/dragon.h"
#include "3dsystem/phd_math.h"
#include "game/draw.h"
#include "game/items.h"
#include "game/missile.h"
#include "specific/game.h"
#include "global/vars.h"

void CreateBartoliLight(short itemNum)
{
	ITEM_INFO* item = &Items[itemNum];
	short fxNum = CreateEffect(item->roomNumber);
	if (fxNum != -1)
	{
		FX_INFO* fx = &Effects[fxNum];
		fx->pos.rotY = GetRandomDraw() * 2;
		fx->pos.x = item->pos.x + (ANGLE(5) * phd_sin(fx->pos.rotY) >> W2V_SHIFT);
		fx->pos.y = (GetRandomDraw() >> 2) + item->pos.y - 1024;
		fx->pos.z = item->pos.z + (ANGLE(5) * phd_cos(fx->pos.rotY) >> W2V_SHIFT);
		fx->roomNumber = item->roomNumber;
		fx->counter = itemNum;
		fx->frameNumber = 0;
		fx->objectID = ID_TWINKLE;
	}
	AddDynamicLight(item->pos.x, item->pos.y, item->pos.z, (4 * GetRandomDraw() >> 15) + 12, (4 * GetRandomDraw() >> 15) + 10);
}

short DragonFire(int x, int y, int z, short speed, short rotY, short roomNum)
{
	short fxnum = CreateEffect(roomNum);
	if (fxnum != -1)
	{
		FX_INFO* fx = &Effects[fxnum];
		fx->pos.x = x;
		fx->pos.y = y;
		fx->pos.z = z;
		fx->roomNumber = roomNum;
		fx->pos.rotZ = 0;
		fx->pos.rotX = 0;
		fx->pos.rotY = rotY;
		fx->speed = 200;
		fx->objectID = ID_MISSILE_FLAME;
		fx->frameNumber = (Objects[fx->objectID].nMeshes + 1) * GetRandomDraw() >> 15;
		fx->shade = 0xE00;
		ShootAtLara(fx);
		if (Objects[ID_DRAGON_FRONT].loaded)
		{
			fx->counter = 0x4000;
			return fxnum;
		}
		fx->counter = 20;
	}
	return fxnum;
}

 /*
  * Inject function
  */
void Inject_Dragon() {
	//INJECT(0x00417780, ControlTwinkle);
	INJECT(0x00417900, CreateBartoliLight);
	INJECT(0x004179E0, DragonFire);
	//INJECT(0x00417A90, DragonCollision);
	//INJECT(0x00417D80, DragonBones);
	//INJECT(0x00417E60, DragonControl);
	//INJECT(0x004183B0, InitialiseBartoli);
	//INJECT(0x004184D0, BartoliControl);
	//INJECT(0x00418670, DinoControl);
}
