#pragma once

enum AUDIO_REVERB_TYPE
{
	REVERB_GAMESTART = -1,
	REVERB_NONE = 0,
	REVERB_GENERIC,
	REVERB_ROOM,
	REVERB_BATHROOM,
	REVERB_STONEROOM,
	REVERB_AUDITORIUM,
	REVERB_CONCERTHALL,
	REVERB_CAVE,
	REVERB_ARENA,
	REVERB_HANGAR,
	REVERB_CARPETEDHALLWAY,
	REVERB_HALLWAY,
	REVERB_STONECORRIDOR,
	REVERB_ALLEY,
	REVERB_FOREST,
	REVERB_CITY,
	REVERB_MOUNTAINS,
	REVERB_QUARRY,
	REVERB_PLAIN,
	REVERB_PARKINGLOT,
	REVERB_SEWERPIPE,
	REVERB_UNDERWATER,
	REVERB_SMALLROOM,
	REVERB_MEDIUMROOM,
	REVERB_LARGEROOM,
	REVERB_MEDIUMHALL,
	REVERB_LARGEHALL,
	REVERB_PLATE,
	REVERB_MAX
};

extern void DSInitialize();
extern void DSRelease();

extern void DSChangeVolume(DWORD channel, int volume);
extern void DSAdjustPitch(DWORD channel, int pitch);
extern void DSAdjustPan(DWORD channel, int pan);
extern bool DSIsChannelPlaying(DWORD channel);
extern void DXStopSample(DWORD channel, bool isLoop = false);
extern int DSGetFreeChannel();
extern bool DXCreateSample(DWORD sampleIdx, LPWAVEFORMATEX format, const LPVOID data, DWORD dataSize);
extern int DXStartSample(DWORD sampleIdx, int volume, int pitch, int pan, DWORD flags);
extern int CalcVolume(int volume);
extern void DXFreeSounds();

extern void S_SoundStopAllSamples();
extern void S_SoundStopSample(DWORD sampleIdx);
extern int S_SoundPlaySample(DWORD sampleIdx, int volume, int pitch, short pan);
extern int S_SoundPlaySampleLooped(DWORD sampleIdx, int volume, int pitch, short pan);
extern bool S_SoundSampleIsPlaying(DWORD channel);
extern void S_SoundSetPanAndVolume(DWORD sampleIdx, short pan, int volume);
extern void S_SoundSetPitch(DWORD sampleIdx, int pitch);
extern void S_DisableReverb();
extern void S_SetForceReverb(AUDIO_REVERB_TYPE reverb);
extern void S_ResetForceReverb();
extern void S_SetReverbType(AUDIO_REVERB_TYPE reverb);
