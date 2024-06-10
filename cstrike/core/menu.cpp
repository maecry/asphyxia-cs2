#include "menu.h"

// used: config variables
#include "variables.h"

// used: iinputsystem
#include "interfaces.h"
#include "../sdk/interfaces/iengineclient.h"
#include "../sdk/interfaces/inetworkclientservice.h"
#include "../sdk/interfaces/iglobalvars.h"
#include "../sdk/interfaces/ienginecvar.h"

// used: overlay's context
#include "../features/visuals/overlay.h"

// used: notifications
#include "../utilities/notify.h"

#pragma region menu_array_entries
static constexpr const char* arrMiscDpiScale[] = {
	"100%",
	"125%",
	"150%",
	"175%",
	"200%"
};

static const std::pair<const char*, const std::size_t> arrColors[] = {
	{ "[accent] - main", Vars.colAccent0 },
	{ "[accent] - dark (hover)", Vars.colAccent1 },
	{ "[accent] - darker (active)", Vars.colAccent2 },
	{ "[primitive] - text", Vars.colPrimtv0 },
	{ "[primitive] - background", Vars.colPrimtv1 },
	{ "[primitive] - disabled", Vars.colPrimtv2 },
	{ "[primitive] - frame background", Vars.colPrimtv3 },
	{ "[primitive] - border", Vars.colPrimtv4 },
};

static constexpr const char* arrMenuAddition[] = {
	"dim",
	"particle",
	"glow"
};
#pragma endregion

void MENU::RenderMainWindow()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	// @test: we should always update the animation?
	animMenuDimBackground.Update(io.DeltaTime, style.AnimationSpeed);
	if (!bMainWindowOpened)
		return;

	const ImVec2 vecScreenSize = io.DisplaySize;
	const float flBackgroundAlpha = animMenuDimBackground.GetValue(1.f);
	flDpiScale = D::CalculateDPI(C_GET(int, Vars.nDpiScale));

	// @note: we call this every frame because we utilizing rainbow color as well! however it's not really performance friendly?
	UpdateStyle(&style);

	if (flBackgroundAlpha > 0.f)
	{
		if (C_GET(unsigned int, Vars.bMenuAdditional) & MENU_ADDITION_DIM_BACKGROUND)
			D::AddDrawListRect(ImGui::GetBackgroundDrawList(), ImVec2(0, 0), vecScreenSize, C_GET(ColorPickerVar_t, Vars.colPrimtv1).colValue.Set<COLOR_A>(125 * flBackgroundAlpha), DRAW_RECT_FILLED);

		if (C_GET(unsigned int, Vars.bMenuAdditional) & MENU_ADDITION_BACKGROUND_PARTICLE)
			menuParticle.Render(ImGui::GetBackgroundDrawList(), vecScreenSize, flBackgroundAlpha);
	}

	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, flBackgroundAlpha);

	ImGui::SetNextWindowPos(ImVec2(vecScreenSize.x * 0.5f, vecScreenSize.y * 0.5f), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(550 * flDpiScale, 350 * flDpiScale), ImGuiCond_Always);

	// handle main window get out of screen bound
	// @note: we call this here so it will override the previous SetNextWindowPos
	if (ImGuiWindow* pMainWindow = ImGui::FindWindowByName(CS_XOR("asphyxia")); pMainWindow != nullptr)
	{
		bool bRequireClamp = false;
		ImVec2 vecWindowPos = pMainWindow->Pos;
		if (pMainWindow->Pos.x < 0.0f)
		{
			bRequireClamp = true;
			vecWindowPos.x = 0.0f;
		}
		else if (pMainWindow->Size.x + pMainWindow->Pos.x > vecScreenSize.x)
		{
			bRequireClamp = true;
			vecWindowPos.x = vecScreenSize.x - pMainWindow->Size.x;
		}
		if (pMainWindow->Pos.y < 0.0f)
		{
			bRequireClamp = true;
			vecWindowPos.y = 0.0f;
		}
		else if (pMainWindow->Size.y + pMainWindow->Pos.y > vecScreenSize.y)
		{
			bRequireClamp = true;
			vecWindowPos.y = vecScreenSize.y - pMainWindow->Size.y;
		}

		if (bRequireClamp) // Necessary to prevent window from constantly undocking itself if docked.
			ImGui::SetNextWindowPos(vecWindowPos, ImGuiCond_Always);
	}

	// render main window
	ImGui::Begin(CS_XOR("asphyxia"), &bMainWindowOpened, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);
	{
		const ImVec2 vecMenuPos = ImGui::GetWindowPos();
		const ImVec2 vecMenuSize = ImGui::GetWindowSize();
		ImDrawList* pDrawList = ImGui::GetWindowDrawList();

		if (C_GET(unsigned int, Vars.bMenuAdditional) & MENU_ADDITION_GLOW)
			D::AddDrawListShadowRect(ImGui::GetBackgroundDrawList(), vecMenuPos, vecMenuPos + vecMenuSize, C_GET(ColorPickerVar_t, Vars.colAccent0).colValue, 64.f * flDpiScale, style.WindowRounding, ImDrawFlags_ShadowCutOutShapeBackground);

		const ImVec2 vecTitleSize = FONT::pMenu[C_GET(int, Vars.nDpiScale)]->CalcTextSizeA(12.f * flDpiScale, FLT_MAX, -1.f, CS_XOR("asphyxia"));
		pDrawList->AddText(ImVec2(vecMenuPos.x + vecMenuSize.x - style.WindowPadding.x - vecTitleSize.x, vecMenuPos.y + style.WindowPadding.y), ImGui::GetColorU32(ImGuiCol_Text), CS_XOR("asphyxia"));

		static const CTab arrTabs[] = {
			{ "ragebot", &T::RageBot },
			{ "legitbot", &T::LegitBot },
			{ "visuals", &T::Visuals },
			{ "miscellaneous", &T::Miscellaneous },
			{ "skins changer", &T::SkinsChanger }
		};

		T::Render(CS_XOR("##main_tabs"), arrTabs, CS_ARRAYSIZE(arrTabs), &nCurrentMainTab);
	}
	ImGui::End();

	ImGui::PopStyleVar();
}

