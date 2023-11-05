#pragma once

class CMaterial2;
class CMaterialData;

namespace F::VISUALS::CHAMS
{
	bool Initialize();
	bool OnDrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMaterialData* pMaterialData, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2);

	// @note: bDisableZBuffering == true to create invisible material
	CMaterial2* CreateMaterial(const char* szName, const char* szMaterialVMAT, const char* szShaderType, bool bBlendMode, bool bTranslucent, bool bDisableZBuffering);

	bool OverrideMaterial(void* pAnimatableSceneObjectDesc, void* pDx11, CMaterialData* pMaterialData, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2);

	inline bool bInitialized = false;
}
