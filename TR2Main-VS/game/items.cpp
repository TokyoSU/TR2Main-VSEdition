/*
 * Copyright (c) 2017-2019 Michael Chaban. All rights reserved.
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
#include "game/items.h"
#include "global/vars.h"

void InitialiseItemArray(int itemCount) {
	int i;

	NextItemFree = LevelItemCount;
	PrevItemActive = -1;
	NextItemActive = -1;

	for (i = LevelItemCount; i + 1 < itemCount; ++i) {
		Items[i].active = 0;
		Items[i].nextItem = i + 1;
	}
	Items[i].nextItem = -1;
}

void InitialiseItem(short itemIndex) {
	ITEM_INFO* item;
	ROOM_INFO* room;
	FLOOR_INFO* floor;

	item = &Items[itemIndex];
	item->animNumber = Objects[item->objectID].animIndex;
	item->frameNumber = Anims[item->animNumber].frameBase;
	item->goalAnimState = Anims[item->animNumber].currentAnimState;
	item->currentAnimState = item->goalAnimState;
	item->requiredAnimState = 0;
	item->pos.rotX = 0;
	item->pos.rotZ = 0;
	item->speed = 0;
	item->fallSpeed = 0;
	item->hitPoints = Objects[item->objectID].hitPoints;
	item->timer = 0;
	item->meshBits = 0xFFFFFFFF;
	item->touchBits = 0;
	item->data = NULL;

	item->active = 0;
	item->status = ITEM_INACTIVE;
	item->gravity = 0;
	item->hitStatus = 0;
	item->collidable = 1;
	item->looked_at = 0;
	item->clear_body = 0;

	if (CHK_ALL(item->flags, IFL_ONESHOT)) {
		item->status = ITEM_INVISIBLE;
		item->flags &= ~IFL_ONESHOT;
	}
	else if (Objects[item->objectID].intelligent) {
		item->status = ITEM_INVISIBLE;
	}

	if (CHK_ALL(item->flags, IFL_CLEARBODY)) {
		item->clear_body = 1;
		item->flags &= ~IFL_CLEARBODY;
	}

	if (CHK_ALL(item->flags, IFL_CODEBITS)) {
		item->flags &= ~IFL_CODEBITS;
		item->flags |= IFL_REVERSE;
		AddActiveItem(itemIndex);
		item->status = ITEM_ACTIVE;
	}

	room = &RoomInfo[item->roomNumber];
	item->nextItem = room->itemNumber;
	room->itemNumber = itemIndex;

	floor = &room->floor[((item->pos.z - room->z) >> WALL_SHIFT) + room->xSize * ((item->pos.x - room->x) >> WALL_SHIFT)];
	item->floor = floor->floor << 8;

	if (SaveGame.bonusFlag && !IsDemoLevelType)
		item->hitPoints *= 2;

	if (Objects[item->objectID].initialise != NULL)
		Objects[item->objectID].initialise(itemIndex);
}

void AddActiveItem(short itemIndex) {
	ITEM_INFO* item = &Items[itemIndex];

	if (Objects[item->objectID].control == NULL) {
		item->status = ITEM_INACTIVE;
	}
	else if (item->active == 0) {
		item->active = 1;
		item->nextActive = NextItemActive;
		NextItemActive = itemIndex;
	}
}

int GlobalItemReplace(int oldItemID, int newItemID) {
	int i, j;
	int result = 0;

	for (i = 0; i < RoomCount; ++i) {
		for (j = RoomInfo[i].itemNumber; j != -1; j = Items[j].nextItem) {
			if (Items[j].objectID == oldItemID) {
				Items[j].objectID = newItemID;
				++result;
			}
		}
	}
	return result;
}

void InitialiseFXArray()
{
	ZeroMemory(Effects, sizeof(Effects));
	NextEffectActive = -1;
	NextEffectFree = 0;
	FX_INFO* fx = Effects;
	for (int i = 1; i < MAX_EFFECTS; i++, fx++)
		fx->nextFx = i;
	fx->nextFx = -1;
}

short CreateEffect(short roomNum)
{
	short result = NextEffectFree;
	if (NextEffectFree != -1)
	{
		FX_INFO* fx = &Effects[NextEffectFree];
		NextEffectFree = fx->nextFx;
		fx->roomNumber = roomNum;
		ROOM_INFO* r = &RoomInfo[roomNum];
		fx->nextFx = r->fxNumber;
		r->fxNumber = result;
		fx->nextActive = NextEffectActive;
		NextEffectActive = result;
		fx->shade = 0x1000;
	}
	return result;
}

void KillEffect(short fxNum)
{
	FX_INFO* fx = &Effects[fxNum];
	ROOM_INFO* room = &RoomInfo[fx->roomNumber];
	FX_INFO* fxNext;

	short nextActive;
	short oldActive = NextEffectActive;
	bool doAssignNext = true;
	if (NextEffectActive == fxNum)
	{
		NextEffectActive = fx->nextActive;
	}
	else if (NextEffectActive != -1)
	{
		while (true)
		{
			fxNext = &Effects[oldActive];
			if (fxNext->nextActive == fxNum)
				break;
			if (fxNext->nextActive == -1)
			{
				doAssignNext = false;
				break;
			}
			oldActive = fxNext->nextActive;
		}
		if (doAssignNext)
		{
			fxNext = &Effects[oldActive];
			fxNext->nextActive = fx->nextActive;
		}
	}

	short oldFx = room->fxNumber;
	doAssignNext = true;
	if (room->fxNumber == fxNum)
	{
		room->fxNumber = fx->nextFx;
	}
	else if (oldFx != -1)
	{
		while (true)
		{
			fxNext = &Effects[oldFx];
			if (fxNext->nextFx == fxNum)
				break;
			if (fxNext->nextFx == -1)
			{
				doAssignNext = false;
				break;
			}
			oldFx = fxNext->nextFx;
		}
		if (doAssignNext)
		{
			fxNext = &Effects[oldFx];
			fxNext->nextFx = fx->nextFx;
		}
	}

	fx->nextFx = NextEffectFree;
	NextEffectFree = fxNum;
}

void EffectNewRoom(short fxNum, short newRoomNum)
{
	FX_INFO* fx = &Effects[fxNum];
	ROOM_INFO* room = &RoomInfo[fx->roomNumber];
	FX_INFO* fxNext;
	short oldFx = room->fxNumber;
	bool doAssignNext = true;

	if (room->fxNumber == fxNum)
	{
		room->fxNumber = fx->nextFx;
	}
	else if (oldFx != -1)
	{
		while (true)
		{
			fxNext = &Effects[oldFx];
			if (fxNext->nextFx == fxNum)
				break;
			if (fxNext->nextFx == -1)
			{
				doAssignNext = false;
				break;
			}
			oldFx = fxNext->nextFx;
		}
		if (doAssignNext)
		{
			fxNext = &Effects[oldFx];
			fxNext->nextFx = fx->nextFx;
		}
	}

	fx->roomNumber = newRoomNum;
	ROOM_INFO* newRoom = &RoomInfo[newRoomNum];
	fx->nextFx = newRoom->fxNumber;
	newRoom->fxNumber = fxNum;
}

/*
 * Inject function
 */
void Inject_Items() {
	INJECT(0x00426CD0, InitialiseItemArray);
	//INJECT(0x00426D30, KillItem);
	//INJECT(0x00426E50, CreateItem);
	INJECT(0x00426E90, InitialiseItem);
	//INJECT(0x00427050, RemoveActiveItem);
	//INJECT(0x004270E0, RemoveDrawnItem);
	INJECT(0x00427150, AddActiveItem);
	//INJECT(0x004271B0, ItemNewRoom);
	INJECT(0x00427250, GlobalItemReplace);
	INJECT(0x004272D0, InitialiseFXArray);
	INJECT(0x00427300, CreateEffect);
	INJECT(0x00427370, KillEffect);
	INJECT(0x00427460, EffectNewRoom);
	//INJECT(0x00427500, ClearBodyBag);
}