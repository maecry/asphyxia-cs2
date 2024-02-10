#pragma once

// used: mem_pad
#include "../../utilities/memory.h"

// Some of this might be incorrect, using this struct fixed everything in my cheat
class IGlobalVars
{
public:
	float flRealTime; //0x0000
	int32_t nFrameCount; //0x0004
	float flFrameTime; //0x0008
	float flFrameTime2; //0x000C
	int32_t nMaxClients; //0x0010
	MEM_PAD(0xC);
	// Everything under this commend is suppose to be in it's own class inside globalvariables,
	// but I'm too lazy to do that, so I'm just going to leave it here.
	MEM_PAD(0x8);
	float flIntervalPerTick; // 0x0008
	float flCurrentTime; //0x000C
	float flCurrentTime2; //0x0010
	MEM_PAD(0x148);
	char* szCurrentMap; //0x0164
	char* szCurrentMapName; //0x016C
};
