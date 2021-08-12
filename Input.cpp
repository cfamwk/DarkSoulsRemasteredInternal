#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Input.h"
#include "Utility.h"

#include <Xinput.h>

int g_MouseX = 0, g_MouseY = 0;
int g_KeysPressed[0xFF] = { 0 };

namespace Plugin
{
	namespace Input {

		bool				initialized				= false;
		FNOnInputCallback	onInputCallback			= NULL;
		HWND				windowHandle			= NULL;
		WNDPROC				originalWindowProc		= NULL;
		HANDLE				xInputHandlerThread		= INVALID_HANDLE_VALUE;
		

		KeyDefinition_t VirtualKeyToKeyDefinition(int virtualkey)
		{
			switch (virtualkey)
			{
			case 1:
				return KEY_LMOUSE;
			case 2:
				return KEY_RMOUSE;
			default:
				return KEY_UNKNOWN;
			}
		}

		/*
			Captures window input messages
		*/
		LRESULT WndProcHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			int keypressed = -1;
			int keystate = 0;
			bool IsMouseMoveMsg = false;

			switch (uMsg)
			{
			case WM_MOUSEMOVE:
				IsMouseMoveMsg = true;
				g_MouseX = (signed short)(lParam);
				g_MouseY = (signed short)(lParam >> 16);
				break;
			case WM_LBUTTONDOWN:
				keypressed = VK_LBUTTON;
				keystate = 1;
				break;
			case WM_LBUTTONUP:
				keypressed = VK_LBUTTON;
				keystate = 0;
				break;
			case WM_LBUTTONDBLCLK:
				keypressed = VK_LBUTTON;
				keystate = 1;
				break;
			case WM_RBUTTONDOWN:
				keypressed = VK_RBUTTON;
				keystate = 1;
				break;
			case WM_RBUTTONUP:
				keypressed = VK_RBUTTON;
				keystate = 0;
				break;
			case WM_KEYDOWN:
				keypressed = wParam;
				keystate = 1;
				break;
			case WM_KEYUP:
				keypressed = wParam;
				keystate = 0;
				break;
			default: break;
			}

			if (keypressed != -1) {
				g_KeysPressed[keypressed] = keystate;
	
				if (onInputCallback)
					onInputCallback(VirtualKeyToKeyDefinition(keypressed), keystate);
			}
			static int wndproccalls = 0;

			return CallWindowProc(originalWindowProc, hWnd, uMsg, wParam, lParam);
		}

		/*
			Hooks the window message handler
		*/
		bool HookWndProc(const char* szWindowClassName)
		{
			windowHandle = FindWindowA(szWindowClassName, 0);

			if (windowHandle == 0 || windowHandle == INVALID_HANDLE_VALUE) {
				msg("Input: Failed finding window");
				return false;
			}

			msg("Input: Hooking window message handler");
			originalWindowProc = (WNDPROC)SetWindowLongPtr(windowHandle, GWLP_WNDPROC, (LONG_PTR)WndProcHook);
		
			return true;
		}

