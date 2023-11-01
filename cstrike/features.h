#pragma once

#include "common.h"

class CUserCmd;
class CCSPlayerController;
class CMaterialData;

namespace F
{
	void OnPresent();
	void OnFrameStageNotify(int nStage);
	void OnCreateMove(CUserCmd* pCmd, CCSPlayerController* pLocalController);
	bool OnDrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMaterialData* pMaterialData, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2);
}
