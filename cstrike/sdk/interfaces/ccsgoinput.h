#pragma once

// used: mem_pad
#include "../../utilities/memory.h"

// used: cusercmd
#include "../datatypes/usercmd.h"

#define MULTIPLAYER_BACKUP 150

class CCSGOInput
{
public:
	MEM_PAD(0x250);
	CUserCmd arrCommands[MULTIPLAYER_BACKUP];
	MEM_PAD(0x1);
	bool bInThirdPerson;
	MEM_PAD(0x22)
	std::int32_t nSequenceNumber;
	std::int32_t nOldSequenceNumber;
	MEM_PAD(0x4);
	double dbUnknown; // look's like a timer
	std::uint64_t nButtonState1;
	std::uint64_t nButtonState2;
	std::uint64_t nButtonState3;
	std::uint64_t nButtonState4; // ?
	MEM_PAD(0xC);
	std::int32_t nMouseDeltaX;
	std::int32_t nMouseDeltaY;
	MEM_PAD(0xC);
	std::int64_t nOldPressedButton;
	bool bIsButtonPressed;
	MEM_PAD(0x10F);
	QAngle_t angViewAngles;
	MEM_PAD(0x8C);
	bool bUnknownBool;
	MEM_PAD(0x7);

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