void MENU::RenderOverlayPreviewWindow()
{
	using namespace F::VISUALS::OVERLAY;

	ImGuiStyle& style = ImGui::GetStyle();
	// @note: call this function inside rendermainwindow, else expect a crash...
	const ImVec2 vecMenuPos = ImGui::GetWindowPos();
	const ImVec2 vecMenuSize = ImGui::GetWindowSize();

	const ImVec2 vecOverlayPadding = ImVec2(30.f * flDpiScale, 50.f * flDpiScale);

	ImGui::SetNextWindowPos(ImVec2(vecMenuPos.x + vecMenuSize.x + style.WindowPadding.x, vecMenuPos.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(175 * flDpiScale, vecMenuSize.y), ImGuiCond_Always);
	ImGui::Begin(CS_XOR("preview"), &bMainWindowOpened, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
	{
		const ImVec2 vecWindowPos = ImGui::GetWindowPos();
		const ImVec2 vecWindowSize = ImGui::GetWindowSize();

		ImDrawList* pDrawList = ImGui::GetWindowDrawList();
		Context_t context;

		ImVec4 vecBox = {
			vecWindowPos.x + vecOverlayPadding.x,
			vecWindowPos.y + vecOverlayPadding.y,
			vecWindowPos.x + vecWindowSize.x - vecOverlayPadding.x,
			vecWindowPos.y + vecWindowSize.y - vecOverlayPadding.y
		};

		if (const auto& boxOverlayConfig = C_GET(FrameOverlayVar_t, Vars.overlayBox); boxOverlayConfig.bEnable)
		{
			const bool bHovered = context.AddBoxComponent(pDrawList, vecBox, 1, boxOverlayConfig.flThickness, boxOverlayConfig.flRounding, boxOverlayConfig.colPrimary, boxOverlayConfig.colOutline);
			if (bHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				ImGui::OpenPopup(CS_XOR("context##box.component"));
		}

		if (const auto& nameOverlayConfig = C_GET(TextOverlayVar_t, Vars.overlayName); nameOverlayConfig.bEnable)
			context.AddComponent(new CTextComponent(true, SIDE_TOP, DIR_TOP, FONT::pVisual, CS_XOR("asphyxia"), Vars.overlayName));

		if (const auto& healthOverlayConfig = C_GET(BarOverlayVar_t, Vars.overlayHealthBar); healthOverlayConfig.bEnable)
		{
			const float flFactor = M_SIN(ImGui::GetTime() * 5.f) * 0.55f + 0.45f;
			context.AddComponent(new CBarComponent(true, SIDE_LEFT, vecBox, flFactor, Vars.overlayHealthBar));
		}

		if (const auto& armorOverlayConfig = C_GET(BarOverlayVar_t, Vars.overlayArmorBar); armorOverlayConfig.bEnable)
		{
			const float flArmorFactor = M_SIN(ImGui::GetTime() * 5.f) * 0.55f + 0.45f;
			context.AddComponent(new CBarComponent(false, SIDE_BOTTOM, vecBox, flArmorFactor, Vars.overlayArmorBar));
		}

		// only render context preview if overlay is enabled
		context.Render(pDrawList, vecBox);

		if (ImGui::BeginPopup(CS_XOR("context##box.component"), ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::ColorEdit4(CS_XOR("primary color##box.component"), &C_GET(FrameOverlayVar_t, Vars.overlayBox).colPrimary);
			ImGui::ColorEdit4(CS_XOR("outline color##box.component"), &C_GET(FrameOverlayVar_t, Vars.overlayBox).colOutline);
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.75f);
			ImGui::SliderFloat(CS_XOR("thickness##box.component"), &C_GET(FrameOverlayVar_t, Vars.overlayBox).flThickness, 1.f, 5.f, CS_XOR("%.1f"), ImGuiSliderFlags_AlwaysClamp);
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.75f);
			ImGui::SliderFloat(CS_XOR("rounding##box.component"), &C_GET(FrameOverlayVar_t, Vars.overlayBox).flRounding, 1.f, 5.f, CS_XOR("%.1f"), ImGuiSliderFlags_AlwaysClamp);

			ImGui::PopStyleVar();

			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void MENU::RenderWatermark()
{
	if (!C_GET(bool, Vars.bWatermark) || !bMainWindowOpened)
		return;

	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.f, 0.f, 0.f, 0.03f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.03f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.03f));
	ImGui::PushFont(FONT::pExtra);
	ImGui::BeginMainMenuBar();
	{
		ImGui::Dummy(ImVec2(1, 1));

#ifdef _DEBUG
		ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), CS_XOR("debug"));
#endif
		if (CRT::StringString(GetCommandLineW(), CS_XOR(L"-insecure")) != nullptr)
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), CS_XOR("insecure"));

		if (I::Engine->IsInGame())
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), CS_XOR("in-game"));

		static ImVec2 vecNameSize = ImGui::CalcTextSize(CS_XOR("asphyxia | " __DATE__ " " __TIME__));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - vecNameSize.x - style.FramePadding.x);
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), CS_XOR("asphyxia | " __DATE__ " " __TIME__));
	}
	ImGui::EndMainMenuBar();
	ImGui::PopFont();
	ImGui::PopStyleColor(3);
}

