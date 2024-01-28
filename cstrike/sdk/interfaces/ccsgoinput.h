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
	MEM_PAD(12344);
	CUserCmd arrCommands[150];
	int8_t N0000065C; //0x8E00
	bool bInThirdPerson;
	MEM_PAD(6);
	QAngle_t m_vecThridpersonAngles;
	MEM_PAD(16);
	int32_t nSequenceNumber;

	int32_t iOldSequence; //0x8E28
	uint32_t pad; //0x8E2C
	double m_Unknown; //0x8E30 look's like a timer
	uint64_t m_iButtonState1; //0x8E38
	MEM_PAD(16);
	uint64_t m_iButtonState2; //0x8E50
	MEM_PAD(32);
	int32_t N0000066A; //0x8E78 padding
	MEM_PAD(12);
	uint32_t N0000066C; //0x8E88 padding
	MEM_PAD(164);
	QAngle_t m_vecViewangles; //0x8F30
	MEM_PAD(232);
	uint32_t N0000069F; //0x9024 padding
	uint32_t m_buttons; //0x9028
	uint32_t N000006A0; //0x902C padding
	MEM_PAD(20);
	int32_t N000006A3; //0x9044 padding
	int32_t N0000178A; //0x9048 padding
	float m_flUnknownTimer; //0x904C
	MEM_PAD(28);

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
