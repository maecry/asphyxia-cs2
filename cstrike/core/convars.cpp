// used: [stl] vector
#include <vector>
// used: [stl] find_if
#include <algorithm>

#include "convars.h"

// used: convar interface
#include "interfaces.h"
#include "../sdk/interfaces/ienginecvar.h"
// used: l_print
#include "../utilities/log.h"

// used: getworkingpath
#include "../core.h"

inline static void WriteConVarType(HANDLE hFile, const uint32_t nType)
{
	switch ((EConVarType)nType)
	{
	case EConVarType_Bool:
		::WriteFile(hFile, CS_XOR("[bool] "), CRT::StringLength(CS_XOR("[bool] ")), nullptr, nullptr);
		break;
	case EConVarType_Int16:
		::WriteFile(hFile, CS_XOR("[int16] "), CRT::StringLength(CS_XOR("[int16] ")), nullptr, nullptr);
		break;
	case EConVarType_UInt16:
		::WriteFile(hFile, CS_XOR("[uint16] "), CRT::StringLength(CS_XOR("[uint16] ")), nullptr, nullptr);
		break;
	case EConVarType_Int32:
		::WriteFile(hFile, CS_XOR("[int32] "), CRT::StringLength(CS_XOR("[int32] ")), nullptr, nullptr);
		break;
	case EConVarType_UInt32:
		::WriteFile(hFile, CS_XOR("[uint32] "), CRT::StringLength(CS_XOR("[uint32] ")), nullptr, nullptr);
		break;
	case EConVarType_Int64:
		::WriteFile(hFile, CS_XOR("[int64] "), CRT::StringLength(CS_XOR("[int64] ")), nullptr, nullptr);
		break;
	case EConVarType_UInt64:
		::WriteFile(hFile, CS_XOR("[uint64] "), CRT::StringLength(CS_XOR("[uint64] ")), nullptr, nullptr);
		break;
	case EConVarType_Float32:
		::WriteFile(hFile, CS_XOR("[float32] "), CRT::StringLength(CS_XOR("[float32] ")), nullptr, nullptr);
		break;
	case EConVarType_Float64:
		::WriteFile(hFile, CS_XOR("[float64] "), CRT::StringLength(CS_XOR("[float64] ")), nullptr, nullptr);
		break;
	case EConVarType_String:
		::WriteFile(hFile, CS_XOR("[string] "), CRT::StringLength(CS_XOR("[string] ")), nullptr, nullptr);
		break;
	case EConVarType_Color:
		::WriteFile(hFile, CS_XOR("[color] "), CRT::StringLength(CS_XOR("[color] ")), nullptr, nullptr);
		break;
	case EConVarType_Vector2:
		::WriteFile(hFile, CS_XOR("[vector2] "), CRT::StringLength(CS_XOR("[vector2] ")), nullptr, nullptr);
		break;
	case EConVarType_Vector3:
		::WriteFile(hFile, CS_XOR("[vector3] "), CRT::StringLength(CS_XOR("[vector3] ")), nullptr, nullptr);
		break;
	case EConVarType_Vector4:
		::WriteFile(hFile, CS_XOR("[vector4] "), CRT::StringLength(CS_XOR("[vector4] ")), nullptr, nullptr);
		break;
	case EConVarType_Qangle:
		::WriteFile(hFile, CS_XOR("[qangle] "), CRT::StringLength(CS_XOR("[qangle] ")), nullptr, nullptr);
		break;
	default:
		::WriteFile(hFile, CS_XOR("[unknown-type] "), CRT::StringLength(CS_XOR("[unknown-type] ")), nullptr, nullptr);
		break;
	}
}

