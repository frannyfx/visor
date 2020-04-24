#include <dxgi.h>

namespace DXGIHook {
	DXGI_RATIONAL GetRefreshRate() {
		DXGI_RATIONAL refreshRate;
		refreshRate.Numerator = 60;
		refreshRate.Denominator = 1;
		return refreshRate;
	}

	DXGI_MODE_DESC CreateBufferDescription() {
		DXGI_MODE_DESC bufferDesc;
		bufferDesc.Width = 100;
		bufferDesc.Height = 100;
		bufferDesc.RefreshRate = GetRefreshRate();
		bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		return bufferDesc;
	}

	DXGI_SAMPLE_DESC CreateSampleDescription() {
		DXGI_SAMPLE_DESC sampleDesc;
		sampleDesc.Count = 1;
		sampleDesc.Quality = 0;
		return sampleDesc;
	}

	DXGI_SWAP_CHAIN_DESC CreateSwapChainDescription(const HWND& hWindow) {
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChainDesc.BufferDesc = CreateBufferDescription();
		swapChainDesc.SampleDesc = CreateSampleDescription();
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.OutputWindow = hWindow;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		return swapChainDesc;
	}
}