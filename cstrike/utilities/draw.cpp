#include "draw.h"

// used: cheat variables
#include "../core/variables.h"
// used: viewmatrix
#include "../core/sdk.h"

// used: m_deg2rad
#include "math.h"
// used: memoryset
#include "crt.h"
// used: easing
#include "easing.h"
// used: ipt
#include "inputsystem.h"

// used: [ext] imgui
#include "../../dependencies/imgui/imgui_freetype.h"
#include "../../dependencies/imgui/imgui_impl_dx11.h"
#include "../../dependencies/imgui/imgui_impl_win32.h"

// used: [resouces] font awesome
#include "../../resources/fa_solid_900.h"
#include "../../resources/font_awesome_5.h"

// used: iinputsystem
#include "../core/interfaces.h"
#include "../sdk/interfaces/iinputsystem.h"
// used: bMainWindowOpened
#include "../core/menu.h"


#pragma region imgui_extended
static constexpr const char* arrKeyNames[] = {
	"",
	"mouse 1", "mouse 2", "cancel", "mouse 3", "mouse 4", "mouse 5", "",
	"backspace", "tab", "", "", "clear", "enter", "", "",
	"shift", "control", "alt", "pause", "caps", "", "", "", "", "", "",
	"escape", "", "", "", "", "space", "page up", "page down",
	"end", "home", "left", "up", "right", "down", "", "", "",
	"print", "insert", "delete", "",
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"", "", "", "", "", "", "",
	"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k",
	"l", "m", "n", "o", "p", "q", "r", "s", "t", "u",
	"v", "w", "x", "y", "z", "lwin", "rwin", "", "", "",
	"num0", "num1", "num2", "num3", "num4", "num5",
	"num6", "num7", "num8", "num9",
	"*", "+", "", "-", ".", "/",
	"f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8",
	"f9", "f10", "f11", "f12", "f13", "f14", "f15", "f16",
	"f17", "f18", "f19", "f20", "f21", "f22", "f23", "f24",
	"", "", "", "", "", "", "", "",
	"num lock", "scroll lock",
	"", "", "", "", "", "", "",
	"", "", "", "", "", "", "",
	"lshift", "rshift", "lctrl",
	"rctrl", "lmenu", "rmenu"
};

void ImGui::HelpMarker(const char* szDescription)
{
	TextDisabled(CS_XOR("(?)"));
	if (IsItemHovered())
	{
		BeginTooltip();
		PushTextWrapPos(450.f);
		TextUnformatted(szDescription);
		PopTextWrapPos();
		EndTooltip();
	}
}

