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

#define SOUND_RANGE						10
#define SOUND_RADIUS					(SOUND_RANGE << WALL_SHIFT)
#define SOUND_RADIUS_SQRD				SQR(SOUND_RADIUS)
#define	SOUND_RANGE_MULT_CONSTANT		((int)(32768 / SOUND_RADIUS))
#define SOUND_MAXVOL_RANGE				1
#define SOUND_MAXVOL_RADIUS				(SOUND_MAXVOL_RANGE << WALL_SHIFT)
#define SOUND_MAXVOL_RADIUS_SQRD		SQR(SOUND_MAXVOL_RADIUS)
#define MAX_VOLUME_CHANGE 				0x1000
#define MAX_PITCH_CHANGE 				6000
#define NO_PAN							(1 << 12)
#define PITCH_WIBBLE					(1 << 13)
#define VOLUME_WIBBLE					(1 << 14)

enum SOUND_FLAGS
{
	NORMAL_SOUND,
	WAIT_SOUND,
	RESTART_SOUND,
	LOOPED_SOUND
};

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

	int nSampleInfo = SampleLut[sampleIdx];
	if (nSampleInfo == -1)
	{
		LogWarn("Sample not exist: %d", sampleIdx);
		SampleLut[sampleIdx] = -2;
		return 0;
	}
	else if (nSampleInfo == -2) // Sample not exist !
		return 0;

	SAMPLE_INFO* pSI = &SampleInfos[nSampleInfo];
	if (pSI->randomness && (GetRandomDraw() > (int)pSI->randomness))
		return 0;

	int distance = 0;
	int pan = 0;
	if (pos)
	{
		int x = (pos->x - Camera.micPos.x);
		int y = (pos->y - Camera.micPos.y);
		int z = (pos->z - Camera.micPos.z);

		if ((x < -SOUND_RADIUS) || (x > SOUND_RADIUS) ||
			(y < -SOUND_RADIUS) || (y > SOUND_RADIUS) ||
			(z < -SOUND_RADIUS) || (z > SOUND_RADIUS))
			return 0;

		distance = SQR(x) + SQR(y) + SQR(z);
		if (distance > SOUND_RADIUS_SQRD)
			return 0;
		if (distance < SOUND_MAXVOL_RADIUS_SQRD)
			distance = 0;
		else
			distance = phd_sqrt(distance) - SOUND_MAXVOL_RADIUS;

		if (!(pSI->flags & NO_PAN))
			pan = phd_atan(z, x) - Camera.actualAngle;
	}
	else
	{
		distance = 0;
	}

	int nVolume = pSI->volume;
	if ((pSI->flags & VOLUME_WIBBLE))
		nVolume -= ((GetRandomDraw() * MAX_VOLUME_CHANGE) >> 15);
	int nAttenuation = distance * distance / (SOUND_RADIUS_SQRD / 0x10000);
	int pitch = 0;
	nVolume = ((65536 - nAttenuation) * nVolume) >> 16;
	if (nVolume <= 0) return 0;
	if (nVolume > 0x7FFF) nVolume = 0x7FFF;
	if (flags & SFX_PITCH)
		pitch = (flags >> 8) & 0xffffff;
	else
		pitch = 0x10000;

	if ((pSI->flags & PITCH_WIBBLE))
		pitch += ((GetRandomDraw() * MAX_PITCH_CHANGE) >> 14) - MAX_PITCH_CHANGE;
	if (pSI->sfxID < 0)
		return 0;

	int nSfxSamples = (pSI->flags >> 2) & 15;
	int nSample = 0;
	if (nSfxSamples == 1)
		nSample = pSI->sfxID;
	else
		nSample = pSI->sfxID + (int)((GetRandomDraw() * nSfxSamples) >> 15);

	int nMode = pSI->flags & 3;
	switch (nMode)
	{
	case WAIT_SOUND:
		for (int nSlot = 0; nSlot < _countof(LaSlot); ++nSlot)
			if (LaSlot[nSlot].nSampleInfo == nSampleInfo)
			{
				if (S_SoundSampleIsPlaying(nSlot))
					return 0;
				LaSlot[nSlot].nSampleInfo = -1;
			}
		break;

	case RESTART_SOUND:
		for (int nSlot = 0; nSlot < _countof(LaSlot); ++nSlot)
			if (LaSlot[nSlot].nSampleInfo == nSampleInfo)
			{
				S_SoundStopSample(nSlot);
				LaSlot[nSlot].nSampleInfo = -1;
				break;
			}
		break;

	case LOOPED_SOUND:
		for (int nSlot = 0; nSlot < _countof(LaSlot); ++nSlot)
			if (LaSlot[nSlot].nSampleInfo == nSampleInfo)
			{
				if (nVolume > LaSlot[nSlot].nVolume)
				{
					LaSlot[nSlot].nVolume = nVolume;
					LaSlot[nSlot].nPan = pan;
					LaSlot[nSlot].nPitch = pitch;
					return 1;
				}
				return 0;
			}
		break;

	case NORMAL_SOUND:
		break;
	}

	int nHandle;
	if (nMode == LOOPED_SOUND)
		nHandle = S_SoundPlaySampleLooped(nSample, nVolume, pitch, pan);
	else
		nHandle = S_SoundPlaySample(nSample, nVolume, pitch, pan);

	if (nHandle >= 0)
	{
		LaSlot[nHandle].nVolume = nVolume;
		LaSlot[nHandle].nPan = pan;
		LaSlot[nHandle].nPitch = pitch;
		LaSlot[nHandle].nSampleInfo = nSampleInfo;
		return 1;
	}

	if (nHandle != -1)
	{
		if (nSample >= 0)
			LogWarn("Can't play SFX %d", sampleIdx);
		pSI->sfxID = -1;
		return 0;
	}

	int nMinVolume = 0x8000;
	int nMinSlot = -1;
	for (int nSlot = 1; nSlot < _countof(LaSlot); ++nSlot)
	{
		if ((LaSlot[nSlot].nSampleInfo >= 0) && (LaSlot[nSlot].nVolume < nMinVolume))
		{
			nMinVolume = LaSlot[nSlot].nVolume;
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
	LaSlot[nMinSlot].nSampleInfo = -1;

	if (nMode == LOOPED_SOUND)
		nHandle = S_SoundPlaySampleLooped(nSample, nVolume, pitch, pan);
	else
		nHandle = S_SoundPlaySample(nSample, nVolume, pitch, pan);

	if (nHandle >= 0)
	{
		LaSlot[nHandle].nVolume = nVolume;
		LaSlot[nHandle].nPan = pan;
		LaSlot[nHandle].nPitch = pitch;
		LaSlot[nHandle].nSampleInfo = nSampleInfo;
		return 1;
	}

	LogWarn("Mad Failure #2 in SoundEffect");
	return 0;
}

void StopSoundEffect(DWORD sampleIdx)
{
	int nSlot;
	int nStartSampleInfo, nEndSampleInfo;
	if (!IsSoundEnabled)
		return;
	nStartSampleInfo = SampleLut[sampleIdx];
	nEndSampleInfo = nStartSampleInfo + ((SampleInfos[nStartSampleInfo].flags >> 2) & 15);
	for (nSlot = 0; nSlot < _countof(LaSlot); ++nSlot) {
		if (LaSlot[nSlot].nSampleInfo >= nStartSampleInfo && LaSlot[nSlot].nSampleInfo < nEndSampleInfo) {
			S_SoundStopSample(nSlot);
			LaSlot[nSlot].nSampleInfo = -1;
		}
	}
}

void SOUND_Init() {
	S_SoundSetMasterVolume(32); // 50% sfx volume

	for (int i = 0; i < 32; ++i)
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
	//INJECT(0x0043F970, SOUND_EndScene);
	//INJECT(0x0043FA00, SOUND_Stop);
	INJECT(0x0043FA30, SOUND_Init);
}