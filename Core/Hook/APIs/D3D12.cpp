#include "D3D12.h"

namespace Hook::D3D12 
{
	// Hooking
	typedef long(__stdcall* D3D12PresentHook) (IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);
	D3D12PresentHook presentHookTrampoline = NULL;
	bool presentCalled = false;
	LPVOID presentHookAddress = NULL;

	// D3D Objects
	ID3D12Device* pDevice;
	DWORD_PTR* pSwapChainVTable = NULL;

	// Instance
	Instance* instance = Instance::GetInstance();

	long __stdcall PresentHook(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags) {
		// Prevent calling while hook is being disabled
		if (instance->IsInstalled() && !instance->IsSuccessful())
			return presentHookTrampoline(pSwapChain, syncInterval, flags);

		// Initialise
		if (!presentCalled) {
			cout << "D3D12 Present hook called." << endl;
			pSwapChain->GetDevice(__uuidof(pDevice), (void**)&pDevice);
			if (pDevice == nullptr) {
				cout << "D3D device version mismatch. Disabling D3D12 hook." << endl;
				Instance::GetInstance()->FinishInstall(false);
				return presentHookTrampoline(pSwapChain, syncInterval, flags);
			}

			// Complete install
			presentCalled = true;
			Instance::GetInstance()->FinishInstall(true);
		}

		return presentHookTrampoline(pSwapChain, syncInterval, flags);
	}

	Instance* Instance::GetInstance() {
		static Instance instance;
		return &instance;
	}

	Instance::Instance() {}

	void Instance::Install() {
		if (installed || enabled)
			return;

		cout << "Installing hooks for D3D12..." << endl;

		// Get a handle on the foreground window
		HWND hWindow = GetForegroundWindow();

		// Get handle on libD3D12
		HMODULE libD3D12;
		if ((libD3D12 = GetModuleHandle(TEXT("d3d12.dll"))) == NULL) {
			cout << "Couldn't get handle on d3d12.dll." << endl;
			FinishInstall(false);
			return;
		}

		// Get handle on libDXGI
		HMODULE libDXGI;
		if ((libDXGI = GetModuleHandle(TEXT("dxgi.dll"))) == NULL) {
			cout << "Couldn't get handle on dxgi.dll." << endl;
			FinishInstall(false);
			return;
		}

		// Get pointer for create factory method
		void* CreateDXGIFactory;
		if ((CreateDXGIFactory = GetProcAddress(libDXGI, "CreateDXGIFactory")) == NULL) {
			cout << "Couldn't get pointer for CreateDXGIFactory method." << endl;
			FinishInstall(false);
			return;
		}

		// Create a DXGI factory
		IDXGIFactory* pFactory;
		if (((long(__stdcall*)(const IID&, void**))(CreateDXGIFactory))(__uuidof(IDXGIFactory), (void**)&pFactory) < 0) {
			cout << "Couldn't create IDXGIFactory." << endl;
			FinishInstall(false);
			return;
		}

		// Create DXGI adapter
		IDXGIAdapter* pAdapter;
		if (pFactory->EnumAdapters(0, &pAdapter) == DXGI_ERROR_NOT_FOUND) {
			cout << "Couldn't create DXGI adapter." << endl;
			FinishInstall(false);
			return;
		}

		// Get pointer for create device method
		void* D3D12CreateDevice;
		if ((D3D12CreateDevice = GetProcAddress(libD3D12, "D3D12CreateDevice")) == NULL) {
			cout << "Couldn't get pointer for D3D12CreateDevice method." << endl;
			FinishInstall(false);
			return;
		}

		// Create the device
		if (((long(__stdcall*)(IUnknown*, D3D_FEATURE_LEVEL, const IID&, void**))(D3D12CreateDevice))(pAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&pDevice) < 0) {
			cout << "Couldn't create D3D12 Device." << endl;
			FinishInstall(false);
			return;
		}

		D3D12_COMMAND_QUEUE_DESC queueDesc;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority = 0;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.NodeMask = 0;

		ID3D12CommandQueue* pCommandQueue;
		if (pDevice->CreateCommandQueue(&queueDesc, __uuidof(ID3D12CommandQueue), (void**)&pCommandQueue) < 0)
		{
			cout << "Failed to create command queue." << endl;
			FinishInstall(false);
			return;
		}

		ID3D12CommandAllocator* pCommandAllocator;
		if (pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&pCommandAllocator) < 0)
		{
			cout << "Failed to create command allocator." << endl;
			FinishInstall(false);
			return;
		}

		ID3D12GraphicsCommandList* pCommandList;
		if (pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&pCommandList) < 0)
		{
			cout << "Failed to create command list." << endl;
			FinishInstall(false);
			return;
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = DXGIHook::CreateSwapChainDescription(hWindow);

		IDXGISwapChain* pSwapChain;
		if (pFactory->CreateSwapChain(pCommandQueue, &swapChainDesc, &pSwapChain) < 0)
		{
			cout << "Failed to create swap chain." << endl;
			FinishInstall(false);
			return;
		}

		// Get VTable
		pSwapChainVTable = (DWORD_PTR*)((DWORD_PTR*)pSwapChain)[0];

		// Release resources
		pDevice->Release();
		pSwapChain->Release();
		pDevice = NULL;
		pSwapChain = NULL;

		presentHookAddress = (LPVOID)pSwapChainVTable[static_cast<uint32_t>(DXGIHook::DXGIOffsets::Present)];
		if (MH_CreateHook(presentHookAddress, &PresentHook, reinterpret_cast<LPVOID*>(&presentHookTrampoline)) != MH_OK) {
			cout << "Failed to install hooks for D3D12." << endl;
			FinishInstall(false);
			return;
		};

		Enable();
	}

	void Instance::Enable() {
		if (MH_EnableHook(presentHookAddress) != MH_OK) {
			cout << "Failed to enable hooks for D3D12." << endl;
			if (!installed) FinishInstall(false);
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