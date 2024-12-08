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
#include "game/sound.h"
#include "3dsystem/phd_math.h"
#include "specific/init_sound_xaudio.h"
#include "specific/sndpc.h"
#include "specific/game.h"
#include "global/vars.h"

static void S_SoundUpdatePanVolume(SOUND_SLOT* slot)
{
	int dx, dy, dz, radius, distance, nPan, nVolume;
	if (slot->distance || slot->pos.x || slot->pos.y || slot->pos.z)
	{
		auto& sampleInfo = SampleInfos.at(slot->sampleIdx);
		dx = slot->pos.x - Camera.pos.x;
		dy = slot->pos.y - Camera.pos.y;
		dz = slot->pos.z - Camera.pos.z;
		radius = sampleInfo.radius << WALL_SHIFT;

		if (ABS(dx) <= radius &&
			ABS(dy) <= radius &&
			ABS(dz) <= radius)
		{
			distance = SQR(dx) + SQR(dy) + SQR(dz);
			if (distance <= SQR(radius))
			{
				if (distance >= 0x100000)
					distance = phd_sqrt(distance) - WALL_SIZE;
				else
					distance = 0;
				nPan = phd_atan(dz, dx) - Camera.actualAngle;
				nVolume = slot->originalVolume;
				int nAttenuation = SQR(distance) / (SQR(radius) / 0x10000);
				nVolume = (nVolume * (0x10000 - nAttenuation)) >> 16;
				if (nVolume > 0)
				{
					if (nVolume > 0x7FFF)
						nVolume = 0x7FFF;
					slot->volume = nVolume;
					slot->pan = nPan;
					slot->distance = distance;
				}
				else
				{
					slot->distance = 0;
					slot->pan = 0;
					slot->volume = 0;
				}
			}
			else
			{
				slot->distance = 0;
				slot->pan = 0;
				slot->volume = 0;
			}
		}
		else
		{
			slot->distance = 0;
			slot->pan = 0;
			slot->volume = 0;
		}
	}
}

int GetRealTrack(int trackID) {
	static char vtracks[] = { 2, 19, 20, 26, -1 };
	int idx = 0;
	int track = 2;

	for (int i = 2; i < trackID; ++i) {
		if ((vtracks[idx] >= 0) && (i == vtracks[idx]))
			++idx;
		else
			++track;
	}
	return track;
}

