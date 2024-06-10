#pragma once
// used: MEM_PAD, FindPattern
#include "../../utilities/memory.h"

struct KV3ID_t
{
	const char* szName;
	std::uint64_t unk0;
	std::uint64_t unk1;
};

class CKeyValues3
{
public:
	MEM_PAD(0x100);
	std::uint64_t uKey;
	void* pValue;
	MEM_PAD(0x8);

	void LoadFromBuffer(const char* szString);
	bool LoadKV3(CUtlBuffer* buffer);

	static CKeyValues3* CreateMaterialResource()
	{
		using fnSetTypeKV3 = CKeyValues3*(CS_FASTCALL*)(CKeyValues3*, unsigned int, unsigned int);
		static const fnSetTypeKV3 oSetTypeKV3 = reinterpret_cast<fnSetTypeKV3>(MEM::FindPattern(CLIENT_DLL, CS_XOR("40 53 48 83 EC 20 48 8B 01 48 8B D9 44")));

		#ifdef CS_PARANOID
		CS_ASSERT(oSetTypeKV3 != nullptr);
		#endif

		CKeyValues3* pKeyValue = new CKeyValues3[0x10];
		return oSetTypeKV3(pKeyValue, 1U, 6U);
	}
};
