#pragma once

#include "../common.h"

#pragma region sdk_definitons
#define GAME_RESOURCE_SERVICE_CLIENT "GameResourceServiceClientV00"
#define SOURCE2_CLIENT "Source2Client00"
#define SCHEMA_SYSTEM "SchemaSystem_00"
#define INPUT_SYSTEM_VERSION "InputSystemVersion00"
#define SOURCE2_ENGINE_TO_CLIENT "Source2EngineToClient00"
#define ENGINE_CVAR "VEngineCvar00"
#define LOCALIZE "Localize_00"
#define NETWORK_CLIENT_SERVICE "NetworkClientService_00"

// @source: master/game/shared/shareddefs.h
#define TICK_INTERVAL (SDK::GlobalVars->m_flIntervalPerTick)
#define TIME_TO_TICKS(TIME) (static_cast<int>(0.5f + static_cast<float>(TIME) / TICK_INTERVAL))
#define TICKS_TO_TIME(TICKS) (TICK_INTERVAL * static_cast<float>(TICKS))
#define ROUND_TO_TICKS(TIME) (TICK_INTERVAL * TIME_TO_TICKS(TIME))
#define TICK_NEVER_THINK (-1)
#pragma endregion

// game interfaces
class ISwapChainDx11;
class IMemAlloc;
class CCSGOInput;
class ISchemaSystem;
class IGlobalVars;
class IInputSystem;
class IGameResourceService;
class ISource2Client;
class IEngineClient;
class IEngineCVar;
class INetworkClientService;

// [d3d] struct
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;

namespace I
{
	bool Setup();

	/* @section: helpers */
	// create and destroy render target view for handling resize
	void CreateRenderTarget();
	void DestroyRenderTarget();

	inline IMemAlloc* MemAlloc = nullptr;
	inline ISwapChainDx11* SwapChain = nullptr;
	inline ID3D11Device* Device = nullptr;
	inline ID3D11DeviceContext* DeviceContext = nullptr;
	inline ID3D11RenderTargetView* RenderTargetView = nullptr;
	inline CCSGOInput* Input = nullptr;
	inline ISchemaSystem* SchemaSystem = nullptr;
	inline IGlobalVars* GlobalVars = nullptr;
	inline IInputSystem* InputSystem = nullptr;
	inline IGameResourceService* GameResourceService = nullptr;
	inline ISource2Client* Client = nullptr;
	inline IEngineClient* Engine = nullptr;
	inline IEngineCVar* Cvar = nullptr;
	inline INetworkClientService* NetworkClientService = nullptr;
}
