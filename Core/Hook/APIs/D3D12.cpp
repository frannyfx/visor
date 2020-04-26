#include <iostream>

// Graphics
#include "DXGI.h"
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

// Hooking
#include <MinHook/MinHook.h>

// GUI
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#if _WIN64
#include <ImGui/imgui_impl_dx12.h>
#endif
#include "../../Engine/Engine.h"
#include "../../Engine/EngineResources.h"

using namespace std;

namespace D3D12Hook {

	// Hooking
	typedef long(__stdcall* D3D12PresentHook) (IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);
	D3D12PresentHook presentHookTrampoline = NULL;
	bool presentCalled = false;

	// D3D Objects
	ID3D12Device* pDevice;
	DWORD_PTR* pSwapChainVTable = NULL;

	long __stdcall PresentHook(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags) {
		if (!presentCalled) {
			cout << "D3D12 Present hook called." << endl;
			presentCalled = true;
		}

		return presentHookTrampoline(pSwapChain, syncInterval, flags);
	}

	void InitialiseD3D12Hooks() {
		cout << "Initialising hooks for D3D12" << endl;

		// Get a handle on the foreground window
		HWND hWindow = GetForegroundWindow();

		// Get handle on libD3D12
		HMODULE libD3D12;
		if ((libD3D12 = GetModuleHandle(TEXT("d3d12.dll"))) == NULL) {
			cout << "Couldn't get handle on d3d12.dll" << endl;
			return;
		}

		// Get handle on libDXGI
		HMODULE libDXGI;
		if ((libDXGI = GetModuleHandle(TEXT("dxgi.dll"))) == NULL) {
			cout << "Couldn't get handle on dxgi.dll" << endl;
			return;
		}

		// Get pointer for create factory method
		void* CreateDXGIFactory;
		if ((CreateDXGIFactory = GetProcAddress(libDXGI, "CreateDXGIFactory")) == NULL) {
			cout << "Couldn't get pointer for CreateDXGIFactory method" << endl;
			return;
		}

		// Create a DXGI factory
		IDXGIFactory* pFactory;
		if (((long(__stdcall*)(const IID&, void**))(CreateDXGIFactory))(__uuidof(IDXGIFactory), (void**)&pFactory) < 0) {
			cout << "Couldn't create IDXGIFactory" << endl;
			return;
		}

		// Create DXGI adapter
		IDXGIAdapter* pAdapter;
		if (pFactory->EnumAdapters(0, &pAdapter) == DXGI_ERROR_NOT_FOUND) {
			cout << "Couldn't create DXGI adapter" << endl;
			return;
		}

		// Get pointer for create device method
		void* D3D12CreateDevice;
		if ((D3D12CreateDevice = GetProcAddress(libD3D12, "D3D12CreateDevice")) == NULL) {
			cout << "Couldn't get pointer for D3D12CreateDevice method" << endl;
			return;
		}

		// Create the device
		if (((long(__stdcall*)(IUnknown*, D3D_FEATURE_LEVEL, const IID&, void**))(D3D12CreateDevice))(pAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&pDevice) < 0) {
			cout << "Couldn't create D3D12 Device" << endl;
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
			cout << "Failed to create command queue" << endl;
			return;
		}

		ID3D12CommandAllocator* pCommandAllocator;
		if (pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&pCommandAllocator) < 0)
		{
			cout << "Failed to create command allocator" << endl;
			return;
		}

		ID3D12GraphicsCommandList* pCommandList;
		if (pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&pCommandList) < 0)
		{
			cout << "Failed to create command list" << endl;
			return;
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = DXGIHook::CreateSwapChainDescription(hWindow);

		IDXGISwapChain* pSwapChain;
		if (pFactory->CreateSwapChain(pCommandQueue, &swapChainDesc, &pSwapChain) < 0)
		{
			cout << "Failed to create swap chain" << endl;
			return;
		}

		// Get VTable
		pSwapChainVTable = (DWORD_PTR*)((DWORD_PTR*)pSwapChain)[0];
		LPVOID toHook = (LPVOID)pSwapChainVTable[140];
		if (MH_CreateHook(toHook, &PresentHook, reinterpret_cast<LPVOID*>(&presentHookTrampoline)) != MH_OK) {
			cout << "Failed to install hooks for D3D12." << endl;
			return;
		};

		if (MH_EnableHook(toHook) != MH_OK) {
			cout << "Failed to enable hooks for D3D12." << endl;
			return;
		}

		cout << pSwapChain << endl;
	}
}