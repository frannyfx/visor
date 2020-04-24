#include <iostream>

// Graphics
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

// Hooking
#include <polyhook2/CapstoneDisassembler.hpp>

#ifdef _WIN64
#include <polyhook2/Detour/x64Detour.hpp>
#else
#include <polyhook2/Detour/x86Detour.hpp>
#endif

// Offsets
#include "D3D9.h"

using namespace std;

namespace D3D9Hook {
	// Hooking
	typedef HRESULT(__stdcall* D3D9EndSceneHook) (LPDIRECT3DDEVICE9* pDevice);
	uint64_t endSceneHookTrampoline = NULL;
	bool endSceneCalled = false;

	// D3D objects
	LPDIRECT3DDEVICE9 device;

	DWORD_PTR* pDeviceVTable = NULL;

	HRESULT __stdcall EndSceneHook(LPDIRECT3DDEVICE9* pDevice) {
		if (!endSceneCalled) {
			cout << "EndScene hook called." << endl;
			endSceneCalled = true;
		}

		return PLH::FnCast(endSceneHookTrampoline, EndSceneHook)(pDevice);
	}

	D3DPRESENT_PARAMETERS GetPresentParameters(const HWND& hWindow, const D3DDISPLAYMODE& displayMode) {
		D3DPRESENT_PARAMETERS parameters;
		parameters.BackBufferWidth = 0;
		parameters.BackBufferHeight = 0;
		parameters.BackBufferFormat = displayMode.Format;
		parameters.BackBufferCount = 0;
		parameters.MultiSampleType = D3DMULTISAMPLE_NONE;
		parameters.MultiSampleQuality = NULL;
		parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		parameters.hDeviceWindow = hWindow;
		parameters.Windowed = 1;
		parameters.EnableAutoDepthStencil = 0;
		parameters.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
		parameters.Flags = NULL;
		parameters.FullScreen_RefreshRateInHz = 0;
		parameters.PresentationInterval = 0;
		return parameters;
	}

	void InitialiseD3D9Hooks() {
		cout << "Installing hooks for D3D9..." << endl;

		// Get window handle
		HWND hWindow = GetForegroundWindow();

		// Get D3D9 API
		LPDIRECT3D9 d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

		// Get display mode
		D3DDISPLAYMODE displayMode;
		d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode);

		// Get parameters & create device
		D3DPRESENT_PARAMETERS parameters = GetPresentParameters(hWindow, displayMode);
		if (d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT, &parameters, &device) < 0) {
			cout << "Failed to create D3D9 device." << endl;
			d3d9->Release();
			return;
		}

		// Get VTable
		pDeviceVTable = (DWORD_PTR*)((DWORD_PTR*)device)[0];

		// Hook EndScene function
		cout << "Hooking EndScene function..." << endl;
#ifdef _WIN64
		PLH::CapstoneDisassembler disassembler(PLH::Mode::x64);
		PLH::x64Detour detour((uint64_t)pDeviceVTable[static_cast<uint32_t>(D3D9Hook::D3D9Offsets::EndScene)], (uint64_t)EndSceneHook, &endSceneHookTrampoline, disassembler);
		detour.hook();
#else
		PLH::CapstoneDisassembler disassembler(PLH::Mode::x64);
		PLH::x86Detour detour((uint64_t)pDeviceVTable[static_cast<uint32_t>(D3D9Hook::D3D9Offsets::EndScene)], (uint64_t)EndSceneHook, &endSceneHookTrampoline, disassembler);
		detour.hook();
#endif
	}
}
