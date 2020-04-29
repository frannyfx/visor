#include "EngineResources.h"

using namespace std;

namespace EngineResources {
	map<TextureID, Texture*> textures;

	// Graphics devices
	LPDIRECT3DDEVICE9* pD3D9Device = nullptr;
	ID3D11Device* pD3D11Device = nullptr;
	ID3D12Device* pD3D12Device = nullptr;

	// DX11 Resources
	vector<ID3D11ShaderResourceView*>  dx11SRVs;

	void SetD3D9Device(LPDIRECT3DDEVICE9* pDevice) {
		pD3D9Device = pDevice;
	}

	void SetD3D11Device(ID3D11Device* pDevice) {
		pD3D11Device = pDevice;
	}

	void SetD3D12Device(ID3D12Device* pDevice) {
		pD3D12Device = pDevice;
	}

	void AddTexture(Texture* texture) {
		// Insert if not yet inserted
		if (textures.find(texture->textureId) == textures.end()) {
			textures.insert(make_pair(texture->textureId, texture));
		}
	}

	void RenderTexture(GraphicsAPI graphicsAPI, TextureID textureId, ImVec2 size) {
		// Don't go forward if the texture is not loaded.
		if (textures.find(textureId) == textures.end()) {
			return;
		}

		// Get texture
		Texture* texture = textures.at(textureId);

		// Get size
		ImVec2 displaySize = ImVec2(size.x == 0 ? texture->width : size.x, size.y == 0 ? texture->height : size.y);

		// Change drawing logic depending on graphics API
		switch (graphicsAPI) {
		case GraphicsAPI::D3D9:
			PDIRECT3DTEXTURE9 d3d9Texture;
			texture->LoadD3D9Texture(pD3D9Device, &d3d9Texture);
			ImGui::Image((void*)d3d9Texture, displaySize);
			break;
		case GraphicsAPI::D3D11:
			ID3D11ShaderResourceView* srv;
			texture->LoadD3D11Texture(pD3D11Device, &srv);
			ImGui::Image(srv, displaySize);
			dx11SRVs.push_back(srv);
			break;
		default:
			cout << "Could not render texture for this graphics API." << endl;
			break;
		}
	}
	
	void Cleanup() {
		// Release DX11 SRVs
		for (vector<ID3D11ShaderResourceView*>::iterator it = dx11SRVs.begin(); it != dx11SRVs.end();) {
			(*it)->Release();
			it = dx11SRVs.erase(it);
		}
	}

}