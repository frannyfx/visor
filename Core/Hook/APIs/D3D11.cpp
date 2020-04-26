#include "D3D11.h"

HRESULT __stdcall Hook::D3D11Hook::PresentHook(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags) {
	if (!presentCalled) {
		cout << "D3D11 Present hook called." << endl;

		// Get swapchain device, make sure it's valid
		pSwapChain->GetDevice(__uuidof(pDevice), (void**)&pDevice);
		if (pDevice == nullptr) {
			cout << "Likely another version of D3D is loaded. Disabling hook..." << endl;
			FinishInstall(false);
			return presentHookTrampoline(pSwapChain, syncInterval, flags);
		}

		// The device is the right version, initialise engine.
		success = true;
		pDevice->GetImmediateContext(&pContext);

		// Get render target
		ID3D11Texture2D* pBackBuffer;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		pDevice->CreateRenderTargetView(pBackBuffer, NULL, &renderTargetView);
		pBackBuffer->Release();

		// Get swapchain description
		DXGI_SWAP_CHAIN_DESC sd;
		pSwapChain->GetDesc(&sd);

		//Initialise our engine
		EngineResources::SetD3D11Device(pDevice);
		//EngineResources::AddTexture(new Texture(TextureID::VISOR_LOGO, "C:\\Users\\blazi\\Desktop\\glasses.png"));

		ImGui_ImplWin32_Init(sd.OutputWindow);
		ImGui_ImplDX11_Init(pDevice, pContext);
		pContext->OMSetRenderTargets(1, &renderTargetView, NULL);

		// Complete install
		presentCalled = true;
		FinishInstall(true);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	Engine::Render(GraphicsAPI::D3D11);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return presentHookTrampoline(pSwapChain, syncInterval, flags);
}

void Hook::D3D11Hook::Install() {
	if (installed)
		return;

	cout << "Initialising hooks for D3D11..." << endl;

	// Get window handle
	HWND hWindow = GetForegroundWindow();

	// Create swap chain description
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC swapChainDesc = DXGIHook::CreateSwapChainDescription(hWindow);
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, NULL, &pContext))) {
		cout << "Failed to create D3D11 device and swapchain." << endl;
		FinishInstall(false);
		return;
	}

	cout << "Created D3D11 device successfully." << endl;

	// Get VTable
	pSwapChainVTable = (DWORD_PTR*)((DWORD_PTR*)pSwapChain)[0];
	pDeviceContextVTable = (DWORD_PTR*)((DWORD_PTR*)pContext)[0];

	// Release resources
	pDevice->Release();
	pSwapChain->Release();
	pDevice = NULL;
	pSwapChain = NULL;

	// Hook it!
	presentHookAddress = (LPVOID)pSwapChainVTable[static_cast<uint32_t>(DXGIHook::DXGIOffsets::Present)];

	if (MH_CreateHook(presentHookAddress, &PresentHook, reinterpret_cast<LPVOID*>(&presentHookTrampoline)) != MH_OK) {
		cout << "Failed to install hooks for D3D11." << endl;
		FinishInstall(false);
		return;
	};

	Enable();
}

void Hook::D3D11Hook::Enable() {
	if (MH_EnableHook(presentHookAddress) != MH_OK) {
		cout << "Failed to enable hooks for D3D11." << endl;
		if (!installed) FinishInstall(false);
		return;
	}
}

void Hook::D3D11Hook::Disable() {
	MH_DisableHook(presentHookAddress);
}