#pragma once

// used: mem_pad
#include "../../utilities/memory.h"

class IGlobalVars
{
public:
	float flRealTime; //0x0000
	int32_t nFrameCount; //0x0004
	float flFrameTime; //0x0008
	float flFrameTime2; //0x000C
	int32_t nMaxClients; //0x0010
	MEM_PAD(0x1C);
	float flFrameTime3; //0x0030
	float flCurrentTime; //0x0034
	float flCurrentTime2; //0x0038
	MEM_PAD(0xC);
	int32_t nTickCount; //0x0048
};