inline static void WriteConVarFlags(HANDLE hFile, const uint32_t nFlags)
{
	if (nFlags & FCVAR_CLIENTDLL)
		::WriteFile(hFile, CS_XOR("[client.dll] "), CRT::StringLength(CS_XOR("[client.dll] ")), nullptr, nullptr);
	else if (nFlags & FCVAR_GAMEDLL)
		::WriteFile(hFile, CS_XOR("[games's dll] "), CRT::StringLength(CS_XOR("[games's dll] ")), nullptr, nullptr);

	if (nFlags & FCVAR_PROTECTED)
		::WriteFile(hFile, CS_XOR("[protected] "), CRT::StringLength(CS_XOR("[protected] ")), nullptr, nullptr);

	if (nFlags & FCVAR_CHEAT)
		::WriteFile(hFile, CS_XOR("[cheat] "), CRT::StringLength(CS_XOR("[cheat] ")), nullptr, nullptr);

	if (nFlags & FCVAR_HIDDEN)
		::WriteFile(hFile, CS_XOR("[hidden] "), CRT::StringLength(CS_XOR("[hidden] ")), nullptr, nullptr);

	if (nFlags & FCVAR_DEVELOPMENTONLY)
		::WriteFile(hFile, CS_XOR("[devonly] "), CRT::StringLength(CS_XOR("[devonly] ")), nullptr, nullptr);

	::WriteFile(hFile, CS_XOR("\n"), CRT::StringLength(CS_XOR("\n")), nullptr, nullptr);
}

bool CONVAR::Dump(const wchar_t* wszFileName)
{
	wchar_t wszDumpFilePath[MAX_PATH];
	if (!CORE::GetWorkingPath(wszDumpFilePath))
		return false;

	CRT::StringCat(wszDumpFilePath, wszFileName);

	HANDLE hOutFile = ::CreateFileW(wszDumpFilePath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hOutFile == INVALID_HANDLE_VALUE)
		return false;

	// @todo: maybe remove this redundant? and put it inside CRT::String_t c'tor
	const std::time_t time = std::time(nullptr);
	std::tm timePoint;
	localtime_s(&timePoint, &time);

	CRT::String_t<64> szTimeBuffer(CS_XOR("[%d-%m-%Y %T] asphyxia | convars dump\n\n"), &timePoint);

	// write current date, time and info
	::WriteFile(hOutFile, szTimeBuffer.Data(), szTimeBuffer.Length(), nullptr, nullptr);

	for (int i = I::Cvar->listConvars.Head(); i != I::Cvar->listConvars.InvalidIndex(); i = I::Cvar->listConvars.Next(i))
	{
		CConVar* pConVar = I::Cvar->listConvars.Element(i);
		if (pConVar != nullptr)
		{
			// dump to file
			WriteConVarType(hOutFile, pConVar->nType);

			CRT::String_t<526> szBuffer(CS_XOR("%s : \"%s\" "), pConVar->szName, pConVar->szDescription[0] == '\0' ? CS_XOR("no description") : pConVar->szDescription);
			::WriteFile(hOutFile, szBuffer.Data(), szBuffer.Length(), nullptr, nullptr);

			// write flags
			WriteConVarFlags(hOutFile, pConVar->nFlags);
		}
	}

	::CloseHandle(hOutFile);

	return true;
}

bool CONVAR::Setup()
{
	bool bSuccess = true;

	m_pitch = I::Cvar->Find(FNV1A::HashConst("m_pitch"));
	bSuccess &= m_pitch != nullptr;

	m_yaw = I::Cvar->Find(FNV1A::HashConst("m_yaw"));
	bSuccess &= m_yaw != nullptr;

	sensitivity = I::Cvar->Find(FNV1A::HashConst("sensitivity"));
	bSuccess &= sensitivity != nullptr;

	game_type = I::Cvar->Find(FNV1A::HashConst("game_type"));
	bSuccess &= game_type != nullptr;

	game_mode = I::Cvar->Find(FNV1A::HashConst("game_mode"));
	bSuccess &= game_mode != nullptr;

	mp_teammates_are_enemies = I::Cvar->Find(FNV1A::HashConst("mp_teammates_are_enemies"));
	bSuccess &= mp_teammates_are_enemies != nullptr;

	sv_autobunnyhopping = I::Cvar->Find(FNV1A::HashConst("sv_autobunnyhopping"));
	bSuccess &= sv_autobunnyhopping != nullptr;

	return bSuccess;
}