int PlaySoundEffect(DWORD sampleIdx, PHD_3DPOS* pos, DWORD flags)
{
	if (!IsSoundEnabled || (flags != SFX_ALWAYS && (flags & SFX_UNDERWATER) != (CHK_ANY(Rooms[Camera.pos.roomNumber].flags, ROOM_UNDERWATER))))
		return 0;
	if (sampleIdx >= SampleLutCount)
		return 0;

	int lut = SampleLut[sampleIdx];
	if (lut == -1)
	{
		LogWarn("Sample not exist: %d", sampleIdx);
		SampleLut[sampleIdx] = -2;
		return 0;
	}
	else if (lut == -2) // Sample not exist !
		return 0;

	SAMPLE_INFO& info = SampleInfos.at(lut);
	if (info.randomness && (GetRandomDraw() & 0xFF) > info.randomness)
		return 0;

	int nDistance = 0;
	int nRadius = info.radius << WALL_SHIFT;
	int pan = 0;
	if (pos)
	{
		int x = (pos->x - Camera.pos.x);
		int y = (pos->y - Camera.pos.y);
		int z = (pos->z - Camera.pos.z);
		if (ABS(x) > nRadius &&
			ABS(y) > nRadius &&
			ABS(z) > nRadius)
			return 0;
		nDistance = SQR(x) + SQR(y) + SQR(z);
		if (nDistance > SQR(nRadius))
			return 0;
		if (nDistance < 0x100000)
			nDistance = 0;
		else
			nDistance = phd_sqrt(nDistance) - WALL_SIZE;
		if (!(info.flags & 0x1000)) // NO_PAN
			pan = phd_atan(z, x) - Camera.actualAngle;
	}
	else
	{
		PHD_3DPOS pos2 = {};
		nDistance = 0;
		pos2.x = 0;
		pos2.y = 0;
		pos2.z = 0;
		pos = &pos2;
	}

	int nVolume = info.volume << 7;
	if ((info.flags & 0x4000))
		nVolume -= (0x1000 * GetRandomDraw()) >> 15;
	int origVolume = nVolume;
	int nAttenuation = SQR(nDistance) / (SQR(nRadius) / 0x10000);
	nVolume = (nVolume * (0x10000 - nAttenuation)) >> 16;
	if (nVolume <= 0) return 0;
	if (nVolume > 0x7FFF) nVolume = 0x7FFF;
	if (CHK_ANY(flags, SFX_UNDERWATER)) // Sound underwater has lower volume.
		nVolume /= 2;

	int pitch = 0;
	if (flags & SFX_PITCH)
		pitch = (flags >> 8) & 0xffffff;
	else
		pitch = 0x10000;

	if (info.flags & 0x2000) // RANDOM_PITCH
		pitch += ((6000 * GetRandomDraw()) >> 14) - 6000;

	if (info.sampleIdx < 0)
		return 0;

	int nSample;
	if (info.flags & 0x0080) // RANDOM_SAMPLE
		nSample = info.sampleIdx + (int)((GetRandomDraw() * info.lutCount) >> 15);
	else
		nSample = info.sampleIdx;

	if (info.flags & 0x0200) // WAIT (OneShot)
	{
		for (int nSlot = 0; nSlot < _countof(LaSlot); ++nSlot)
		{
			auto& slot = LaSlot[nSlot];
			if (slot.sampleIdx == lut)
			{
				if (S_SoundSampleIsPlaying(nSlot))
					return 0;
				slot.originalVolume = origVolume;
				slot.sampleIdx = -1;
			}
		}
	}
	else if (info.flags & 0x0400) // REWOUND (OneShot)
	{
		for (int nSlot = 0; nSlot < _countof(LaSlot); ++nSlot)
		{
			auto& slot = LaSlot[nSlot];
			if (slot.sampleIdx == lut)
			{
				S_SoundStopSample(nSlot);
				slot.originalVolume = origVolume;
				slot.sampleIdx = -1;
				break;
			}
		}
	}
	else if (info.flags & 0x0800) // LOOPED
	{
		for (int nSlot = 0; nSlot < _countof(LaSlot); ++nSlot)
		{
			auto& slot = LaSlot[nSlot];
			if (slot.sampleIdx == lut)
			{
				if (nVolume > slot.volume)
				{
					slot.volume = nVolume;
					slot.originalVolume = origVolume;
					slot.pan = pan;
					slot.pitch = pitch;
					slot.distance = nDistance;
					if (pos)
					{
						slot.pos.x = pos->x;
						slot.pos.y = pos->y;
						slot.pos.z = pos->z;
					}
					return 1;
				}
				return 0;
			}
		}
	}

	int nHandle;
	if (info.flags & 0x0800) // LOOPED
		nHandle = S_SoundPlaySampleLooped(nSample, nVolume, pitch, pan);
	else
		nHandle = S_SoundPlaySample(nSample, nVolume, pitch, pan);

	if (nHandle >= 0)
	{
		auto& slot = LaSlot[nHandle];
		slot.volume = nVolume;
		slot.originalVolume = origVolume;
		slot.pan = pan;
		slot.pitch = pitch;
		slot.sampleIdx = lut;
		slot.distance = nDistance;
		if (pos)
		{
			slot.pos.x = pos->x;
			slot.pos.y = pos->y;
			slot.pos.z = pos->z;
		}
		return 1;
	}

	if (nHandle != -1)
	{
		if (nSample >= 0)
			LogWarn("Can't play SFX %d", sampleIdx);
		info.sampleIdx = -1;
		return 0;
	}

	int nMinVolume = 0x8000;
	int nMinSlot = -1;
	for (int nSlot = 1; nSlot < _countof(LaSlot); ++nSlot)
	{
		auto& slot = LaSlot[nSlot];
		if ((slot.sampleIdx >= 0) && (slot.volume < nMinVolume))
		{
			nMinVolume = slot.volume;
			nMinSlot = nSlot;
		}
	}

	if (nMinSlot < 0)
	{
		LogWarn("Mad Failure #3 in SoundEffect");
		return 0;
	}

	if (nVolume < nMinVolume)
		return 0;

	S_SoundStopSample(nMinSlot);
	LaSlot[nMinSlot].sampleIdx = -1;

	if (info.flags & 0x0800)
		nHandle = S_SoundPlaySampleLooped(nSample, nVolume, pitch, pan);
	else
		nHandle = S_SoundPlaySample(nSample, nVolume, pitch, pan);

	if (nHandle >= 0)
	{
		auto& slot = LaSlot[nHandle];
		slot.volume = nVolume;
		slot.originalVolume = origVolume;
		slot.pan = pan;
		slot.pitch = pitch;
		slot.sampleIdx = lut;
		slot.distance = nDistance;
		slot.pos.x = pos->x;
		slot.pos.y = pos->y;
		slot.pos.z = pos->z;
		return 1;
	}

	LogWarn("Mad Failure #2 in SoundEffect");
	return 0;
}

