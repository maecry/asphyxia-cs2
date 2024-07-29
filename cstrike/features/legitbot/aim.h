#pragma once

class CUserCmd;
class CBaseUserCmdPB;
class CCSGOInputHistoryEntryPB;

class CCSPlayerController;
class C_CSPlayerPawn;

struct QAngle_t;

namespace F::LEGITBOT::AIM
{
	void OnMove(CUserCmd* pCmd, CBaseUserCmdPB* pBaseCmd, CCSPlayerController* pLocalController, C_CSPlayerPawn* pLocalPawn);

	void AimAssist(CBaseUserCmdPB* pUserCmd, C_CSPlayerPawn* pLocalPawn, CCSPlayerController* pLocalController);
}
