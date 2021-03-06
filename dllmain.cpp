// dllmain.cpp : Defines the entry point for the DLL application.

#include "dllmain.h"

#include "DSR.h"

HMODULE g_hModule = 0;

namespace Plugin
{

	bool RunDSRPlugin()
	{
#ifdef _DEBUG
		Utility::SpawnConsole();
		msg("Press F2 to install..");
		while (!GetAsyncKeyState(VK_F2)) Sleep(100);
#endif
		DSR::InstallPlugin() ? msg("Sucessfully installed plugin") : msg("Plugin installation failed");
		Sleep(1000);

#ifdef _DEBUG
		msg("Press F2 to uninstall..");
		while (!GetAsyncKeyState(VK_F2)) Sleep(100);
		DSR::UninstallPlugin() ? msg("Succesfully uninstalled plugin") : msg("Plugin uninstallation failed!");
		Sleep(500);

		Utility::CloseConsole();
		FreeLibraryAndExitThread(g_hModule, 0);
#endif

	}
	
	void TMain()
	{
		RunDSRPlugin();
	}
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	{
		g_hModule = hModule;
		HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Plugin::TMain, 0, 0, 0);
		CloseHandle(hThread);
		break;
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

