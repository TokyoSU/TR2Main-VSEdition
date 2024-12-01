#include "precompiled.h"
#include "init_sound_xaudio.h"
#include "global/types.h"
#include "global/vars.h"

#define XAUDIO2_HELPER_FUNCTIONS
#pragma comment(lib, "xaudio2")
#include <xaudio2.h>
#include <xaudio2fx.h>

struct ReverbInfo
{
	XAUDIO2FX_REVERB_I3DL2_PARAMETERS Parameters;
	float Volume;
};

static IXAudio2* XA_Engine = NULL;
static IXAudio2MasteringVoice* XA_AudioVoice = NULL;
static IUnknown* XA_ReverbEffect = NULL;
static IXAudio2SourceVoice* XA_Voices[32] = { NULL }; // Channels
static IXAudio2SubmixVoice* XA_SoundFXVoice = NULL;
static XAUDIO2_BUFFER XA_Buffers[256] = { NULL }; // Loaded samples.
static ReverbInfo XA_ReverbPreset[REVERB_MAX] = {
	{XAUDIO2FX_I3DL2_PRESET_GENERIC, 5.0F},
	{XAUDIO2FX_I3DL2_PRESET_ROOM, 5.0F},
	{XAUDIO2FX_I3DL2_PRESET_BATHROOM, 3.0F},
	{XAUDIO2FX_I3DL2_PRESET_STONEROOM, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_AUDITORIUM, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_CONCERTHALL, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_CAVE, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_ARENA, 5.0F},
	{XAUDIO2FX_I3DL2_PRESET_HANGAR, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_HALLWAY, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_ALLEY, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_FOREST, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_CITY, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_MOUNTAINS, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_QUARRY, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_PLAIN, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_PARKINGLOT, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_SEWERPIPE, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_UNDERWATER, 3.0F},
	{XAUDIO2FX_I3DL2_PRESET_SMALLROOM, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_LARGEROOM, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_LARGEHALL, 4.0F},
	{XAUDIO2FX_I3DL2_PRESET_PLATE, 4.0F}
};
static XAUDIO2FX_REVERB_PARAMETERS XA_ReverbType[REVERB_MAX];
static WAVEFORMATEX XA_PCMFormat;
static AUDIO_REVERB_TYPE XA_CurrentReverb = REVERB_NONE;
static AUDIO_REVERB_TYPE XA_ForcedReverb = REVERB_NONE;

void DSInitialize()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if FAILED(XAudio2Create(&XA_Engine, NULL, XAUDIO2_DEFAULT_PROCESSOR))
	{
		LogWarn("Failed to create the audio engine, disabling audio...");
		IsSoundEnabled = false;
	}
	if FAILED(XAudio2CreateReverb(&XA_ReverbEffect, 0))
		LogWarn("Failed to setup reverb effects, reverb will be disabled !");

	if FAILED(XA_Engine->CreateMasteringVoice(&XA_AudioVoice, 2, 22050, NULL, NULL, NULL, AudioCategory_GameEffects))
		LogWarn("Failed to create the master voice, disabling audio...");

	XAUDIO2_EFFECT_DESCRIPTOR effects[] = {
		{ XA_ReverbEffect, TRUE, 1 }  // Attach the reverb effect
	};
	XAUDIO2_EFFECT_CHAIN effectChain = { 1, effects };
	if FAILED(XA_Engine->CreateSubmixVoice(&XA_SoundFXVoice, 1, 22050, NULL, NULL, nullptr, &effectChain))
		LogWarn("Failed to create the soundfx voice, disabling soundfx...");
	XA_SoundFXVoice->SetVolume(3.0F, XAUDIO2_COMMIT_NOW);

	XAUDIO2_SEND_DESCRIPTOR sendDescriptor = { NULL, XA_SoundFXVoice };
	XAUDIO2_VOICE_SENDS sendList = { 1, &sendDescriptor };

	XA_PCMFormat.wFormatTag = WAVE_FORMAT_PCM;
	XA_PCMFormat.cbSize = 0;
	XA_PCMFormat.nChannels = 1;
	XA_PCMFormat.nAvgBytesPerSec = 22050;
	XA_PCMFormat.nSamplesPerSec = 11025;
	XA_PCMFormat.nBlockAlign = 2;
	XA_PCMFormat.wBitsPerSample = 16;

	for (int i = 0; i < _countof(XA_Voices); i++)
	{
		if FAILED(XA_Engine->CreateSourceVoice(&XA_Voices[i], &XA_PCMFormat, 0, XAUDIO2_MAX_FREQ_RATIO, NULL, &sendList, 0))
			LogWarn("Failed to create the channel number: %d", i);
	}
	for (int i = 0; i < _countof(XA_ReverbPreset); i++)
		ReverbConvertI3DL2ToNative(&XA_ReverbPreset[i].Parameters, &XA_ReverbType[i], FALSE);

	IsSoundEnabled = true;
	S_DisableReverb();
}

