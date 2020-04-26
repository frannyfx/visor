#pragma once
#include <Windows.h>
#include <iostream>

// Hooking
#include <MinHook/MinHook.h>
#include "HookBase.h"

// GUI
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include "../../Engine/Engine.h"

// GUI
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx12.h>
#include "../../Engine/Engine.h"
#include "../../Engine/EngineResources.h"

using namespace std;

namespace Hook::OpenGL {
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
