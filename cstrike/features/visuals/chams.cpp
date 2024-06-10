#include "chams.h"

// used: game's interfaces
#include "../../core/interfaces.h"
#include "../../sdk/interfaces/imaterialsystem.h"
#include "../../sdk/interfaces/igameresourceservice.h"
#include "../../sdk/interfaces/cgameentitysystem.h"
#include "../../sdk/interfaces/iresourcesystem.h"
#include "../../core/sdk.h"
#include "../../sdk/entity.h"

// used: original call in hooked function
#include "../../core/hooks.h"

// used: cheat variables
#include "../../core/variables.h"

CStrongHandle<CMaterial2> F::VISUALS::CHAMS::CreateMaterial(const char* szMaterialName, const char szVmatBuffer[])
{
	CKeyValues3* pKeyValues3 = CKeyValues3::CreateMaterialResource();
	pKeyValues3->LoadFromBuffer(szVmatBuffer);

	CStrongHandle<CMaterial2> pCustomMaterial = {};
	MEM::fnCreateMaterial(nullptr, &pCustomMaterial, szMaterialName, pKeyValues3, 0, 1);

	return pCustomMaterial;
}

struct CustomMaterial_t
{
	CStrongHandle<CMaterial2> pMaterial;
	CStrongHandle<CMaterial2> pMaterialInvisible;
};

static CustomMaterial_t arrMaterials[VISUAL_MATERIAL_MAX];

bool F::VISUALS::CHAMS::Initialize()
{
	// check if we already initialized materials
	if (bInitialized)
		return bInitialized;

	arrMaterials[VISUAL_MATERIAL_PRIMARY_WHITE] = CustomMaterial_t{
		.pMaterial = CreateMaterial(CS_XOR("materials/dev/primary_white.vmat"), szVMatBufferWhiteVisible),
		.pMaterialInvisible = CreateMaterial(CS_XOR("materials/dev/primary_white.vmat"), szVMatBufferWhiteInvisible)
	};

	arrMaterials[VISUAL_MATERIAL_ILLUMINATE] = CustomMaterial_t{
		.pMaterial = CreateMaterial(CS_XOR("materials/dev/primary_white.vmat"), szVMatBufferIlluminateVisible),
		.pMaterialInvisible = CreateMaterial(CS_XOR("materials/dev/primary_white.vmat"), szVMatBufferIlluminateInvisible)
	};

	bInitialized = true;
	for (auto& [pMaterial, pMaterialInvisible] : arrMaterials)
	{
		if (pMaterial == nullptr || pMaterialInvisible == nullptr)
			bInitialized = false;
	}

	return bInitialized;
}

void F::VISUALS::CHAMS::Destroy()
{
	for (auto& [pVisible, pInvisible] : arrMaterials)
	{
		if (pVisible)
			I::ResourceHandleUtils->DeleteResource(pVisible.pBinding);

		if (pInvisible)
			I::ResourceHandleUtils->DeleteResource(pInvisible.pBinding);
	}
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

	if (CRT::StringCompare(pClassInfo->szName, CS_XOR("C_CSPlayerPawn")) != 0)
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

bool F::VISUALS::CHAMS::OverrideMaterial(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2)
{
	const auto oDrawObject = H::hkDrawObject.GetOriginal();
	const CustomMaterial_t customMaterial = arrMaterials[C_GET(int, Vars.nVisualChamMaterial)];

	if (C_GET(bool, Vars.bVisualChamsIgnoreZ))
	{
		arrMeshDraw->pMaterial = customMaterial.pMaterialInvisible;
		arrMeshDraw->colValue = C_GET(Color_t, Vars.colVisualChamsIgnoreZ);
		oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);
	}

	arrMeshDraw->pMaterial = customMaterial.pMaterial;
	arrMeshDraw->colValue = C_GET(Color_t, Vars.colVisualChams);
	oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);

	return true;
}
