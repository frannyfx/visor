#pragma once
#include <Windows.h>

// Define which graphics hooks to support
<<<<<<< HEAD
#define VISOR_HOOK_D3D9			1;
#define VISOR_HOOK_D3D10		0;
#define VISOR_HOOK_D3D11		1;
#define VISOR_HOOK_D3D12		0;
#define VISOR_HOOK_OPENGL		1;
=======
#define VISOR_HOOK_D3D9			0;
#define VISOR_HOOK_D3D10		0;
#define VISOR_HOOK_D3D11		1;
#define VISOR_HOOK_D3D12		0;
#define VISOR_HOOK_OPENGL		0;
>>>>>>> 443a09f41652986a930aa2c0de1f0cdb0c1fa1f4
#define VISOR_HOOK_VULKAN		0;

DWORD __stdcall InitialiseHooks(LPVOID);