#pragma once

// used: [d3d]
#include <d3d11.h>

#include "../common.h"

#include "../sdk/datatypes/color.h"
#include "../sdk/datatypes/vector.h"

// used: [ext] imgui
#include "../../dependencies/imgui/imgui.h"
#include "../../dependencies/imgui/imgui_internal.h"

// forward declarations
struct KeyBind_t;

#pragma region draw_objects_enumerations

enum ERectRenderFlags : unsigned int
{
	DRAW_RECT_NONE = 0,
	DRAW_RECT_OUTLINE = (1 << 0),
	DRAW_RECT_BORDER = (1 << 1),
	DRAW_RECT_FILLED = (1 << 2)
};

enum ECircleRenderFlags : unsigned int
{
	DRAW_CIRCLE_NONE = 0,
	DRAW_CIRCLE_OUTLINE = (1 << 0),
	DRAW_CIRCLE_FILLED = (1 << 1)
};

enum ETriangleRenderFlags : unsigned int
{
	DRAW_TRIANGLE_NONE = 0,
	DRAW_TRIANGLE_OUTLINE = (1 << 0),
	DRAW_TRIANGLE_FILLED = (1 << 1)
};

enum EQuadRenderFlags : unsigned int
{
	DRAW_QUAD_NONE = 0,
	DRAW_QUAD_OUTLINE = (1 << 0),
	DRAW_QUAD_FILLED = (1 << 1)
};

enum EPolygonRenderFlags : unsigned int
{
	DRAW_POLYGON_NONE = 0,
	DRAW_POLYGON_OUTLINE = (1 << 0),
	DRAW_POLYGON_FILLED = (1 << 1)
};

enum ETextRenderFlags : unsigned int
{
	DRAW_TEXT_NONE = 0,
	DRAW_TEXT_DROPSHADOW = (1 << 0),
	DRAW_TEXT_OUTLINE = (1 << 1)
};

#pragma endregion

#pragma region draw_structures
// predefined custom user type
struct ColorPickerVar_t;

typedef double (*EasingFunction_t)(double);
struct AnimationHandler_t
{
	// default: ease::in/outsine
	AnimationHandler_t(EasingFunction_t fnIn = nullptr, EasingFunction_t fnOut = nullptr) :
		fnEaseIn(fnIn), fnEaseOut(fnOut), bSwitch(false), bLastSwitch(false), flElapsedTime(0.f), flValue(0.1f){};
	~AnimationHandler_t() = default;

	// Has to be called every frame
	void Update(const float flDeltaTime, const float flDuration);

	// Get the current value multiplied by a scale
 	float GetValue(float flScale = 1.0f)
	{
		return flValue * flScale;
	} 

	const bool GetSwitch() const	
	{
		return bSwitch;
	}

	// switch state
	void Switch()
	{
		bSwitch = !bSwitch;
	}

	void SetSwitch(const bool bState)
	{
		bSwitch = bState;
	}

private:
	// Set to true for ease-in animation, false for ease-out
	bool bSwitch = 0;
	bool bLastSwitch = bSwitch;

	float flElapsedTime = 0.f;

	// Current value of the animation
	float flValue = 0.1f;

	// Ease in and out functions Declaration
	EasingFunction_t fnEaseIn = nullptr;
	EasingFunction_t fnEaseOut = nullptr;
};

#pragma endregion

/*
 * FONTS
 */
namespace FONT
{
	// 0. verdana, size: 12px * scaledDPI(1.0f->2.0f); lighthinting
	inline ImFont* pMenu[5];
	// 1. verdana, size: 14px; bold
	inline ImFont* pExtra;
	// 2. tahoma, size: 16px; lighthinting
	inline ImFont* pVisual;
}

// extended imgui functionality
namespace ImGui
{
	/* @section: main */
	void HelpMarker(const char* szDescription);
	bool HotKey(const char* szLabel, unsigned int* pValue);
	bool HotKey(const char* szLabel, KeyBind_t* pKeyBind, const bool bAllowSwitch = true);
	bool MultiCombo(const char* szLabel, unsigned int* pFlags, const char* const* arrItems, int nItemsCount);
	bool BeginListBox(const char* szLabel, int nItemsCount, int nHeightInItems = -1);

	/* @section: wrappers */
	bool ColorEdit3(const char* szLabel, Color_t* pColor, ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_DisplayHex);
	bool ColorEdit4(const char* szLabel, Color_t* pColor, ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_DisplayHex);

