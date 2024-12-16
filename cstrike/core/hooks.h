#pragma once

// used: [d3d] api
#include <d3d11.h>
#include <dxgi1_2.h>

// used: chookobject
#include "../utilities/detourhook.h"

// used: viewmatrix_t
#include "../sdk/datatypes/matrix.h"

namespace VTABLE
{
	namespace D3D
	{
		enum
		{
			PRESENT = 8U,
			RESIZEBUFFERS = 13U,
			RESIZEBUFFERS_CSTYLE = 39U,
		};
	}

	namespace DXGI
	{
		enum
		{
			CREATESWAPCHAIN = 10U,
		};
	}

	namespace CLIENT
	{
		enum
		{
			CREATEMOVE = 21U, // Outdated (Using pattern for now)
			MOUSEINPUTENABLED = 19U,
			FRAMESTAGENOTIFY = 36U,
		};
	}

	namespace INPUT
	{
		enum
		{
			VALID_VIEWANGLE = 7U,
		};
	}

	namespace INPUTSYSTEM
	{
		enum
		{
			ISRELATIVEMOUSEMODE = 76U,
		};
	}
}

class CRenderGameSystem;
class IViewRender;
class CCSGOInput;
class CViewSetup;
class CUserCmd;
class CMeshData;

namespace H
{
	bool Setup();
	void Destroy();

	/* @section: handlers */
	// d3d11 & wndproc
	HRESULT WINAPI Present(IDXGISwapChain* pSwapChain, UINT uSyncInterval, UINT uFlags);
	HRESULT CS_FASTCALL ResizeBuffers(IDXGISwapChain* pSwapChain, std::uint32_t nBufferCount, std::uint32_t nWidth, std::uint32_t nHeight, DXGI_FORMAT newFormat, std::uint32_t nFlags);
	HRESULT WINAPI CreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain);
	long CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// game's functions
	ViewMatrix_t* CS_FASTCALL GetMatrixForView(CRenderGameSystem* pRenderGameSystem, IViewRender* pViewRender, ViewMatrix_t* pOutWorldToView, ViewMatrix_t* pOutViewToProjection, ViewMatrix_t* pOutWorldToProjection, ViewMatrix_t* pOutWorldToPixels);
	bool CS_FASTCALL CreateMove(CCSGOInput* pInput, int nSlot, CUserCmd* cmd);
	bool CS_FASTCALL MouseInputEnabled(void* pThisptr);
	void CS_FASTCALL FrameStageNotify(void* rcx, int nFrameStage);
	__int64* CS_FASTCALL LevelInit(void* pClientModeShared, const char* szNewMap);
	__int64 CS_FASTCALL LevelShutdown(void* pClientModeShared);
	void CS_FASTCALL OverrideView(void* pClientModeCSNormal, CViewSetup* pSetup);
	void CS_FASTCALL DrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2);
	void* IsRelativeMouseMode(void* pThisptr, bool bActive);

	/* @section: managers */
	inline CBaseHookObject<decltype(&Present)> hkPresent = {};
	inline CBaseHookObject<decltype(&ResizeBuffers)> hkResizeBuffers = {};
	inline CBaseHookObject<decltype(&CreateSwapChain)> hkCreateSwapChain = {};
	inline CBaseHookObject<decltype(&WndProc)> hkWndProc = {};

	inline CBaseHookObject<decltype(&GetMatrixForView)> hkGetMatrixForView = {};
	inline CBaseHookObject<decltype(&CreateMove)> hkCreateMove = {};
	inline CBaseHookObject<decltype(&MouseInputEnabled)> hkMouseInputEnabled = {};
	inline CBaseHookObject<decltype(&IsRelativeMouseMode)> hkIsRelativeMouseMode = {};
	inline CBaseHookObject<decltype(&FrameStageNotify)> hkFrameStageNotify = {};
	inline CBaseHookObject<decltype(&LevelInit)> hkLevelInit = {};
	inline CBaseHookObject<decltype(&LevelShutdown)> hkLevelShutdown = {};
	inline CBaseHookObject<decltype(&OverrideView)> hkOverrideView = {};

	inline CBaseHookObject<decltype(&DrawObject)> hkDrawObject = {};
}
