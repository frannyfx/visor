#include "D3D9.h"

namespace Hook::D3D9 {
	// Hooking
	typedef HRESULT(__stdcall* D3D9PresentHook) (LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
	D3D9PresentHook presentHookTrampoline = NULL;
	bool presentCalled = false;
	LPVOID presentHookAddress = NULL;

	// D3D objects
	LPDIRECT3DDEVICE9 device;

	DWORD_PTR* pDeviceVTable = NULL;

	// Instance
	Instance* instance = Instance::GetInstance();

	HRESULT __stdcall PresentHook(LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) {
		// Prevent calling while hook is being disabled
		if (instance->IsInstalled() && !instance->IsSuccessful())
			return presentHookTrampoline(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
		
		// Initialise
		if (!presentCalled) {
			cout << "D3D9 Present hook called." << endl;
			if (pDevice == nullptr) {
				cout << "Got null D3D9 device. Disabling hook." << endl;
				Instance::GetInstance()->FinishInstall(false);
				return presentHookTrampoline(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
			}

			// Get device parameters
			D3DDEVICE_CREATION_PARAMETERS parameters;
			pDevice->GetCreationParameters(&parameters);

			// Initialise our engine
			EngineResources::SetD3D9Device(&pDevice);
			EngineResources::AddTexture(new Texture(TextureID::VISOR_LOGO, "C:\\Users\\blazi\\Desktop\\glasses.png"));

			// Initialise ImGui
			ImGui_ImplWin32_Init(parameters.hFocusWindow);
			ImGui_ImplDX9_Init(pDevice);

			// Complete install
			instance->FinishInstall(true);
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

	Instance* Instance::GetInstance() {
		static Instance instance;
		return &instance;
	}

	Instance::Instance() {}

	void Instance::Install() {
		if (installed || enabled)
			return;

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
			d3d9 = NULL;
			FinishInstall(false);
			return;
		}

		// Get VTable
		pDeviceVTable = (DWORD_PTR*)((DWORD_PTR*)device)[0];

		// Release resources
		CloseHandle(hWindow);
		d3d9->Release();
		d3d9 = NULL;
		device->Release();
		device = NULL;

		// Hook Present function
		cout << "Hooking Present function..." << endl;
		presentHookAddress = (LPVOID)pDeviceVTable[static_cast<uint32_t>(Hook::D3D9::Offsets::Present)];
		if (MH_CreateHook(presentHookAddress, &PresentHook, reinterpret_cast<LPVOID*>(&presentHookTrampoline)) != MH_OK) {
			cout << "Failed to install hooks for D3D9." << endl;
			FinishInstall(false);
			return;
		}

		Enable();
	}

	void Instance::Enable() {
		if (MH_EnableHook(presentHookAddress) != MH_OK) {
			cout << "Failed to enable hooks for D3D9." << endl;
			FinishInstall(false);
			return;
		}
	}

	void Instance::Disable() {
		if (!installed || !enabled)
			return;

		MH_DisableHook(presentHookAddress);
	}

	void Instance::Uninstall() {
		if (!installed || enabled)
			return;

		MH_RemoveHook(presentHookAddress);
	}
}

