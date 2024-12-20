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
#include "game/sphere.h"
#include "game/effects.h"
#include "global/vars.h"

void BaddieBiteEffect(ITEM_INFO* item, const BITE_INFO* bite)
{
	PHD_VECTOR pos = {};
	pos.x = bite->x;
	pos.y = bite->y;
	pos.z = bite->z;
	GetJointAbsPosition(item, &pos, bite->meshIndex);
	DoBloodSplat(pos.x, pos.y, pos.z, item->speed, item->pos.rotY, item->roomNumber);
}

 /*
  * Inject function
  */
void Inject_Sphere() {
	//INJECT(0x0043FA60, TestCollision);
	//INJECT(0x0043FB90, GetSpheres);
	//INJECT(0x0043FE70, GetJointAbsPosition);
	INJECT(0x00440010, BaddieBiteEffect);
}
