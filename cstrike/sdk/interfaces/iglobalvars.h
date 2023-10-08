#pragma once

// used: mem_pad
#include "../../utilities/memory.h"

class IGlobalVars
{
public:
	float flRealTime; //0x0000
	__int32 nFrameCount; //0x0004
	float flFrameTime; //0x0008
	float flFrameTime2; //0x000C
	__int32 mMaxclients; //0x0010
	float flIntervalPerTick; //0x0014
	__int32 N0000007F; //0x0018
	__int32 N0000008B; //0x001C
	void* unkfunc; //0x0020
	float N00000081; //0x0028
	float flCurtime; //0x002C
	float flCurtime2; //0x0030
	MEM_PAD(0xC); //0x0034
	__int32 nTickCount; //0x0040
	float flIntervalPerTick2; //0x0044
	void* pCurrentNetChannel; //0x0048
	MEM_PAD(0x130); //0x0050
	char* szCurrentMap; //0x0180
	char* szCurrentMapName; //0x0188

}; //Size=0x0190

static_assert(sizeof(IGlobalVars) == 0x190);