#pragma once
#include <dxgi.h>

enum DXGIOffsets {
	// IUnknown
	QueryInterface = 0,
	AddRef = 1,
	Release = 2,

	// IDXGIObject
	SetPrivateData = 3,
	SetPrivateDataInterface = 4,
	GetPrivateData = 5,
	//GetParent = 6,

	// IDXGIDeviceSubObject
	GetDevice = 7,

	// IDXGISwapChain
	Present = 8,
	GetBuffer = 9,
	SetFullscreenState = 10,
	GetFullscreenState = 11,
	GetDesc = 12,
	ResizeBuffers = 13,
	ResizeTarget = 14,
	GetContainingOutput = 15,
	GetFrameStatistics = 16,
	GetLastPresentCount = 17
};

DXGI_RATIONAL GetRefreshRate();
DXGI_MODE_DESC CreateBufferDescription();
DXGI_SAMPLE_DESC CreateSampleDescription();
DXGI_SWAP_CHAIN_DESC CreateSwapChainDescription(HWND hWindow);