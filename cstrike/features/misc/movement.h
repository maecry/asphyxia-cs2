#pragma once

class CUserCmd;
class CUserCmdBase;
class CSubTickCmd;

class CCSPlayerController;
class C_CSPlayerPawn;

struct QAngle_t;

namespace F::MISC::MOVEMENT
{
	void OnMove(CUserCmd* pCmd, CCSPlayerController* pLocalController, C_CSPlayerPawn* pLocalPawn);

	void BunnyHop(CUserCmd* pCmd, CUserCmdBase* pUserCmd, C_CSPlayerPawn* pLocalPawn);
	void MovementCorrection(CUserCmdBase* pUserCmd, CSubTickCmd* pSubTickCmd, const QAngle_t& angDesiredViewPoint);

	// will call MovementCorrection && validate user's angView to avoid untrusted ban
	void ValidateUserCommand(CUserCmd* pCmd, CUserCmdBase* pUserCmd, CSubTickCmd* pSubTickCmd);
}