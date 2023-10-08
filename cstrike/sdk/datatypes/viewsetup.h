#pragma once

// used: mem_pad
#include "../../utilities/memory.h"

// used: vector_t
#include "vector.h"
// used: qangle_t
#include "qangle.h"

class CViewSetup
{
public:
	MEM_PAD(0x490);
	float flOrthoLeft; // 0x0494
	float flOrthoTop; // 0x0498
	float flOrthoRight; // 0x049C
	float flOrthoBottom; // 0x04A0
	MEM_PAD(0x38);
	float flFov; // 0x04D8
	float flFovViewmodel; // 0x04DC
	Vector_t vecOrigin; // 0x04E0
	MEM_PAD(0xC); // 0x04EC
	QAngle_t angView; // 0x04F8
	MEM_PAD(0x14); // 0x0504
	float flAspectRatio; // 0x0518
};