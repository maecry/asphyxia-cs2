// used: get_x_lparam, get_y_lparam
#include <windowsx.h>

#include "inputsystem.h"

// used: menu open/panic keys
#include "../core/variables.h"
// used: wndproc hook
#include "../core/hooks.h"
// used: menu variables
#include "../core/menu.h"
// used: iinputsystem
#include "../core/interfaces.h"
#include "../sdk/interfaces/iinputsystem.h"

// used: [ext] imrect
#include "../dependencies/imgui/imgui_internal.h"

static BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	const auto MainWindow = [handle]()
	{
		return GetWindow(handle, GW_OWNER) == nullptr &&
		IsWindowVisible(handle) && handle != GetConsoleWindow();
	};

	DWORD nPID = 0;
	GetWindowThreadProcessId(handle, &nPID);

	if (GetCurrentProcessId() != nPID || !MainWindow())
		return TRUE;

	*reinterpret_cast<HWND*>(lParam) = handle;
	return FALSE;
}

bool IPT::Setup()
{
	while (hWindow == nullptr)
	{
		EnumWindows(::EnumWindowsCallback, reinterpret_cast<LPARAM>(&hWindow));
		::Sleep(200U);
	}

	// change window message handle to our
	pOldWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(H::WndProc)));
	if (pOldWndProc == nullptr)
		return false;

	return true;
}

void IPT::Destroy()
{
	MENU::bMainWindowOpened = false;
	::Sleep(200U);

	if (pOldWndProc != nullptr)
	{
		SetWindowLongPtrW(hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(pOldWndProc));
		pOldWndProc = nullptr;
	}
}

bool IPT::OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// prevent process when e.g. binding something in-menu
	if (wParam != C_GET(unsigned int, Vars.nMenuKey) && wParam != C_GET(unsigned int, Vars.nPanicKey) && MENU::bMainWindowOpened)
		return false;

	// current active key
	int nKey = 0;
	// current active key state
	KeyState_t state = KEY_STATE_NONE;

	switch (uMsg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (wParam < 256U)
		{
			nKey = static_cast<int>(wParam);
			state = KEY_STATE_DOWN;
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (wParam < 256U)
		{
			nKey = static_cast<int>(wParam);
			state = KEY_STATE_UP;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		nKey = VK_LBUTTON;
		state = uMsg == WM_LBUTTONUP ? KEY_STATE_UP : KEY_STATE_DOWN;
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		nKey = VK_RBUTTON;
		state = uMsg == WM_RBUTTONUP ? KEY_STATE_UP : KEY_STATE_DOWN;
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
		nKey = VK_MBUTTON;
		state = uMsg == WM_MBUTTONUP ? KEY_STATE_UP : KEY_STATE_DOWN;
		break;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDBLCLK:
		nKey = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2);
		state = uMsg == WM_XBUTTONUP ? KEY_STATE_UP : KEY_STATE_DOWN;
		break;
	default:
		return false;
	}

	// save key states
	if (state == KEY_STATE_UP && arrKeyState[nKey] == KEY_STATE_DOWN) // if swap states it will be pressed state
		arrKeyState[nKey] = KEY_STATE_RELEASED;
	else
		arrKeyState[nKey] = state;

	return true;
}

bool IPT::GetBindState(KeyBind_t& keyBind)
{
	if (keyBind.uKey == 0U)
		return false;

	switch (keyBind.nMode)
	{
	case EKeyBindMode::HOLD:
		keyBind.bEnable = IsKeyDown(keyBind.uKey);
		break;
	case EKeyBindMode::TOGGLE:
		if (IsKeyReleased(keyBind.uKey))
			keyBind.bEnable = !keyBind.bEnable;
		break;
	}

	return keyBind.bEnable;
}

bool IPT::IsHovered(const ImVec2& vecPosition, const ImVec2& vecSize)
{
	const ImVec2 vecMousePosition = ImGui::GetMousePos();
	return ImRect(vecPosition, vecPosition + vecSize).Contains(vecMousePosition);
}
