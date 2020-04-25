#include <iostream>

// Graphics
#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")

// Hooking
#include <MinHook/MinHook.h>

// Offsets
#include "DXGI.h"

// GUI
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx11.h>
#include "../../Engine/Engine.h"
#include "../../Engine/EngineResources.h"

using namespace std;


namespace D3D11Hook {
	// Hooking
	typedef HRESULT(__stdcall* D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);
	D3D11PresentHook presentHookTrampoline = NULL;
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
			cout << "D3D11 Present hook called." << endl;
			
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

			//Initialise our engine
			EngineResources::SetD3D11Device(pDevice);
			EngineResources::AddTexture(new Texture(TextureID::VISOR_LOGO, "C:\\Users\\blazi\\Desktop\\squidward.jpg"));

			ImGui_ImplWin32_Init(sd.OutputWindow);
			ImGui_ImplDX11_Init(pDevice, pContext);
			pContext->OMSetRenderTargets(1, &renderTargetView, NULL);
			
			presentCalled = true;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		Engine::Render(GraphicsAPI::D3D11);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		return presentHookTrampoline(pSwapChain, syncInterval, flags);
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

		// Setup ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();

		// Hook Present function
		cout << "Hooking Present function..." << endl;

		LPVOID toHook = (LPVOID)pSwapChainVTable[static_cast<uint32_t>(DXGIHook::DXGIOffsets::Present)];

		if (MH_CreateHook(toHook, &PresentHook, reinterpret_cast<LPVOID*>(&presentHookTrampoline)) != MH_OK) {
			cout << "Failed to install hooks for D3D11." << endl;
			return;
		};

		if (MH_EnableHook(toHook) != MH_OK) {
			cout << "Failed to enable hooks for D3D11." << endl;
			return;
		}
	}
}