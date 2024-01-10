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

struct CustomMaterial_t
{
	CMaterial2* pMaterial;
	CMaterial2* pMaterialInvisible;

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
		.pMaterialInvisible = CreateMaterial(CS_XOR("primary_white_invisible"), CS_XOR("materials/dev/primary_white.vmat"), CS_XOR("csgo_unlitgeneric.vfx"), true, true, true)
	};

	arrMaterials[VISUAL_MATERIAL_WIREFRAME] = CustomMaterial_t{
		.pMaterial = CreateMaterial(CS_XOR("wireframe"), CS_XOR("materials/dev/debug_wireframe.vmat"), CS_XOR("csgo_unlitgeneric.vfx"), true, true, false),
		.pMaterialInvisible = CreateMaterial(CS_XOR("wireframe_invisible"), CS_XOR("materials/dev/debug_wireframe.vmat"), CS_XOR("csgo_unlitgeneric.vfx"), true, true, true)
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
#ifdef TEST_CREATE_CHAMS
	for (auto& [pMaterial] : arrMaterials)
	{
		if (pMaterial != nullptr)
			I::ResourceHandleUtils->DeleteResource(pMaterial.pBinding);
	}
#endif
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

#ifdef TEST_CREATE_CHAMS
CStrongHandle<CMaterial2> F::VISUALS::CHAMS::CreateMaterial(const char* szName)
{
	const char szVmatBuffer[] = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
	{
		Shader = "csgo_unlitgeneric.vfx"
		F_DISABLE_Z_BUFFERING = 0
		g_flBumpStrength = 1
		g_vColorTint = [1.000000, 1.000000, 1.000000, 0.000000]
		g_vGlossinessRange = [0.000000, 1.000000, 0.000000, 0.000000]
		g_vNormalTexCoordScale = [1.000000, 1.000000, 0.000000, 0.000000]
		g_vTexCoordOffset = [0.000000, 0.000000, 0.000000, 0.000000]
		g_vTexCoordScale = [1.000000, 1.000000, 0.000000, 0.000000]
		g_tColor = resource:"materials/dev/primary_white_color_tga_f7b257f6.vtex"
		g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
		g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
	})";

	auto buffer = new unsigned char[0x100 + sizeof(CKeyValues3)];
	CRT::MemorySet(buffer, 0, sizeof(buffer));
	CKeyValues3* kv3 = (CKeyValues3*)(buffer + 0x100);

	KV3ID_t kv3ID;
	kv3ID.szName = "generic";
	kv3ID.unk0 = 0x469806E97412167C;
	kv3ID.unk1 = 0xE73790B53EE6F2AF;

	if (!MEM::fnLoadKV3(kv3, nullptr, szVmatBuffer, &kv3ID, nullptr))
		throw;

	CStrongHandle<CMaterial2> custom_material;
	MEM::fnCreateMaterial(nullptr, &custom_material, szName, kv3, 0, 1);

	return custom_material;
}

CStrongHandle<CMaterial2> F::VISUALS::CHAMS::CreateMaterialInvis(const char* szName)
{
	const char szVmatBuffer[] = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
	{
		Shader = "csgo_unlitgeneric.vfx"
		F_DISABLE_Z_BUFFERING = 1
		g_flBumpStrength = 1
		g_vColorTint = [1.000000, 1.000000, 1.000000, 0.000000]
		g_vGlossinessRange = [0.000000, 1.000000, 0.000000, 0.000000]
		g_vNormalTexCoordScale = [1.000000, 1.000000, 0.000000, 0.000000]
		g_vTexCoordOffset = [0.000000, 0.000000, 0.000000, 0.000000]
		g_vTexCoordScale = [1.000000, 1.000000, 0.000000, 0.000000]
		g_tColor = resource:"materials/dev/primary_white_color_tga_f7b257f6.vtex"
		g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
		g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
	})";

	auto buffer = new unsigned char[0x100 + sizeof(CKeyValues3)];
	CRT::MemorySet(buffer, 0, sizeof(buffer));
	CKeyValues3* kv3 = (CKeyValues3*)(buffer + 0x100);

	KV3ID_t kv3ID;
	kv3ID.szName = "csgo_unlitgeneric2";
	kv3ID.unk0 = 0x469806E97412167C;
	kv3ID.unk1 = 0xE73790B53EE6F2AF;

	if (!MEM::fnLoadKV3(kv3, nullptr, szVmatBuffer, &kv3ID, nullptr))
		throw;

	CStrongHandle<CMaterial2> custom_material;
	MEM::fnCreateMaterial(nullptr, &custom_material, szName, kv3, 0, 1);

	return custom_material;
}
#endif

CMaterial2* F::VISUALS::CHAMS::CreateMaterial(const char* szName, const char* szMaterialVMAT, const char* szShaderType, bool bBlendMode, bool bTranslucent, bool bDisableZBuffering)
{
	CMeshData* pData = reinterpret_cast<CMeshData*>(static_cast<std::byte*>(I::MemAlloc->Alloc(0x200)) + 0x50);
	CMaterial2** pMatPrototype;

	I::MaterialSystem2->FindOrCreateFromResource(&pMatPrototype, szMaterialVMAT);
	if (pMatPrototype == nullptr)
		return nullptr;
	// @note: SetCreateDataByMaterial failed on release build idk why
#ifdef _DEBUG
	I::MaterialSystem2->SetCreateDataByMaterial(pData, &pMatPrototype);
	pData->SetShaderType(szShaderType);

	pData->SetMaterialFunction(CS_XOR("F_DISABLE_Z_BUFFERING"), bDisableZBuffering ? 1 : 0);
	pData->SetMaterialFunction(CS_XOR("F_BLEND_MODE"), bBlendMode ? 1 : 0);
	pData->SetMaterialFunction(CS_XOR("F_TRANSLUCENT"), bTranslucent ? 1 : 0);

	CMaterial2** pMaterial;
	I::MaterialSystem2->CreateMaterial(&pMaterial, szName, pData);
	I::MemAlloc->Free(pData);
	return *pMaterial;
#endif
	return *pMatPrototype;
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
