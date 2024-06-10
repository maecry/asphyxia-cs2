#pragma once
// used: MEM_PAD
#include "../../utilities/memory.h"

class CUtlBuffer
{
public:
	MEM_PAD(0x80);

	CUtlBuffer(int a1, int nSize, int a3)
	{
		#ifdef CS_PARANOID
		CS_ASSERT(MEM::fnUtlBufferInit != nullptr);
		#endif

		MEM::fnUtlBufferInit(this, a1, nSize, a3);
	}

	void PutString(const char* szString)
	{
		#ifdef CS_PARANOID
		CS_ASSERT(MEM::fnUtlBufferPutString != nullptr);
		#endif

		MEM::fnUtlBufferPutString(this, szString);
	}

	void EnsureCapacity(int nSize)
	{
		#ifdef CS_PARANOID
		CS_ASSERT(MEM::fnUtlBufferEnsureCapacity != nullptr);
		#endif

		MEM::fnUtlBufferEnsureCapacity(this, nSize);
	}
};
