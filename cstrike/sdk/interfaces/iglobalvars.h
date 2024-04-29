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
	int32_t m_nMaxClients; //0x0010
	MEM_PAD(0x14);
	float flFrameTime3; //0x0028
	float flCurrentTime; //0x002C
	float flCurrentTime2; //0x0030
	MEM_PAD(0xC); //0x0034
	int32_t nTickCount; //0x0040
	MEM_PAD(0x138); //0x0044
	char* szCurrentMap; //0x017C
	char* szCurrentMapName; //0x0180
};
