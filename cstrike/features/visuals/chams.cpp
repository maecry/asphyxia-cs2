#include "chams.h"

// used: game's interfaces
#include "../../core/interfaces.h"
#include "../../sdk/interfaces/imaterialsystem.h"
#include "../../core/sdk.h"
#include "../../sdk/entity.h"

// used: original call in hooked function
#include "../../core/hooks.h"

// used: cheat variables
#include "../../core/variables.h"

struct CustomMaterial_t
{
	CMaterial2* pMaterial = nullptr;
	CMaterial2* pMaterialInvisible = nullptr;

	//bool bIsOwner = false;
};

static CustomMaterial_t arrMaterials[VISUAL_MATERIAL_MAX];

bool F::VISUALS::CHAMS::Initialize()
{
	// check if we already initialized materials
	if (bInitialized)
		return bInitialized;

	arrMaterials[VISUAL_MATERIAL_PRIMARY_WHITE] = CustomMaterial_t{
		.pMaterial = CreateMaterial(CS_XOR("primary_white"), CS_XOR("materials/dev/primary_white.vmat"), CS_XOR("csgo_unlitgeneric.vfx"), true, true, false),
		.pMaterialInvisible = CreateMaterial(CS_XOR("primary_white"), CS_XOR("materials/dev/primary_white.vmat"), CS_XOR("csgo_unlitgeneric.vfx"), true, true, true)
	};

	arrMaterials[VISUAL_MATERIAL_WIREFRAME] = CustomMaterial_t{
		.pMaterial = CreateMaterial(CS_XOR("wireframe"), CS_XOR("materials/dev/debug_wireframe.vmat"), CS_XOR("csgo_unlitgeneric.vfx"), true, true, false),
		.pMaterialInvisible = CreateMaterial(CS_XOR("wireframe"), CS_XOR("materials/dev/debug_wireframe.vmat"), CS_XOR("csgo_unlitgeneric.vfx"), true, true, true)
	};

	bInitialized = true;
	for (auto& [pMaterial, pMaterialInvisible] : arrMaterials)
	{
		if (pMaterial == nullptr || pMaterialInvisible == nullptr)
			bInitialized = false;
	}

	return bInitialized;
}

bool F::VISUALS::CHAMS::OnDrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMaterialData* pMaterialData, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2)
{
	if (!bInitialized)
		return false;

	if (!C_GET(bool, Vars.bVisualChams))
		return false;

	// state showing when we've drawn custom model and don't need to draw original
	// @note: you can also use it to not draw model at all
	bool bDrawnModel = false;

	switch (pMaterialData->pObjectInfo->nId)
	{
	case 113: // ct model
		// no teammate chams
		if (SDK::LocalPawn->GetTeam() == TEAM_CT)
			break;

		bDrawnModel = OverrideMaterial(pAnimatableSceneObjectDesc, pDx11, pMaterialData, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);

		break;
	case 104: // t model
		// no teammate chams
		if (SDK::LocalPawn->GetTeam() == TEAM_TT)
			break;

		bDrawnModel = OverrideMaterial(pAnimatableSceneObjectDesc, pDx11, pMaterialData, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);

		break;
	// case 38: arms model
	//	break;
	default:
		bDrawnModel = false;
		break;
	}

	return bDrawnModel;
}

CMaterial2* F::VISUALS::CHAMS::CreateMaterial(const char* szName, const char* szMaterialVMAT, const char* szShaderType, bool bBlendMode, bool bTranslucent, bool bDisableZBuffering)
{
	CMaterialData* pData = reinterpret_cast<CMaterialData*>(MEM_STACKALLOC(0x1050));
	CMaterial2** pMatPrototype;

	I::MaterialSystem2->FindOrCreateFromResource(&pMatPrototype, szMaterialVMAT);
	if (pMatPrototype == nullptr)
		return nullptr;

	I::MaterialSystem2->SetCreateDataByMaterial(&pMatPrototype, pData);
	pData->SetShaderType(szShaderType);

	pData->SetMaterialFunction(CS_XOR("F_DISABLE_Z_BUFFERING"), bDisableZBuffering ? 1 : 0);
	pData->SetMaterialFunction(CS_XOR("F_BLEND_MODE"), bBlendMode ? 1 : 0);
	pData->SetMaterialFunction(CS_XOR("F_TRANSLUCENT"), bTranslucent ? 1 : 0);

	CMaterial2** pMaterial;
	I::MaterialSystem2->CreateMaterial(&pMaterial, szName, pData);
	return *pMaterial;
}

bool F::VISUALS::CHAMS::OverrideMaterial(void* pAnimatableSceneObjectDesc, void* pDx11, CMaterialData* pMaterialData, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2)
{
	const CustomMaterial_t customMaterial = arrMaterials[C_GET(int, Vars.nVisualChamMaterial)];

	if (C_GET(bool, Vars.bVisualChamsIgnoreZ))
	{
		pMaterialData->pMaterial = customMaterial.pMaterialInvisible;
		pMaterialData->colValue = C_GET(Color_t, Vars.colVisualChamsIgnoreZ);
		H::hkDrawObject.CallOriginal(pAnimatableSceneObjectDesc, pDx11, pMaterialData, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);
	}

	pMaterialData->pMaterial = customMaterial.pMaterial;
	pMaterialData->colValue = C_GET(Color_t, Vars.colVisualChams);
	H::hkDrawObject.CallOriginal(pAnimatableSceneObjectDesc, pDx11, pMaterialData, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);

	return true;
}
