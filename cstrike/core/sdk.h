#pragma once

// used: viewmatrix_t
#include "../sdk/datatypes/matrix.h"
// used: color_t
#include "../sdk/datatypes/color.h"
// used: cmd
#include "../sdk/datatypes/usercmd.h"

#pragma region sdk_definitions
// @source: master/public/worldsize.h
// world coordinate bounds
#define MAX_COORD_FLOAT 16'384.f
#define MIN_COORD_FLOAT (-MAX_COORD_FLOAT)

// @source: master/public/vphysics_interface.h
// coordinates are in HL units. 1 unit == 1 inch
#define METERS_PER_INCH 0.0254f
#pragma endregion

class CCSPlayerController;
class C_CSPlayerPawn;

namespace SDK
{
	// capture game's exported functions
	bool Setup();

	inline ViewMatrix_t ViewMatrix = ViewMatrix_t();
	inline Vector_t CameraPosition = Vector_t();
	inline CCSPlayerController* LocalController = nullptr;
	inline C_CSPlayerPawn* LocalPawn = nullptr;
	inline CUserCmd* Cmd = nullptr;

	inline void(CS_CDECL* fnConColorMsg)(const Color_t&, const char*, ...) = nullptr;
}
