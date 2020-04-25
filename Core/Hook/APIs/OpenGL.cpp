#include <Windows.h>
#include <iostream>

// Hooking
#include "../../Include/MinHook/MinHook.h"

// GUI
<<<<<<< HEAD
=======
#include "../../Include/ImGui/imgui.h"
#include "../../Include/ImGui/imgui_impl_win32.h"
>>>>>>> 443a09f41652986a930aa2c0de1f0cdb0c1fa1f4
#include "../../Engine/Engine.h"

using namespace std;

namespace OpenGLHook {
	// Hooking
	typedef BOOL(__stdcall* owglSwapBuffers)(HDC hdc);
	owglSwapBuffers twglSwapBuffers = NULL;
	bool swapBuffersCalled = false;

	BOOL __stdcall hkwglSwapBuffers(HDC hdc) {
		if (!swapBuffersCalled) {
			cout << "OpenGL SwapBuffers hook called." << endl;


			swapBuffersCalled = true;
		}

		return twglSwapBuffers(hdc);
	}

	void InitialiseOpenGLHooks() {
		cout << "Installing hooks for OpenGL..." << endl;

		LPVOID toHook = GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers");

		if (MH_CreateHook(toHook, &hkwglSwapBuffers, reinterpret_cast<LPVOID*>(&twglSwapBuffers)) != MH_OK) {
			cout << "Failed to install hooks for OpenGL." << endl;
			return;
		};

		if (MH_EnableHook(toHook) != MH_OK) {
			cout << "Failed to enable hooks for OpenGL." << endl;
			return;
		}
	}
}
