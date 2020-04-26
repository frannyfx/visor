#pragma once
#include <iostream>

// Graphics
#include "DXGI.h"
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

// Hooking
#include <MinHook/MinHook.h>
#include "HookBase.h"

// GUI
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include "../../Engine/Engine.h"
#include "../../Engine/EngineResources.h"

using namespace std;

namespace Hook::D3D12 {
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