bool ImGui::HotKey(const char* szLabel, unsigned int* pValue)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* pWindow = g.CurrentWindow;

	if (pWindow->SkipItems)
		return false;

	ImGuiIO& io = g.IO;
	const ImGuiStyle& style = g.Style;
	const ImGuiID nIndex = pWindow->GetID(szLabel);

	const float flWidth = CalcItemWidth();
	const ImVec2 vecLabelSize = CalcTextSize(szLabel, nullptr, true);

	const ImRect rectFrame(pWindow->DC.CursorPos + ImVec2(vecLabelSize.x > 0.0f ? style.ItemInnerSpacing.x + GetFrameHeight() : 0.0f, 0.0f), pWindow->DC.CursorPos + ImVec2(flWidth, vecLabelSize.x > 0.0f ? vecLabelSize.y + style.FramePadding.y : 0.f));
	const ImRect rectTotal(rectFrame.Min, rectFrame.Max);

	ItemSize(rectTotal, style.FramePadding.y);
	if (!ItemAdd(rectTotal, nIndex, &rectFrame))
		return false;

	const bool bHovered = ItemHoverable(rectFrame, nIndex, ImGuiItemFlags_None);
	if (bHovered)
	{
		SetHoveredID(nIndex);
		g.MouseCursor = ImGuiMouseCursor_TextInput;
	}

	const bool bClicked = bHovered && io.MouseClicked[0];
	const bool bDoubleClicked = bHovered && io.MouseDoubleClicked[0];
	if (bClicked || bDoubleClicked)
	{
		if (g.ActiveId != nIndex)
		{
			CRT::MemorySet(io.MouseDown, 0, sizeof(io.MouseDown));
			CRT::MemorySet(io.KeysDown, 0, sizeof(io.KeysDown));
			*pValue = 0U;
		}

		SetActiveID(nIndex, pWindow);
		FocusWindow(pWindow);
	}

	bool bValueChanged = false;
	if (unsigned int nKey = *pValue; g.ActiveId == nIndex)
	{
		for (int n = 0; n < IM_ARRAYSIZE(io.MouseDown); n++)
		{
			if (IsMouseDown(n))
			{
				switch (n)
				{
				case 0:
					nKey = VK_LBUTTON;
					break;
				case 1:
					nKey = VK_RBUTTON;
					break;
				case 2:
					nKey = VK_MBUTTON;
					break;
				case 3:
					nKey = VK_XBUTTON1;
					break;
				case 4:
					nKey = VK_XBUTTON2;
					break;
				}

				bValueChanged = true;
				ClearActiveID();
			}
		}

		if (!bValueChanged)
		{
			for (int n = VK_BACK; n <= VK_RMENU; n++)
			{
				if (IsKeyDown((ImGuiKey)n))
				{
					nKey = n;
					bValueChanged = true;
					ClearActiveID();
				}
			}
		}

		if (IsKeyPressed(ImGuiKey_Escape))
		{
			*pValue = 0U;
			ClearActiveID();
		}
		else
			*pValue = nKey;
	}

	char szBuffer[64] = {};
	char* szBufferEnd = CRT::StringCopy(szBuffer, "  ");
	if (*pValue != 0 && g.ActiveId != nIndex)
		szBufferEnd = CRT::StringCat(szBufferEnd, arrKeyNames[*pValue]);
	else if (g.ActiveId == nIndex)
		szBufferEnd = CRT::StringCat(szBufferEnd, CS_XOR("press"));
	else
		szBufferEnd = CRT::StringCat(szBufferEnd, CS_XOR("none"));
	CRT::StringCat(szBufferEnd, "  ");

	// modified by asphyxia
	PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, -1));

	const ImVec2 vecBufferSize = CalcTextSize(szBuffer);
	RenderFrame(ImVec2(rectFrame.Max.x - vecBufferSize.x, rectTotal.Min.y), ImVec2(rectFrame.Max.x, rectTotal.Min.y + style.FramePadding.y + vecBufferSize.y), GetColorU32((bHovered || bClicked || bDoubleClicked) ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, style.FrameRounding);
	pWindow->DrawList->AddText(ImVec2(rectFrame.Max.x - vecBufferSize.x, rectTotal.Min.y + style.FramePadding.y), GetColorU32(g.ActiveId == nIndex ? ImGuiCol_Text : ImGuiCol_TextDisabled), szBuffer);

	if (vecLabelSize.x > 0.f)
		RenderText(ImVec2(rectTotal.Min.x, rectTotal.Min.y + style.FramePadding.y), szLabel);

	PopStyleVar();
	return bValueChanged;
}

bool ImGui::HotKey(const char* szLabel, KeyBind_t* pKeyBind, const bool bAllowSwitch)
{
	const bool bValueChanged = HotKey(szLabel, &pKeyBind->uKey);

	if (bAllowSwitch)
	{
		char* szUniqueID = static_cast<char*>(MEM_STACKALLOC(CRT::StringLength(szLabel) + 6));
		CRT::StringCat(CRT::StringCopy(szUniqueID, CS_XOR("key##")), szLabel);

		if (IsItemClicked(ImGuiMouseButton_Right))
			OpenPopup(szUniqueID);

		if (BeginPopup(szUniqueID))
		{
			SetNextItemWidth(ImGui::GetWindowWidth() * 0.75f);
			if (Combo(CS_XOR("##keybind.mode"), reinterpret_cast<int*>(&pKeyBind->nMode), CS_XOR("Hold\0Toggle\0\0")))
				CloseCurrentPopup();

			EndPopup();
		}

		MEM_STACKFREE(szUniqueID);
	}

	return bValueChanged;
}

