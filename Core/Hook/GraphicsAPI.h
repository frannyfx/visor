#pragma once
enum class GraphicsAPI {
	D3D9,
	D3D10,
	D3D11,
	D3D12,
	OPENGL,
	VULKAN
};

struct LoadedGraphicsAPIs {
	bool D3D9Loaded;
	bool D3D10Loaded;
	bool D3D11Loaded;
	bool D3D12Loaded;
	bool OpenGLLoaded;
	bool VulkanLoaded;
};