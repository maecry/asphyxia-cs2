#include "chams.h"

// used: game's interfaces
#include "../../core/interfaces.h"
#include "../../sdk/interfaces/imaterialsystem.h"
#include "../../sdk/interfaces/igameresourceservice.h"
#include "../../sdk/interfaces/cgameentitysystem.h"
#include "../../core/sdk.h"
#include "../../sdk/entity.h"

// used: original call in hooked function
#include "../../core/hooks.h"

// used: cheat variables
#include "../../core/variables.h"

struct CustomMaterial_t
{
	CMaterial2* pMaterial = nullptr;
	//CMaterial2* pMaterialInvisible = nullptr;

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
		//.pMaterialInvisible = CreateMaterial(CS_XOR("primary_white_invisible"), CS_XOR("materials/dev/primary_white.vmat"), CS_XOR("csgo_unlitgeneric.vfx"), true, true, true)
	};

	arrMaterials[VISUAL_MATERIAL_WIREFRAME] = CustomMaterial_t{
		.pMaterial = CreateMaterial(CS_XOR("wireframe"), CS_XOR("materials/dev/debug_wireframe.vmat"), CS_XOR("csgo_unlitgeneric.vfx"), true, true, false),
		//.pMaterialInvisible = CreateMaterial(CS_XOR("wireframe_invisible"), CS_XOR("materials/dev/debug_wireframe.vmat"), CS_XOR("csgo_unlitgeneric.vfx"), true, true, true)
	};

	bInitialized = true;
	for (auto& [pMaterial] : arrMaterials)
	{
		if (pMaterial == nullptr)
			bInitialized = false;
	}

	return bInitialized;
}

bool F::VISUALS::CHAMS::OnDrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2)
{
	if (!bInitialized)
		return false;

	if (!C_GET(bool, Vars.bVisualChams))
		return false;

	if (arrMeshDraw == nullptr)
		return false;

	if (arrMeshDraw->pSceneAnimatableObject == nullptr)
		return false;

	CBaseHandle hOwner = arrMeshDraw->pSceneAnimatableObject->hOwner;

	auto pEntity = I::GameResourceService->pGameEntitySystem->Get<C_BaseEntity>(hOwner);
	if (pEntity == nullptr)
		return false;

	SchemaClassInfoData_t* pClassInfo;
	pEntity->GetSchemaClassInfo(&pClassInfo);
	if (pClassInfo == nullptr)
		return false;

	if (pEntity->IsViewModel() || pEntity->IsWeapon() || CRT::StringCompare(pClassInfo->szNname, CS_XOR("C_CSPlayerPawn")) != 0)
		return false;

	auto pPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(hOwner);
	if (pPawn == nullptr)
		return false;

	if (!pPawn->IsOtherEnemy(SDK::LocalPawn))
		return false;

	// alive state
	if (pPawn->GetHealth() == 0)
		return false;

	return OverrideMaterial(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);
}

CMaterial2* F::VISUALS::CHAMS::CreateMaterial(const char* szName, const char* szMaterialVMAT, const char* szShaderType, bool bBlendMode, bool bTranslucent, bool bDisableZBuffering)
{
	CMeshData* pData = reinterpret_cast<CMeshData*>(static_cast<std::byte*>(MEM_STACKALLOC(0x200)) + 0x50);
	CMaterial2** pMatPrototype;

	I::MaterialSystem2->FindOrCreateFromResource(&pMatPrototype, szMaterialVMAT);
	if (pMatPrototype == nullptr)
		return nullptr;

	// idk why SetCreateDataByMaterial failed on release build
#ifdef _DEBUG
	I::MaterialSystem2->SetCreateDataByMaterial(pData, &pMatPrototype);
	pData->SetShaderType(szShaderType);

	pData->SetMaterialFunction(CS_XOR("F_DISABLE_Z_BUFFERING"), bDisableZBuffering ? 1 : 0);
	pData->SetMaterialFunction(CS_XOR("F_BLEND_MODE"), bBlendMode ? 1 : 0);
	pData->SetMaterialFunction(CS_XOR("F_TRANSLUCENT"), bTranslucent ? 1 : 0);

	CMaterial2** pMaterial;
	I::MaterialSystem2->CreateMaterial(&pMaterial, szName, pData);
	return *pMaterial;
#endif
	return *pMatPrototype;
}

bool F::VISUALS::CHAMS::OverrideMaterial(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2)
{
	const auto oDrawObject = H::hkDrawObject.GetOriginal();
	const CustomMaterial_t customMaterial = arrMaterials[C_GET(int, Vars.nVisualChamMaterial)];

	arrMeshDraw->pMaterial = customMaterial.pMaterial;
	arrMeshDraw->colValue = C_GET(Color_t, Vars.colVisualChams);
	oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);

	return true;
}
