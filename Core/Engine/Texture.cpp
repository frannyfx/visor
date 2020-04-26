#include <iostream>
#include "Texture.h"
#include <D3DX/d3dx9tex.h>

using namespace std;

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "../Include/STB/stb_image.h"

#pragma comment(lib, "d3dx9.lib")

bool Texture::LoadD3D9Texture(LPDIRECT3DDEVICE9* pDevice, PDIRECT3DTEXTURE9* outTexture) {
	// Load cached texture
	if (d3d9Texture != NULL) {
		*outTexture = d3d9Texture;
		return TRUE;
	}

	// Create texture
	PDIRECT3DTEXTURE9 texture;
	if (D3DXCreateTextureFromFileA(*pDevice, filename.c_str(), &texture) != S_OK) {
		cout << "Failed to load D3D9 texture." << endl;
		return FALSE;
	}

	// Get texture description to get width and height
	D3DSURFACE_DESC textureDescription;
	texture->GetLevelDesc(0, &textureDescription);
	width = textureDescription.Width;
	height = textureDescription.Height;

	// Set cache and output texture
	d3d9Texture = texture;
	*outTexture = texture;
	return TRUE;
}

bool Texture::LoadD3D11Texture(ID3D11Device* pDevice, ID3D11ShaderResourceView** outSRV) {
	// Only load the image if not loaded yet
	if (d3d11ImageData == nullptr) {
		d3d11ImageData = stbi_load(filename.c_str(), &width, &height, NULL, 4);
		if (d3d11ImageData == NULL) {
			return FALSE;
		}
	}

	// Create texture description
	D3D11_TEXTURE2D_DESC textureDescription;
	ZeroMemory(&textureDescription, sizeof(textureDescription));
	textureDescription.Width = width;
	textureDescription.Height = height;
	textureDescription.MipLevels = 1;
	textureDescription.ArraySize = 1;
	textureDescription.ArraySize = 1;
	textureDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDescription.SampleDesc.Count = 1;
	textureDescription.Usage = D3D11_USAGE_DEFAULT;
	textureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDescription.CPUAccessFlags = 0;

	// Create texture
	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = d3d11ImageData;
	subResource.SysMemPitch = textureDescription.Width * 4;
	subResource.SysMemSlicePitch = 0;
	pDevice->CreateTexture2D(&textureDescription, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDescription;
	ZeroMemory(&srvDescription, sizeof(srvDescription));
	srvDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDescription.Texture2D.MipLevels = textureDescription.MipLevels;
	srvDescription.Texture2D.MostDetailedMip = 0;
	pDevice->CreateShaderResourceView(pTexture, &srvDescription, outSRV);
	pTexture->Release();
	return TRUE;
}

void Texture::UnloadD3D11Texture() {
	stbi_image_free(d3d11ImageData);
}