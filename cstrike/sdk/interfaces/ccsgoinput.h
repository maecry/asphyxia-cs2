#pragma once

// used: mem_pad
#include "../../utilities/memory.h"

// used: cusercmd
#include "../datatypes/usercmd.h"

#define MULTIPLAYER_BACKUP 150

class CCSGOInput
{
public:
	void* pVtable;
	MEM_PAD(0x248);
	CUserCmd arrCommands[150];
	MEM_PAD(0x1);
	bool bInThirdPerson;
	MEM_PAD(0x22);
	int32_t nSequenceNumber;

	CUserCmd* GetUserCmd()
	{
		return &arrCommands[nSequenceNumber % MULTIPLAYER_BACKUP];
	}

	void SetViewAngle(QAngle_t& angView)
	{
		// @ida: this got called before GetMatricesForView
		using fnSetViewAngle = std::int64_t(CS_FASTCALL*)(void*, std::int32_t, QAngle_t&);
		static auto oSetViewAngle = reinterpret_cast<fnSetViewAngle>(MEM::FindPattern(CLIENT_DLL, CS_XOR("F2 41 0F 10 00 4C 63 CA")));

#ifdef CS_PARANOID
		CS_ASSERT(oSetViewAngle != nullptr);
#endif
		oSetViewAngle(this, 0, std::ref(angView));
	}
};