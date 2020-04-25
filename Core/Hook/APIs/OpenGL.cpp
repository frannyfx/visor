#include <Windows.h>
#include <iostream>

// Graphics
#include "OpenGL.h"

// Hooking
#include <polyhook2/CapstoneDisassembler.hpp>

#ifdef _WIN64
#include <polyhook2/Detour/x64Detour.hpp>
#else
#include <polyhook2/Detour/x86Detour.hpp>
#endif

// GUI
#include "../../Include/ImGui/imgui.h"
#include "../../Include/ImGui/imgui_impl_win32.h"
#include "../../Include/ImGui/imgui_impl_dx11.h"
#include "../../Engine/Engine.h"

using namespace std;

typedef HRESULT(__stdcall* OpenGLSwapBuffersHook)();
uint64_t swapBuffersTrampoline = NULL;
bool swapBuffersCalled = false;

HRESULT __stdcall swapBuffersHook() {
	cout << "Kush" << endl;

	return PLH::FnCast(swapBuffersTrampoline, swapBuffersHook)();
}

void InitialiseOpenGLHooks() {
	cout << "Installing hooks for OpenGL..." << endl;

	// Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

#ifdef _WIN64
	PLH::CapstoneDisassembler disassembler(PLH::Mode::x64);
	PLH::x64Detour detour((uint64_t)GetProcAddress(GetModuleHandle(TEXT("opengl32.dll")), "wglSwapBuffers"), (uint64_t)swapBuffersHook, &swapBuffersTrampoline, disassembler);
	detour.hook();
#else
	PLH::CapstoneDisassembler disassembler(PLH::Mode::x86);
	PLH::x86Detour detour((uint64_t)GetProcAddress(GetModuleHandle(TEXT("opengl32.dll")), "wglSwapBuffers"), (uint64_t)swapBuffersHook, &swapBuffersTrampoline, disassembler);
	detour.hook();
#endif
}