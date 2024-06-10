#pragma once

#include <iostream>
// used: MEM_PAD and virtual funcs
#include "../utilities/memory.h"
// used: CUtlBuffer
#include "../sdk/datatypes/utlbuffer.h"
// used: CBaseUserCmdPB
#include "../sdk/datatypes/usercmd.h";

namespace CRC
{
	struct CInButtonStateNoVTable
	{
	public:
		std::uint64_t nValue;
		std::uint64_t nValueChanged;
		std::uint64_t nValueScroll;
	};

	struct SavedData_t
	{
		CInButtonStateNoVTable nButtons;
		QAngle_t angView;
	};

	inline SavedData_t savedData;

	inline void Save(CBaseUserCmdPB* pBaseCmd)
	{
		if (pBaseCmd->pViewAngles != nullptr)
			savedData.angView = pBaseCmd->pViewAngles->angValue;

		savedData.nButtons.nValue = pBaseCmd->pInButtonState->nValue;
		savedData.nButtons.nValueChanged = pBaseCmd->pInButtonState->nValueChanged;
		savedData.nButtons.nValueScroll = pBaseCmd->pInButtonState->nValueScroll;
	}

	inline void Apply(CUserCmd* pCmd)
	{
		CBaseUserCmdPB* pBaseCmd = pCmd->csgoUserCmd.pBaseCmd;
		if (pBaseCmd == nullptr)
			return;

		pCmd->nButtons.nValue = savedData.nButtons.nValue;
		pCmd->nButtons.nValueChanged = savedData.nButtons.nValueChanged;
		pCmd->nButtons.nValueScroll = savedData.nButtons.nValueScroll;

		if (pBaseCmd->pViewAngles != nullptr)
			pBaseCmd->pViewAngles->angValue = savedData.angView;
	}

	inline bool CalculateCRC(CBaseUserCmdPB* pBaseCmd)
	{
		int nCalcualtedCRCSize = pBaseCmd->CalculateCmdCRCSize();
		CUtlBuffer protobufBuffer(0, 0, 0);
		protobufBuffer.EnsureCapacity(nCalcualtedCRCSize + 1);

		using fnSerializePartialToArray = bool(__fastcall*)(CBaseUserCmdPB*, CUtlBuffer, int);
		static const fnSerializePartialToArray oSerializePartialToArray = reinterpret_cast<fnSerializePartialToArray>(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 18 55 56 57 48 81 EC 90 00 00 00 48")));

		#ifdef CS_PARANOID
		CS_ASSERT(oSerializePartialToArray != nullptr);
		#endif

		if (oSerializePartialToArray(pBaseCmd, protobufBuffer, nCalcualtedCRCSize))
		{
			std::uintptr_t* pMessage = reinterpret_cast<uintptr_t*>(I::MemAlloc->Alloc(0x18));
			pBaseCmd->nCachedBits |= 1;
			auto nHasBits = static_cast<uint32_t>(pBaseCmd->nHasBits & 0xFFFFFFFFFFFFFFFC);
			if ((pBaseCmd->nHasBits & 1) != 0)
				nHasBits = static_cast<uint32_t>(nHasBits);

			using fnWriteMessage = void(__fastcall*)(std::uintptr_t*, CUtlBuffer, int);
			static const fnWriteMessage oWriteMessage = reinterpret_cast<fnWriteMessage>(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 10 48 89 6C 24 18 48 89 7C 24 20 41 56 48 83 EC 20 48 BF")));

			#ifdef CS_PARANOID
			CS_ASSERT(oWriteMessage != nullptr);
			#endif

			using fnSetMessageData = std::string*(__fastcall*)(void*, std::uintptr_t*, void*);
			static const fnSetMessageData oSetMessageData = reinterpret_cast<fnSetMessageData>(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 20 55 56 57 48 83 EC 30 49")));

			#ifdef CS_PARANOID
			CS_ASSERT(oSetMessageData != nullptr);
			#endif

			oWriteMessage(pMessage, protobufBuffer, nCalcualtedCRCSize);
			pBaseCmd->strMoveCrc = oSetMessageData(&pBaseCmd->strMoveCrc, pMessage, &nHasBits);
			I::MemAlloc->Free(pMessage);

			return true;
		}

		return false;
	}
}
