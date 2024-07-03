#pragma once

// used: getexportaddress
#include "../../utilities/memory.h"

class IInputSystem
{
public:
	bool IsRelativeMouseMode()
	{
		// @ida: 'IInputSystem::SetRelativeMouseMode'.
		return *reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + 0x4D);
	}

	void* GetSDLWindow()
	{
		// @ida: IInputSystem::DebugSpew -> #STR: "Current coordinate bias %s: %g,%g scale %g,%g\n"
		return *reinterpret_cast<void**>(reinterpret_cast<std::uintptr_t>(this) + 0x26A8);
	}
};
