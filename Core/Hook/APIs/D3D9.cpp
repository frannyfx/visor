#include <iostream>

// Graphics
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

// Hooking
#include <MinHook/MinHook.h>

// Offsets
#include "D3D9.h"

// GUI
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx9.h>
#include "../../Engine/Engine.h"
#include "../../Engine/EngineResources.h"

using namespace std;

namespace D3D9Hook {
	// Hooking
	typedef HRESULT(__stdcall* D3D9PresentHook) (LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
	D3D9PresentHook presentHookTrampoline = NULL;
	bool presentCalled = false;

	// D3D objects
	LPDIRECT3DDEVICE9 device;

	DWORD_PTR* pDeviceVTable = NULL;

	HRESULT __stdcall PresentHook(LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) {
		if (!presentCalled) {
			cout << "D3D9 Present hook called." << endl;

			// Get device parameters
			D3DDEVICE_CREATION_PARAMETERS parameters;
			pDevice->GetCreationParameters(&parameters);

			// Initialise our engine
			EngineResources::SetD3D9Device(&pDevice);
			//EngineResources::AddTexture(new Texture(TextureID::VISOR_LOGO, "C:\\Users\\blazi\\Desktop\\squidward.jpg"));

			// Initialise ImGui
			ImGui_ImplWin32_Init(parameters.hFocusWindow);
			ImGui_ImplDX9_Init(pDevice);

			presentCalled = true;
		}

		// Render
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		Engine::Render(GraphicsAPI::D3D9);
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		return presentHookTrampoline(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
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

		// Setup ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();

		// Hook Present function
		cout << "Hooking Present function..." << endl;

		LPVOID toHook = (LPVOID)pDeviceVTable[static_cast<uint32_t>(D3D9Hook::D3D9Offsets::Present)];

		if (MH_CreateHook(toHook, &PresentHook, reinterpret_cast<LPVOID*>(&presentHookTrampoline)) != MH_OK) {
			cout << "Failed to install hooks for D3D9." << endl;
			return;
		}

		if (MH_EnableHook(toHook) != MH_OK) {
			cout << "Failed to enable hooks for D3D9." << endl;
			return;
		}
	}
}

