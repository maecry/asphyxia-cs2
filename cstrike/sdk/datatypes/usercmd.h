#pragma once

// usd: qangle_t
#include "qangle.h"

// @source: master/game/shared/in_buttons.h
enum ECommandButtons : int
{
	IN_ATTACK = (1 << 0),
	IN_JUMP = (1 << 1),
	IN_DUCK = (1 << 2),
	IN_FORWARD = (1 << 3),
	IN_BACK = (1 << 4),
	IN_USE = (1 << 5),
	IN_CANCEL = (1 << 6),
	IN_LEFT = (1 << 7),
	IN_RIGHT = (1 << 8),
	IN_MOVELEFT = (1 << 9),
	IN_MOVERIGHT = (1 << 10),
	IN_SECOND_ATTACK = (1 << 11),
	IN_RUN = (1 << 12),
	IN_RELOAD = (1 << 13),
	IN_LEFT_ALT = (1 << 14),
	IN_RIGHT_ALT = (1 << 15),
	IN_SCORE = (1 << 16),
	IN_SPEED = (1 << 17),
	IN_WALK = (1 << 18),
	IN_ZOOM = (1 << 19),
	IN_FIRST_WEAPON = (1 << 20),
	IN_SECOND_WEAPON = (1 << 21),
	IN_BULLRUSH = (1 << 22),
	IN_FIRST_GRENADE = (1 << 23),
	IN_SECOND_GRENADE = (1 << 24),
	IN_MIDDLE_ATTACK = (1 << 25),
	IN_USE_OR_RELOAD = (1 << 26)
};

class CBasePB
{
	MEM_PAD(0x18); // 0x0
};

class CCmdQAngle : public CBasePB
{
public:
	QAngle_t angValue; // 0x18
};

class CCmdVector : public CBasePB
{
public:
	Vector4D_t vecValue; // 0x18
};

static_assert(sizeof(CCmdQAngle) == 0x24);


class CCSGOInterpolationInfo : public CBasePB
{
public:
	float flFraction; // 0x18
	int nSrcTick; // 0x1C
	int nDstTick; // 0x20
};

// credits: https://www.unknowncheats.me/forum/3872928-post1311.html
class CCSGOInputHistoryEntryPB : public CBasePB
{
public:
	CCmdQAngle* pViewCmd; // 0x18
	CCmdVector* pShootOriginCmd; // 0x20
	CCmdVector* pTargetHeadOriginCmd; // 0x28
	CCmdVector* pTargetAbsOriginCmd; // 0x30
	CCmdQAngle* pTargetViewCmd; // 0x38
	CCSGOInterpolationInfo* cl_interp; // 0x40
	CCSGOInterpolationInfo* sv_interp0; // 0x48
	CCSGOInterpolationInfo* sv_interp1; // 0x50
	CCSGOInterpolationInfo* player_interp; // 0x58
	int nRenderTickCount; // 0x60
	float flRenderTickFraction; // 0x64
	int nPlayerTickCount; // 0x68
	float flPlayerTickFraction; // 0x6C
	int nFrameNumber; // 0x70
	int nTargetEntIndex; // 0x74
};

class CCSGOUserCmdPB
{
public:
	int32_t nTickCount; // 0x0
	MEM_PAD(0x4); // 0x4
	void* pInputHistory; // 0x8

	CCSGOInputHistoryEntryPB* GetInputHistoryEntry(std::int32_t nTick)
	{
		if (nTick < this->nTickCount)
		{
			CCSGOInputHistoryEntryPB** arrTickList = reinterpret_cast<CCSGOInputHistoryEntryPB**>(reinterpret_cast<std::uintptr_t>(pInputHistory) + 0x8);
			return arrTickList[nTick];
		}

		return nullptr;
	}
};

static_assert(sizeof(CCSGOUserCmdPB) == 0x10);

class CBaseUserCmdPB : public CBasePB
{
public:
	MEM_PAD(0x28); // 0x18
	CCmdQAngle* pCmdView; // 0x40
	int nCommandNumber; // 0x48
	int ntickCount; // 0x4C
	float flForwardMove; // 0x50
	float flSideMove; // 0x54	
	float flUpMove; // 0x58
	int32_t nImpulse; // 0x5C
	int32_t nWeaponSelect; // 0x60
	int32_t nRandomSeed; // 0x64 
	int32_t nMousedX; // 0x68
	int32_t nMousedY; // 0x6C
	bool bHasBeenPredicted; // 0x70
	uint32_t nConsumedServerAngleChanges; // 0x74
	int32_t nCmdFlags; // 0x78
	uint32_t hPawnEntity; // 0x7C
};

struct StartHistoryIndexTick_t
{
	int nAttack1;
	int nAttack2;
	int nAttack3;
};

struct ButtonState_t
{
	uint64_t nValue;
	uint64_t nValueChanged;
	uint64_t nValueScroll;
};

class CUserCmd
{
public:
	MEM_PAD(0x20); // 0x0
	CCSGOUserCmdPB csgoUserCmd; // 0x20
	CBaseUserCmdPB* pBaseCmd; // 0x30
	StartHistoryIndexTick_t nStartHistoryIndexAttack; // 0x38
	MEM_PAD(0x8); // 0x44
	ButtonState_t nButtons; // 0x4C
	MEM_PAD(0x8); // 0x64

	void SetSubTickAngle(const QAngle_t& angView)
	{
		for (int i = 0; i < this->csgoUserCmd.nTickCount; i++)
		{
			CCSGOInputHistoryEntryPB* pInputEntry = this->csgoUserCmd.GetInputHistoryEntry(i);
			if (pInputEntry == nullptr)
				continue;

			if (pInputEntry->pViewCmd == nullptr)
				continue;

			pInputEntry->pViewCmd->angValue = angView;
		}
	}
};

static_assert(sizeof(CUserCmd) == 0x70);
