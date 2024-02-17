#include "entity.h"

// used: convars
#include "../core/convars.h"
#include "interfaces/cgameentitysystem.h"
#include "interfaces/ienginecvar.h"
#include "interfaces/iengineclient.h"

// used: cmds
#include "../core/sdk.h"

// used: game's definitions, enums
#include "const.h"

// global empty vector for when we can't get the origin
static Vector_t vecEmpty = Vector_t(0, 0, 0);

CCSPlayerController* CCSPlayerController::GetLocalPlayerController()
{
	const int nIndex = I::Engine->GetLocalPlayer();
	return I::GameResourceService->pGameEntitySystem->Get<CCSPlayerController>(nIndex);
}

const Vector_t& CCSPlayerController::GetPawnOrigin()
{
	CBaseHandle hPawn = this->GetPawnHandle();
	if (!hPawn.IsValid())
		return vecEmpty;

	C_CSPlayerPawn* pPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(hPawn);
	if (pPawn == nullptr)
		return vecEmpty;

	return pPawn->GetSceneOrigin();
}

C_BaseEntity* C_BaseEntity::GetLocalPlayer()
{
	const int nIndex = I::Engine->GetLocalPlayer();
	return I::GameResourceService->pGameEntitySystem->Get(nIndex);
}

const Vector_t& C_BaseEntity::GetSceneOrigin()
{
	if (this->GetGameSceneNode())
		return GetGameSceneNode()->GetAbsOrigin();

	return vecEmpty;
}

bool CCSPlayerController::CanShoot(C_CSWeaponBaseGun* pBaseWeapon)
{
	const float flServerTime = TICKS_TO_TIME(this->GetTickBase());
	C_CSPlayerPawn* pPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(this->GetPawnHandle());
	if (!pPawn)
		return false;

	CCSPlayer_WeaponServices* pPlayerWeaponServices = static_cast<CCSPlayer_WeaponServices*>(pPawn->GetWeaponServices());
	if (!pPlayerWeaponServices)
		return false;

	// check is have ammo
	if (pBaseWeapon->GetClip1() <= 0)
		return false;

	// is player ready to shoot
	if (pPlayerWeaponServices->GetNextAttack( ) > flServerTime)
		return false;

	const short nDefinitionIndex = pBaseWeapon->GetAttributeManager()->GetItem()->GetItemDefinitionIndex();
	// check is weapon with burst mode
	if ((nDefinitionIndex == WEAPON_FAMAS || nDefinitionIndex == WEAPON_GLOCK_18) &&
	// check is burst mode
	pBaseWeapon->IsBurstMode() && pBaseWeapon->GetBurstShotsRemaining() > 0)
		return true;

	// is weapon ready to shoot
	if (pBaseWeapon->GetNextPrimaryAttackTick() > this->GetTickBase())
		return false;

	// check for revolver cocking ready
	if (nDefinitionIndex == WEAPON_R8_REVOLVER && pBaseWeapon->GetPostponeFireReadyFrac() > flServerTime)
		return false;

	return true;
}

bool CCSPlayerController::IsFiring(C_CSWeaponBaseGun* pBaseWeapon)
{
	if (!pBaseWeapon)
		return false;

	const float flServerTime = TICKS_TO_TIME(this->GetTickBase());
	const short nDefinitionIndex = pBaseWeapon->GetAttributeManager()->GetItem()->GetItemDefinitionIndex();
	CCSWeaponBaseVData* pWeaponBaseVData = pBaseWeapon->GetWeaponVData();
	if (!pWeaponBaseVData)
		return false;

	if (C_BaseCSGrenade* pGrenade = reinterpret_cast<C_BaseCSGrenade*>(pBaseWeapon); pGrenade != nullptr && pWeaponBaseVData->GetWeaponType() == WEAPONTYPE_GRENADE)
		return !pGrenade->IsPinPulled() && pGrenade->GetThrowTime() > 0.f && pGrenade->GetThrowTime() < flServerTime;
	else if (pWeaponBaseVData->GetWeaponType() == WEAPONTYPE_KNIFE)
		return (SDK::Cmd->nButtons.nValue & (IN_ATTACK) || SDK::Cmd->nButtons.nValue & (IN_SECOND_ATTACK)) && CanShoot(static_cast<C_CSWeaponBaseGun*>(pBaseWeapon));
	else
		return SDK::Cmd->nButtons.nValue & (IN_ATTACK) && CanShoot(pBaseWeapon);
}

bool C_CSPlayerPawn::IsOtherEnemy(C_CSPlayerPawn* pOther)
{
	// check are other player is invalid or we're comparing against ourselves
	if (pOther == nullptr || this == pOther)
		return false;

	if (CONVAR::game_type->value.i32 == GAMETYPE_FREEFORALL && CONVAR::game_mode->value.i32 == GAMEMODE_FREEFORALL_SURVIVAL)
		// check is not teammate
		return (this->GetSurvivalTeam() != pOther->GetSurvivalTeam());

	// @todo: check is deathmatch
	if (CONVAR::mp_teammates_are_enemies->value.i1)
		return true;

	return this->GetAssociatedTeam() != pOther->GetAssociatedTeam();
}

int C_CSPlayerPawn::GetAssociatedTeam()
{
	const int nTeam = this->GetTeam();

	// @todo: check is coaching, currently cs2 doesnt have sv_coaching_enabled, so just let it be for now...
	//if (CONVAR::sv_coaching_enabled->GetBool() && nTeam == TEAM_SPECTATOR)
	//	return this->GetCoachingTeam();

	return nTeam;
}
