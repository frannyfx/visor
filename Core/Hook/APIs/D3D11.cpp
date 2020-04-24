#include <iostream>

// Graphics
#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")

// Hooking
#include <polyhook2/CapstoneDisassembler.hpp>
#include "../../Include/FW1FontWrapper/FW1FontWrapper.h"

#ifdef _WIN64
#include <polyhook2/Detour/x64Detour.hpp>
#else
#include <polyhook2/Detour/x86Detour.hpp>
#endif

// Offsets
#include "DXGI.h"

using namespace std;

// Hooking
typedef HRESULT(__stdcall* D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);
uint64_t presentHookTrampoline = NULL;
bool presentCalled = false;

IFW1Factory* pFW1Factory = NULL;
IFW1FontWrapper* pFontWrapper = NULL;

// DirectX objects
ID3D11Device* pDevice;
ID3D11DeviceContext* pContext;
IDXGISwapChain* pSwapChain;

DWORD_PTR* pSwapChainVTable = NULL;
DWORD_PTR* pDeviceContextVTable = NULL;

HRESULT __stdcall PresentHook(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags) {
	if (!presentCalled) {
		cout << "Present hook called." << endl;
		pSwapChain->GetDevice(__uuidof(pDevice), (void**)&pDevice);
		pDevice->GetImmediateContext(&pContext);

		FW1CreateFactory(FW1_VERSION, &pFW1Factory);
		pFW1Factory->CreateFontWrapper(pDevice, L"Arial", &pFontWrapper);
		pFW1Factory->Release();
		presentCalled = true;
	}

	pFontWrapper->DrawString(pContext, L"VISOR", 14, 16, 16, 0xffff0000, FW1_RESTORESTATE);
	return PLH::FnCast(presentHookTrampoline, PresentHook)(pSwapChain, syncInterval, flags);
}

void InitialiseD3D11Hooks() {
	cout << "Initialising hook for D3D11..." << endl;

	// Get window handle
	HWND hWindow = GetForegroundWindow();

	// Create swap chain description
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC swapChainDesc = CreateSwapChainDescription(hWindow);
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, NULL, &pContext))) {
		cout << "Failed to create D3D11 device and swapchain." << endl;
		return;
	}

	cout << "Created DirectX device successfully." << endl;

	// Get VTable
	pSwapChainVTable = (DWORD_PTR*)((DWORD_PTR*)pSwapChain)[0];
	pDeviceContextVTable = (DWORD_PTR*)((DWORD_PTR*)pContext)[0];

	// Hook present function
	cout << "Hooking Present function..." << endl;
#ifdef _WIN64
	PLH::CapstoneDisassembler disassembler(PLH::Mode::x64);
	PLH::x64Detour detour((uint64_t)pSwapChainVTable[DXGIOffsets::Present], (uint64_t)PresentHook, &presentHookTrampoline, disassembler);
	detour.hook();
#else
	PLH::CapstoneDisassembler disassembler(PLH::Mode::x86);
	PLH::x86Detour detour((uint64_t)pSwapChainVTable[DXGIOffsets::Present], (uint64_t)PresentHook, &presentHookTrampoline, disassembler);
	detour.hook();
#endif
}