#pragma once

namespace Plugin
{
	namespace Input {

		enum KeyDefinition_t : int {
			//Mouse & keyboard
			KEY_UNKNOWN = -1,
			KEY_LMOUSE = 1,
			KEY_RMOUSE = 2,

			//CONTROLLER
			KEY_CONTROLLER_UNKNOWN = -2,
			KEY_CONTROLLER_LTRIGGER = 100,
			KEY_CONTROLLER_RTRIGGER = 101,
			KEY_CONTROLLER_LANALOGX = 102,
			KEY_CONTROLLER_LANALOGY = 103,
			KEY_CONTROLLER_RANALOGX = 104,
			KEY_CONTROLLER_RANALOGY = 105,
			KEY_CONTROLLER_A,
			KEY_CONTROLLER_B,
			KEY_CONTROLLER_X,
			KEY_CONTROLLER_Y,
			KEY_CONTROLLER_DPAD_UP,
			KEY_CONTROLLER_DPAD_DOWN,
			KEY_CONTROLLER_DPAD_LEFT,
			KEY_CONTROLLER_DPAD_RIGHT,
			KEY_CONTROLLER_LEFT_SHOULDER,
			KEY_CONTROLLER_RIGHT_SHOULDER,
			KEY_CONTROLLER_START,
			KEY_CONTROLLER_SELECT,

		};

		typedef void(*FNOnInputCallback)(KeyDefinition_t key, float state);

		bool HookInput(const char * szWindowClassName, FNOnInputCallback callback);

		bool UnhookInput();

	}
}