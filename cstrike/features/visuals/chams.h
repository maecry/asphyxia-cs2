#pragma once

class CMaterial2;
class CMeshData;

namespace F::VISUALS::CHAMS
{
	bool Initialize();
	bool OnDrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2);

	// @note: bDisableZBuffering == true to create invisible material
	CMaterial2* CreateMaterial(const char* szName, const char* szMaterialVMAT, const char* szShaderType, bool bBlendMode, bool bTranslucent, bool bDisableZBuffering);

	bool OverrideMaterial(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2);

	inline bool bInitialized = false;
}