void MENU::UpdateStyle(ImGuiStyle* pStyle)
{
	ImGuiStyle& style = pStyle != nullptr ? *pStyle : ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = C_GET(ColorPickerVar_t, Vars.colPrimtv0).colValue.GetVec4(1.f); // primtv 0
	style.Colors[ImGuiCol_TextDisabled] = C_GET(ColorPickerVar_t, Vars.colPrimtv2).colValue.GetVec4(0.85f); // primtv 2
	style.Colors[ImGuiCol_TextSelectedBg] = C_GET(ColorPickerVar_t, Vars.colAccent1).colValue.GetVec4(0.85f); // accent 1

	style.Colors[ImGuiCol_WindowBg] = C_GET(ColorPickerVar_t, Vars.colPrimtv3).colValue.GetVec4(0.95f); // primtv 3
	style.Colors[ImGuiCol_ChildBg] = C_GET(ColorPickerVar_t, Vars.colPrimtv1).colValue.GetVec4(1.0f); // primtv 1
	style.Colors[ImGuiCol_PopupBg] = C_GET(ColorPickerVar_t, Vars.colPrimtv1).colValue.GetVec4(1.0f); // primtv 1
	style.Colors[ImGuiCol_WindowShadow] = C_GET(ColorPickerVar_t, Vars.colAccent2).colValue.GetVec4(0.80f); // accent 3

	style.Colors[ImGuiCol_Border] = C_GET(ColorPickerVar_t, Vars.colPrimtv4).colValue.GetVec4(0.50f); // primtv 4
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f); // clear

	style.Colors[ImGuiCol_FrameBg] = C_GET(ColorPickerVar_t, Vars.colPrimtv3).colValue.GetVec4(1.0f); // primtv 3
	style.Colors[ImGuiCol_FrameBgHovered] = C_GET(ColorPickerVar_t, Vars.colAccent1).colValue.GetVec4(0.8f); // accent 1
	style.Colors[ImGuiCol_FrameBgActive] = C_GET(ColorPickerVar_t, Vars.colAccent2).colValue.GetVec4(0.6f); // accent 0

	style.Colors[ImGuiCol_TitleBg] = C_GET(ColorPickerVar_t, Vars.colAccent0).colValue.GetVec4(0.20f); // accent 0
	style.Colors[ImGuiCol_TitleBgActive] = C_GET(ColorPickerVar_t, Vars.colAccent2).colValue.GetVec4(0.50f); // accent 1
	style.Colors[ImGuiCol_TitleBgCollapsed] = C_GET(ColorPickerVar_t, Vars.colAccent1).colValue.GetVec4(0.20f); // accent 1

	style.Colors[ImGuiCol_MenuBarBg] = C_GET(ColorPickerVar_t, Vars.colPrimtv1).colValue.GetVec4(0.70f); // primtv 1

	style.Colors[ImGuiCol_ScrollbarBg] = C_GET(ColorPickerVar_t, Vars.colPrimtv3).colValue.GetVec4(0.30f); // primtv 3
	style.Colors[ImGuiCol_ScrollbarGrab] = C_GET(ColorPickerVar_t, Vars.colAccent2).colValue.GetVec4(1.00f); // accent 3
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = C_GET(ColorPickerVar_t, Vars.colAccent1).colValue.GetVec4(0.90f); // primtv 5
	style.Colors[ImGuiCol_ScrollbarGrabActive] = C_GET(ColorPickerVar_t, Vars.colAccent2).colValue.GetVec4(0.50f); // primtv 2

	style.Colors[ImGuiCol_CheckMark] = C_GET(ColorPickerVar_t, Vars.colAccent0).colValue.GetVec4(1.00f); // primtv 0

	style.Colors[ImGuiCol_SliderGrab] = C_GET(ColorPickerVar_t, Vars.colAccent0).colValue.GetVec4(1.0f); // accent 0
	style.Colors[ImGuiCol_SliderGrabActive] = C_GET(ColorPickerVar_t, Vars.colAccent1).colValue.GetVec4(0.8f); // accent 1

	style.Colors[ImGuiCol_Button] = C_GET(ColorPickerVar_t, Vars.colPrimtv3).colValue.GetVec4(1.0f); // primtv 3
	style.Colors[ImGuiCol_ButtonHovered] = C_GET(ColorPickerVar_t, Vars.colAccent1).colValue.GetVec4(1.00f); // primtv 5
	style.Colors[ImGuiCol_ButtonActive] = C_GET(ColorPickerVar_t, Vars.colAccent2).colValue.GetVec4(1.00f); // accent 0

	style.Colors[ImGuiCol_Header] = C_GET(ColorPickerVar_t, Vars.colAccent0).colValue.GetVec4(1.00f); // accent 0
	style.Colors[ImGuiCol_HeaderHovered] = C_GET(ColorPickerVar_t, Vars.colAccent1).colValue.GetVec4(1.00f); // primtv 5
	style.Colors[ImGuiCol_HeaderActive] = C_GET(ColorPickerVar_t, Vars.colAccent2).colValue.GetVec4(1.0f); // primtv 3

	style.Colors[ImGuiCol_Separator] = C_GET(ColorPickerVar_t, Vars.colPrimtv3).colValue.GetVec4(1.0f); // primtv 3
	style.Colors[ImGuiCol_SeparatorHovered] = C_GET(ColorPickerVar_t, Vars.colAccent1).colValue.GetVec4(1.00f); // primtv 5
	style.Colors[ImGuiCol_SeparatorActive] = C_GET(ColorPickerVar_t, Vars.colAccent2).colValue.GetVec4(1.00f); // accent 0

	style.Colors[ImGuiCol_ResizeGrip] = C_GET(ColorPickerVar_t, Vars.colAccent0).colValue.GetVec4(1.00f); // accent 0
	style.Colors[ImGuiCol_ResizeGripHovered] = C_GET(ColorPickerVar_t, Vars.colAccent1).colValue.GetVec4(0.70f); // primtv 5
	style.Colors[ImGuiCol_ResizeGripActive] = C_GET(ColorPickerVar_t, Vars.colAccent2).colValue.GetVec4(1.0f); // accent 1

	style.Colors[ImGuiCol_Tab] = C_GET(ColorPickerVar_t, Vars.colPrimtv1).colValue.GetVec4(0.80f); // primtv 1
	style.Colors[ImGuiCol_TabHovered] = C_GET(ColorPickerVar_t, Vars.colAccent1).colValue.GetVec4(0.80f); // primtv 5
	style.Colors[ImGuiCol_TabActive] = C_GET(ColorPickerVar_t, Vars.colAccent2).colValue.GetVec4(0.70f); // accent 0
	style.Colors[ImGuiCol_TabUnfocused] = C_GET(ColorPickerVar_t, Vars.colAccent1).colValue.GetVec4(0.70f); // primtv 5
	style.Colors[ImGuiCol_TabUnfocusedActive] = C_GET(ColorPickerVar_t, Vars.colAccent2).colValue.GetVec4(0.60f); // accent 0

	style.Colors[ImGuiCol_PlotLines] = C_GET(ColorPickerVar_t, Vars.colAccent0).colValue.GetVec4(1.00f); // accent 0
	style.Colors[ImGuiCol_PlotLinesHovered] = C_GET(ColorPickerVar_t, Vars.colAccent0).colValue.GetVec4(0.50f); // accent 0
	style.Colors[ImGuiCol_PlotHistogram] = C_GET(ColorPickerVar_t, Vars.colAccent0).colValue.GetVec4(1.00f); // accent 0
	style.Colors[ImGuiCol_PlotHistogramHovered] = C_GET(ColorPickerVar_t, Vars.colAccent0).colValue.GetVec4(0.50f); // accent 0

	style.Colors[ImGuiCol_DragDropTarget] = C_GET(ColorPickerVar_t, Vars.colAccent2).colValue.GetVec4(0.80f); // accent 3

	style.Colors[ImGuiCol_ModalWindowDimBg] = C_GET(ColorPickerVar_t, Vars.colPrimtv4).colValue.GetVec4(0.25f); // primtv 4

	style.Colors[ImGuiCol_ControlBg] = C_GET(ColorPickerVar_t, Vars.colPrimtv2).colValue.GetVec4(1.0f); // primtv 3
	style.Colors[ImGuiCol_ControlBg] = C_GET(ColorPickerVar_t, Vars.colPrimtv4).colValue.GetVec4(1.0f); // primtv 5
	style.Colors[ImGuiCol_ControlBg] = C_GET(ColorPickerVar_t, Vars.colPrimtv1).colValue.GetVec4(0.10f); // primtv 2

	style.Colors[ImGuiCol_Triangle] = C_GET(ColorPickerVar_t, Vars.colPrimtv0).colValue.GetVec4(1.f); // accent 0

	C_GET(ColorPickerVar_t, Vars.colPrimtv0).UpdateRainbow(); // (text)
	C_GET(ColorPickerVar_t, Vars.colPrimtv1).UpdateRainbow(); // (background)
	C_GET(ColorPickerVar_t, Vars.colPrimtv2).UpdateRainbow(); // (disabled)
	C_GET(ColorPickerVar_t, Vars.colPrimtv3).UpdateRainbow(); // (control bg)
	C_GET(ColorPickerVar_t, Vars.colPrimtv4).UpdateRainbow(); // (border)

	C_GET(ColorPickerVar_t, Vars.colAccent0).UpdateRainbow(); // (main)
	C_GET(ColorPickerVar_t, Vars.colAccent1).UpdateRainbow(); // (dark)
	C_GET(ColorPickerVar_t, Vars.colAccent2).UpdateRainbow(); // (darker)

	// update animation speed
	style.AnimationSpeed = C_GET(float, Vars.flAnimationSpeed) / 10.f;
}

