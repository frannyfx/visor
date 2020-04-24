#include <iostream>

// Graphics
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

// Hooking
#include <polyhook2/CapstoneDisassembler.hpp>

#ifdef _WIN64
#include <polyhook2/Detour/x64Detour.hpp>
#else
#include <polyhook2/Detour/x86Detour.hpp>
#endif

// Offsets
#include "D3D9.h"

// GUI
#include "../../Include/ImGui/imgui.h"
#include "../../Include/ImGui/imgui_impl_win32.h"
#include "../../Include/ImGui/imgui_impl_dx9.h"

using namespace std;

static void ShowExampleAppSimpleOverlay(bool* p_open);

namespace D3D9Hook {
	// Hooking
	typedef HRESULT(__stdcall* D3D9EndSceneHook) (LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
	uint64_t presentHookTrampoline = NULL;
	bool presentCalled = false;

	// D3D objects
	LPDIRECT3DDEVICE9 device;

	DWORD_PTR* pDeviceVTable = NULL;

	HRESULT __stdcall PresentHook(LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) {
		if (!presentCalled) {
			cout << "D3D9 Present hook called." << endl;

			// Get device parameters
			D3DDEVICE_CREATION_PARAMETERS parameters;
			pDevice->GetCreationParameters(&parameters);

			// Initialise ImGui
			ImGui_ImplWin32_Init(parameters.hFocusWindow);
			ImGui_ImplDX9_Init(pDevice);

			presentCalled = true;
		}

		// Render
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		bool show_demo_window = true;
		ShowExampleAppSimpleOverlay(&show_demo_window);
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		return PLH::FnCast(presentHookTrampoline, PresentHook)(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	}

	D3DPRESENT_PARAMETERS GetPresentParameters(const HWND& hWindow, const D3DDISPLAYMODE& displayMode) {
		D3DPRESENT_PARAMETERS parameters;
		parameters.BackBufferWidth = 0;
		parameters.BackBufferHeight = 0;
		parameters.BackBufferFormat = displayMode.Format;
		parameters.BackBufferCount = 0;
		parameters.MultiSampleType = D3DMULTISAMPLE_NONE;
		parameters.MultiSampleQuality = NULL;
		parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		parameters.hDeviceWindow = hWindow;
		parameters.Windowed = 1;
		parameters.EnableAutoDepthStencil = 0;
		parameters.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
		parameters.Flags = NULL;
		parameters.FullScreen_RefreshRateInHz = 0;
		parameters.PresentationInterval = 0;
		return parameters;
	}

	void InitialiseD3D9Hooks() {
		cout << "Installing hooks for D3D9..." << endl;

		// Get window handle
		HWND hWindow = GetForegroundWindow();

		// Get D3D9 API
		LPDIRECT3D9 d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

		// Get display mode
		D3DDISPLAYMODE displayMode;
		d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode);

		// Get parameters & create device
		D3DPRESENT_PARAMETERS parameters = GetPresentParameters(hWindow, displayMode);
		if (d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT, &parameters, &device) < 0) {
			cout << "Failed to create D3D9 device." << endl;
			d3d9->Release();
			return;
		}

		// Get VTable
		pDeviceVTable = (DWORD_PTR*)((DWORD_PTR*)device)[0];

		// Setup ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();

		// Hook Present function
		cout << "Hooking EndScene function..." << endl;
#ifdef _WIN64
		PLH::CapstoneDisassembler disassembler(PLH::Mode::x64);
		PLH::x64Detour detour((uint64_t)pDeviceVTable[static_cast<uint32_t>(D3D9Hook::D3D9Offsets::Present)], (uint64_t)PresentHook, &presentHookTrampoline, disassembler);
		detour.hook();
#else
		PLH::CapstoneDisassembler disassembler(PLH::Mode::x64);
		PLH::x86Detour detour((uint64_t)pDeviceVTable[static_cast<uint32_t>(D3D9Hook::D3D9Offsets::Present)], (uint64_t)PresentHook, &presentHookTrampoline, disassembler);
		detour.hook();
#endif
	}
}

static void ShowExampleAppSimpleOverlay(bool* p_open)
{
	const float DISTANCE = 10.0f;
	static int corner = 0;
	ImGuiIO& io = ImGui::GetIO();
	if (corner != -1)
	{
		ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
		ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	}
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	if (ImGui::Begin("Example: Simple overlay", p_open, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
	{
		ImGui::Text("Simple overlay\n" "in the corner of the screen.\n" "(right-click to change position)");
		ImGui::Separator();
		if (ImGui::IsMousePosValid())
			ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
		else
			ImGui::Text("Mouse Position: <invalid>");
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
			if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
			if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
			if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
			if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
			if (p_open && ImGui::MenuItem("Close")) *p_open = false;
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}
