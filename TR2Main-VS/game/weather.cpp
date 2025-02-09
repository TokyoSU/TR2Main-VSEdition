#include "precompiled.h"
#include "weather.h"
#include "3dsystem/phd_math.h"
#include "3dsystem/scalespr.h"
#include "game/control.h"
#include "game/collide.h"
#include "game/effects.h"
#include "specific/init.h"
#include "specific/game.h"
#include "specific/output.h"
#include "modding/mod_utils.h"

struct WEATHER_INFO
{
	int x, y, z;
	int xv, yv, zv;
	short room;
	BYTE life;
	bool on;
	bool stopped;
};

static WEATHER_INFO SnowList[MAX_WEATHER_SNOW], RainList[MAX_WEATHER_RAIN];
static int SnowCount = 0, RainCount = 0;

static GAME_VECTOR WEATHER_GetRoomSquareRandomPos(short roomNumber)
{
	GAME_VECTOR vec = { 0, 0, 0, 0 };
	ROOM_INFO* r = &Rooms[roomNumber];

	// Calculate the room's boundaries in world coordinates
	int sizeX = BLOCK(r->xSize);  // Full width of the room (east-west)
	int sizeZ = BLOCK(r->zSize); // Full length of the room (north-south)
	int minX = r->x - sizeX;  // Minimum X position (center minus half the width)
	int maxX = r->x + sizeX;  // Maximum X position (center plus half the width)
	int minZ = r->z - sizeZ; // Minimum Z position (center minus half the length)
	int maxZ = r->z + sizeZ; // Maximum Z position (center plus half the length)

	// Ensure that the random values are distributed across the entire room size
	// Generate random X and Z positions within the room boundaries
	vec.x = minX + (GetRandomDrawWithNegInt() % (maxX - minX + 1));
	vec.y = r->maxCeiling + 1; // Start Y position; GetCeiling will adjust it
	vec.z = minZ + (GetRandomDrawWithNegInt() % (maxZ - minZ + 1));

	// Get the ceiling Y position
	short roomNum = roomNumber;
	vec.y = GetCeiling(GetFloor(vec.x, vec.y, vec.z, &roomNum), vec.x, vec.y, vec.z);
	if (vec.y == NO_HEIGHT)
		return vec;
	vec.roomNumber = roomNum;
	vec.boxNumber = TRUE;
	return vec;
}

void WEATHER_UpdateAndDrawRain()
{
	for (int i = 0; i < MAX_WEATHER_RAIN; i++) {
		auto& rainDrop = RainList[i];

		for (short j = 0; j < RoomCount; j++) {
			auto* r = &Rooms[j];
			if (CHK_ANY(r->flags, ROOM_RAIN)) {
				if (!rainDrop.on && RainCount < Mod.rainDensity) {
					GAME_VECTOR vec = WEATHER_GetRoomSquareRandomPos(r->index);
					if (vec.boxNumber == TRUE)
					{
						rainDrop.x = vec.x;
						rainDrop.y = vec.y;
						rainDrop.z = vec.z;
						rainDrop.xv = (GetRandomDraw() & 7) - 4;
						rainDrop.yv = (GetRandomDraw() & 14) + GetRenderScale(8);
						rainDrop.zv = (GetRandomDraw() & 7) - 4;
						rainDrop.life = 255;
						rainDrop.room = vec.roomNumber;
						rainDrop.on = true;
						rainDrop.stopped = false;
						RainCount++;
					}
				}
			}
		}

		if (rainDrop.on)
		{
			auto* floor = GetFloor(rainDrop.x, rainDrop.y, rainDrop.z, &rainDrop.room);
			int height = GetHeight(floor, rainDrop.x, rainDrop.y, rainDrop.z);
			if ((height == NO_HEIGHT || height <= rainDrop.y) || CHK_ANY(Rooms[rainDrop.room].flags, ROOM_UNDERWATER)) {
				int ceiling = GetCeiling(floor, rainDrop.x, rainDrop.y, rainDrop.z);
				if (ceiling != NO_HEIGHT && ceiling <= rainDrop.y)
				{
					if (Mod.rainSplashEnabled)
						CreateRainSpash(Mod.rainSplashColor, rainDrop.x, rainDrop.y - 64, rainDrop.z, Mod.rainSplashSize, rainDrop.room);
				}
				rainDrop.on = false;
				RainCount--;
				continue;
			}
			
			if (Mod.rainDoDamageOnHit)
			{
				PHD_3DPOS effectPos = {};
				effectPos.x = rainDrop.x;
				effectPos.y = rainDrop.y;
				effectPos.z = rainDrop.z;
				if (ItemNearLara(&effectPos, Mod.rainDamageRange))
				{
					DoBloodSplat(rainDrop.x, rainDrop.y, rainDrop.z, 5, LaraItem->pos.rotY + ANGLE(180), rainDrop.room);
					LaraItem->hitPoints -= Mod.rainDamage;
					LaraItem->hitStatus = TRUE;
					rainDrop.on = false;
					RainCount--;
					continue;
				}
			}

			rainDrop.x += rainDrop.xv + 4 * SmokeWindX;
			rainDrop.y += rainDrop.yv << 3;
			rainDrop.z += rainDrop.zv + 4 * SmokeWindZ;

			int rnd = GetRandomDraw();
			if ((rnd & 3) != 3) {
				rainDrop.xv += (rnd & 3) - 1;
				rainDrop.xv = std::clamp(rainDrop.xv, -4, 4);
			}

			rnd = (rnd >> 2) & 3;
			if (rnd != 3) {
				rainDrop.zv += rnd - 1;
				rainDrop.zv = std::clamp(rainDrop.zv, -4, 4);
			}

			rainDrop.life -= 2;
			if (rainDrop.life <= 0) {
				rainDrop.on = false;
				RainCount--;
			}

			S_DrawSprite(SPR_ABS | (Mod.isRainOpaque ? 0 : SPR_SEMITRANS) | SPR_SCALE, rainDrop.x, rainDrop.y, rainDrop.z, Objects[ID_WEATHER_SPRITE].meshIndex, 0, 1024);
		}
	}
}

