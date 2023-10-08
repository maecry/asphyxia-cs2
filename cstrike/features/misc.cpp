#include "misc.h"

// used: movement callback
#include "misc/movement.h"

void F::MISC::OnMove(CUserCmd* pCmd, CCSPlayerController* pLocalController, C_CSPlayerPawn* pLocalPawn)
{
	// process movement
	MOVEMENT::OnMove(pCmd, pLocalController, pLocalPawn);
}