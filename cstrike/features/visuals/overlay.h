#pragma once

#include "../../common.h"

// used: draw system
#include "../../utilities/draw.h"

class CCSPlayerController;
class C_BaseEntity;
class C_CSPlayerPawn;

namespace F::VISUALS::OVERLAY
{
	enum EAlignSide : std::uint8_t
	{
		SIDE_LEFT = 0U,
		SIDE_TOP,
		SIDE_RIGHT,
		SIDE_BOTTOM,
		SIDE_MAX
	};

	enum EAlignDirection : std::uint8_t
	{
		DIR_LEFT = 0U,
		DIR_TOP,
		DIR_RIGHT,
		DIR_BOTTOM,
		DIR_MAX = 4U // @todo: rework stuff based on this cuz one component can have only 3 possible directions at same time. vertical side: left & right + top | bottom, horizontal side: top & bottom + left | right
	};

	class CBaseComponent
	{
	public:
		[[nodiscard]] virtual ImVec2 GetBasePosition(const ImVec4& box) const;

		[[nodiscard]] virtual bool IsDirectional() const
		{
			return false;
		}

		virtual void Render(ImDrawList* pDrawList, const ImVec2& vecPosition) = 0;

		EAlignSide nSide = SIDE_TOP;
		ImVec2 vecOffset = {};
		ImVec2 vecSize = {};
	};

	class CBaseDirectionalComponent : public CBaseComponent
	{
	public:
		[[nodiscard]] ImVec2 GetBasePosition(const ImVec4& box) const final;

		[[nodiscard]] bool IsDirectional() const final
		{
			return true;
		}

		EAlignDirection nDirection = DIR_TOP;
	};

	class CBarComponent : public CBaseComponent
	{
	public:
		CBarComponent(const bool bIsMenuItem, const EAlignSide nAlignSide, const ImVec4& vecBox, const float flProgressFactor, const std::size_t uOverlayVarIndex);

		void Render(ImDrawList* pDrawList, const ImVec2& vecPosition) final;

	private:
		bool bIsMenuItem = false;
		// bar progress
		float flProgressFactor = 0.0f;
		// hovered state for context menu
		bool bIsHovered = false;
		// config variables
		std::size_t uOverlayVarIndex = 0ULL;
	};

	class CTextComponent : public CBaseDirectionalComponent
	{
	public:
		CTextComponent(const bool bIsMenuItem, const EAlignSide nAlignSide, const EAlignDirection nAlignDirection, const ImFont* pFont, const char* szText, const std::size_t uOverlayVarIndex);
		~CTextComponent();

		void Render(ImDrawList* pDrawList, const ImVec2& vecPosition) final;

	private:
		bool bIsMenuItem = false;
		// font & text for displaying
		const ImFont* pFont = nullptr;
		char* szText = nullptr;
		// hovered state for context menu
		bool bIsHovered = false;
		// config variables
		std::size_t uOverlayVarIndex = 0ULL;
	};

	/*
	 * overlay component auto-positioning system
	 * @note: was designed to support the reordering of components that can be implemented with minimal effort
	 *
	 * currently supported next sides and sub-directions:
	 *
	 *                DIR_TOP
	 *                   ^
	 *                   |
	 *        DIR_LEFT <-o-> DIR_RIGHT
	 * DIR_LEFT <-o *---------* o-> DIR_RIGHT
	 *            | |         | |
	 *            v |         | v
	 *   DIR_BOTTOM |         | DIR_BOTTOM
	 *              |         |
	 *      DIR_TOP |         | DIR_TOP
	 *            ^ |         | ^
	 *            | |         | |
	 *            o *---------* o
	 *        DIR_LEFT <-o-> DIR_RIGHT
	 *                   |
	 *                   v
	 *              DIR_BOTTOM
	 */
	struct Context_t
	{
		/* @section: special case components */
		/// add the box component to overlay
		/// @remarks: current implementation expects this to be first component, it's an immediate rendering component
		/// @return: if the box component is hovered
		bool AddBoxComponent(ImDrawList* pDrawList, const ImVec4& vecBox, const int nType, float flThickness, float flRounding, const Color_t& colPrimary, const Color_t& colOutline);
		/// add the frame component to overlay
		/// @remarks: current implementation expects this to be added after components that should be inside it, it's an immediate rendering component
		/// @returns: size constraints of the added frame
		ImVec4 AddFrameComponent(ImDrawList* pDrawList, const ImVec2& vecScreen, const EAlignSide nSide, const Color_t& colBackground, const float flRounding, const ImDrawFlags nRoundingCorners);

		/* @section: common components */
		/// add new component to overlay
		/// @param[in] pComponent pointer to the one of supported component types
		void AddComponent(CBaseComponent* pComponent);

		/* @section: get */
		/// @returns: size of the all directional components currently assigned to @a'nSide'
		[[nodiscard]] ImVec2 GetTotalDirectionalSize(const EAlignSide nSide) const;

		// calculate final position of components and render them
		void Render(ImDrawList* pDrawList, const ImVec4& vecBox) const;

	private:
		// storage of all components
		std::vector<CBaseComponent*> vecComponents = {};
		// additional spacing between components
		float flComponentSpacing = 1.0f;
		// summary padding of all align sides
		float arrSidePaddings[SIDE_MAX] = {};
		// summary padding for all align directions of all align sides
		float arrSideDirectionPaddings[SIDE_MAX][DIR_MAX] = {};
	};

	/* @section: callbacks */
	void OnFrameStageNotify(CCSPlayerController* pLocalController);
		
	/* @section: get */
	/// get bounding box of entity
	/// @returns: true if entity has collision and all points of box are visible on screen, false otherwise
	bool GetEntityBoundingBox(C_CSPlayerPawn* pEntity, ImVec4* pVecOut);

	/* @section: main */
	// draw box, bars, text infos, etc at player position
	void Player(CCSPlayerController* pLocal, CCSPlayerController* pPlayer, const float flDistance);
}
