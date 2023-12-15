//#include "antiaim.h"
//#include <ctime>
//#include <cmath>
//#include "../core/variables.h"
//
//void antiaim::RunAA(CUserCmd* pCmd)
//{
//	if (!C_GET(bool, Vars.bAntiAim)) // Checking if anti aim is enabled
//		return;
//
//	if (!I::Engine->IsConnected() || !I::Engine->IsInGame()) // Checking if you are connected and in game
//	{
//		return;
//	}
//
//	if (!SDK::LocalController || !SDK::LocalController->IsPawnAlive()) // Checking if your spectating and alive
//	{
//		return;
//	}
//
//	if (SDK::Cmd->nButtons.nValue & ECommandButtons::IN_USE || SDK::Cmd->nButtons.nValue & ECommandButtons::IN_ATTACK)
//	{
//		return;
//	}
//
//	if (int32_t nMoveType = SDK::LocalController->GetMoveType(); nMoveType == MOVETYPE_NOCLIP || nMoveType == MOVETYPE_LADDER)
//		return;
//
//	C_CSPlayerPawn* pLocalPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(SDK::LocalController->GetPawnHandle());
//
//	if (pLocalPawn == nullptr)
//		return;
//
//	QAngle_t engine_angles;
//	I::Input->GetViewAngle(engine_angles);
//
//	static float yaw_base = pCmd->pBaseCmd->pCmdView->angValue.y;
//	static float pitch_base = pCmd->pBaseCmd->pCmdView->angValue.x;
//
//	switch (C_GET(int, Vars.bBaseYawType))
//	{
//	case 0:
//		return;
//		break;
//	case 1:
//		yaw_base += -180.f; // Backwards
//		break;
//	case 2:
//		yaw_base += 180.f; // Forwards
//		break;
//	default:
//		break;
//	}
//
//	switch (C_GET(int, Vars.bPitchType))
//	{
//	case 0:
//		break;
//	case 1:// Down
//		pitch_base = 120.f;
//		break;
//	case 2:// Up
//		pitch_base = -120.f;
//		break;
//	case 3:// Flick
//		pitch_base += std::round(MATH::random_angle(120.f, -120.f));
//		break;
//	default:
//		break;
//	}
//
//	yaw_base = C_GET(int, Vars.bBaseYawType);
//
//	int jitter_range = C_GET(int, Vars.bJitterRange) * 0.8f;
//
//	switch (C_GET(int, Vars.bJitterType))
//	{
//	case 0:
//		yaw_base += MATH::random_angle(-jitter_range, jitter_range); // center jitter
//		break;
//	case 1: // 3-way jitter
//		// Coming soon
//		break;
//	case 2:// Spin
//		// Coming soon
//		break;
//	default:
//		break;
//	}
//
//
//	pCmd->SetSubTickAngle(pCmd->pBaseCmd->pCmdView->angValue);
//	pLocalPawn->GetLowerBodyYawTarget() = MATH::random_angle(-FLT_MAX, FLT_MAX);
//	pCmd->pBaseCmd->pCmdView->angValue.y = MATH::normalize_yaw(yaw_base);
//	pCmd->pBaseCmd->pCmdView->angValue.x = MATH::normalize_yaw(pitch_base);
//}
