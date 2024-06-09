#include "misc.h"

// used: movement callback
#include "misc/movement.h"

void F::MISC::OnMove(CUserCmd* pCmd, CBaseUserCmdPB* pBaseCmd, CCSPlayerController* pLocalController, C_CSPlayerPawn* pLocalPawn)
{
	// process movement
	MOVEMENT::OnMove(pCmd, pBaseCmd, pLocalController, pLocalPawn);
}
