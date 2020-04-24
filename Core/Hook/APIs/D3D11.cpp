#include <iostream>

// Graphics
#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")

// Hooking
#include <polyhook2/CapstoneDisassembler.hpp>

#ifdef _WIN64
#include <polyhook2/Detour/x64Detour.hpp>
#else
#include <polyhook2/Detour/x86Detour.hpp>
#endif

// Offsets
#include "DXGI.h"

// GUI
#include "../../Include/ImGui/imgui.h"
#include "../../Include/ImGui/imgui_impl_win32.h"
#include "../../Include/ImGui/imgui_impl_dx11.h"

using namespace std;

namespace D3D11Hook {
	// Hooking
	typedef HRESULT(__stdcall* D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);
	uint64_t presentHookTrampoline = NULL;
	bool presentCalled = false;

	// D3D objects
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pContext;
	IDXGISwapChain* pSwapChain;
	ID3D11RenderTargetView* renderTargetView;

	DWORD_PTR* pSwapChainVTable = NULL;
	DWORD_PTR* pDeviceContextVTable = NULL;

	HRESULT __stdcall PresentHook(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags) {
		if (!presentCalled) {
			cout << "Present hook called." << endl;
			
			// Get swapchain device
			pSwapChain->GetDevice(__uuidof(pDevice), (void**)&pDevice);
			pDevice->GetImmediateContext(&pContext);

			// Get render target
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &renderTargetView);
			pBackBuffer->Release();

			// Get swapchain description
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);

			ImGui_ImplWin32_Init(sd.OutputWindow);
			ImGui_ImplDX11_Init(pDevice, pContext);
			
			presentCalled = true;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		pContext->OMSetRenderTargets(1, &renderTargetView, NULL);

		bool show_demo_window = true;
		ImGui::ShowDemoWindow(&show_demo_window);
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		return PLH::FnCast(presentHookTrampoline, PresentHook)(pSwapChain, syncInterval, flags);
	}

	void InitialiseD3D11Hooks() {
		cout << "Initialising hooks for D3D11..." << endl;

		// Get window handle
		HWND hWindow = GetForegroundWindow();

		// Create swap chain description
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
		DXGI_SWAP_CHAIN_DESC swapChainDesc = DXGIHook::CreateSwapChainDescription(hWindow);
		if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, NULL, &pContext))) {
			cout << "Failed to create D3D11 device and swapchain." << endl;
			return;
		}

		cout << "Created DirectX device successfully." << endl;

		// Get VTable
		pSwapChainVTable = (DWORD_PTR*)((DWORD_PTR*)pSwapChain)[0];
		pDeviceContextVTable = (DWORD_PTR*)((DWORD_PTR*)pContext)[0];

		// Set up UI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();

		// Hook Present function
		cout << "Hooking Present function..." << endl;
#ifdef _WIN64
		PLH::CapstoneDisassembler disassembler(PLH::Mode::x64);
		PLH::x64Detour detour((uint64_t)pSwapChainVTable[static_cast<uint32_t>(DXGIHook::DXGIOffsets::Present)], (uint64_t)PresentHook, &presentHookTrampoline, disassembler);
		detour.hook();
#else
		PLH::CapstoneDisassembler disassembler(PLH::Mode::x86);
		PLH::x86Detour detour((uint64_t)pSwapChainVTable[static_cast<uint32_t>(DXGIHook::DXGIOffsets::Present)], (uint64_t)PresentHook, &presentHookTrampoline, disassembler);
		detour.hook();
#endif
	}
}
