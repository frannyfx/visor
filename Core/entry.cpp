#include <Windows.h>
#include "hook/hook.h"
#include <iostream>

using namespace std;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reasonForCall, LPVOID lpReserved) {
	switch (reasonForCall) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);

		// Create debug console
#ifdef _DEBUG
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		cout << "Visor injected successfully." << endl;
#endif
		// Create thread
		CreateThread(NULL, 0, InstallHooks, NULL, 0, NULL);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}