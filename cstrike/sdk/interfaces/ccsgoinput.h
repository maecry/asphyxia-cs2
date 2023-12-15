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

	void SetViewAngleVector(Vector_t& angView)
	{
		// @ida: this got called before GetMatricesForView
		using fnSetViewAngle = std::int64_t(CS_FASTCALL*)(void*, std::int32_t, Vector_t&);
		static auto oSetViewAngle = reinterpret_cast<fnSetViewAngle>(MEM::FindPattern(CLIENT_DLL, CS_XOR("F2 41 0F 10 00 4C 63 CA"))); // \xF2\x41\x0F\x10?\x4C\x63\xCA

#ifdef CS_PARANOID
		CS_ASSERT(oSetViewAngle != nullptr);
#endif
		oSetViewAngle(this, 0, std::ref(angView));
	}

	void GetViewAngle(QAngle_t& angles)
	{
		using function_t = void*(__fastcall*)(CCSGOInput*, std::int32_t);
		static function_t fn = reinterpret_cast<function_t>(MEM::FindPattern(CLIENT_DLL, CS_XOR("4C 63 C2 4B 8D 04 40 8B 84 C1 A0 45 ? ? 85 C0 74 22"))); // \x4C\x63\xC2\x4B\x8D\x04\x40\x4C\x8D\x0C\xC1

		angles = *reinterpret_cast<QAngle_t*>(fn(this, 0));
	}

	Vector_t GetViewAngleVector()
	{
		using function_t = void*(__fastcall*)(CCSGOInput*, std::int32_t);
		static function_t fn = reinterpret_cast<function_t>(MEM::FindPattern(CLIENT_DLL, CS_XOR("4C 63 C2 4B 8D 04 40 8B 84 C1 A0 45 ? ? 85 C0 74 22")));

		Vector_t* viewAngle = reinterpret_cast<Vector_t*>(fn(this, 0));
		return *viewAngle;
	}
};