bool ImGui::MultiCombo(const char* szLabel, unsigned int* pFlags, const char* const* arrItems, int nItemsCount)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* pWindow = g.CurrentWindow;

	if (pWindow->SkipItems)
		return false;

	IM_ASSERT(nItemsCount < 32); // bitflags shift overflow, decrease items count or change variable type

	const ImGuiStyle& style = g.Style;
	const ImVec2 vecLabelSize = CalcTextSize(szLabel, nullptr, true);
	const float flActiveWidth = CalcItemWidth() - (vecLabelSize.x > 0.0f ? style.ItemInnerSpacing.x + GetFrameHeight() : 0.0f);

	std::vector<const char*> vecActiveItems = {};

	// collect active items
	for (int i = 0; i < nItemsCount; i++)
	{
		if (*pFlags & (1 << i))
			vecActiveItems.push_back(arrItems[i]);
	}

	// fuck it, stl still haven't boost::join, fmt::join replacement
	std::string strBuffer = {};
	for (std::size_t i = 0U; i < vecActiveItems.size(); i++)
	{
		strBuffer.append(vecActiveItems[i]);

		if (i < vecActiveItems.size() - 1U)
			strBuffer.append(", ");
	}

	if (strBuffer.empty())
		strBuffer.assign("none");
	else
	{
		const char* szWrapPosition = g.Font->CalcWordWrapPositionA(GetCurrentWindow()->FontWindowScale, strBuffer.data(), strBuffer.data() + strBuffer.length(), flActiveWidth - style.FramePadding.x * 2.0f);
		const std::size_t nWrapLength = szWrapPosition - strBuffer.data();

		if (nWrapLength > 0U && nWrapLength < strBuffer.length())
		{
			strBuffer.resize(nWrapLength);
			strBuffer.append("...");
		}
	}

	bool bValueChanged = false;
	if (BeginCombo(szLabel, strBuffer.c_str()))
	{
		for (int i = 0; i < nItemsCount; i++)
		{
			const int nCurrentFlag = (1 << i);
			if (Selectable(arrItems[i], (*pFlags & nCurrentFlag), ImGuiSelectableFlags_DontClosePopups))
			{
				// flip bitflag
				*pFlags ^= nCurrentFlag;
				bValueChanged = true;
			}
		}

		EndCombo();
	}

	return bValueChanged;
}

bool ImGui::BeginListBox(const char* szLabel, int nItemsCount, int nHeightInItems)
{
	float height = GetTextLineHeightWithSpacing() * ((nHeightInItems < 0 ? ImMin(nItemsCount, 7) : nHeightInItems) + 0.25f) + GetStyle().FramePadding.y * 2.0f;
	return BeginListBox(szLabel, ImVec2(0.0f, height));
}

bool ImGui::ColorEdit3(const char* szLabel, Color_t* pColor, ImGuiColorEditFlags flags)
{
	return ColorEdit4(szLabel, pColor, flags | ImGuiColorEditFlags_NoAlpha);
}

bool ImGui::ColorEdit4(const char* szLabel, Color_t* pColor, ImGuiColorEditFlags flags)
{
	float arrColor[4];
	pColor->BaseAlpha(arrColor);

	if (ColorEdit4(szLabel, &arrColor[0], flags))
	{
		*pColor = Color_t::FromBase4(arrColor);
		return true;
	}

	return false;
}

bool ImGui::ColorEdit3(const char* szLabel, ColorPickerVar_t* pColor, ImGuiColorEditFlags flags)
{
	return ColorEdit4(szLabel, pColor, flags | ImGuiColorEditFlags_NoAlpha);
}

