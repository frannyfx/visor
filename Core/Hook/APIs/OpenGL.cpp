#include "OpenGL.h"

namespace Hook::OpenGL {
	// Hooking
	typedef BOOL(__stdcall* owglSwapBuffers)(HDC hdc);
	owglSwapBuffers swapBuffersTrampoline = NULL;
	bool swapBuffersCalled = false;
	LPVOID swapBuffersAddress = NULL;

	// Instance
	Instance* instance = Instance::GetInstance();

	BOOL __stdcall hkwglSwapBuffers(HDC hdc) {
		// Prevent calling while hook is being disabled
		if (instance->IsInstalled() && !instance->IsSuccessful())
			return swapBuffersTrampoline(hdc);

		// Initialise
		if (!swapBuffersCalled) {
			cout << "OpenGL SwapBuffers hook called." << endl;

			// Complete installation
			swapBuffersCalled = true;
			instance->FinishInstall(true);
		}

		return swapBuffersTrampoline(hdc);
	}

	Instance* Instance::GetInstance() {
		static Instance instance;
		return &instance;
	}

	Instance::Instance() {}

	void Instance::Install() {
		cout << "Installing hooks for OpenGL..." << endl;

		// Hook swap buffers
		swapBuffersAddress = GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers");
		if (MH_CreateHook(swapBuffersAddress, &hkwglSwapBuffers, reinterpret_cast<LPVOID*>(&swapBuffersTrampoline)) != MH_OK) {
			cout << "Failed to install hooks for OpenGL." << endl;
			FinishInstall(false);
			return;
		};

		Enable();
	}

	void Instance::Enable() {
		if (MH_EnableHook(swapBuffersAddress) != MH_OK) {
			cout << "Failed to enable hooks for OpenGL." << endl;
			return;
		}
	}

	void Instance::Disable() {
		MH_DisableHook(swapBuffersAddress);
	}

	void Instance::Uninstall() {
		MH_RemoveHook(swapBuffersAddress);
	}
}
