#pragma once

// used: mem_pad
#include "../../utilities/memory.h"

class CGameEntitySystem;

class IGameResourceService
{
public:
	MEM_PAD(0x58);
	CGameEntitySystem* pGameEntitySystem;
};