void WEATHER_UpdateAndDrawSnow()
{
	for (int i = 0; i < MAX_WEATHER_SNOW; i++) {
		auto& snowDrop = SnowList[i];

		for (short j = 0; j < RoomCount; j++)
		{
			auto* r = &Rooms[j];
			if (CHK_ANY(r->flags, ROOM_SNOW)) {
				if (!snowDrop.on && SnowCount < Mod.snowDensity) {
					GAME_VECTOR vec = WEATHER_GetRoomSquareRandomPos(r->index);
					if (vec.boxNumber == TRUE)
					{
						snowDrop.x = vec.x;
						snowDrop.y = vec.y;
						snowDrop.z = vec.z;
						snowDrop.xv = (GetRandomDraw() & 7) - 4;
						snowDrop.yv = (GetRandomDraw() % 24 + 8) << 3;
						snowDrop.zv = (GetRandomDraw() & 7) - 4;
						snowDrop.life = 255;
						snowDrop.room = vec.roomNumber;
						snowDrop.on = true;
						snowDrop.stopped = false;
						SnowCount++;
					}
				}
			}
		}

		if (snowDrop.on)
		{
			if (CHK_ANY(Rooms[snowDrop.room].flags, ROOM_UNDERWATER))
			{
				snowDrop.on = false;
				SnowCount--;
				continue;
			}

			if (!snowDrop.stopped)
			{
				auto* floor = GetFloor(snowDrop.x, snowDrop.y, snowDrop.z, &snowDrop.room); // Update room number if it has changed rooms.
				auto height = GetHeight(floor, snowDrop.x, snowDrop.y, snowDrop.z);
				if (height == NO_HEIGHT || height <= snowDrop.y)
				{
					snowDrop.xv = 0;
					snowDrop.yv = 0;
					snowDrop.zv = 0;
					snowDrop.y = height;
					snowDrop.stopped = true;
					if (snowDrop.life > 16)
						snowDrop.life = 16;
				}
				else
				{
					snowDrop.x += snowDrop.xv;
					snowDrop.y += (snowDrop.yv & 0xF8) >> 2;
					snowDrop.z += snowDrop.zv;
				}
			}

			if (snowDrop.life <= 0)
			{
				snowDrop.on = false;
				SnowCount--;
				continue;
			}

			snowDrop.life--;
			if (!snowDrop.stopped)
			{
				if (snowDrop.xv < SmokeWindX << 1)
					snowDrop.xv++;
				else if (snowDrop.xv > SmokeWindX << 1)
					snowDrop.xv--;
				if (snowDrop.zv < SmokeWindZ << 1)
					snowDrop.zv++;
				else if (snowDrop.zv > SmokeWindZ << 1)
					snowDrop.zv--;
				if ((snowDrop.yv & 7) != 7)
					snowDrop.yv++;
			}

			// Draw the sprite on the scene.
			BYTE c;
			if ((snowDrop.yv & 7) < 7)
				c = snowDrop.yv & 7;
			else if (snowDrop.life > 18.0f)
				c = 16;
			else
				c = snowDrop.life; // Below 255, use life directly
			c <<= 3; // Adjust brightness scaling
			S_DrawSprite(RGB_MAKE(c, c, c) | SPR_TINT | SPR_ABS | (Mod.isSnowOpaque ? 0 : SPR_SEMITRANS) | SPR_SCALE, snowDrop.x, snowDrop.y, snowDrop.z, Objects[ID_WEATHER_SPRITE].meshIndex + 1, 0, 1024);
		}
	}
}