void DSRelease()
{
	if (!IsSoundEnabled)
		return;

	for (int i = 0; i < _countof(XA_Voices); i++)
	{
		if (XA_Voices[i] != NULL)
		{
			XA_Voices[i]->DestroyVoice();
			XA_Voices[i] = NULL;
		}
	}

	if (XA_SoundFXVoice != NULL)
	{
		XA_SoundFXVoice->DisableEffect(0, XAUDIO2_COMMIT_NOW);
		XA_SoundFXVoice->DestroyVoice();
		XA_SoundFXVoice = NULL;
	}
	if (XA_ReverbEffect != NULL)
	{
		XA_ReverbEffect->Release();
		XA_ReverbEffect = NULL;
	}
	if (XA_AudioVoice != NULL)
	{
		XA_AudioVoice->DestroyVoice();
		XA_AudioVoice = NULL;
	}
	if (XA_Engine != NULL)
	{
		XA_Engine->Release();
		XA_Engine = NULL;
	}
	CoUninitialize();
}

void DSChangeVolume(DWORD channel, int volume)
{
	if (XA_Voices[channel] != NULL)
	{
		// Convert decibels to linear amplitude ratio
		float fvolume = XAudio2DecibelsToAmplitudeRatio(volume / 150.0F);

		// Apply the volume to the voice
		XA_Voices[channel]->SetChannelVolumes(1, &fvolume, XAUDIO2_COMMIT_NOW);
	}
}

void DSAdjustPitch(DWORD channel, int pitch)
{
	if (XA_Voices[channel] != NULL)
	{
		DWORD frequency = DWORD((float)pitch / 65536.0F * 11050.0F);
		if (frequency < 100)
			frequency = 100;
		else if (frequency > 100000)
			frequency = 100000;
		XA_Voices[channel]->SetFrequencyRatio(frequency / 11050.0F, XAUDIO2_COMMIT_NOW);
	}
}

void DSAdjustPan(DWORD channel, int pan)
{
	float matrix[2] = {};

	if (XA_Voices[channel])
	{
		if (pan < 0)
		{
			if (pan < -0x4000)
				pan = -0x4000 - pan;
		}
		else if (pan > 0 && pan > 0x4000)
		{
			pan = 0x8000 - pan;
		}

		pan >>= 4;
		LogDebug("Pan: %d", pan);

		if (pan == 0)
		{
			matrix[0] = 1.0F;
			matrix[1] = 1.0F;
		}
		else if (pan < 0)
		{
			matrix[0] = 1.0F;
			matrix[1] = XAudio2DecibelsToAmplitudeRatio(pan / 100.0F);
		}
		else
		{
			matrix[0] = XAudio2DecibelsToAmplitudeRatio(-pan / 100.0F);
			matrix[1] = 1.0F;
		}

		XA_Voices[channel]->SetOutputMatrix(NULL, 1, 2, matrix, XAUDIO2_COMMIT_NOW);
	}
}

void DXStopSample(DWORD channel)
{
	if (channel >= 0 && XA_Voices[channel])
	{
		if FAILED(XA_Voices[channel]->Stop(0, XAUDIO2_COMMIT_NOW))
			LogWarn("Failed to stop sample id: %d", channel);
		if FAILED(XA_Voices[channel]->FlushSourceBuffers())
			LogWarn("Failed to flush sample id: %d", channel);
	}
}

bool DSIsChannelPlaying(DWORD channel)
{
	if (XA_Voices[channel] != NULL)
	{
		XAUDIO2_VOICE_STATE state;
		XA_Voices[channel]->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);
		if (state.BuffersQueued)
			return true;
	}
	return false;
}

bool DXCreateSample(DWORD sampleIdx, LPWAVEFORMATEX format, const LPVOID data, DWORD dataSize)
{
	if (DSound == NULL)
		return 0;

	if (format->nSamplesPerSec != 11025)
		LogWarn("Incorrect SamplesPerSec for SampleID: %d, Found: %d", sampleIdx, format->nSamplesPerSec);

	auto* buffer = &XA_Buffers[sampleIdx];
	buffer->pAudioData = (BYTE*)malloc(dataSize);
	if (buffer->pAudioData == NULL)
		return false;
	memcpy((void*)buffer->pAudioData, data, dataSize);
	buffer->AudioBytes = dataSize;
	return 1;
}

int DSGetFreeChannel()
{
	for (int i = 0; i < _countof(XA_Voices); i++)
	{
		if (!DSIsChannelPlaying(i))
			return i;
	}
	return -1;
}

