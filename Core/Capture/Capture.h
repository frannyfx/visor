#pragma once
#include <chrono>
#include <iostream>
using namespace std;
using namespace chrono;

namespace Capture {
	void Start();
	void SetResolution(int w, int h);
	void AddFrame(char* bufferPointer, int size);
	bool ShouldCapture();
	void ReceivedReady();
}