#pragma region menu_tabs

void T::Render(const char* szTabBar, const CTab* arrTabs, const unsigned long long nTabsCount, int* nCurrentTab, ImGuiTabBarFlags flags)
{
	if (ImGui::BeginTabBar(szTabBar, flags))
	{
		for (std::size_t i = 0U; i < nTabsCount; i++)
		{
			// add tab
			if (ImGui::BeginTabItem(arrTabs[i].szName))
			{
				// set current tab index
				*nCurrentTab = (int)i;
				ImGui::EndTabItem();
			}
		}

		// render inner tab
		if (arrTabs[*nCurrentTab].pRenderFunction != nullptr)
			arrTabs[*nCurrentTab].pRenderFunction();

		ImGui::EndTabBar();
	}
}

void T::RageBot()
{ }

void T::LegitBot()
{ }

void T::Visuals()
{
	ImGuiStyle& style = ImGui::GetStyle();

	MENU::RenderOverlayPreviewWindow();

	ImGui::Columns(2, CS_XOR("##visuals_collumns"), false);
	{
		static float flOverlayChildSize = 0.f;
		ImGui::BeginChild(CS_XOR("visuals.overlay"), ImVec2(0.f, flOverlayChildSize), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted("overlay");
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::Checkbox(CS_XOR("enable##overlay"), &C_GET(bool, Vars.bVisualOverlay));

			ImGui::BeginDisabled(!C_GET(bool, Vars.bVisualOverlay));
			{
				ImGui::Checkbox(CS_XOR("bounding box"), &C_GET(FrameOverlayVar_t, Vars.overlayBox).bEnable);
				ImGui::Checkbox(CS_XOR("name"), &C_GET(TextOverlayVar_t, Vars.overlayName).bEnable);
				ImGui::Checkbox(CS_XOR("health bar"), &C_GET(BarOverlayVar_t, Vars.overlayHealthBar).bEnable);
				ImGui::Checkbox(CS_XOR("armor bar"), &C_GET(BarOverlayVar_t, Vars.overlayArmorBar).bEnable);
			}
			ImGui::EndDisabled();

			ImGui::PopStyleVar();

			flOverlayChildSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
		}
		ImGui::EndChild();

		ImGui::BeginChild(CS_XOR("visuals.chams"), ImVec2{}, true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted("chams");
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::Checkbox(CS_XOR("enable##chams"), &C_GET(bool, Vars.bVisualChams));
			ImGui::Combo(CS_XOR("materials"), &C_GET(int, Vars.nVisualChamMaterial), CS_XOR("white\0illuminate\0"));
			ImGui::Checkbox(CS_XOR("enable invisible chams##chams"), &C_GET(bool, Vars.bVisualChamsIgnoreZ));

			ImGui::ColorEdit4(CS_XOR("visible color"), &C_GET(Color_t, Vars.colVisualChams));
			if (C_GET(bool, Vars.bVisualChamsIgnoreZ))
				ImGui::ColorEdit4(CS_XOR("invisible color"), &C_GET(Color_t, Vars.colVisualChamsIgnoreZ));

			ImGui::PopStyleVar();
		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
	}
	ImGui::Columns(1);
}

void T::Miscellaneous()
{
	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::Columns(2, CS_XOR("##misc_collumns"), false);
	{
		ImGui::BeginChild(CS_XOR("misc.general"), ImVec2(0, ImGui::GetContentRegionAvail().y / 2.f), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(CS_XOR("general"));
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::Checkbox(CS_XOR("watermark"), &C_GET(bool, Vars.bWatermark));

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 0.f, 1.f));
			if (ImGui::Button(CS_XOR("unlock hidden cvars"), ImVec2(-1, 15 * MENU::flDpiScale)))
			{
				I::Cvar->UnlockHiddenCVars();
				NOTIFY::Push({ N_TYPE_INFO, CS_XOR("unlocked all hidden cvars") });
			}

			ImGui::PopStyleColor();

			ImGui::PopStyleVar();
		}
		ImGui::EndChild();

		ImGui::BeginChild(CS_XOR("misc.movement"), ImVec2{}, true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(CS_XOR("movement"));
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::Checkbox(CS_XOR("auto bunny-hopping"), &C_GET(bool, Vars.bAutoBHop));
			if (C_GET(bool, Vars.bAutoBHop))
				ImGui::SliderInt(CS_XOR("chance"), &C_GET(int, Vars.nAutoBHopChance), 0, 100, CS_XOR("%d%%"));

			ImGui::Checkbox(CS_XOR("auto strafe"), &C_GET(bool, Vars.bAutoStrafe));

			ImGui::PopStyleVar();
		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
		static int nCurrentSettingSubTab = 0;
		ImGui::BeginChild(CS_XOR("misc.settings"), ImVec2(0, ImGui::GetContentRegionAvail().y / 2.f), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(CS_XOR("settings"));

				ImGui::EndMenuBar();
			}

			static const CTab arrSettingTabs[] = {
				CTab{ "configuration", []()
				{
					ImGui::Columns(2, CS_XOR("#CONFIG"), false);
					{
						ImGui::PushItemWidth(-1);

						// check selected configuration for magic value
						if (nSelectedConfig == ~1U)
						{
							// set default configuration as selected on first use
							for (std::size_t i = 0U; i < C::vecFileNames.size(); i++)
							{
								if (CRT::StringCompare(C::vecFileNames[i], CS_XOR(CS_CONFIGURATION_DEFAULT_FILE_NAME CS_CONFIGURATION_FILE_EXTENSION)) == 0)
									nSelectedConfig = i;
							}
						}

						if (ImGui::BeginListBox(CS_XOR("##config.list"), C::vecFileNames.size(), 5))
						{
							for (std::size_t i = 0U; i < C::vecFileNames.size(); i++)
							{
								// @todo: imgui cant work with wstring
								CRT::String_t<MAX_PATH> szFileName(C::vecFileNames[i]);
								if (ImGui::Selectable(szFileName.Data(), (nSelectedConfig == i)))
									nSelectedConfig = i;
							}

							ImGui::EndListBox();
						}

						ImGui::PopItemWidth();
					}
					ImGui::NextColumn();
					{
						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, 0));
						ImGui::PushItemWidth(-1);
						if (ImGui::InputTextWithHint(CS_XOR("##config.file"), CS_XOR("create new..."), szConfigFile, sizeof(szConfigFile), ImGuiInputTextFlags_EnterReturnsTrue))
						{
							// check if the filename isn't empty
							if (const std::size_t nConfigFileLength = CRT::StringLength(szConfigFile); nConfigFileLength > 0U)
							{
								CRT::WString_t wszConfigFile(szConfigFile);

								if (C::CreateFile(wszConfigFile.Data()))
									// set created config as selected @todo: dependent on current 'C::CreateFile' behaviour, generally it must be replaced by search
									nSelectedConfig = C::vecFileNames.size() - 1U;

								// clear string
								CRT::MemorySet(szConfigFile, 0U, sizeof(szConfigFile));
							}
						}
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(CS_XOR("press enter to create new configuration"));

						if (ImGui::Button(CS_XOR("save"), ImVec2(-1, 15 * MENU::flDpiScale)))
							C::SaveFile(nSelectedConfig);

						if (ImGui::Button(CS_XOR("load"), ImVec2(-1, 15 * MENU::flDpiScale)))
							C::LoadFile(nSelectedConfig);

						if (ImGui::Button(CS_XOR("remove"), ImVec2(-1, 15 * MENU::flDpiScale)))
							ImGui::OpenPopup(CS_XOR("confirmation##config.remove"));

						if (ImGui::Button(CS_XOR("refresh"), ImVec2(-1, 15 * MENU::flDpiScale)))
							C::Refresh();

						ImGui::PopItemWidth();
						ImGui::PopStyleVar();
					}
					ImGui::Columns(1);

					if (ImGui::BeginPopupModal(CS_XOR("confirmation##config.remove"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
					{
						CRT::String_t<MAX_PATH> szCurrentConfig(C::vecFileNames[nSelectedConfig]);

						ImGui::Text(CS_XOR("are you sure you want to remove \"%s\" configuration?"), szCurrentConfig);
						ImGui::Spacing();

						if (ImGui::Button(CS_XOR("no"), ImVec2(ImGui::GetContentRegionAvail().x / 2.f, 0)))
							ImGui::CloseCurrentPopup();

						ImGui::SameLine();

						if (ImGui::Button(CS_XOR("yes"), ImVec2(ImGui::GetContentRegionAvail().x, 0)))
						{
							C::RemoveFile(nSelectedConfig);

							// reset selected configuration index
							nSelectedConfig = ~0U;

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndPopup();
					}
				} },
				CTab{ "menu", []()
				{
					static int nSelectedColor = 0;

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, 0));

					ImGui::HotKey(CS_XOR("menu key"), &C_GET(unsigned int, Vars.nMenuKey));

					if (ImGui::BeginCombo(CS_XOR("dpi scale"), arrMiscDpiScale[C_GET(int, Vars.nDpiScale)]))
					{
						for (int i = 0; i < IM_ARRAYSIZE(arrMiscDpiScale); i++)
						{
							if (ImGui::Selectable(arrMiscDpiScale[i], (C_GET(int, Vars.nDpiScale) == i)))
								C_GET(int, Vars.nDpiScale) = i;
						}

						ImGui::EndCombo();
					}

					ImGui::MultiCombo(CS_XOR("additional settings"), &C_GET(unsigned int, Vars.bMenuAdditional), arrMenuAddition, CS_ARRAYSIZE(arrMenuAddition));

					ImGui::SliderFloat(CS_XOR("animation speed"), &C_GET(float, Vars.flAnimationSpeed), 1.f, 10.f);

					ImGui::SeparatorText(CS_XOR("colors pallete"));

					ImGui::PushItemWidth(-1);

					if (ImGui::BeginListBox(CS_XOR("##themes.select"), CS_ARRAYSIZE(arrColors), 5))
					{
						for (std::size_t i = 0U; i < CS_ARRAYSIZE(arrColors); i++)
						{
							const char* szColorName = arrColors[i].first;

							if (ImGui::Selectable(szColorName, (i == nSelectedColor)))
								nSelectedColor = (int)i;
						}

						ImGui::EndListBox();
					}

					ImGui::ColorEdit4(CS_XOR("##themes.picker"), &C_GET(ColorPickerVar_t, arrColors[nSelectedColor].second), ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_DisplayRGB);
					ImGui::PopItemWidth();
					ImGui::PopStyleVar();
				} }
			};

			Render(CS_XOR("##misc.setttings.tab"), arrSettingTabs, CS_ARRAYSIZE(arrSettingTabs), &nCurrentSettingSubTab);
		}
		ImGui::EndChild();
	}
	ImGui::Columns(1);
}

