#pragma once

// used: callvfunc
#include "../../utilities/memory.h"

struct ResourceBinding_t;

class IResourceSystem
{
public:
	void* QueryInterface(const char* szInterfaceName)
	{
		return MEM::CallVFunc<void*, 2U>(this, szInterfaceName);
	}
};

class CResourceHandleUtils
{
public:
	void DeleteResource(const ResourceBinding_t* pBinding)
	{
		MEM::CallVFunc<void, 2U>(this, pBinding);
	}
};
