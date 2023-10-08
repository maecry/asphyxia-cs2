#pragma once

// @test: using interfaces in the header | not critical but could blow up someday with thousands of errors or affect to compilation time etc
// used: cgameentitysystem, ischemasystem
#include "../core/interfaces.h"
#include "interfaces/igameresourceservice.h"
#include "interfaces/ischemasystem.h"

// used: schema field
#include "../core/schema.h"

// used: l_print
#include "../utilities/log.h"
// used: vector_t
#include "datatypes/vector.h"
// used: qangle_t
#include "datatypes/qangle.h"
// used: ctransform
#include "datatypes/transform.h"

// used: cbasehandle
#include "entity_handle.h"
// used: game's definitions
#include "const.h"

using GameTime_t = std::int32_t;
using GameTick_t = std::int32_t;

class CEntityInstance;

class CEntityIdentity
{
public:
	CS_CLASS_NO_INITIALIZER(CEntityIdentity);

	// @note: handle index is not entity index
	SCHEMA_ADD_OFFSET(std::uint32_t, GetIndex, 0x10);
	SCHEMA_ADD_FIELD(const char*, GetDesignerName, CS_XOR("CEntityIdentity->m_designerName"));
	SCHEMA_ADD_FIELD(std::uint32_t, GetFlags, CS_XOR("CEntityIdentity->m_flags"));

	[[nodiscard]] bool IsValid()
	{
		return GetIndex() != INVALID_EHANDLE_INDEX;
	}

	[[nodiscard]] int GetEntryIndex()
	{
		if (!IsValid())
			return ENT_ENTRY_MASK;
		
		return GetIndex() & ENT_ENTRY_MASK;
	}

	[[nodiscard]] int GetSerialNumber()
	{
		return GetIndex() >> NUM_SERIAL_NUM_SHIFT_BITS;
	}

	CEntityInstance* pInstance; // 0x00
};

class CEntityInstance
{
public:
	CS_CLASS_NO_INITIALIZER(CEntityInstance);

	void GetSchemaClassInfo(SchemaClassInfoData_t** pReturn)
	{
		return MEM::CallVFunc<void, 34U>(this, pReturn);
	}

	[[nodiscard]] CBaseHandle GetRefEHandle()
	{
		CEntityIdentity* pIdentity = GetIdentity();
		if (pIdentity == nullptr)
			return CBaseHandle();

		return CBaseHandle(pIdentity->GetEntryIndex(), pIdentity->GetSerialNumber() - (pIdentity->GetFlags() & 1));
	}

	SCHEMA_ADD_FIELD(CEntityIdentity*, GetIdentity, CS_XOR("CEntityInstance->m_pEntity"));
};

class CCollisionProperty
{
public:
	CS_CLASS_NO_INITIALIZER(CCollisionProperty);

	SCHEMA_ADD_FIELD(Vector_t, GetMins, CS_XOR("CCollisionProperty->m_vecMins"));
	SCHEMA_ADD_FIELD(Vector_t, GetMaxs, CS_XOR("CCollisionProperty->m_vecMaxs"));
};

class CGameSceneNode
{
public:
	CS_CLASS_NO_INITIALIZER(CGameSceneNode);

	SCHEMA_ADD_FIELD(CTransform, GetNodeToWorld, CS_XOR("CGameSceneNode->m_nodeToWorld"));
	SCHEMA_ADD_FIELD(CEntityInstance*, GetOwner, CS_XOR("CGameSceneNode->m_pOwner"));

	SCHEMA_ADD_FIELD(Vector_t, GetAbsOrigin, CS_XOR("CGameSceneNode->m_vecAbsOrigin"));
	SCHEMA_ADD_FIELD(Vector_t, GetRenderOrigin, CS_XOR("CGameSceneNode->m_vRenderOrigin"));

	SCHEMA_ADD_FIELD(QAngle_t, GetAngleRotation, CS_XOR("CGameSceneNode->m_angRotation"));
	SCHEMA_ADD_FIELD(QAngle_t, GetAbsAngleRotation, CS_XOR("CGameSceneNode->m_angAbsRotation"));

	SCHEMA_ADD_FIELD(bool, IsDormant, CS_XOR("CGameSceneNode->m_bDormant"));
};

class C_BaseEntity : public CEntityInstance
{
public:
	CS_CLASS_NO_INITIALIZER(C_BaseEntity);

	// @note: we shouldn't use this, just use GetSchemaClassInfo(...) and compare the class name instead
	[[nodiscard]] bool IsBasePlayerController()
	{
		return MEM::CallVFunc<bool, 144U>(this);
	}

