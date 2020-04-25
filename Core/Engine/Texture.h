#pragma once
#include <string>
#include <d3d9.h>
#include <d3d11.h>

using namespace std;

enum class TextureID {
	NONE = -1,
	VISOR_LOGO = 0,
	VISOR_CAPTURE_INDICATOR = 1,
	VISOR_CAPTURE_INDICATOR_ACTIVE = 2
};

class Texture {
public:
	TextureID textureId;
	string filename;
	int width;
	int height;

	// Loaded textures
	PDIRECT3DTEXTURE9 d3d9Texture;
	unsigned char* d3d11ImageData;

	Texture(void) : textureId(TextureID::NONE) {}
	Texture(TextureID textureId, string filename) : textureId(textureId), filename(filename), width(-1), height(-1)  {
		d3d9Texture = NULL;
		d3d11ImageData = nullptr;
	}

	bool LoadD3D9Texture(LPDIRECT3DDEVICE9* pDevice, PDIRECT3DTEXTURE9* outTexture);
	bool LoadD3D11Texture(ID3D11Device* pDevice, ID3D11ShaderResourceView** outSRV);
	void UnloadD3D11Texture();
};