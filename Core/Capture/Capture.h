#pragma once
#include <chrono>
#include <iostream>
#include <D3DX/d3dx9tex.h>

using namespace std;
using namespace chrono;

namespace Capture {
	void Start();
	void SetResolution(int w, int h);
	void AddFrame(LPD3DXBUFFER &buffer);
	bool ShouldCapture();
	void ReceivedReady();
}