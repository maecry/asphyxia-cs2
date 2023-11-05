#pragma once

class CMaterialData;

namespace F::VISUALS
{
	bool Setup();
	void OnFrame(const int nStage);
	bool OnDrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMaterialData* pMaterialData, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2);
}