void T::SkinsChanger()
{
}

#pragma endregion

#pragma region menu_particle

void MENU::ParticleContext_t::Render(ImDrawList* pDrawList, const ImVec2& vecScreenSize, const float flAlpha)
{
	if (this->vecParticles.empty())
	{
		for (int i = 0; i < 100; i++)
			this->AddParticle(ImGui::GetIO().DisplaySize);
	}

	for (auto& particle : this->vecParticles)
	{
		this->DrawParticle(pDrawList, particle, C_GET(ColorPickerVar_t, Vars.colAccent0).colValue.Set<COLOR_A>(flAlpha * 255));
		this->UpdatePosition(particle, vecScreenSize);
		this->FindConnections(pDrawList, particle, C_GET(ColorPickerVar_t, Vars.colAccent2).colValue.Set<COLOR_A>(flAlpha * 255), 200.f);
	}
}

void MENU::ParticleContext_t::AddParticle(const ImVec2& vecScreenSize)
{
	// exceeded limit
	if (this->vecParticles.size() >= 200UL)
		return;

	// @note: random speed value
	static constexpr float flSpeed = 100.f;
	this->vecParticles.emplace_back(
	ImVec2(MATH::fnRandomFloat(0.f, vecScreenSize.x), MATH::fnRandomFloat(0.f, vecScreenSize.y)),
	ImVec2(MATH::fnRandomFloat(-flSpeed, flSpeed), MATH::fnRandomFloat(-flSpeed, flSpeed)));
}

