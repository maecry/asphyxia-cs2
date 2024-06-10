#pragma once

// used: call virtual function
#include "../../utilities/memory.h"

// used: color_t
#include "../datatypes/color.h"
// used: stronghandle
#include "../datatypes/stronghandle.h"
// used: keyvalue3
#include "../datatypes/keyvalue3.h"
// used vector4d_t
#include "../datatypes/vector.h"

// used: cbasehandle
#include "../entity_handle.h"

class CMaterial2
{
public:
	virtual const char* GetName() = 0;
	virtual const char* GetShareName() = 0;
};

// idk
struct MaterialKeyVar_t
{
	std::uint64_t uKey;
	const char* szName;

	MaterialKeyVar_t(std::uint64_t uKey, const char* szName) :
		uKey(uKey), szName(szName) { }

	MaterialKeyVar_t(const char* szName, bool bShouldFindKey = false) :
		szName(szName)
	{
		uKey = bShouldFindKey ? FindKey(szName) : 0x0;
	}

	std::uint64_t FindKey(const char* szName)
	{
		using fnFindKeyVar = std::uint64_t(CS_FASTCALL*)(const char*, unsigned int, int);
		static auto oFindKeyVar = reinterpret_cast<fnFindKeyVar>(MEM::FindPattern(PARTICLES_DLL, CS_XOR("48 89 5C 24 ? 57 48 81 EC ? ? ? ? 33 C0 8B DA")));

#ifdef CS_PARANOID
		CS_ASSERT(oFindKeyVar != nullptr);
#endif

		// idk those enum flags, just saw it called like that soooo yea
		return oFindKeyVar(szName, 0x12, 0x31415926);
	}
};

class CObjectInfo
{
	MEM_PAD(0xB0);
	int nId;
};

class CSceneAnimatableObject
{
	MEM_PAD(0xB8);
	CBaseHandle hOwner;
};

// the naming is incorrect but i dont care atm
class CMeshData
{
public:
	void SetShaderType(const char* szShaderName)
	{
		// @ida: #STR: shader, spritecard.vfx
		using fnSetMaterialShaderType = void(CS_FASTCALL*)(void*, MaterialKeyVar_t, const char*, int);
		static auto oSetMaterialShaderType = reinterpret_cast<fnSetMaterialShaderType>(MEM::FindPattern(PARTICLES_DLL, CS_XOR("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 54 41 56 41 57 48 83 EC ? 0F B6 01 45 0F B6 F9 8B 2A 4D 8B E0 4C 8B 72 ? 48 8B F9 C0 E8 ? 24 ? 3C ? 74 ? 41 B0 ? B2 ? E8 ? ? ? ? 0F B6 07 33 DB C0 E8 ? 24 ? 3C ? 75 ? 48 8B 77 ? EB ? 48 8B F3 4C 8D 44 24 ? C7 44 24 ? ? ? ? ? 48 8D 54 24 ? 89 6C 24 ? 48 8B CE 4C 89 74 24 ? E8 ? ? ? ? 8B D0 83 F8 ? 75 ? 45 33 C9 89 6C 24 ? 4C 8D 44 24 ? 4C 89 74 24 ? 48 8B D7 48 8B CE E8 ? ? ? ? 8B D0 0F B6 0F C0 E9 ? 80 E1 ? 80 F9 ? 75 ? 48 8B 4F ? EB ? 48 8B CB 8B 41 ? 85 C0 74 ? 48 8D 59 ? 83 F8 ? 76 ? 48 8B 1B 48 63 C2 4D 85 E4")));

		#ifdef CS_PARANOID
		CS_ASSERT(oSetMaterialShaderType != nullptr);
		#endif

		MaterialKeyVar_t shaderVar(0x162C1777, CS_XOR("shader"));
		oSetMaterialShaderType(this, shaderVar, szShaderName, 0x18);
	}

	void SetMaterialFunction(const char* szFunctionName, int nValue)
	{
		using fnSetMaterialFunction = void(__fastcall*)(void*, MaterialKeyVar_t, int, int);
		static auto oSetMaterialFunction = reinterpret_cast<fnSetMaterialFunction>(MEM::FindPattern(PARTICLES_DLL, CS_XOR("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 54 41 56 41 57 48 83 EC ? 0F B6 01 45 0F B6 F9 8B 2A 48 8B F9")));

		#ifdef CS_PARANOID
		CS_ASSERT(oSetMaterialFunction != nullptr);
		#endif

		MaterialKeyVar_t functionVar(szFunctionName, true);
		oSetMaterialFunction(this, functionVar, nValue, 0x18);
	}

	MEM_PAD(0x18); // 0x0
	CSceneAnimatableObject* pSceneAnimatableObject; // 0x18
	CMaterial2* pMaterial; // 0x20
	MEM_PAD(0x18); // 0x28
	Color_t colValue; // 0x40
	MEM_PAD(0x4); // 0x44
	CObjectInfo* pObjectInfo; // 0x48
};

class IMaterialSystem2
{
public:
	CMaterial2*** FindOrCreateFromResource(CMaterial2*** pOutMaterial, const char* szMaterialName)
	{
		return MEM::CallVFunc<CMaterial2***, 14U>(this, pOutMaterial, szMaterialName);
	}

	CMaterial2** CreateMaterial(CMaterial2*** pOutMaterial, const char* szMaterialName, CMeshData* pData)
	{
		return MEM::CallVFunc<CMaterial2**, 29U>(this, pOutMaterial, szMaterialName, pData, 0, 0, 0, 0, 0, 1);
	}

	void SetCreateDataByMaterial(const void* pData, CMaterial2*** const pInMaterial)
	{
		return MEM::CallVFunc<void, 37U>(this, pInMaterial, pData);
	}
};
