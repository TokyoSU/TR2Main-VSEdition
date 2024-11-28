#include "precompiled.h"
#include "weather.h"
#include "3dsystem/math_tbls.h"
#include "3dsystem/scalespr.h"
#include "game/control.h"
#include "game/effects.h"
#include "specific/game.h"
#include "specific/output.h"
#include "global/types.h"
#include "global/vars.h"
#if defined(FEATURE_MOD_CONFIG)
#include "modding/mod_utils.h"
#endif

struct RAINDROP {
	int x, y, z;        // Position of the raindrop
	int xv, yv, zv;     // Velocity of the raindrop
	short currentRoom;  // Room the raindrop is currently in
	bool on;            // Whether the raindrop is active
	BYTE life;          // Remaining life of the raindrop
};

struct SNOWFLAKE
{
	int x;
	int y;
	int z;
	int xv;
	int yv;
	int zv;
	short currentRoom;
	bool on;
	bool stopped;
	BYTE life;
	BYTE pad;
};

static RAINDROP raindrops[MAX_WEATHER_RAIN];
static SNOWFLAKE snowflakes[MAX_WEATHER_SNOW];
static short NumRaindropAlive = 0;
static short NumSnowflakeAlive = 0;

// Initialize a new raindrop
static void InitializeRaindrop(RAINDROP& rainDrop) {
	short rad = GetRandomDraw() & 8191;
	short angle = GetRandomDraw() & 8190;

	int x = LaraItem->pos.x + (rad * rcossin_tbl[angle] >> 12);
	int y = LaraItem->pos.y - BLOCK(1) - (GetRandomDraw() & 0x7FF);
	int z = LaraItem->pos.z + (rad * rcossin_tbl[angle + 1] >> 12);
	short roomNum = LaraItem->roomNumber;

	auto* floor = GetFloor(x, y, z, &roomNum);
	if (!floor) return;

	int height = GetHeight(floor, x, y, z);
	if (height == NO_HEIGHT || height <= y) return;

	int ceiling = GetCeiling(floor, x, y, z);
	if (ceiling == NO_HEIGHT || ceiling >= y) return;

	if (!CHK_ANY(RoomInfo[roomNum].flags, ROOM_OUTSIDE | ROOM_HORIZON)) return;

	rainDrop.x = x;
	rainDrop.y = y;
	rainDrop.z = z;
	rainDrop.xv = (GetRandomDraw() & 7) - 4;
	rainDrop.yv = (GetRandomDraw() & 14) + GetRenderScale(8);
	rainDrop.zv = (GetRandomDraw() & 7) - 4;
	rainDrop.life = 86 - (rainDrop.yv << 1);
	rainDrop.currentRoom = roomNum;
	rainDrop.on = true;
	NumRaindropAlive++;
}

// Update an active raindrop
static void UpdateRaindrop(RAINDROP& rainDrop) {
	auto* floor = GetFloor(rainDrop.x, rainDrop.y, rainDrop.z, &rainDrop.currentRoom);
	int height = GetHeight(floor, rainDrop.x, rainDrop.y, rainDrop.z);

	if ((height == NO_HEIGHT || height <= rainDrop.y) || CHK_ANY(RoomInfo[rainDrop.currentRoom].flags, ROOM_UNDERWATER)) {
		int ceiling = GetCeiling(floor, rainDrop.x, rainDrop.y, rainDrop.z);
		if (ceiling != NO_HEIGHT && ceiling < rainDrop.y) // Avoid splash spawning on the ceiling !
		{
#if defined(FEATURE_MOD_CONFIG)
			if (Mod.rainSplashEnabled)
				CreateRainSpash(RGB_MAKE(255, 0, 0), rainDrop.x, rainDrop.y, rainDrop.z, Mod.rainSplashSize, rainDrop.currentRoom);
#else
			CreateRainSpash(rainDrop.x, rainDrop.y, rainDrop.z, 128, rainDrop.currentRoom);
#endif
		}
		rainDrop.on = false;
		NumRaindropAlive--;
		return;
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
		NumRaindropAlive--;
	}
}

// Draw all active raindrops
static void DrawRaindrops() {
	OBJECT_INFO* obj = &Objects[ID_WEATHER_SPRITE];
	for (int i = 0; i < MAX_WEATHER_RAIN; i++) {
		auto& rainDrop = raindrops[i];
		if (rainDrop.on) {
			S_DrawSprite(SPR_ABS | SPR_SEMITRANS | SPR_SCALE, rainDrop.x, rainDrop.y, rainDrop.z, obj->meshIndex, 0, 512);
		}
	}
}