void MENU::ParticleContext_t::DrawParticle(ImDrawList* pDrawList, ParticleData_t& particle, const Color_t& colPrimary)
{
	D::AddDrawListCircle(pDrawList, particle.vecPosition, 2.f, colPrimary, 12, DRAW_CIRCLE_OUTLINE | DRAW_CIRCLE_FILLED);
}

void MENU::ParticleContext_t::FindConnections(ImDrawList* pDrawList, ParticleData_t& particle, const Color_t& colPrimary, float flMaxDistance)
{
	for (auto& currentParticle : this->vecParticles)
	{
		// skip current particle
		if (&currentParticle == &particle)
			continue;

		/// @note: calcuate length distance 2d, return FLT_MAX if failed
		const float flDistance = ImLength(particle.vecPosition - currentParticle.vecPosition, FLT_MAX);
		if (flDistance <= flMaxDistance)
			this->DrawConnection(pDrawList, particle, currentParticle, (flMaxDistance - flDistance) / flMaxDistance, colPrimary);
	}
}

void MENU::ParticleContext_t::DrawConnection(ImDrawList* pDrawList, ParticleData_t& particle, ParticleData_t& otherParticle, float flAlpha, const Color_t& colPrimary) const
{
	D::AddDrawListLine(pDrawList, particle.vecPosition, otherParticle.vecPosition, colPrimary.Set<COLOR_A>(flAlpha * 255), 1.f);
}

void MENU::ParticleContext_t::UpdatePosition(ParticleData_t& particle, const ImVec2& vecScreenSize) const
{
	this->ResolveScreenCollision(particle, vecScreenSize);

	ImGuiStyle& style = ImGui::GetStyle();

	// move particle
	particle.vecPosition.x += (particle.vecVelocity.x * style.AnimationSpeed * 10.f) * ImGui::GetIO().DeltaTime;
	particle.vecPosition.y += (particle.vecVelocity.y * style.AnimationSpeed * 10.f) * ImGui::GetIO().DeltaTime;
}

void MENU::ParticleContext_t::ResolveScreenCollision(ParticleData_t& particle, const ImVec2& vecScreenSize) const
{
	if (particle.vecPosition.x + particle.vecVelocity.x > vecScreenSize.x || particle.vecPosition.x + particle.vecVelocity.x < 0)
		particle.vecVelocity.x = -particle.vecVelocity.x;

	if (particle.vecPosition.y + particle.vecVelocity.y > vecScreenSize.y || particle.vecPosition.y + particle.vecVelocity.y < 0)
		particle.vecVelocity.y = -particle.vecVelocity.y;
}

#pragma endregion