	// same with this
	[[nodiscard]] bool IsWeapon()
	{
		return MEM::CallVFunc<bool, 149U>(this);
	}

	// same with this
	[[nodiscard]] bool IsViewModel()
	{
		return MEM::CallVFunc<bool, 242U>(this);
	}

	static C_BaseEntity* GetLocalPlayer();

	// get entity origin on scene
	[[nodiscard]] const Vector_t& GetSceneOrigin();

	SCHEMA_ADD_FIELD(CGameSceneNode*, GetGameSceneNode, CS_XOR("C_BaseEntity->m_pGameSceneNode"));
	SCHEMA_ADD_FIELD(CCollisionProperty*, GetCollision, CS_XOR("C_BaseEntity->m_pCollision"));
	SCHEMA_ADD_FIELD(std::uint8_t, GetTeam, CS_XOR("C_BaseEntity->m_iTeamNum"));
	SCHEMA_ADD_FIELD(CBaseHandle, GetOwnerHandle, CS_XOR("C_BaseEntity->m_hOwnerEntity"));
	SCHEMA_ADD_FIELD(Vector_t, GetBaseVelocity, CS_XOR("C_BaseEntity->m_vecBaseVelocity"));
	SCHEMA_ADD_FIELD(Vector_t, GetAbsVelocity, CS_XOR("C_BaseEntity->m_vecAbsVelocity"));
	SCHEMA_ADD_FIELD(bool, IsTakingDamage, CS_XOR("C_BaseEntity->m_bTakesDamage"));
	SCHEMA_ADD_FIELD(std::uint32_t, GetFlags, CS_XOR("C_BaseEntity->m_fFlags"));
	SCHEMA_ADD_FIELD(std::int32_t, GetEflags, CS_XOR("C_BaseEntity->m_iEFlags"));
	SCHEMA_ADD_FIELD(std::int32_t, GetMoveType, CS_XOR("C_BaseEntity->m_MoveType"));
	SCHEMA_ADD_FIELD(std::uint8_t, GetLifeState, CS_XOR("C_BaseEntity->m_lifeState"));
	SCHEMA_ADD_FIELD(std::int32_t, GetHealth, CS_XOR("C_BaseEntity->m_iMaxHealth"));
	SCHEMA_ADD_FIELD(std::int32_t, GetMaxHealth, CS_XOR("C_BaseEntity->m_iHealth"));
	SCHEMA_ADD_FIELD(float, GetWaterLevel, CS_XOR("C_BaseEntity->m_flWaterLevel"));
};

class CGlowProperty;

class C_BaseModelEntity : public C_BaseEntity
{
public:
	CS_CLASS_NO_INITIALIZER(C_BaseModelEntity);

	SCHEMA_ADD_FIELD(CCollisionProperty, GetCollisionInstance, CS_XOR("C_BaseModelEntity->m_Collision"));
	SCHEMA_ADD_FIELD(CGlowProperty, GetGlowProperty, CS_XOR("C_BaseModelEntity->m_Glow"));
	SCHEMA_ADD_FIELD(Vector_t, GetViewOffset, CS_XOR("C_BaseModelEntity->m_vecViewOffset"));
	SCHEMA_ADD_FIELD(GameTime_t, GetCreationTime, CS_XOR("C_BaseModelEntity->m_flCreateTime"));
	SCHEMA_ADD_FIELD(GameTick_t, GetCreationTick, CS_XOR("C_BaseModelEntity->m_nCreationTick"));
	SCHEMA_ADD_FIELD(CBaseHandle, GetMoveParent, CS_XOR("C_BaseModelEntity->m_hOldMoveParent"));
	SCHEMA_ADD_FIELD(std::float_t, GetAnimTime, CS_XOR("C_BaseModelEntity->m_flAnimTime"));
	SCHEMA_ADD_FIELD(std::float_t, GetSimulationTime, CS_XOR("C_BaseModelEntity->m_flSimulationTime"));
};

class CPlayer_WeaponServices;
class CPlayer_ItemServices;
class CPlayer_CameraServices;

class C_BasePlayerPawn : public C_BaseModelEntity
{
public:
	CS_CLASS_NO_INITIALIZER(C_BasePlayerPawn);

	SCHEMA_ADD_FIELD(CBaseHandle, GetControllerHandle, CS_XOR("C_BasePlayerPawn->m_hController"));
	SCHEMA_ADD_FIELD(CPlayer_WeaponServices*, GetWeaponServices, CS_XOR("C_BasePlayerPawn->m_pWeaponServices"));
	SCHEMA_ADD_FIELD(CPlayer_ItemServices*, GetItemServices, CS_XOR("C_BasePlayerPawn->m_pItemServices"));
	SCHEMA_ADD_FIELD(CPlayer_CameraServices*, GetCameraServices, CS_XOR("C_BasePlayerPawn->m_pCameraServices"));
};

