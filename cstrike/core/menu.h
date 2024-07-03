#pragma once

// used: [stl] vector
#include <vector>

#include "../common.h"

// used: [ext] imgui, draw, animation
#include "../utilities/draw.h"

#define MENU_MAX_BACKGROUND_PARTICLES 100

class CTab
{
public:
	const char* szName;
	void (*pRenderFunction)();
};

namespace MENU
{
	void RenderMainWindow();
	void RenderOverlayPreviewWindow();
	void RenderWatermark();

	void UpdateStyle(ImGuiStyle* pStyle = nullptr);

	/* @section: particles */
	struct ParticleData_t
	{
		ParticleData_t(const ImVec2& vecPosition, const ImVec2& vecVelocity) :
			vecPosition(vecPosition), vecVelocity(vecVelocity) { }

		// current particle position
		ImVec2 vecPosition = {};
		// current particle velocity
		ImVec2 vecVelocity = {};
	};

	struct ParticleContext_t
	{
		ParticleContext_t(const int nMaxParticles = 100)
		{
			// allocate memory for particles
			this->vecParticles.reserve(nMaxParticles);
			// create particles if needed
		}

		~ParticleContext_t()
		{
			// since no memory allocated, just clear vector
			this->vecParticles.clear();
		}

		void Render(ImDrawList* pDrawList, const ImVec2& vecScreenSize, const float flAlpha);

		// create particle with random velocity/position
		void AddParticle(const ImVec2& vecScreenSize);
		// current size of particles
		const size_t Count() const { return this->vecParticles.size(); }
	private:
		// draw particle (circle)
		void DrawParticle(ImDrawList* pDrawList, ParticleData_t& particle, const Color_t& colPrimary);

		// find & draw connection as a line between particles
		void FindConnections(ImDrawList* pDrawList, ParticleData_t& particle, const Color_t& colPrimary, float flMaxDistance);
		void DrawConnection(ImDrawList* pDrawList, ParticleData_t& particle, ParticleData_t& otherParticle, float flAlpha, const Color_t& colPrimary) const;

		// update particle position/velocity
		// reversed direction when particle is out of screen
		void UpdatePosition(ParticleData_t& particle, const ImVec2& vecScreenSize) const;
		void ResolveScreenCollision(ParticleData_t& particle, const ImVec2& vecScreenSize) const;

		// all our particles data
		std::vector<ParticleData_t> vecParticles;
	};

	inline bool bMainWindowOpened = false;
	inline bool bMainActive = false;
	inline int nCurrentMainTab = 0;
	inline ParticleContext_t menuParticle = ParticleContext_t(MENU_MAX_BACKGROUND_PARTICLES);
	inline AnimationHandler_t animMenuDimBackground;
	inline float flDpiScale = 1.f;
}

namespace T
{
	/* @section: main */
	void Render(const char* szTabBar, const CTab* arrTabs, const unsigned long long nTabsCount, int* nCurrentTab, ImGuiTabBarFlags flags = ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_NoTooltip);

	/* @section: tabs */
	void RageBot();
	void LegitBot();
	void Visuals();
	void Miscellaneous();
	void SkinsChanger();

	/* @section: values */
	// user-defined configuration filename in miscellaneous tab
	inline char szConfigFile[256U] = {};
	// current selected configuration in miscellaneous tab
	inline unsigned long long nSelectedConfig = ~1U;
	// current sub tab overlay in visuals tab
	inline int nCurrentOverlaySubtab = 0;
}
