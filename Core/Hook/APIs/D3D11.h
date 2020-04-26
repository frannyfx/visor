#pragma once
#include <iostream>

// Graphics
#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")

// Hooking
#include "HookBase.h"
#include <MinHook/MinHook.h>

// Offsets
#include "DXGI.h"

// GUI
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx11.h>
#include "../../Engine/Engine.h"
#include "../../Engine/EngineResources.h"

namespace Hook {
	class D3D11Hook : public Hook::HookBase {
	private:
		// Hooking
		typedef HRESULT(__stdcall* D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);
		D3D11PresentHook presentHookTrampoline = NULL;
		bool presentCalled = false;
		LPVOID presentHookAddress = NULL;

		// Direct3D
		ID3D11Device* pDevice;
		ID3D11DeviceContext* pContext;
		IDXGISwapChain* pSwapChain;
		ID3D11RenderTargetView* renderTargetView;

		DWORD_PTR* pSwapChainVTable = NULL;
		DWORD_PTR* pDeviceContextVTable = NULL;

		// Hook
		HRESULT __stdcall PresentHook(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);

	public: 
		void Install();
		void Enable();
		void Disable();
		void Uninstall();
		
	};
}