class CCSPlayer_ViewModelServices;

class C_CSPlayerPawnBase : public C_BasePlayerPawn
{
public:
	CS_CLASS_NO_INITIALIZER(C_CSPlayerPawnBase);

	SCHEMA_ADD_FIELD(CCSPlayer_ViewModelServices*, GetViewModelServices, CS_XOR("C_CSPlayerPawnBase->m_pViewModelServices"));
	SCHEMA_ADD_FIELD(bool, IsScoped, CS_XOR("C_CSPlayerPawnBase->m_bIsScoped"));
	SCHEMA_ADD_FIELD(bool, IsDefusing, CS_XOR("C_CSPlayerPawnBase->m_bIsDefusing"));
	SCHEMA_ADD_FIELD(bool, IsGrabbingHostage, CS_XOR("C_CSPlayerPawnBase->m_bIsGrabbingHostage"));
	SCHEMA_ADD_FIELD(float, GetLowerBodyYawTarget, CS_XOR("C_CSPlayerPawnBase->m_flLowerBodyYawTarget"));
	SCHEMA_ADD_FIELD(int, GetShotsFired, CS_XOR("C_CSPlayerPawnBase->m_iShotsFired"));
	SCHEMA_ADD_FIELD(float, GetFlashMaxAlpha, CS_XOR("C_CSPlayerPawnBase->m_flFlashMaxAlpha"));
	SCHEMA_ADD_FIELD(float, GetFlashDuration, CS_XOR("C_CSPlayerPawnBase->m_flFlashDuration"));
	SCHEMA_ADD_FIELD(Vector_t, GetLastSmokeOverlayColor, CS_XOR("C_CSPlayerPawnBase->m_vLastSmokeOverlayColor"));
	SCHEMA_ADD_FIELD(int, GetSurvivalTeam, CS_XOR("C_CSPlayerPawnBase->m_nSurvivalTeam")); // danger zone
};

class C_CSPlayerPawn : public C_CSPlayerPawnBase
{
public:
	CS_CLASS_NO_INITIALIZER(C_CSPlayerPawn);

	[[nodiscard]] bool IsOtherEnemy(C_CSPlayerPawn* pOther);
	[[nodiscard]] int GetAssociatedTeam();
};

class CBasePlayerController : public C_BaseModelEntity
{
public:
	CS_CLASS_NO_INITIALIZER(CBasePlayerController);

	SCHEMA_ADD_FIELD(std::uint64_t, GetSteamId, CS_XOR("CBasePlayerController->m_steamID"));
	SCHEMA_ADD_FIELD(CBaseHandle, GetPawnHandle, CS_XOR("CBasePlayerController->m_hPawn"));
	SCHEMA_ADD_FIELD(bool, IsLocalPlayerController, CS_XOR("CBasePlayerController->m_bIsLocalPlayerController"));
};

class CCSPlayerController : public CBasePlayerController
{
public:
	CS_CLASS_NO_INITIALIZER(CCSPlayerController);

	[[nodiscard]] static CCSPlayerController* GetLocalPlayerController();

	// @note: always get origin from pawn not controller
	[[nodiscard]] const Vector_t& GetPawnOrigin();

	SCHEMA_ADD_FIELD(std::uint32_t, GetPing, CS_XOR("CCSPlayerController->m_iPing"));
	SCHEMA_ADD_FIELD(const char*, GetPlayerName, CS_XOR("CCSPlayerController->m_sSanitizedPlayerName"));
	SCHEMA_ADD_FIELD(std::int32_t, GetPawnHealth, CS_XOR("CCSPlayerController->m_iPawnHealth"));
	SCHEMA_ADD_FIELD(std::int32_t, GetPawnArmor, CS_XOR("CCSPlayerController->m_iPawnArmor"));
	SCHEMA_ADD_FIELD(bool, IsPawnHasDefuser, CS_XOR("CCSPlayerController->m_bPawnHasDefuser"));
	SCHEMA_ADD_FIELD(bool, IsPawnHasHelmet, CS_XOR("CCSPlayerController->m_bPawnHasHelmet"));
	SCHEMA_ADD_FIELD(bool, IsPawnAlive, CS_XOR("CCSPlayerController->m_bPawnIsAlive"));
	SCHEMA_ADD_FIELD(CBaseHandle, GetPlayerPawnHandle, CS_XOR("CCSPlayerController->m_hPlayerPawn"));
};
