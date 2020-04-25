#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <d3d9.h>
#include <d3d11.h>

#include "../Include/ImGui/imgui.h"
#include "Texture.h"
#include "../Hook/GraphicsAPI.h"

namespace EngineResources {
	void SetD3D9Device(LPDIRECT3DDEVICE9* pDevice);
	void SetD3D11Device(ID3D11Device* pDevice);
	void AddTexture(Texture* texture);
	void RenderTexture(GraphicsAPI graphicsAPI, TextureID textureId, ImVec2 size = ImVec2(0, 0));
	void Cleanup();
}