// Main function for managing rain behavior
void UpdateRain()
{
	for (int i = 0; i < MAX_WEATHER_RAIN; i++) {
		auto& rainDrop = raindrops[i];
#if defined(FEATURE_MOD_CONFIG)
		if (!rainDrop.on && NumRaindropAlive < Mod.rainDensity) {
#else
		if (!rainDrop.on && NumRaindropAlive < MAX_WEATHER_RAIN_ALIVE) {
#endif
			InitializeRaindrop(rainDrop);
		}

		if (rainDrop.on) {
			UpdateRaindrop(rainDrop);
		}
	}

	DrawRaindrops();
}

void DoSnow()
{
	// UPDATE
	OBJECT_INFO* obj = &Objects[ID_WEATHER_SPRITE];
	for (int i = 0; i < MAX_WEATHER_SNOW; i++)
	{
		auto& snow = snowflakes[i];
#if defined(FEATURE_MOD_CONFIG)
		if (!snow.on && NumSnowflakeAlive < Mod.snowDensity)
#else
		if (!rainDrop.on && NumSnowflakeAlive < MAX_WEATHER_SNOW_ALIVE)
#endif
		{
			short rad = GetRandomDraw() & 8190;
			short angle = GetRandomDraw() & 8190;
			int x = LaraItem->pos.x + (rad * rcossin_tbl[angle] >> 12);
			int y = LaraItem->pos.y - BLOCK(1) - (GetRandomDraw() & 0x7FF);
			int z = LaraItem->pos.z + (rad * rcossin_tbl[angle + 1] >> 12);
			short roomNum = LaraItem->roomNumber;
			auto* floor = GetFloor(x, y, z, &roomNum);
			if (floor == NULL)
				continue;
			auto height = GetHeight(floor, x, y, z);
			if (height == NO_HEIGHT || height <= y)
				continue;
			int ceiling = GetCeiling(floor, x, y, z);
			if (ceiling == NO_HEIGHT || ceiling >= y)
				continue;

			if (!CHK_ANY(RoomInfo[roomNum].flags, ROOM_OUTSIDE | ROOM_HORIZON)) return;

			snow.x = x;
			snow.y = y;
			snow.z = z;
			snow.xv = (GetRandomDraw() & 7) - 4;
			snow.yv = (GetRandomDraw() & 38 + 8) << 3;
			snow.zv = (GetRandomDraw() & 7) - 4;
			snow.life = 128 - (snow.yv << 1);
			snow.currentRoom = roomNum;
			snow.on = true;
			snow.stopped = false;
			NumSnowflakeAlive++;
		}

		int ox = snow.x;
		int oy = snow.y;
		int oz = snow.z;

		if (snow.on)
		{
			auto* floor = GetFloor(snow.x, snow.y, snow.z, &snow.currentRoom); // Update room number if it has changed rooms.
			auto height = GetHeight(floor, snow.x, snow.y, snow.z);
			if (height == NO_HEIGHT || CHK_ANY(RoomInfo[snow.currentRoom].flags, ROOM_UNDERWATER))
			{
				snow.on = false;
				NumSnowflakeAlive--;
				continue;
			}

			if (!snow.stopped)
			{
				if (height <= snow.y)
				{
					snow.x = ox;
					snow.y = height;
					snow.z = oz;
					snow.stopped = true;
					if (snow.life > 16)
						snow.life = 16;
				}
				else
				{
					snow.x += snow.xv;
					snow.y += (snow.yv & 0xF8) >> 2;
					snow.z += snow.zv;
				}
			}
			
			if (snow.life <= 0)
			{
				snow.on = false;
				NumSnowflakeAlive--;
				continue;
			}

			snow.life -= 2;

			if (!snow.stopped)
			{
				if (snow.xv < SmokeWindX << 1)
					snow.xv++;
				else if (snow.xv > SmokeWindX << 1)
					snow.xv--;

				if (snow.zv < SmokeWindZ << 1)
					snow.zv++;
				else if (snow.zv > SmokeWindZ << 1)
					snow.zv--;

				if ((snow.yv & 7) != 7)
					snow.yv++;
			}
		}
	}

	// DRAW
	for (int i = 0; i < MAX_WEATHER_SNOW; i++)
	{
		auto& snow = snowflakes[i];
		if (snow.on)
		{
			BYTE c;
			if ((snow.yv & 7) < 7)
				c = snow.yv & 7;
			else if (snow.life > 18)
				c = 15;
			else
				c = snow.life;
			c <<= 3;
			S_DrawSprite(RGB_MAKE(c, c, c) | SPR_TINT | SPR_ABS | SPR_SCALE, snow.x, snow.y, snow.z, obj->meshIndex + 1, 0, 512);
		}
	}
}