bool ImGui::ColorEdit4(const char* szLabel, ColorPickerVar_t* pColorVar, ImGuiColorEditFlags flags)
{
	const bool bResult = ColorEdit4(szLabel, &pColorVar->colValue, flags);

	// switch rainbow mode on middle mouse click
	if (IsItemHovered())
	{
		// tooltip for turn on/off rainbow mode
		BeginTooltip();
		{
			PushTextWrapPos(450.f);
			TextUnformatted(CS_XOR("use mouse middle-click to turn on/off rainbow mode!"));
			PopTextWrapPos();
		}
		EndTooltip();

		if (IsMouseClicked(ImGuiMouseButton_Middle))
			pColorVar->bRainbow = !pColorVar->bRainbow;
	}

	// open the context popup
	OpenPopupOnItemClick(CS_XOR("context##color.picker"), ImGuiPopupFlags_MouseButtonRight);
	// @todo: cleaner code
	SetNextWindowSize(ImVec2((pColorVar->bRainbow ? 120.f : 60.f) * D::CalculateDPI(C_GET(int, Vars.nDpiScale)), 0.f));
	if (BeginPopup(CS_XOR("context##color.picker")))
	{
		if (Button(CS_XOR("copy##color.picker"), ImVec2(-1, 15 * D::CalculateDPI(C_GET(int, Vars.nDpiScale)))))
		{
			// @todo: im32 hex format is AARRGGBB, but we need RRGGBBAA
			CRT::String_t<64U> szBuffer(CS_XOR("#%X"), pColorVar->colValue.GetU32());
			SetClipboardText(szBuffer.Data());
			szBuffer.Clear();

			CloseCurrentPopup();
		}

		if (Button(CS_XOR("paste##color.picker"), ImVec2(-1, 15 * D::CalculateDPI(C_GET(int, Vars.nDpiScale)))))
		{
			const char* szClipboardText = GetClipboardText();
			// @note: +1U for '#' prefix skipping
			const ImU32 uConvertedColor = CRT::StringToInteger<ImU32>(szClipboardText + 1U, nullptr, 16);

			pColorVar->colValue = Color_t(uConvertedColor);
			CloseCurrentPopup();
		}

		if (pColorVar->bRainbow)
		{
			// @note: urgh padding moment idk
			SetNextItemWidth(ImGui::GetWindowWidth() * 0.90f + 1.f);
			SliderFloat(CS_XOR("##speed.color.picker"), &pColorVar->flRainbowSpeed, 0.f, 5.f, CS_XOR("speed: %.1f"), ImGuiSliderFlags_AlwaysClamp);
		}

		EndPopup();
	}

	return bResult;
}

#pragma endregion

// thread-safe draw data mutex
static SRWLOCK drawLock = {};

static void* __cdecl ImGuiAllocWrapper(const std::size_t nSize, [[maybe_unused]] void* pUserData = nullptr)
{
	return MEM::HeapAlloc(nSize);
}

static void __cdecl ImGuiFreeWrapper(void* pMemory, [[maybe_unused]] void* pUserData = nullptr) noexcept
{
	MEM::HeapFree(pMemory);
}

bool D::Setup(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// check is it were already initialized
	if (bInitialized)
		return true;

	ImGui::SetAllocatorFunctions(ImGuiAllocWrapper, ImGuiFreeWrapper);

	ImGui::CreateContext();

	// setup platform and renderer bindings
	if (!ImGui_ImplWin32_Init(hWnd))
		return false;

	if (!ImGui_ImplDX11_Init(pDevice, pContext))
		return false;

	// create draw data containers
	pDrawListActive = IM_NEW(ImDrawList)(ImGui::GetDrawListSharedData());
	pDrawListSafe = IM_NEW(ImDrawList)(ImGui::GetDrawListSharedData());
	pDrawListRender = IM_NEW(ImDrawList)(ImGui::GetDrawListSharedData());

	// setup styles
#pragma region draw_setup_style
	ImGuiStyle& style = ImGui::GetStyle();
	style.Alpha = 1.0f;
	style.WindowPadding = ImVec2(8, 8);
	style.WindowRounding = 4.0f;
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(32, 32);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.ChildRounding = 4.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 4.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4, 2);
	style.FrameRounding = 4.0f;
	style.FrameBorderSize = 1.0f;
	style.ItemSpacing = ImVec2(8, 4);
	style.ItemInnerSpacing = ImVec2(4, 4);
	style.IndentSpacing = 6.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 6.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 0.0f;
	style.GrabRounding = 4.0f;
	style.TabRounding = 4.0f;
	style.TabBorderSize = 1.0f;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.5f);
	style.WindowShadowSize = 0.f;
	style.AntiAliasedLines = true;
	style.AntiAliasedFill = true;
	style.AntiAliasedLinesUseTex = true;
	style.ColorButtonPosition = ImGuiDir_Right;
