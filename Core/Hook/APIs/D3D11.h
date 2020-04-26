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

namespace Hook::D3D11 {
	class Instance : public Hook::HookBase {
	private:
		// Singleton
		static Instance* instance;
		Instance();

	public: 
		static Instance* GetInstance();

		void Install();
		void Enable();
		void Disable();
		void Uninstall();
	};
}

