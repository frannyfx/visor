#pragma once
#include <Windows.h>

// Define which graphics hooks to support
#define VISOR_HOOK_D3D9			1;
#define VISOR_HOOK_D3D10		1;
#define VISOR_HOOK_D3D11		0;
#define VISOR_HOOK_D3D12		_WIN64;
#define VISOR_HOOK_OPENGL		1;
#define VISOR_HOOK_VULKAN		1;

DWORD __stdcall InitialiseHooks(LPVOID);