#pragma endregion

	ImGuiIO& io = ImGui::GetIO();

	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.FontDataOwnedByAtlas = false;

	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	ImFontConfig imVerdanaConfig;
	imVerdanaConfig.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_LightHinting;
	for (int i = 0; i < CS_ARRAYSIZE(FONT::pMenu); i++)
	{
		const float flFontSize = 12.f * CalculateDPI(i);
		FONT::pMenu[i] = io.Fonts->AddFontFromFileTTF(CS_XOR("C:\\Windows\\Fonts\\Verdana.ttf"), flFontSize, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic());
		io.Fonts->AddFontFromMemoryTTF((void*)fa_solid_900, sizeof(fa_solid_900), flFontSize, &icons_config, icons_ranges);
	}

	imVerdanaConfig.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_Bold;
	FONT::pExtra = io.Fonts->AddFontFromFileTTF(CS_XOR("C:\\Windows\\Fonts\\Verdana.ttf"), 14.f, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic());

	ImFontConfig imTahomaConfig;
	imTahomaConfig.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_LightHinting;
	FONT::pVisual = io.Fonts->AddFontFromFileTTF(CS_XOR("C:\\Windows\\Fonts\\Tahoma.ttf"), 14.f, &imTahomaConfig, io.Fonts->GetGlyphRangesCyrillic());

	io.Fonts->FontBuilderFlags = ImGuiFreeTypeBuilderFlags_LightHinting;
	bInitialized = io.Fonts->Build();
	return bInitialized;
}

void D::Destroy()
{
	// check is it already destroyed or wasn't initialized at all
	if (!bInitialized)
		return;

	// free draw data containers
	IM_DELETE(pDrawListActive);
	IM_DELETE(pDrawListSafe);
	IM_DELETE(pDrawListRender);

	// shutdown imgui directx9 renderer binding
	ImGui_ImplDX11_Shutdown();

	// shutdown imgui win32 platform binding
	ImGui_ImplWin32_Shutdown();

	// destroy imgui context
	ImGui::DestroyContext();

	bInitialized = false;
}

#pragma region draw_callbacks

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool D::OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// check is drawing initialized
	if (!bInitialized)
		return false;

	IPT::OnWndProc(hWnd, uMsg, wParam, lParam);

	// switch menu state
	if (IPT::IsKeyReleased(C_GET(unsigned int, Vars.nMenuKey)))
	{
		MENU::bMainWindowOpened = !MENU::bMainWindowOpened;
		// update animation
		MENU::animMenuDimBackground.Switch();
		// handle mouse input when menu is opened
		if (I::InputSystem->IsRelativeMouseMode())
		{
			// set input system mouse mode
			MEM::fnSetRelativeMouseMode(!MENU::bMainWindowOpened);
			// set input system window grab state
			MEM::fnSetWindowGrab(I::InputSystem->GetSDLWindow(), !MENU::bMainWindowOpened);
			// warp our cursor into middle of the screen
			const ImVec2 vecScreenCenter = ImGui::GetIO().DisplaySize / 2.f;
			MEM::fnWarpMouseInWindow(nullptr, vecScreenCenter.x, vecScreenCenter.y);
		}
	}

	// handle ImGui's window messages and block game's input if menu is opened
	return ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam) || MENU::bMainWindowOpened;
}

