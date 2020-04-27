#include <vector>
#include <iostream>
#include <Windows.h>
#include <chrono>
#include <thread>

#include "Hook.h"
#include "GraphicsAPI.h"
#include "../Engine/Engine.h"
#include <MinHook/MinHook.h>
#include "../Communication/Discovery.h"

#if _WIN64
#pragma comment(lib, "Include/MinHook/lib/libMinHook-x64-v141-mdd.lib")
#else
#pragma comment(lib, "Include/MinHook/lib/libMinHook-x86-v141-mdd.lib")
#endif

#if VISOR_HOOK_D3D9
#include "APIs/D3D9.h"
#endif

#if VISOR_HOOK_DXGI
#if VISOR_HOOK_D3D10
#include "APIs/D3D10.h"
#endif

#if VISOR_HOOK_D3D11
#include "APIs/D3D11.h"
#endif

#if VISOR_HOOK_D3D12
#include "APIs/D3D12.h"
#endif
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

#if VISOR_HOOK_DXGI
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
	Discovery::Start();

	// Initialise MinHook
	MH_Initialize();

	// Initialise Engine
	Engine::Initialise();

#if VISOR_HOOK_D3D9
	if (loaded.D3D9Loaded) {
		Hook::D3D9::Instance* d3d9Instance = Hook::D3D9::Instance::GetInstance();
		d3d9Instance->Install();
		d3d9Instance->WaitForInstall();
		if (d3d9Instance->IsInstalled() && !d3d9Instance->IsSuccessful()) {
			d3d9Instance->Disable();
			d3d9Instance->Uninstall();
		}
	}
#endif

#if VISOR_HOOK_DXGI
	// Prevent loading newer versions if previous ones succeeded
	bool dxgiSuccessful = false;

	// Instances
	Hook::D3D11::Instance* d3d11Instance = Hook::D3D11::Instance::GetInstance();
	Hook::D3D12::Instance* d3d12Instance = Hook::D3D12::Instance::GetInstance();

#if VISOR_HOOK_D3D10
	if (!dxgiSuccessful && loaded.D3D10Loaded) {
		
	}
#endif

#if VISOR_HOOK_D3D11
	if (!dxgiSuccessful && loaded.D3D11Loaded) {
		d3d11Instance->Install();
		d3d11Instance->WaitForInstall();
		if (d3d11Instance->IsInstalled() && !d3d11Instance->IsSuccessful()) {
			d3d11Instance->Disable();
			d3d11Instance->Uninstall();
		}

		dxgiSuccessful = d3d11Instance->IsSuccessful();
	}
#endif

#if VISOR_HOOK_D3D12
	if (!dxgiSuccessful && loaded.D3D12Loaded) {
		d3d12Instance->Install();
		d3d12Instance->WaitForInstall();
		if (d3d12Instance->IsInstalled() && !d3d12Instance->IsSuccessful()) {
			d3d12Instance->Disable();
			d3d12Instance->Uninstall();
		}

		dxgiSuccessful = d3d12Instance->IsSuccessful();
	}
#endif
#endif

#if VISOR_HOOK_OPENGL
	if (loaded.OpenGLLoaded) {
		Hook::OpenGL::Instance* openGLInstance = Hook::OpenGL::Instance::GetInstance();
		openGLInstance->Install();
		openGLInstance->WaitForInstall();
		if (openGLInstance->IsInstalled() && !openGLInstance->IsSuccessful()) {
			openGLInstance->Disable();
			openGLInstance->Uninstall();
		}
	}
#endif

#if VISOR_HOOK_VULKAN
	if (loaded.VulkanLoaded) {
		//InitialiseVulkanHooks();
	}
#endif
	
	return NULL;
}