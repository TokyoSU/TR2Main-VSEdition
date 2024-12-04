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
#include "specific/init_sound.h"
#include "specific/init_sound_xaudio.h"
#include "global/vars.h"

extern void FlaggedStringCreate(STRING_FLAGGED* item, DWORD dwSize);
extern void FlaggedStringDelete(STRING_FLAGGED* item);
extern bool FlaggedStringCopy(STRING_FLAGGED* dst, STRING_FLAGGED* src);

#ifdef FEATURE_EXTENDED_LIMITS
DWORD SampleFreqs[370];
LPDIRECTSOUNDBUFFER SampleBuffers[370];
#endif // FEATURE_EXTENDED_LIMITS

SOUND_ADAPTER_NODE* GetSoundAdapter(GUID* lpGuid) {
	SOUND_ADAPTER_NODE* adapter;

	if (lpGuid != NULL) {
		for (adapter = SoundAdapterList.head; adapter; adapter = adapter->next) {
			if (!memcmp(&adapter->body.adapterGuid, lpGuid, sizeof(GUID)))
				return adapter;
		}
	}
	return PrimarySoundAdapter;
}

void WinSndFreeAllSamples() {
	DXFreeSounds();
}

bool WinSndMakeSample(DWORD sampleIdx, LPWAVEFORMATEX format, const LPVOID data, DWORD dataSize) {
	return DXCreateSample(sampleIdx, format, data, dataSize);
}

bool WinSndIsChannelPlaying(DWORD channel) {
	return DSIsChannelPlaying(channel);
}

int WinSndPlaySample(DWORD sampleIdx, int volume, DWORD pitch, int pan, DWORD flags) {
	return DXStartSample(sampleIdx, volume, pitch, pan, flags);
}

int WinSndGetFreeChannelIndex() {
	return DSGetFreeChannel();
}

void WinSndAdjustVolumeAndPan(int channel, int volume, int pan) {
	DSAdjustVolume(channel, volume);
	DSAdjustPan(channel, pan);
}

void WinSndAdjustPitch(int channel, DWORD pitch) {
	DSAdjustPitch(channel, pitch);
}

void WinSndStopSample(int channel) {
	DXStopSample(channel);
}

bool WinSndInit() {
	SOUND_ADAPTER_NODE* node, * nextNode;

	for (node = SoundAdapterList.head; node; node = nextNode) {
		nextNode = node->next;
		FlaggedStringDelete(&node->body.module);
		FlaggedStringDelete(&node->body.description);
		delete(node);
	}

	SoundAdapterList.head = NULL;
	SoundAdapterList.tail = NULL;
	SoundAdapterList.dwCount = 0;

	PrimarySoundAdapter = NULL;

	if (!DSoundEnumerate(&SoundAdapterList))
		return false;

	for (node = SoundAdapterList.head; node; node = node->next) {
		if (node->body.lpAdapterGuid == NULL) { // Primary adapter GUID is NULL
			PrimarySoundAdapter = node;
			break;
		}
	}

	return true;
}

bool DSoundEnumerate(SOUND_ADAPTER_LIST* adapterList) {
	return SUCCEEDED(DirectSoundEnumerate(DSoundEnumCallback, (LPVOID)adapterList));
}

BOOL CALLBACK DSoundEnumCallback(LPGUID lpGuid, LPCTSTR lpcstrDescription, LPCTSTR lpcstrModule, LPVOID lpContext) {
	SOUND_ADAPTER_LIST* adapterList = (SOUND_ADAPTER_LIST*)lpContext;
	SOUND_ADAPTER_NODE* adapterNode = new SOUND_ADAPTER_NODE;

	if (adapterNode == NULL)
		return TRUE;

	adapterNode->next = NULL;
	adapterNode->previous = adapterList->tail;

	if (!adapterList->head)
		adapterList->head = adapterNode;

	if (adapterList->tail)
		adapterList->tail->next = adapterNode;

	adapterList->tail = adapterNode;
	adapterList->dwCount++;

	if (lpGuid == NULL) {
		memset(&adapterNode->body.adapterGuid, 0, sizeof(GUID));
		adapterNode->body.lpAdapterGuid = NULL;
	}
	else {
		adapterNode->body.adapterGuid = *lpGuid;
		adapterNode->body.lpAdapterGuid = &adapterNode->body.adapterGuid;
	}

	FlaggedStringCreate(&adapterNode->body.description, 256);
	FlaggedStringCreate(&adapterNode->body.module, 256);
	lstrcpy(adapterNode->body.description.lpString, lpcstrDescription);
	lstrcpy(adapterNode->body.module.lpString, lpcstrModule);

	return TRUE;
}