void D::NewFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void D::Render()
{
	ImGui::Render();
	RenderDrawData(ImGui::GetDrawData());
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

#pragma endregion

#pragma region draw_main

void D::RenderDrawData(ImDrawData* pDrawData)
{
	if (::TryAcquireSRWLockExclusive(&drawLock))
	{
		*pDrawListRender = *pDrawListSafe;
		::ReleaseSRWLockExclusive(&drawLock);
	}

	if (pDrawListRender->CmdBuffer.empty())
		return;

	// remove trailing command if unused
	// @note: equivalent to  pDrawList->_PopUnusedDrawCmd()
	if (const ImDrawCmd& lastCommand = pDrawListRender->CmdBuffer.back(); lastCommand.ElemCount == 0 && lastCommand.UserCallback == nullptr)
	{
		pDrawListRender->CmdBuffer.pop_back();
		if (pDrawListRender->CmdBuffer.empty())
			return;
	}

	ImGuiContext* pContext = ImGui::GetCurrentContext();
	ImGuiViewportP* pViewport = pContext->Viewports[0];
	ImVector<ImDrawList*>* vecDrawLists = pViewport->DrawDataBuilder.Layers[0];
	vecDrawLists->push_front(pDrawListRender); // this one being most background

	pDrawData->CmdLists.push_front(pDrawListRender);
	pDrawData->CmdListsCount = vecDrawLists->Size;
	pDrawData->TotalVtxCount += pDrawListRender->VtxBuffer.Size;
	pDrawData->TotalIdxCount += pDrawListRender->IdxBuffer.Size;
}

void D::ResetDrawData()
{
	pDrawListActive->_ResetForNewFrame();
	pDrawListActive->PushTextureID(ImGui::GetIO().Fonts->TexID);
	pDrawListActive->PushClipRectFullScreen();
}

void D::SwapDrawData()
{
	::AcquireSRWLockExclusive(&drawLock);

	IM_ASSERT(pDrawListActive->VtxBuffer.Size == 0 || pDrawListActive->_VtxWritePtr == pDrawListActive->VtxBuffer.Data + pDrawListActive->VtxBuffer.Size);
	IM_ASSERT(pDrawListActive->IdxBuffer.Size == 0 || pDrawListActive->_IdxWritePtr == pDrawListActive->IdxBuffer.Data + pDrawListActive->IdxBuffer.Size);

	if (!(pDrawListActive->Flags & ImDrawListFlags_AllowVtxOffset))
		IM_ASSERT(static_cast<int>(pDrawListActive->_VtxCurrentIdx) == pDrawListActive->VtxBuffer.Size);

	*pDrawListSafe = *pDrawListActive;

	::ReleaseSRWLockExclusive(&drawLock);
}

#pragma endregion

#pragma region draw_bindings

bool D::WorldToScreen(const Vector_t& vecOrigin, ImVec2* pvecScreen)
{
	const float flWidth = SDK::ViewMatrix[3][0] * vecOrigin.x + SDK::ViewMatrix[3][1] * vecOrigin.y + SDK::ViewMatrix[3][2] * vecOrigin.z + SDK::ViewMatrix[3][3];

	// check is point can't fit on screen, because it's behind us
	if (flWidth < 0.001f)
		return false;

	// compute the scene coordinates of a point in 3D
	const float flInverse = 1.0f / flWidth;
	pvecScreen->x = (SDK::ViewMatrix[0][0] * vecOrigin.x + SDK::ViewMatrix[0][1] * vecOrigin.y + SDK::ViewMatrix[0][2] * vecOrigin.z + SDK::ViewMatrix[0][3]) * flInverse;
	pvecScreen->y = (SDK::ViewMatrix[1][0] * vecOrigin.x + SDK::ViewMatrix[1][1] * vecOrigin.y + SDK::ViewMatrix[1][2] * vecOrigin.z + SDK::ViewMatrix[1][3]) * flInverse;

	// screen transform
	// get the screen position in pixels of given point
	const ImVec2 vecDisplaySize = ImGui::GetIO().DisplaySize;
	pvecScreen->x = (vecDisplaySize.x * 0.5f) + (pvecScreen->x * vecDisplaySize.x) * 0.5f;
	pvecScreen->y = (vecDisplaySize.y * 0.5f) - (pvecScreen->y * vecDisplaySize.y) * 0.5f;
	return true;
}

float D::CalculateDPI(const int nScaleTarget)
{
	switch ((EMiscDpiScale)nScaleTarget)
	{
	case EMiscDpiScale::MISC_DPISCALE_DEFAULT:
		return 1.f;
	case EMiscDpiScale::MISC_DPISCALE_125:
		return 1.25f;
	case EMiscDpiScale::MISC_DPISCALE_150:
		return 1.5f;
	case EMiscDpiScale::MISC_DPISCALE_175:
		return 1.75f;
	case EMiscDpiScale::MISC_DPISCALE_200:
		return 2.f;
	default:
		return 1.f;
	}
}

void D::AddDrawListRect(ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, const Color_t& colRect, const unsigned int uFlags, const Color_t& colOutline, const float flRounding, const ImDrawFlags roundingCorners, float flThickness, const float flOutlineThickness)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	const ImU32 colRectPacked = colRect.GetU32();
	const ImU32 colOutlinePacked = colOutline.GetU32();

	if (uFlags & DRAW_RECT_FILLED)
		pDrawList->AddRectFilled(vecMin, vecMax, colRectPacked, flRounding, roundingCorners);
	else
	{
		pDrawList->AddRect(vecMin, vecMax, colRectPacked, flRounding, roundingCorners, flThickness);
		flThickness *= 0.5f;
	}

	const float flHalfOutlineThickness = flOutlineThickness * 0.5f;
	const ImVec2 vecThicknessOffset = { flThickness + flHalfOutlineThickness, flThickness + flHalfOutlineThickness };

	if (uFlags & DRAW_RECT_BORDER)
		pDrawList->AddRect(vecMin + vecThicknessOffset, vecMax - vecThicknessOffset, colOutlinePacked, flRounding, roundingCorners, flOutlineThickness);

	if (uFlags & DRAW_RECT_OUTLINE)
		pDrawList->AddRect(vecMin - vecThicknessOffset, vecMax + vecThicknessOffset, colOutlinePacked, flRounding, roundingCorners, flOutlineThickness);
}

