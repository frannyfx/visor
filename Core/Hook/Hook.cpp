#include <vector>
#include <iostream>
#include <Windows.h>

#include "Hook.h"
#include "GraphicsAPI.h"
#include "../Engine/Engine.h"
#include "../Include/MinHook/MinHook.h"
#include "../Communication/Discovery.h"

#if _WIN64
#pragma comment(lib, "Include/MinHook/lib/MinHook.x64.lib")
#else
#pragma comment(lib, "Include/MinHook/lib/MinHook.x86.lib")
#endif

#if VISOR_HOOK_D3D9
#include "APIs/D3D9.h"
#endif

#if VISOR_HOOK_D3D10
#include "APIs/D3D10.h"
#endif

#if VISOR_HOOK_D3D11
#include "APIs/D3D11.h"
#endif

#if VISOR_HOOK_D3D12
#include "APIs/D3D12.h"
#endif

#if VISOR_HOOK_OPENGL
#include "APIs/OpenGL.h"
#endif

#if VISOR_HOOK_VULKAN
#include "APIs/Vulkan.h"
#endif

using namespace std;

LoadedGraphicsAPIs DetectGraphicsAPI() {
	LoadedGraphicsAPIs loaded = { false, false, false, false, false, false };

#if VISOR_HOOK_D3D9
	HANDLE d3d9Handle = GetModuleHandle(TEXT("d3d9.dll"));
	loaded.D3D9Loaded = d3d9Handle != NULL;
#endif

#if VISOR_HOOK_D3D10
	HANDLE d3d10Handle = GetModuleHandle(TEXT("d3d10.dll"));
	loaded.D3D10Loaded = d3d10Handle != NULL;
#endif

#if VISOR_HOOK_D3D11
	HANDLE d3d11Handle = GetModuleHandle(TEXT("d3d11.dll"));
	loaded.D3D11Loaded = d3d11Handle != NULL;
#endif

#if VISOR_HOOK_D3D12
	HANDLE d3d12Handle = GetModuleHandle(TEXT("d3d12.dll"));
	loaded.D3D12Loaded = d3d12Handle != NULL;
#endif

#if VISOR_HOOK_OPENGL
	HANDLE openGLHandle = GetModuleHandle(TEXT("opengl32.dll"));
	loaded.OpenGLLoaded = openGLHandle != NULL;
#endif

#if VISOR_HOOK_VULKAN
	HANDLE vulkanHandle = GetModuleHandle(TEXT("vulkan-1.dll"));
	loaded.VulkanLoaded = vulkanHandle != NULL;
#endif

	return loaded;
}

DWORD __stdcall InitialiseHooks(LPVOID) {
	LoadedGraphicsAPIs loaded = DetectGraphicsAPI();

	// Prevent loading if no graphics APIs are loaded.
	if (!loaded.D3D9Loaded && !loaded.D3D10Loaded && !loaded.D3D11Loaded && !loaded.D3D12Loaded && !loaded.OpenGLLoaded && !loaded.VulkanLoaded) {
		cout << "No supported graphics APIs loaded!\nVisor exiting..." << endl;
		return NULL;
	}

	// Start pipe client
	//Discovery::Start();

	// Initialise MinHook
	MH_Initialize();

#if VISOR_HOOK_D3D9
	if (loaded.D3D9Loaded) {
		D3D9Hook::InitialiseD3D9Hooks();
	}
#endif

#if VISOR_HOOK_D3D10
	if (loaded.D3D10Loaded) {
		InitialiseD3D10Hooks();
	}
#endif

#if VISOR_HOOK_D3D11
	if (loaded.D3D11Loaded) {
		D3D11Hook::InitialiseD3D11Hooks();
	}
#endif

#if VISOR_HOOK_D3D12
	if (loaded.D3D12Loaded) {
		InitialiseD3D12Hooks();
	}
#endif

#if VISOR_HOOK_OPENGL
	if (loaded.OpenGLLoaded) {
		OpenGLHook::InitialiseOpenGLHooks();
	}
#endif

#if VISOR_HOOK_VULKAN
	if (loaded.VulkanLoaded) {
		InitialiseVulkanHooks();
	}
#endif

	Engine::ShowNotification("Visor injected successfully.", "frannyfx and Neriuzz made this shit.");
	return NULL;
}
