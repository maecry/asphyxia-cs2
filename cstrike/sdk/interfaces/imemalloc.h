#pragma once
// used: mem::CallVFunc
#include "../../utilities/memory.h"
#pragma warning(push)
#pragma warning(disable : 4191)

class IMemAlloc
{
public:
	void* Alloc(std::size_t nSize)
	{
		return MEM::CallVFunc<void*, 1>(this, nSize);
	}

	void* ReAlloc(const void* pMemory, std::size_t nSize)
	{
		return MEM::CallVFunc<void*, 2>(this, pMemory, nSize);
	}

	void Free(const void* pMemory)
	{
		return MEM::CallVFunc<void, 3>(this, pMemory);
	}

	std::size_t GetSize(const void* pMemory)
	{
		return MEM::CallVFunc<std::size_t, 21>(this, pMemory);
	}
};

#pragma warning(pop)