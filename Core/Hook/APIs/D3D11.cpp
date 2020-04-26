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
#include <condition_variable>

using namespace std;

namespace D3D11Hook {
	// Forward declaration
	void Uninstall();

	// Hooking
	typedef HRESULT(__stdcall* D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);
	D3D11PresentHook presentHookTrampoline = NULL;
	bool presentCalled = false;
	LPVOID toHook = NULL;

	// Installation
	mutex installationMutex;
	condition_variable installedCV;
	bool installed = false;
	bool success = false;

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
			
			// Get swapchain device, make sure it's valid
			pSwapChain->GetDevice(__uuidof(pDevice), (void**)&pDevice);
			if (pDevice == nullptr) {
				cout << "Likely another version of D3D is loaded. Disabling hook..." << endl;
				success = false;
				installedCV.notify_all();
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
			presentCalled = true;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		Engine::Render(GraphicsAPI::D3D11);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		return presentHookTrampoline(pSwapChain, syncInterval, flags);
	}

	void FinishInstall(bool s) {
		installed = true;
		success = s;
		installedCV.notify_all();
	}

	bool GetInstalled() {
		return installed;
	}

	DWORD WINAPI Install(LPVOID) {
		Hook();
		std::unique_lock<std::mutex> lock(installationMutex);
		installedCV.wait(lock, GetInstalled);
		if (!success) {

			Uninstall();
		}
		return NULL;
	}

	void Hook() {
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

		cout << "Created D3D11 device successfully." << endl;

		// Get VTable
		pSwapChainVTable = (DWORD_PTR*)((DWORD_PTR*)pSwapChain)[0];
		pDeviceContextVTable = (DWORD_PTR*)((DWORD_PTR*)pContext)[0];

		// Release resources
		pDevice->Release();
		pSwapChain->Release();
		pDevice = NULL;
		pSwapChain = NULL;

		// Hook Present function.
		toHook = (LPVOID)pSwapChainVTable[static_cast<uint32_t>(DXGIHook::DXGIOffsets::Present)];
		if (MH_CreateHook(toHook, &PresentHook, reinterpret_cast<LPVOID*>(&presentHookTrampoline)) != MH_OK) {
			cout << "Failed to install hooks for D3D11." << endl;
			return;
		};

		if (MH_EnableHook(toHook) != MH_OK) {
			cout << "Failed to enable hooks for D3D11." << endl;
			return;
		}
	}

	void Uninstall() {
		MH_DisableHook(toHook);
		//MH_RemoveHook(toHook);
	}
}