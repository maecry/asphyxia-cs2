#include "entity.h"

// used: convars
#include "../core/convars.h"
#include "interfaces/cgameentitysystem.h"
#include "interfaces/ienginecvar.h"
#include "interfaces/iengineclient.h"

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
	CBaseHandle hPawnHandle = this->GetPawnHandle();
	if (!hPawnHandle.IsValid())
		return vecEmpty;

	C_CSPlayerPawn* pPlayerPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(hPawnHandle);
	if (pPlayerPawn == nullptr)
		return vecEmpty;

	return pPlayerPawn->GetSceneOrigin();
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

bool C_CSPlayerPawn::CanAttack(const float flServerTime)
{
	// check is player ready to attack
	if (CCSPlayer_WeaponServices* pWeaponServices = this->GetWeaponServices(); pWeaponServices != nullptr)
		if (this->IsWaitForNoAttack() || pWeaponServices->GetNextAttack() > flServerTime)
			return false;

	return true;
}


std::uint32_t C_CSPlayerPawn::GetOwnerHandleIndex()
{
	std::uint32_t Result = -1;
	if (this && GetCollision() && !(GetCollision()->GetSolidFlags() & 4))
		Result = this->GetOwnerHandle().GetEntryIndex();

	return Result;
}

std::uint16_t C_CSPlayerPawn::GetCollisionMask()
{
	if (this && GetCollision())
		return GetCollision()->CollisionMask(); // Collision + 0x38

	return 0;
}

bool C_CSWeaponBaseGun::CanPrimaryAttack(const int nWeaponType, const float flServerTime)
{
	// check are weapon support burst mode and it's ready to attack
	if (this->IsBurstMode())
	{
		// check is it ready to attack
		if (this->GetBurstShotsRemaining() > 0 /*&& this->GetNextBurstShotTime() <= flServerTime*/)
			return true;
	}

		// check is weapon ready to attack
	if (this->GetNextPrimaryAttackTick() > TIME_TO_TICKS(flServerTime))
		return false;

	// we doesn't need additional checks for knives
	if (nWeaponType == WEAPONTYPE_KNIFE)
		return true;

	// check do weapon have ammo
	if (this->GetClip1() <= 0)
		return false;

	const ItemDefinitionIndex_t nDefinitionIndex = this->GetAttributeManager()->GetItem()->GetItemDefinitionIndex();

	// check for revolver cocking ready
	if (nDefinitionIndex == WEAPON_R8_REVOLVER && this->GetPostponeFireReadyFrac() > flServerTime)
		return false;

	return true;
}

bool C_CSWeaponBaseGun::CanSecondaryAttack(const int nWeaponType, const float flServerTime)
{
	// check is weapon ready to attack
	if (this->GetNextSecondaryAttackTick() > TIME_TO_TICKS(flServerTime))
		return false;

	// we doesn't need additional checks for knives
	if (nWeaponType == WEAPONTYPE_KNIFE)
		return true;

	// check do weapon have ammo
	if (this->GetClip1() <= 0)
		return false;

	// only revolver is allowed weapon for secondary attack
	if (this->GetAttributeManager()->GetItem()->GetItemDefinitionIndex() != WEAPON_R8_REVOLVER)
		return false;

	return true;
}
