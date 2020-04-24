#include <iostream>

// Graphics
#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")

// Offsets
#include "DXGI.h"

using namespace std;

// DirectX objects
ID3D11Device* pDevice;
ID3D11DeviceContext* pContext;
IDXGISwapChain* pSwapChain;

DWORD_PTR* pSwapChainVTable = NULL;
DWORD_PTR* pDeviceContextVTable = NULL;

uint64_t presentHookTrampoline = NULL;

void InitialiseD3D11Hooks() {
	cout << "Initialising hook for D3D11..." << endl;

	// Get the handle for the module
	/*HMODULE d3d11 = GetModuleHandle(TEXT("d3d11.dll"));
	if (d3d11 == NULL) {
		cout << "Could not get handle for D3D11." << endl;
		return;
	}

	void* D3D11CreateDeviceAndSwapChain;
	if ((D3D11CreateDeviceAndSwapChain = GetProcAddress(d3d11, "D3D11CreateDeviceAndSwapChain")) == NULL) {
		cout << "Could not get D3D11CreateDeviceAndSwapChain address." << endl;
		return;
	}*/

	// Get window handle
	HWND hWindow = GetForegroundWindow();

	// Create swap chain description
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC swapChainDesc = CreateSwapChainDescription(hWindow);
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, NULL, &pContext))) {
		cout << "Failed to create D3D11 device and swapchain." << endl;
		return;
	}

	cout << "Created DirectX device successfully." << endl;

	// Get VTable
	pSwapChainVTable = (DWORD_PTR*)((DWORD_PTR*)pSwapChain)[0];
	pDeviceContextVTable = (DWORD_PTR*)((DWORD_PTR*)pContext)[0];

	cout << "D3D11_Present offset is: " << pSwapChainVTable[DXGIOffsets::Present] << endl;
}