		/*
			Handles XInput(Controller)
		*/
		bool RunXInputHandler = true;
		void TXInputHandler(void)
		{
			//We can't staticly link xinput, as once FreeLibraryAndExitThread will unload it.
			HMODULE hXInput = GetModuleHandle("XINPUT1_4.dll");
			if (hXInput == INVALID_HANDLE_VALUE || hXInput == NULL) {
				
				hXInput = LoadLibrary("XINPUT1_4.dll");

				if (hXInput == INVALID_HANDLE_VALUE || hXInput == NULL) {
					msg("Input: Failed getting xinput module");
					return;
				}
			}

			typedef DWORD(*_stdcall FNXInputGetState)(DWORD dwUserIndex, XINPUT_STATE* pState);
			FNXInputGetState fnXInputGetState = (FNXInputGetState)GetProcAddress(hXInput, "XInputGetState");

			if (fnXInputGetState == nullptr)
				return;

			XINPUT_STATE lastState[3] = { 0 };
			ZeroMemory(&lastState, sizeof(lastState) );

			msg("Input: Running XINPUT handler");

			while (RunXInputHandler)
			{
				Sleep(1);

				DWORD dwResult;
				for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
				{
					XINPUT_STATE currentState;

					ZeroMemory(&currentState, sizeof(XINPUT_STATE));
					dwResult = fnXInputGetState(i, &currentState);

					if (dwResult != ERROR_SUCCESS)
						continue;

					auto CalcAnalogInput = [&](const SHORT analog, const SHORT deadzone)
					{
						float fl = fmaxf(-1, (float)analog / 32767);

						if (abs(analog) < deadzone)
							fl = 0.f;

						return fl;
					};

					if (CalcAnalogInput(currentState.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) != CalcAnalogInput(lastState[i].Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
						onInputCallback(KEY_CONTROLLER_LANALOGX, CalcAnalogInput(currentState.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));

					if (CalcAnalogInput(currentState.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) != CalcAnalogInput(lastState[i].Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
						onInputCallback(KEY_CONTROLLER_LANALOGY, CalcAnalogInput(currentState.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));

					if (CalcAnalogInput(currentState.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) != CalcAnalogInput(lastState[i].Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
						onInputCallback(KEY_CONTROLLER_RANALOGX, CalcAnalogInput(currentState.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));

					if (CalcAnalogInput(currentState.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) != CalcAnalogInput(lastState[i].Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
						onInputCallback(KEY_CONTROLLER_RANALOGY, CalcAnalogInput(currentState.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));


					auto CalcTriggerInput = [&](const SHORT trigger)
					{
						return (float)trigger / 255;
					};

					if (CalcTriggerInput(currentState.Gamepad.bLeftTrigger) != CalcTriggerInput(lastState[i].Gamepad.bLeftTrigger))
						onInputCallback(KEY_CONTROLLER_LTRIGGER, CalcTriggerInput(currentState.Gamepad.bLeftTrigger));

					if (CalcTriggerInput(currentState.Gamepad.bRightTrigger) != CalcTriggerInput(lastState[i].Gamepad.bRightTrigger))
						onInputCallback(KEY_CONTROLLER_RTRIGGER, CalcTriggerInput(currentState.Gamepad.bRightTrigger));

					if (currentState.Gamepad.wButtons != lastState[i].Gamepad.wButtons)
					{

						auto TestButtonChanged = [&](SHORT ButtonState, SHORT LastButtonState, SHORT ButtonConstant)
						{
							if ((ButtonState & ButtonConstant) == (LastButtonState & ButtonConstant))
								return false;

							return true;

						};

						auto currentButtons = currentState.Gamepad.wButtons;
						auto lastButtons = lastState[i].Gamepad.wButtons;

						if (TestButtonChanged(currentButtons, lastButtons, XINPUT_GAMEPAD_A))
							onInputCallback(KEY_CONTROLLER_A, currentButtons & XINPUT_GAMEPAD_A ? 1.f : 0.f);
						else if (TestButtonChanged(currentButtons, lastButtons, XINPUT_GAMEPAD_B))
							onInputCallback(KEY_CONTROLLER_B, currentButtons & XINPUT_GAMEPAD_B ? 1.f : 0.f);
						else if (TestButtonChanged(currentButtons, lastButtons, XINPUT_GAMEPAD_X))
							onInputCallback(KEY_CONTROLLER_X, currentButtons & XINPUT_GAMEPAD_X ? 1.f : 0.f);
						else if (TestButtonChanged(currentButtons, lastButtons, XINPUT_GAMEPAD_Y))
							onInputCallback(KEY_CONTROLLER_Y, currentButtons & XINPUT_GAMEPAD_Y ? 1.f : 0.f);
						else if (TestButtonChanged(currentButtons, lastButtons, XINPUT_GAMEPAD_DPAD_UP))
							onInputCallback(KEY_CONTROLLER_DPAD_UP, currentButtons & XINPUT_GAMEPAD_DPAD_UP ? 1.f : 0.f);
						else if (TestButtonChanged(currentButtons, lastButtons, XINPUT_GAMEPAD_DPAD_DOWN))
							onInputCallback(KEY_CONTROLLER_DPAD_DOWN, currentButtons & XINPUT_GAMEPAD_DPAD_DOWN ? 1.f : 0.f);
						else if (TestButtonChanged(currentButtons, lastButtons, XINPUT_GAMEPAD_DPAD_LEFT))
							onInputCallback(KEY_CONTROLLER_DPAD_LEFT, currentButtons & XINPUT_GAMEPAD_DPAD_LEFT ? 1.f : 0.f);
						else if (TestButtonChanged(currentButtons, lastButtons, XINPUT_GAMEPAD_DPAD_RIGHT))
							onInputCallback(KEY_CONTROLLER_DPAD_RIGHT, currentButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 1.f : 0.f);
						//else if (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT || lastState[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
						//	onInputCallback(KEY_CONTROLLER_DPAD_RIGHT, currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 1.f : 0.f);
						//else if (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER || lastState[i].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
						//	onInputCallback(KEY_CONTROLLER_LEFT_SHOULDER, currentState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ? 1.f : 0.f);
						//else if (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER || lastState[i].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
						//	onInputCallback(KEY_CONTROLLER_RIGHT_SHOULDER, currentState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ? 1.f : 0.f);
						//else if (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_START || lastState[i].Gamepad.wButtons & XINPUT_GAMEPAD_START)
						//	onInputCallback(KEY_CONTROLLER_START, currentState.Gamepad.wButtons & XINPUT_GAMEPAD_START ? 1.f : 0.f);
						//else if (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK || lastState[i].Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
						//	onInputCallback(KEY_CONTROLLER_SELECT, currentState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK ? 1.f : 0.f);
						else
							onInputCallback(KEY_CONTROLLER_UNKNOWN, currentState.Gamepad.wButtons);

					}
					lastState[i] = currentState;
				}
			}
		}
		/*
			Starts a thread that handles XInput (Controller)
		*/
		bool StartXInputCaptureThread()
		{
			xInputHandlerThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)TXInputHandler, 0, 0, 0);
		
			return xInputHandlerThread != INVALID_HANDLE_VALUE;
		}

		bool HookInput(const char* szWindowClassName, FNOnInputCallback callback)
		{
			if (initialized)
				return true; 
			
			onInputCallback = callback;

			if (!HookWndProc(szWindowClassName))
				return false;

			if (!StartXInputCaptureThread())
				return false;
			
			Sleep(20);

			initialized = true;

			return true;
		}
		bool UnhookInput()
		{
			if (!initialized)
				return true;

			SetWindowLongPtr(windowHandle, GWLP_WNDPROC, (LONG_PTR)originalWindowProc);
			onInputCallback = 0;

			RunXInputHandler = false;
			DWORD tExitCode = 0;
			while (!GetExitCodeThread(xInputHandlerThread, &tExitCode))
				Sleep(100);

			CloseHandle(xInputHandlerThread);
			
			initialized = false;

			return true;
		}
	}
}