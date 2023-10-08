#pragma once

class CConVar;

namespace CONVAR
{
	// dump convars to file
	bool Dump(const wchar_t* wszFileName);
	// setup convars
	bool Setup();

	inline CConVar* m_pitch = nullptr;
	inline CConVar* m_yaw = nullptr;
	inline CConVar* sensitivity = nullptr;

	inline CConVar* game_type = nullptr;
	inline CConVar* game_mode = nullptr;

	inline CConVar* mp_teammates_are_enemies = nullptr;

	inline CConVar* sv_autobunnyhopping = nullptr;
}