void D::AddDrawListRectMultiColor(ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, const Color_t& colUpperLeft, const Color_t& colUpperRight, const Color_t& colBottomRight, const Color_t& colBottomLeft)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	pDrawList->AddRectFilledMultiColor(vecMin, vecMax, colUpperLeft.GetU32(), colUpperRight.GetU32(), colBottomRight.GetU32(), colBottomLeft.GetU32());
}

void D::AddDrawListCircle(ImDrawList* pDrawList, const ImVec2& vecCenter, const float flRadius, const Color_t& colCircle, const int nSegments, const unsigned int uFlags, const Color_t& colOutline, float flThickness, const float flOutlineThickness)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	const ImU32 colCirclePacked = colCircle.GetU32();

	if (uFlags & DRAW_CIRCLE_FILLED)
	{
		pDrawList->AddCircleFilled(vecCenter, flRadius, colCirclePacked, nSegments);
		flThickness = 0.0f;
	}
	else
		pDrawList->AddCircle(vecCenter, flRadius, colCirclePacked, nSegments, flThickness);

	if (uFlags & DRAW_CIRCLE_OUTLINE)
		pDrawList->AddCircle(vecCenter, flRadius + flOutlineThickness, colOutline.GetU32(), nSegments, flThickness + flOutlineThickness);
}

void D::AddDrawListArc(ImDrawList* pDrawList, const ImVec2& vecPosition, const float flRadius, const float flMinimumAngle, const float flMaximumAngle, const Color_t& colArc, const float flThickness)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	pDrawList->PathArcTo(vecPosition, flRadius, M_DEG2RAD(flMinimumAngle), M_DEG2RAD(flMaximumAngle), 32);
	pDrawList->PathStroke(colArc.GetU32(), false, flThickness);
}

void D::AddDrawListLine(ImDrawList* pDrawList, const ImVec2& vecFirst, const ImVec2& vecSecond, const Color_t& colLine, const float flThickness)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	pDrawList->AddLine(vecFirst, vecSecond, colLine.GetU32(), flThickness);
}

void D::AddDrawListTriangle(ImDrawList* pDrawList, const ImVec2& vecFirst, const ImVec2& vecSecond, const ImVec2& vecThird, const Color_t& colTriangle, const unsigned int uFlags, const Color_t& colOutline, const float flThickness)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	const ImU32 colTrianglePacked = colTriangle.GetU32();

	if (uFlags & DRAW_TRIANGLE_FILLED)
		pDrawList->AddTriangleFilled(vecFirst, vecSecond, vecThird, colTrianglePacked);
	else
		pDrawList->AddTriangle(vecFirst, vecSecond, vecThird, colTrianglePacked, flThickness);

	if (uFlags & DRAW_TRIANGLE_OUTLINE)
		pDrawList->AddTriangle(vecFirst, vecSecond, vecThird, colOutline.GetU32(), flThickness + 1.0f);
}

