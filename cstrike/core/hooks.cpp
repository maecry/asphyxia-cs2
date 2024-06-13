#include "hooks.h"

// used: variables
#include "variables.h"

// used: game's sdk
#include "../sdk/interfaces/iswapchaindx11.h"
#include "../sdk/interfaces/iviewrender.h"
#include "../sdk/interfaces/cgameentitysystem.h"
#include "../sdk/interfaces/ccsgoinput.h"
#include "../sdk/interfaces/iinputsystem.h"
#include "../sdk/interfaces/iengineclient.h"
#include "../sdk/interfaces/inetworkclientservice.h"
#include "../sdk/interfaces/iglobalvars.h"
#include "../sdk/interfaces/imaterialsystem.h"

// used: viewsetup
#include "../sdk/datatypes/viewsetup.h"

// used: entity
#include "../sdk/entity.h"

// used: get virtual function, find pattern, ...
#include "../utilities/memory.h"
// used: inputsystem
#include "../utilities/inputsystem.h"
// used: draw
#include "../utilities/draw.h"

// used: features callbacks
#include "../features.h"
// used: CRC rebuild
#include "../features/CRC.h"

// used: game's interfaces
#include "interfaces.h"
#include "sdk.h"

// used: menu
#include "menu.h"

bool H::Setup()
{
	if (MH_Initialize() != MH_OK)
	{
		L_PRINT(LOG_ERROR) << CS_XOR("failed to initialize minhook");

		return false;
	}
	L_PRINT(LOG_INFO) << CS_XOR("minhook initialization completed");

	if (!hkPresent.Create(MEM::GetVFunc(I::SwapChain->pDXGISwapChain, VTABLE::D3D::PRESENT), reinterpret_cast<void*>(&Present)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"Present\" hook has been created");

	if (!hkResizeBuffers.Create(MEM::GetVFunc(I::SwapChain->pDXGISwapChain, VTABLE::D3D::RESIZEBUFFERS), reinterpret_cast<void*>(&ResizeBuffers)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"ResizeBuffers\" hook has been created");

	// creat swap chain hook
	IDXGIDevice* pDXGIDevice = NULL;
	I::Device->QueryInterface(IID_PPV_ARGS(&pDXGIDevice));

	IDXGIAdapter* pDXGIAdapter = NULL;
	pDXGIDevice->GetAdapter(&pDXGIAdapter);

	IDXGIFactory* pIDXGIFactory = NULL;
	pDXGIAdapter->GetParent(IID_PPV_ARGS(&pIDXGIFactory));

	if (!hkCreateSwapChain.Create(MEM::GetVFunc(pIDXGIFactory, VTABLE::DXGI::CREATESWAPCHAIN), reinterpret_cast<void*>(&CreateSwapChain)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"CreateSwapChain\" hook has been created");

	pDXGIDevice->Release();
	pDXGIDevice = nullptr;
	pDXGIAdapter->Release();
	pDXGIAdapter = nullptr;
	pIDXGIFactory->Release();
	pIDXGIFactory = nullptr;

	// @ida: class CViewRender->OnRenderStart call GetMatricesForView
	if (!hkGetMatrixForView.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("40 53 48 81 EC ? ? ? ? 49 8B C1")), reinterpret_cast<void*>(&GetMatrixForView)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"GetMatrixForView\" hook has been created");

	// @ida: #STR: cl: CreateMove clamped invalid attack history index %d in frame history to -1. Was %d, frame history size %d.\n
	if (!hkCreateMove.Create(MEM::GetVFunc(I::Input, VTABLE::CLIENT::CREATEMOVE), reinterpret_cast<void*>(&CreateMove)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"CreateMove\" hook has been created");

	if (!hkMouseInputEnabled.Create(MEM::GetVFunc(I::Input, VTABLE::CLIENT::MOUSEINPUTENABLED), reinterpret_cast<void*>(&MouseInputEnabled)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"MouseInputEnabled\" hook has been created");

	if (!hkFrameStageNotify.Create(MEM::GetVFunc(I::Client, VTABLE::CLIENT::FRAMESTAGENOTIFY), reinterpret_cast<void*>(&FrameStageNotify)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"FrameStageNotify\" hook has been created");

	// @ida: ClientModeShared -> #STR: "mapname", "transition", "game_newmap"
	if (!hkLevelInit.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 ? 56 48 83 EC ? 48 8B 0D ? ? ? ? 48 8B F2")), reinterpret_cast<void*>(&LevelInit)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"LevelInit\" hook has been created");

	// @ida: ClientModeShared -> #STR: "map_shutdown"
	if (!hkLevelShutdown.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 83 EC ? 48 8B 0D ? ? ? ? 48 8D 15 ? ? ? ? 45 33 C9 45 33 C0 48 8B 01 FF 50 ? 48 85 C0 74 ? 48 8B 0D ? ? ? ? 48 8B D0 4C 8B 01 41 FF 50 ? 48 83 C4")), reinterpret_cast<void*>(&LevelShutdown)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"LevelShutdown\" hook has been created");

	// @note: seems to do nothing for now...
	// @ida: ClientModeCSNormal->OverrideView idx 15
	//v21 = flSomeWidthSize * 0.5;
	//v22 = *flSomeHeightSize * 0.5;
	//*(float*)(pSetup + 0x49C) = v21; // m_OrthoRight
	//*(float*)(pSetup + 0x494) = -v21; // m_OrthoLeft
	//*(float*)(pSetup + 0x498) = -v22; // m_OrthoTop
	//*(float*)(pSetup + 0x4A0) = v22; // m_OrthoBottom
	//if (!hkOverrideView.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC ? 48 8B FA E8")), reinterpret_cast<void*>(&OverrideView)))
	//	return false;

	//L_PRINT(LOG_INFO) << CS_XOR("\"OverrideView\" hook has been created");

	if (!hkDrawObject.Create(MEM::FindPattern(SCENESYSTEM_DLL, CS_XOR("48 8B C4 53 41 54 41 55 48 81 EC ? ? ? ? 4D 63 E1")), reinterpret_cast<void*>(&DrawObject)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"DrawObject\" hook has been created");

	return true;
}

void H::Destroy()
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);

	MH_Uninitialize();
}

HRESULT __stdcall H::Present(IDXGISwapChain* pSwapChain, UINT uSyncInterval, UINT uFlags)
{
	const auto oPresent = hkPresent.GetOriginal();

	// recreate it if it's not valid
	if (I::RenderTargetView == nullptr)
		I::CreateRenderTarget();

	// set our render target
	if (I::RenderTargetView != nullptr)
		I::DeviceContext->OMSetRenderTargets(1, &I::RenderTargetView, nullptr);

	F::OnPresent();

	return oPresent(I::SwapChain->pDXGISwapChain, uSyncInterval, uFlags);
}

HRESULT CS_FASTCALL H::ResizeBuffers(IDXGISwapChain* pSwapChain, std::uint32_t nBufferCount, std::uint32_t nWidth, std::uint32_t nHeight, DXGI_FORMAT newFormat, std::uint32_t nFlags)
{
	const auto oResizeBuffer = hkResizeBuffers.GetOriginal();

	auto hResult = oResizeBuffer(pSwapChain, nBufferCount, nWidth, nHeight, newFormat, nFlags);
	if (SUCCEEDED(hResult))
		I::CreateRenderTarget();

	return hResult;
}

HRESULT __stdcall H::CreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
	const auto oCreateSwapChain = hkCreateSwapChain.GetOriginal();

	I::DestroyRenderTarget();
	L_PRINT(LOG_INFO) << CS_XOR("render target view has been destroyed");

	return oCreateSwapChain(pFactory, pDevice, pDesc, ppSwapChain);
}

long H::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (D::OnWndProc(hWnd, uMsg, wParam, lParam))
		return 1L;

	return ::CallWindowProcW(IPT::pOldWndProc, hWnd, uMsg, wParam, lParam);
}

ViewMatrix_t* CS_FASTCALL H::GetMatrixForView(CRenderGameSystem* pRenderGameSystem, IViewRender* pViewRender, ViewMatrix_t* pOutWorldToView, ViewMatrix_t* pOutViewToProjection, ViewMatrix_t* pOutWorldToProjection, ViewMatrix_t* pOutWorldToPixels)
{
	const auto oGetMatrixForView = hkGetMatrixForView.GetOriginal();
	ViewMatrix_t* matResult = oGetMatrixForView(pRenderGameSystem, pViewRender, pOutWorldToView, pOutViewToProjection, pOutWorldToProjection, pOutWorldToPixels);

	// get view matrix
	SDK::ViewMatrix = *pOutWorldToProjection;
	// get camera position
	// @note: ida @GetMatrixForView(global_pointer, pRenderGameSystem + 16, ...)
	SDK::CameraPosition = pViewRender->vecOrigin;

	return matResult;
}

bool CS_FASTCALL H::CreateMove(CCSGOInput* pInput, int nSlot, bool bActive)
{
	const auto oCreateMove = hkCreateMove.GetOriginal();
	const bool bResult = oCreateMove(pInput, nSlot, bActive);

	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return bResult;

	CUserCmd* pCmd = SDK::Cmd = pInput->GetUserCmd();
	if (pCmd == nullptr)
		return bResult;

	CBaseUserCmdPB* pBaseCmd = pCmd->csgoUserCmd.pBaseCmd;
	if (pBaseCmd == nullptr)
		return bResult;

	SDK::LocalController = CCSPlayerController::GetLocalPlayerController();
	if (SDK::LocalController == nullptr)
		return bResult;

	SDK::LocalPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(SDK::LocalController->GetPawnHandle());
	if (SDK::LocalPawn == nullptr)
		return bResult;

	F::OnCreateMove(pCmd, pBaseCmd, SDK::LocalController);

	CRC::Save(pBaseCmd);
	if (CRC::CalculateCRC(pBaseCmd) == true)
		CRC::Apply(pCmd);

	return bResult;
}

bool CS_FASTCALL H::MouseInputEnabled(void* pThisptr)
{
	const auto oMouseInputEnabled = hkMouseInputEnabled.GetOriginal();
	return MENU::bMainWindowOpened ? false : oMouseInputEnabled(pThisptr);
}

void CS_FASTCALL H::FrameStageNotify(void* rcx, int nFrameStage)
{
	const auto oFrameStageNotify = hkFrameStageNotify.GetOriginal();
	F::OnFrameStageNotify(nFrameStage);

	return oFrameStageNotify(rcx, nFrameStage);
}

__int64* CS_FASTCALL H::LevelInit(void* pClientModeShared, const char* szNewMap)
{
	const auto oLevelInit = hkLevelInit.GetOriginal();
	// if global variables are not captured during I::Setup or we join a new game, recapture it
	if (I::GlobalVars == nullptr)
		I::GlobalVars = *reinterpret_cast<IGlobalVars**>(MEM::ResolveRelativeAddress(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 0D ? ? ? ? 48 89 41")), 0x3, 0x7));

	return oLevelInit(pClientModeShared, szNewMap);
}

__int64 CS_FASTCALL H::LevelShutdown(void* pClientModeShared)
{
	const auto oLevelShutdown = hkLevelShutdown.GetOriginal();
	// reset global variables since it got discarded by the game
	I::GlobalVars = nullptr;

	return oLevelShutdown(pClientModeShared);
}

void CS_FASTCALL H::OverrideView(void* pClientModeCSNormal, CViewSetup* pSetup)
{
	const auto oOverrideView = hkOverrideView.GetOriginal();
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return hkOverrideView.GetOriginal()(pClientModeCSNormal, pSetup);

	oOverrideView(pClientModeCSNormal, pSetup);
}

void CS_FASTCALL H::DrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2)
{
	const auto oDrawObject = hkDrawObject.GetOriginal();
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);

	if (SDK::LocalController == nullptr || SDK::LocalPawn == nullptr)
		return oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);

	if (!F::OnDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2))
		oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);
}
