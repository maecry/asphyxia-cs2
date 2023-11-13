#include "visuals.h"

// used: source sdk
#include "../sdk/interfaces/iengineclient.h"
#include "../sdk/entity.h"

// used: overlay
#include "visuals/overlay.h"
#include "visuals/chams.h"

#include "../core/sdk.h"

using namespace F;

bool F::VISUALS::Setup()
{
	if (!CHAMS::Initialize())
	{
		L_PRINT(LOG_ERROR) << CS_XOR("failed to initialize chams");
		return false;
	}

	return true;
}

void F::VISUALS::OnDestroy()
{
	CHAMS::Destroy();
}

void VISUALS::OnFrame(const int nStage)
{
	if (nStage == FRAME_RENDER_END)
	{
		// check is render initialized
		if (!D::bInitialized)
			return;

		/*
		 * game and our gui are based on immediate render mode principe
		 * this means that we should always reset draw data from previous frame and re-store it again
		 */
		D::ResetDrawData();

		if (CCSPlayerController* pLocal = CCSPlayerController::GetLocalPlayerController(); pLocal != nullptr)
		{
			OVERLAY::OnFrameStageNotify(pLocal);
		}

		D::SwapDrawData();
	}
}

bool F::VISUALS::OnDrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2)
{
	return CHAMS::OnDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);
}
