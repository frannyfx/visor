#include <Windows.h>
//#include "hook/hook.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reasonForCall, LPVOID lpReserved) {
	switch (reasonForCall) {
	case DLL_PROCESS_ATTACH:
		MessageBoxA(0, "Hello", "Injected", 0);
		//CreateThread(NULL, 0, InstallHooks, NULL, 0, NULL);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}
