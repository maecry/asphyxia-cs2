#pragma once

// used: [stl] vector
#include <vector>
// used: draw wrapper
#include "draw.h"

#pragma region notify_definitions
#define NOFITY_TEXT_SIZE 64U // max size of notification text
// @todo: use ImStyle?
#define NOTIFY_ANIMATION_TIME 15.0 // time in ms to show/hide notification
#define NOTIFY_DELETE_TIME 30.0 // time in ms to delete notification
#pragma endregion

#pragma region notify_enumerations
using NotificationType_t = int;

enum ENotificationType : NotificationType_t
{
	N_TYPE_DEFAULT = 0,
	N_TYPE_INFO,
	N_TYPE_SUCCESS,
	N_TYPE_WARNING,
	N_TYPE_ERROR,
	N_TYPE_MAX
};

using NotificationState_t = int;

enum ENotificationState : NotificationState_t
{
	N_STATE_START = 0,
	N_STATE_STAY,
	N_STATE_END,
	N_STATE_EXPIRED,
	N_STATE_MAX
};

#pragma endregion

namespace NOTIFY
{
	struct NotificationData_t
	{
		NotificationData_t(ENotificationType nType, const char* szFormat, ...);

		/// @return color of notification type
		const Color_t& GetTypeColor() const;
		/// @return c-type string of notification text
		const char* Data() const;
		/// @return icon of notification type
		const char* GetIcon() const;
		/// @return time difference between creation and current time
		const float GetTimeDelta(const float flCurrentTime) const;

		int nType = 0;
		char szBuffer[NOFITY_TEXT_SIZE];
		float flCreateionTime = 0.0;
		AnimationHandler_t animHandler = AnimationHandler_t();
	};

	/* @section: main */
	// push notification to queue
	void Push(const NotificationData_t& notification);
	// pop notification from queue
	void _Remove(size_t nIndex);
	// render notifications
	void Render();

	/* @section: values */
	// maximum time to show notification
	inline constexpr float _MAX_TIME = 5.f;
	// maximum count of notifications
	inline std::vector<NotificationData_t> vecNotifications = {};
}