int DXStartSample(DWORD sampleIdx, int volume, int pitch, int pan, DWORD flags)
{
	int channel = DSGetFreeChannel();
	if (channel < 0)
		return -1;
	auto* voice = XA_Voices[channel];
	DSChangeVolume(channel, volume);
	DSAdjustPitch(channel, pitch);
	DSAdjustPan(channel, pan);
	auto* buffer = &XA_Buffers[sampleIdx];
	buffer->LoopCount = flags;
	if FAILED(voice->SubmitSourceBuffer(buffer, 0))
		LogWarn("Failed to submit source buffer for sampleID: %d", sampleIdx);
	if FAILED(voice->Start(0, XAUDIO2_COMMIT_NOW))
		LogWarn("Failed to play sampleID: %d", sampleIdx);
	return channel;
}

int CalcVolume(int volume)
{
	// Adjust the volume to a decibel range
	int result = 10000 - int(float(0x7FFF - volume) * 0.30518511F);

	// Clamp the result to valid decibel range [-10000, 0]
	if (result > 0) result = 0;
	else if (result < -10000) result = -10000;

	// Apply master volume scaling
	result -= (100 - S_MasterVolume) * 50;

	// Clamp again after scaling
	if (result > 0) result = 0;
	if (result < -10000) result = -10000;

	return result;
}

void S_SoundStopAllSamples()
{
	for (int i = 0; i < _countof(XA_Voices); i++)
		DXStopSample(i);
}

void S_SoundStopSample(DWORD sampleIdx)
{
	DXStopSample(sampleIdx);
}

int S_SoundPlaySample(DWORD sampleIdx, int volume, int pitch, short pan)
{
	return DXStartSample(sampleIdx, CalcVolume(volume), pitch, pan, 0);
}

int S_SoundPlaySampleLooped(DWORD sampleIdx, int volume, int pitch, short pan)
{
	return DXStartSample(sampleIdx, CalcVolume(volume), pitch, pan, XAUDIO2_LOOP_INFINITE);
}

void DXFreeSounds()
{
	S_SoundStopAllSamples();

	for (int i = 0; i < _countof(XA_Buffers); i++)
	{
		if (XA_Buffers[i].pAudioData != NULL)
		{
			free((void*)XA_Buffers[i].pAudioData);
			XA_Buffers[i].pAudioData = 0;
		}
	}
}

bool S_SoundSampleIsPlaying(DWORD sampleIdx)
{
	if (IsSoundEnabled && DSIsChannelPlaying(sampleIdx))
		return true;
	return false;
}

void S_SoundSetPanAndVolume(DWORD sampleIdx, short pan, int volume)
{
	if (IsSoundEnabled)
	{
		DSChangeVolume(sampleIdx, CalcVolume(volume));
		DSAdjustPan(sampleIdx, pan);
	}
}

void S_SoundSetPitch(DWORD sampleIdx, int pitch)
{
	if (IsSoundEnabled)
		DSAdjustPitch(sampleIdx, pitch);
}

void S_DisableReverb()
{
	XA_SoundFXVoice->DisableEffect(0, XAUDIO2_COMMIT_NOW);
	XA_SoundFXVoice->SetVolume(1.0F, XAUDIO2_COMMIT_NOW);
}

void S_SetForceReverb(AUDIO_REVERB_TYPE reverb)
{
	XA_ForcedReverb = reverb;
	S_SetReverbType(reverb);
}

void S_ResetForceReverb()
{
	XA_ForcedReverb = REVERB_NONE;
	S_SetReverbType(REVERB_NONE);
}

void S_SetReverbType(AUDIO_REVERB_TYPE reverb)
{
	if (XA_SoundFXVoice == NULL)
		return;

	if (XA_ForcedReverb != REVERB_NONE)
		reverb = XA_ForcedReverb;

	if (XA_CurrentReverb != reverb)
	{
		auto& reverbPreset = XA_ReverbPreset[reverb];
		auto& reverbType = XA_ReverbType[reverb];
		if (reverb != REVERB_NONE)
		{
			XA_SoundFXVoice->EnableEffect(0, XAUDIO2_COMMIT_NOW);
			XA_SoundFXVoice->SetVolume(reverbPreset.Volume, XAUDIO2_COMMIT_NOW);
			XA_SoundFXVoice->SetEffectParameters(0, &reverbType, sizeof(XAUDIO2FX_REVERB_PARAMETERS), XAUDIO2_COMMIT_NOW);
			
		}
		else
		{
			XA_SoundFXVoice->DisableEffect(0, XAUDIO2_COMMIT_NOW);
			XA_SoundFXVoice->SetVolume(1.0F, XAUDIO2_COMMIT_NOW);
		}
		XA_CurrentReverb = reverb;
	}
}
