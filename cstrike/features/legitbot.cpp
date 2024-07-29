#include "legitbot.h"

// used: movement callback
#include "legitbot/aim.h"

void F::LEGITBOT::OnMove(CUserCmd* pCmd, CBaseUserCmdPB* pBaseCmd, CCSPlayerController* pLocalController, C_CSPlayerPawn* pLocalPawn)
{
	AIM::OnMove(pCmd, pBaseCmd, pLocalController, pLocalPawn);
}
