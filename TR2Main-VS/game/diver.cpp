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
#include "game/diver.h"
#include "3dsystem/phd_math.h"
#include "game/box.h"
#include "game/control.h"
#include "game/items.h"
#include "game/missile.h"
#include "global/vars.h"
#include "people.h"
#include "effects.h"

typedef enum {
    DIVER_EMPTY,
    DIVER_UNDERWATER_SWIM,
    DIVER_WADE_SWIM,
    DIVER_SHOOT1,
    DIVER_AIM1,
    DIVER_NULL1,
    DIVER_AIM2,
    DIVER_SHOOT2,
    DIVER_NULL2,
    DIVER_DEATH
} DIVER_STATE;

const BITE_INFO HarpoonBite = { 17, 164, 44, 18 };

short Harpoon(int x, int y, int z, short speed, short rotY, short roomNum)
{
    bool isUnderwater = CHK_ANY(RoomInfo[roomNum].flags, ROOM_UNDERWATER);
    short fxNum = CreateEffect(roomNum);
    if (fxNum != -1)
    {
        FX_INFO* fx = &Effects[fxNum];
        fx->pos.x = x;
        fx->pos.y = y;
        fx->pos.z = z;
        fx->roomNumber = roomNum;
        fx->pos.rotZ = 0;
        fx->pos.rotX = 0;
        fx->pos.rotY = rotY;
        fx->speed = isUnderwater ? 150 : 250;
        fx->fallspeed = 0;
        fx->frameNumber = 0;
        fx->objectID = ID_MISSILE_HARPOON;
        fx->shade = 0xE00;
        int height = isUnderwater ? 0 : CLICK(3);
        if (!isUnderwater && ItemNearLara(&fx->pos, BLOCK(3)))
            height = 0;
        ShootAtLara2(fx, height);
    }
    return 0;
}

int GetWaterSurface(int x, int y, int z, short roomNum)
{
    ROOM_INFO* room = &RoomInfo[roomNum];
    FLOOR_INFO* floor = GetFloorSector(x, z, room);

    if (CHK_ANY(room->flags, ROOM_UNDERWATER))
    {
        BYTE skyRoom = floor->skyRoom;
        if (skyRoom != NO_ROOM)
        {
            while (skyRoom != NO_ROOM)
            {
                ROOM_INFO* r1 = &RoomInfo[skyRoom];
                if (!CHK_ANY(r1->flags, ROOM_UNDERWATER))
                    break;
                floor = GetFloorSector(x, z, r1);
                if (floor->skyRoom == NO_ROOM)
                    return NO_HEIGHT;
                skyRoom = floor->skyRoom;
            }
            return floor->ceiling << 8;
        }
        return NO_HEIGHT;
    }

    BYTE pitRoom = floor->pitRoom;
    if (pitRoom == NO_ROOM)
        return NO_HEIGHT;

    while (pitRoom != NO_ROOM)
    {
        ROOM_INFO* r2 = &RoomInfo[pitRoom];
        if (CHK_ANY(r2->flags, ROOM_UNDERWATER))
            break;
        floor = GetFloorSector(x, z, r2);
        if (floor->pitRoom == NO_ROOM)
            return NO_HEIGHT;
        pitRoom = floor->pitRoom;
    }

    return floor->floor << 8;
}

void DiverControl(short itemNum)
{
    if (!CreatureActive(itemNum))
        return;

    ITEM_INFO* item = &Items[itemNum];
    CREATURE_INFO* creature = GetCreatureInfo(item);
    if (creature == NULL) return; // NOTE: Not in the original code..
    GAME_VECTOR src = {}, target = {};
    BOOL target_on_sight = FALSE;
    short headAngle = 0, neckAngle = 0, angle = 0;
    int waterHeight = GetWaterSurface(item->pos.x, item->pos.y, item->pos.z, item->roomNumber) + CLICK(2);

    if (item->hitPoints <= 0)
    {
        if (item->currentAnimState != 9)
            SetAnimation(item, 16, 9);
        CreatureFloat(itemNum);
        return;
    }
    else
    {
        AI_INFO ai;
        CreatureAIInfo(item, &ai);
        CreatureMood(item, &ai, FALSE);

        if (Lara.water_status == LWS_AboveWater)
            target_on_sight = Targetable2(item, &ai, CLICK(1), 612);
        else if (ai.angle >= -PHD_45 && ai.angle < PHD_45)
            target_on_sight = Targetable2(item, &ai, 0, 0);
        else
            target_on_sight = FALSE;

        angle = CreatureTurn(item, creature->maximum_turn);
        switch (item->currentAnimState)
        {
        case DIVER_UNDERWATER_SWIM:
            creature->maximum_turn = 546;
            if (target_on_sight)
                neckAngle = -ai.angle;

            if (creature->target.y < waterHeight && item->pos.y < (waterHeight + creature->LOT.fly) && target_on_sight)
                item->goalAnimState = DIVER_WADE_SWIM;
            else if (creature->mood == MOOD_ESCAPE)
                break;
            else if (target_on_sight)
                item->goalAnimState = DIVER_AIM1;
            break;
        case DIVER_WADE_SWIM:
            creature->maximum_turn = 546;
            if (target_on_sight)
                headAngle = ai.angle;

            if (creature->target.y > waterHeight || !target_on_sight)
                item->goalAnimState = DIVER_UNDERWATER_SWIM;
            else if (creature->mood == MOOD_ESCAPE)
                break;
            else if (target_on_sight)
                item->goalAnimState = DIVER_AIM2;
            break;
        case DIVER_SHOOT1:
            if (target_on_sight)
                neckAngle = -ai.angle;

            if (!creature->flags)
            {
                CreatureEffect(item, &HarpoonBite, Harpoon);
                creature->flags = 1;
            }
            break;
        case DIVER_AIM1:
            creature->flags = 0;
            neckAngle = -ai.angle;

            if (!target_on_sight || creature->mood == MOOD_ESCAPE || (creature->target.y < waterHeight && item->pos.y < (waterHeight + creature->LOT.fly)))
                item->goalAnimState = DIVER_UNDERWATER_SWIM;
            else
                item->goalAnimState = DIVER_SHOOT1;
            break;
        case DIVER_AIM2:
            creature->flags = 0;
            if (target_on_sight)
                headAngle = ai.angle;

            if (creature->target.y > waterHeight || !target_on_sight)
                item->goalAnimState = DIVER_WADE_SWIM;
            else if (creature->mood == MOOD_ESCAPE)
                break;
            else if (target_on_sight)
                item->goalAnimState = DIVER_SHOOT2;
            break;
        case DIVER_SHOOT2:
            if (target_on_sight)
                headAngle = ai.angle;
            if (!creature->flags)
            {
                CreatureEffect(item, &HarpoonBite, Harpoon);
                creature->flags = 1;
            }
            break;
        }
    }

    CreatureHead(item, headAngle);
    CreatureNeck(item, neckAngle);
    CreatureAnimation(itemNum, angle, 0);
    switch (item->currentAnimState)
    {
    case DIVER_UNDERWATER_SWIM:
    case DIVER_AIM1:
    case DIVER_SHOOT1:
        CreatureUnderwater(item, CLICK(2));
        break;
    default:
        item->pos.y = waterHeight - CLICK(2);
        break;
    }
}

 /*
  * Inject function
  */
void Inject_Diver() {
	INJECT(0x00416BF0, Harpoon);
	INJECT(0x00416C70, GetWaterSurface);
	INJECT(0x00416D80, DiverControl);
}

