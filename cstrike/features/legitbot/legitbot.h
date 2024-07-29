#pragma once
class CUserCmd;
class CBaseUserCmdPB;
class CCSPlayerController;
class C_CSPlayerPawn;

namespace F::LEGITBOT
{
	void OnMove(CBaseUserCmdPB* pBaseCmd, CCSPlayerController* pLocalController, C_CSPlayerPawn* pLocalPawn);
}
