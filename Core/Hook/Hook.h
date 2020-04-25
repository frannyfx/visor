#pragma once
#include <Windows.h>

// Define which graphics hooks to support
#define VISOR_HOOK_D3D9			0;
#define VISOR_HOOK_D3D10		0;
#define VISOR_HOOK_D3D11		1;
#define VISOR_HOOK_D3D12		0;
#define VISOR_HOOK_OPENGL		0;
#define VISOR_HOOK_VULKAN		0;

DWORD __stdcall InitialiseHooks(LPVOID);