#include <Windows.h>
#include <wrl/client.h>
#include <iostream>

// Graphics
#include <d3d12.h>
#include <dxgi1_5.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

// Hooking
#include <MinHook/MinHook.h>

// Offsets
#include "DXGI.h"

// GUI
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx12.h>
#include "../../Engine/Engine.h"
#include "../../Engine/EngineResources.h"

using namespace std;
using namespace Microsoft::WRL;

namespace D3D12Hook {
	// Hooking
	typedef long(__stdcall* D3D12PresentHook) (IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);
	D3D12PresentHook presentHookTrampoline = NULL;
	bool presentCalled = false;

	// D3D objects
	ID3D12Device* pDevice;
	IDXGISwapChain3* pSwapChain;
	IDXGIFactory5* pFactory;

	long __stdcall PresentHook(UINT syncInterval, UINT flags) {
		if (!presentCalled) {
			cout << "D3D12 Present hook called." << endl;

			//Get the device
			pSwapChain->GetDevice(__uuidof(ID3D12Device), (void**)&pDevice);
			cout << "Device ptr: " << pDevice << endl;

			presentCalled = true;
		}

		/*ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		Engine::Render(GraphicsAPI::D3D12);
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), NULL);*/

		return presentHookTrampoline(pSwapChain, syncInterval, flags);
	}

	void InitialiseD3D12Hooks() {
		cout << "Initialising hooks for D3D12" << endl;

		if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&pFactory)))) {
			cout << "Failed to create DXGI factory" << endl;
			return;
		}

		ComPtr<IDXGIAdapter1> pAdapter;
		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, pAdapter.ReleaseAndGetAddressOf()); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc = {};
			pAdapter->GetDesc1(&desc);
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

			if (SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr))) break;
		}

		if (FAILED(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&pDevice)))) {
			cout << "Failed to create D3D12 device" << endl;
			return;
		}

		ID3D12CommandQueue* pQ;
		D3D12_COMMAND_QUEUE_DESC qDesc{};

		qDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		qDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		qDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		qDesc.NodeMask = 0;
		if (FAILED(pDevice->CreateCommandQueue(&qDesc, IID_PPV_ARGS(&pQ))))
			return;

		// Describe and create the swap chain.
		DXGI_SWAP_CHAIN_DESC1 sc_desc{};
		sc_desc.BufferCount = 3;
		sc_desc.Width = 800;
		sc_desc.Height = 600;
		sc_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sc_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sc_desc.SampleDesc.Count = 1;

		HWND hTemp;
		// Initialize the window class.
		WNDCLASSEX windowClass = { 0 };
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = DefWindowProc;
		windowClass.hInstance = GetModuleHandle(NULL);
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.lpszClassName = L"DXSampleClass";
		RegisterClassEx(&windowClass);

		// Create the window and store a handle to it.
		hTemp = CreateWindow(windowClass.lpszClassName, TEXT("Window"), WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, nullptr, nullptr, windowClass.hInstance, NULL);

		IDXGISwapChain1* pSC;
		if (FAILED(pFactory->MakeWindowAssociation(hTemp, DXGI_MWA_NO_ALT_ENTER)))
			return;
		if (FAILED(pFactory->CreateSwapChainForHwnd(pQ, hTemp, &sc_desc, nullptr, nullptr, &pSC)))
			return;

		LPVOID toHook = (LPVOID) reinterpret_cast<D3D12PresentHook>(reinterpret_cast<INT_PTR*>(reinterpret_cast<INT_PTR*>(pSC)[0])[8]);
		DestroyWindow(hTemp);
		pSC->Release();
		pQ->Release();


		// Setup ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();

		// Hook Present function
		cout << "Hooking Present function" << endl;
		cout << toHook << endl;

		if (MH_CreateHook(toHook, &PresentHook, reinterpret_cast<LPVOID*>(&presentHookTrampoline)) != MH_OK) {
			cout << "Failed to install hooks for D3D12." << endl;
			return;
		}

		if (MH_EnableHook(toHook) != MH_OK) {
			cout << "Failed to enable hooks for D3D12." << endl;
			return;
		}

	}
}