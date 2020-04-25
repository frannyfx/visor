#include <Windows.h>
#include <iostream>

// Graphics
#include "OpenGL.h"

// Hooking
#include "../../Include/MinHook/MinHook.h"

// GUI
#include "../../Include/ImGui/imgui.h"
#include "../../Include/ImGui/imgui_impl_win32.h"
#include "../../Include/ImGui/imgui_impl_dx11.h"
#include "../../Engine/Engine.h"

using namespace std;

typedef BOOL(__stdcall* owglSwapBuffers)(HDC);
// Trampoline function
owglSwapBuffers twglSwapBuffers = NULL;

BOOL __stdcall hkwglSwapBuffers(HDC hdc) {
	cout << "Hooked" << endl;
	return twglSwapBuffers(hdc);
}

void InitialiseOpenGLHooks() {
	cout << "Installing hooks for OpenGL..." << endl;
	if (MH_Initialize() != MH_OK) return;

	if (MH_CreateHook(GetProcAddress(GetModuleHandle(TEXT("opengl32.dll")), "wglSwapBuffer"), &hkwglSwapBuffers, reinterpret_cast<LPVOID*>(&twglSwapBuffers)) != MH_OK) return;
}