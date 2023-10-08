#pragma once
class CUserCmd;
class CCSPlayerController;
class C_CSPlayerPawn;

namespace F::MISC
{
	void OnMove(CUserCmd* pCmd, CCSPlayerController* pLocalController, C_CSPlayerPawn* pLocalPawn);
}