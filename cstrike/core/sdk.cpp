#include "sdk.h"

// used: getmodulebasehandle
#include "../utilities/memory.h"

bool SDK::Setup()
{
	bool bSuccess = true;

	const void* hTier0Lib = MEM::GetModuleBaseHandle(TIER0_DLL);
	if (hTier0Lib == nullptr)
		return false;

	fnConColorMsg = reinterpret_cast<decltype(fnConColorMsg)>(MEM::GetExportAddress(hTier0Lib, CS_XOR("?ConColorMsg@@YAXAEBVColor@@PEBDZZ")));
	bSuccess &= fnConColorMsg != nullptr;

	return bSuccess;
}