	bool ColorEdit3(const char* szLabel, ColorPickerVar_t* pColor, ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_DisplayHex);
	bool ColorEdit4(const char* szLabel, ColorPickerVar_t* pColorVar, ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_DisplayHex);
}

/*
 * DRAW
 * - rendering framework
 */
namespace D
{
	// initialize rendering engine, create fonts, set styles etc
	bool Setup(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	// shutdown rendering engine
	void Destroy();

	/* @section: callbacks */
	// handle input window message and save keys states in array
	bool OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	/* @section: main */
	// render primitives by stored safe data
	void RenderDrawData(ImDrawData* pDrawData);
	// reset active draw data container
	void ResetDrawData();
	// swap active draw data container to safe one
	void SwapDrawData();
	// call it before rendering
	void NewFrame();
	// call to render all primitives
	void Render();

	/* @section: get */
	/// convert world space to screen space coordinates by game's conversion matrix
	/// @param[out] pvecScreen output for converted screen position
	/// @returns: true if converted coordinates fit into display size, false otherwise
	bool WorldToScreen(const Vector_t& vecOrigin, ImVec2* pvecScreen);
	float CalculateDPI(const int nScaleTarget = 0);

	/* @section: bindings */
	void AddDrawListRect(ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, const Color_t& colRect, const unsigned int uFlags = DRAW_RECT_NONE, const Color_t& colOutline = Color_t(0, 0, 0, 255), const float flRounding = 0.f, const ImDrawFlags roundingCorners = ImDrawFlags_RoundCornersAll, float flThickness = 1.0f, const float flOutlineThickness = 1.0f);
	void AddDrawListRectMultiColor(ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, const Color_t& colUpperLeft, const Color_t& colUpperRight, const Color_t& colBottomRight, const Color_t& colBottomLeft);
	void AddDrawListCircle(ImDrawList* pDrawList, const ImVec2& vecCenter, const float flRadius, const Color_t& colCircle, const int nSegments, const unsigned int uFlags = DRAW_CIRCLE_NONE, const Color_t& colOutline = Color_t(0, 0, 0, 255), const float flThickness = 1.0f, const float flOutlineThickness = 1.0f);
	void AddDrawListArc(ImDrawList* pDrawList, const ImVec2& vecPosition, const float flRadius, const float flMinimumAngle, const float flMaximumAngle, const Color_t& colArc = Color_t(255, 255, 255, 255), const float flThickness = 1.0f);
	void AddDrawListLine(ImDrawList* pDrawList, const ImVec2& vecFirst, const ImVec2& vecSecond, const Color_t& colLine, const float flThickness = 1.0f);
	void AddDrawListTriangle(ImDrawList* pDrawList, const ImVec2& vecFirst, const ImVec2& vecSecond, const ImVec2& vecThird, const Color_t& colTriangle, const unsigned int uFlags = DRAW_TRIANGLE_NONE, const Color_t& colOutline = Color_t(0, 0, 0, 255), const float flThickness = 0.f);
	void AddDrawListQuad(ImDrawList* pDrawList, const ImVec2& vecFirst, const ImVec2& vecSecond, const ImVec2& vecThird, const ImVec2& vecFourth, const Color_t& colQuad, const unsigned int uFlags = DRAW_QUAD_NONE, const Color_t& colOutline = Color_t(0, 0, 0, 255), const float flThickness = 0.f);
	void AddDrawListPolygon(ImDrawList* pDrawList, const ImVec2* vecPoints, const int nPointsCount, const Color_t& colPolygon, unsigned int uFlags = DRAW_POLYGON_NONE, const Color_t& colOutline = Color_t(0, 0, 0, 255), const bool bClosed = true, const float flThickness = 1.0f);
	void AddDrawListText(ImDrawList* pDrawList, const ImFont* pFont, const ImVec2& vecPosition, const char* szText, const Color_t& colText, const unsigned int uFlags = DRAW_TEXT_NONE, const Color_t& colOutline = Color_t(0, 0, 0, 255), const float flThickness = 1.0f);
	void AddDrawListShadowRect(ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, const Color_t& colShadow, float flThickness = 32.f, float flRounding = 0.0f, ImDrawFlags roundingCorners = ImDrawFlags_RoundCornersAll);

	/* @section: values */
	// rendering engine initialization state
	inline bool bInitialized = false;
	// active draw data container used to store
	inline ImDrawList* pDrawListActive = nullptr;
	// safe draw data container
	inline ImDrawList* pDrawListSafe = nullptr;
	// actual draw data container used to render
	inline ImDrawList* pDrawListRender = nullptr;
}