void D::AddDrawListQuad(ImDrawList* pDrawList, const ImVec2& vecFirst, const ImVec2& vecSecond, const ImVec2& vecThird, const ImVec2& vecFourth, const Color_t& colQuad, const unsigned int uFlags, const Color_t& colOutline, const float flThickness)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	const ImU32 colQuadPacked = colQuad.GetU32();

	if (uFlags & DRAW_QUAD_FILLED)
		pDrawList->AddQuadFilled(vecFirst, vecSecond, vecThird, vecFourth, colQuadPacked);
	else
		pDrawList->AddQuad(vecFirst, vecSecond, vecThird, vecFourth, colQuadPacked, flThickness);

	if (uFlags & DRAW_QUAD_OUTLINE)
		pDrawList->AddQuad(vecFirst, vecSecond, vecThird, vecFourth, colOutline.GetU32(), flThickness + 1.0f);
}

void D::AddDrawListPolygon(ImDrawList* pDrawList, const ImVec2* vecPoints, const int nPointsCount, const Color_t& colPolygon, unsigned int uFlags, const Color_t& colOutline, const bool bClosed, const float flThickness)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	const ImU32 colPolygonPacked = colPolygon.GetU32();

	if (uFlags & DRAW_POLYGON_FILLED)
		pDrawList->AddConvexPolyFilled(vecPoints, nPointsCount, colPolygonPacked);
	else
		pDrawList->AddPolyline(vecPoints, nPointsCount, colPolygonPacked, bClosed, flThickness);

	if (uFlags & DRAW_POLYGON_OUTLINE)
		pDrawList->AddPolyline(vecPoints, nPointsCount, colOutline.GetU32(), bClosed, flThickness + 1.0f);
}

void D::AddDrawListText(ImDrawList* pDrawList, const ImFont* pFont, const ImVec2& vecPosition, const char* szText, const Color_t& colText, const unsigned int uFlags, const Color_t& colOutline, const float flThickness)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	// set font texture
	pDrawList->PushTextureID(pFont->ContainerAtlas->TexID);

	const ImU32 colOutlinePacked = colOutline.GetU32();

	if (uFlags & DRAW_TEXT_DROPSHADOW)
		pDrawList->AddText(pFont, pFont->FontSize, vecPosition + ImVec2(flThickness, flThickness), colOutlinePacked, szText);
	else if (uFlags & DRAW_TEXT_OUTLINE)
	{
		pDrawList->AddText(pFont, pFont->FontSize, vecPosition + ImVec2(flThickness, -flThickness), colOutlinePacked, szText);
		pDrawList->AddText(pFont, pFont->FontSize, vecPosition + ImVec2(-flThickness, flThickness), colOutlinePacked, szText);
	}

	pDrawList->AddText(pFont, pFont->FontSize, vecPosition, colText.GetU32(), szText);
	pDrawList->PopTextureID();
}

void D::AddDrawListShadowRect(ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, const Color_t& colShadow, float flThickness, float flRounding, ImDrawFlags roundingCorners)
{
	if (pDrawList == nullptr)
		pDrawList = pDrawListActive;

	pDrawList->AddShadowRect(vecMin, vecMax, colShadow.GetU32(), flThickness, ImVec2(0, 0), roundingCorners, flRounding);
}

#pragma endregion

#pragma region draw_structures

void AnimationHandler_t::Update(const float flDeltaTime, const float flDuration)
{
	if (fnEaseIn == nullptr)
		fnEaseIn = &EASING::InSine;

	if (fnEaseOut == nullptr)
		fnEaseOut = &EASING::OutSine;

	// Reset the elapsed time if the bool switches
	if (bSwitch != bLastSwitch)
		flElapsedTime = 0;

	flElapsedTime = MATH::Max(0.0f, MATH::Min(flElapsedTime, flDuration));
	float flTime = flElapsedTime / flDuration;

	// Determine the initial and target value based on the current state
	float flInitialValue = bSwitch ? 0.1f : flValue;
	float flTargetValue = bSwitch ? 1.0f : 0.1f; /*(1.0f is max value)*/

	// Select the appropriate easing function based on the current state
	EasingFunction_t fnCurrentEase = bSwitch ? fnEaseIn : fnEaseOut;

	// Apply the appropriate easing function based on fade-in or fade-out (with lerping, which is basically what's the math were doing)
	flValue = (flInitialValue + (flTargetValue - flInitialValue)) * (float)fnCurrentEase(flTime);
	flValue = MATH::Clamp(flValue, 0.1f, 1.0f);

	flElapsedTime += flDeltaTime;
	bLastSwitch = bSwitch;
}

#pragma endregion
