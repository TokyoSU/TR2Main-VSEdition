#include "precompiled.h"
#include "global/types.h"
#include "global/vars.h"

ROOM_INFO* Rooms = NULL;
SOUND_SLOT LaSlot[32] = {};
DWORD SampleLutCount = 0;
short* SampleLut = 0;
std::vector<SAMPLE_INFO> SampleInfos;