void StopSoundEffect(DWORD sampleIdx)
{
	if (!IsSoundEnabled) return;
	if (sampleIdx >= SampleLutCount) return;

	auto lut = SampleLut[sampleIdx];
	auto& sampleInfo = SampleInfos.at(lut);
	auto lut_end = lut + sampleInfo.lutCount;

	for (int i = 0; i < _countof(LaSlot); ++i)
	{
		auto& slot = LaSlot[i];
		if (slot.sampleIdx >= lut && slot.sampleIdx < lut_end)
		{
			S_SoundStopSample(i);
			slot.sampleIdx = -1;
		}
	}
}

void SOUND_EndScene()
{
	for (int i = 0; i < _countof(LaSlot); i++)
	{
		SOUND_SLOT* slot = &LaSlot[i];
		if (slot->sampleIdx < 0)
			continue;

		SAMPLE_INFO& sampleInfo = SampleInfos.at(slot->sampleIdx);
		if (sampleInfo.flags & 0x0800) // Looped ?
		{
			if (!slot->volume)
			{
				S_SoundStopSample(i);
				slot->sampleIdx = -1;
				continue;
			}

			S_SoundUpdatePanVolume(slot);
			S_SoundSetPanAndVolume(i, slot->pan, slot->volume);
			S_SoundSetPitch(i, slot->pitch);
			slot->volume = 0;
		}
		else if (!S_SoundSampleIsPlaying(i))
			slot->sampleIdx = -1;
	}
}

void SOUND_Stop()
{
	if (IsSoundEnabled)
	{
		S_SoundStopAllSamples();
		for (int i = 0; i < _countof(LaSlot); i++)
			LaSlot[i].sampleIdx = -1;
	}
}

void SOUND_Init() {
	S_SoundSetMasterVolume(32); // 50% sfx volume
	for (int i = 0; i < _countof(LaSlot); ++i)
		SfxInfos[i].sampleIdx = -1;
	SoundIsActive = TRUE;
}

/*
 * Inject function
 */
void Inject_Sound() {
	INJECT(0x0043F430, GetRealTrack);
	INJECT(0x0043F470, PlaySoundEffect);
	INJECT(0x0043F910, StopSoundEffect);
	INJECT(0x0043F970, SOUND_EndScene);
	INJECT(0x0043FA00, SOUND_Stop);
	INJECT(0x0043FA30, SOUND_Init);
}