#pragma once

#include "common.h"

class CUserCmd;
class CCSPlayerController;

namespace F
{
	void OnPresent();
	void OnFrameStageNotify(int nStage);
	void OnCreateMove(CUserCmd* pCmd, CCSPlayerController* pLocalController);
}