#pragma once

// used: call virtual function
#include "../../utilities/memory.h"

// forward declarations
struct IDXGISwapChain;

class ISwapChainDx11
{
	MEM_PAD(0x178);
	IDXGISwapChain* pDXGISwapChain;
};