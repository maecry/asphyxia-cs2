#pragma once

#include "../../sdk/datatypes/stronghandle.h"



class CMaterial2;
class CMeshData;

namespace F::VISUALS::CHAMS
{
	bool Initialize();
	void Destroy();

	bool OnDrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2);

	// @note: bDisableZBuffering == true to create invisible material
#ifdef TEST_CREATE_CHAMS
	CStrongHandle<CMaterial2> CreateMaterial(const char* szName);
	CStrongHandle<CMaterial2> CreateMaterialInvis(const char* szName);
#endif
	CMaterial2* CreateMaterial(const char* szName, const char* szMaterialVMAT, const char* szShaderType, bool bBlendMode, bool bTranslucent, bool bDisableZBuffering);

	bool OverrideMaterial(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2);

	inline bool bInitialized = false;
}
