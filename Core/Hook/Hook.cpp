#include <Windows.h>
#include <fstream>
#include <string>
#include <iostream>

// DirectX
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

// Detour
#ifdef _WIN64
#include <polyhook2/Detour/x64Detour.hpp>
#else
#include <polyhook2/Detour/x86Detour.hpp>
#endif

// Hooking libraries
#include <polyhook2/CapstoneDisassembler.hpp>
#include "../include/FW1FontWrapper/FW1FontWrapper.h"

using namespace std;

typedef HRESULT(__stdcall* D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);

// DX
ID3D11Device* pDevice;
ID3D11DeviceContext* pContext;
IDXGISwapChain* pSwapChain;

DWORD_PTR* pSwapChainVTable = NULL;
DWORD_PTR* pDeviceContextVTable = NULL;

D3D11PresentHook* originalPresent;
uint64_t presentHookTrampoline = NULL;

/*
void Log(const std::string& text) {
	std::ofstream log("C:\\Users\\blazi\\Desktop\\kush.log", std::ofstream::app | std::ofstream::out);
	log << text << std::endl;
}*/

// Rendering text
IFW1Factory* pFW1Factory = NULL;
IFW1FontWrapper* pFontWrapper = NULL;

// Logic
bool presentCalled = false;

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

	pFontWrapper->DrawString(pContext, L"Visor injected successfully", 14, 16, 16, 0xffff0000, FW1_RESTORESTATE);
	return PLH::FnCast(presentHookTrampoline, PresentHook)(pSwapChain, syncInterval, flags);
}


DWORD __stdcall InstallHooks(LPVOID) {
	cout << "Installing hooks..." << endl;

	// Get window handle
	HWND hWindow = GetForegroundWindow();
	
	// Create swap chain description
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWindow;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, NULL, &pContext))) {
		cout << "Failed to create DirectX device and swapchain." << endl;
		return NULL;
	}

	cout << "Created DirectX device successfully." << endl;
	
	pSwapChainVTable = (DWORD_PTR*)((DWORD_PTR*)pSwapChain)[0];
	pDeviceContextVTable = (DWORD_PTR*)((DWORD_PTR*)pContext)[0];

#ifdef _WIN64
	PLH::CapstoneDisassembler disassembler(PLH::Mode::x64);
	PLH::x64Detour detour((uint64_t)(pSwapChainVTable[8]), (uint64_t)PresentHook, &presentHookTrampoline, disassembler);
	detour.hook();
#else
	PLH::CapstoneDisassembler disassembler(PLH::Mode::x86);
	PLH::x86Detour detour((uint64_t)(pSwapChainVTable[8]), (uint64_t)PresentHook, &presentHookTrampoline, disassembler);
	detour.hook();
#endif

	cout << "Hooked Present function successfully." << endl;

	pDevice->Release();
	pContext->Release();
	pSwapChain->Release();
	return NULL;
}

void UninstallHooks() {
	// Unhook function


	// Release resources
	//pFontWrapper->Release();
}