void WinSndStart(HWND hWnd) {
	memset(SampleBuffers, 0, sizeof(SampleBuffers));
	memset(ChannelBuffers, 0, sizeof(ChannelBuffers));

	Camera.isLaraMic = SavedAppSettings.LaraMic;
	IsSoundEnabled = false;

	if (!SavedAppSettings.SoundEnabled || SavedAppSettings.PreferredSoundAdapter == NULL)
		return;

	SOUND_ADAPTER* preferred = &SavedAppSettings.PreferredSoundAdapter->body;
	CurrentSoundAdapter = *preferred;

	FlaggedStringCopy(&CurrentSoundAdapter.description, &preferred->description);
	FlaggedStringCopy(&CurrentSoundAdapter.module, &preferred->module);

	if (!DSoundCreate(CurrentSoundAdapter.lpAdapterGuid))
		return;

	if (hWnd == NULL)
		hWnd = HGameWindow;

	if FAILED(DSound->SetCooperativeLevel(hWnd, DSSCL_EXCLUSIVE))
		throw ERR_CantSetDSCooperativeLevel;

	if (DSoundBufferTest())
		IsSoundEnabled = true;
}

bool DSoundCreate(GUID* lpGuid) {
#if (DIRECTSOUND_VERSION >= 0x800)
	DSInitialize();
	return SUCCEEDED(DirectSoundCreate8(lpGuid, &DSound, NULL));
#else // (DIRECTSOUND_VERSION >= 0x800)
	return SUCCEEDED(DirectSoundCreate(lpGuid, &DSound, NULL));
#endif // (DIRECTSOUND_VERSION >= 0x800)
}

bool DSoundBufferTest() {
	WAVEFORMATEX format;
	DSBUFFERDESC desc;
	LPDIRECTSOUNDBUFFER dsBuffer;

	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	desc.dwBufferBytes = 0;
	desc.dwReserved = 0;
	desc.lpwfxFormat = NULL;

	if FAILED(DSound->CreateSoundBuffer(&desc, &dsBuffer, NULL))
		return false;

	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 2;
	format.nSamplesPerSec = 11025;
	format.nAvgBytesPerSec = 44100;
	format.nBlockAlign = 4;
	format.wBitsPerSample = 16;
	format.cbSize = 0;

	bool result = SUCCEEDED(dsBuffer->SetFormat(&format));
	dsBuffer->Release();
	return result;
}

void WinSndFinish() {
	DSRelease();
}

bool WinSndIsSoundEnabled() {
	return IsSoundEnabled;
}

/*
 * Inject function
 */
void Inject_InitSound() {
	INJECT(0x00447C70, GetSoundAdapter);
	INJECT(0x00447CC0, WinSndFreeAllSamples);
	INJECT(0x00447CF0, WinSndMakeSample);
	INJECT(0x00447E00, WinSndIsChannelPlaying);
	INJECT(0x00447E50, WinSndPlaySample);
	INJECT(0x00447F40, WinSndGetFreeChannelIndex);
	INJECT(0x00447F80, WinSndAdjustVolumeAndPan);
	INJECT(0x00447FB0, WinSndAdjustPitch);
	INJECT(0x00447FF0, WinSndStopSample);
	INJECT(0x00448060, WinSndInit);
	INJECT(0x00448100, DSoundEnumerate);
	INJECT(0x00448120, DSoundEnumCallback);
	INJECT(0x00448210, WinSndStart);
	INJECT(0x00448390, DSoundCreate);
	INJECT(0x004483B0, DSoundBufferTest);
	INJECT(0x00448480, WinSndFinish);
	INJECT(0x004484B0, WinSndIsSoundEnabled);
}