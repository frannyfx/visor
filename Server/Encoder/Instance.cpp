#include "Instance.h"

void Encoder::Instance::Start() {
	
}

DWORD WINAPI Encoder::Instance::Run(LPVOID) {
	// Setup encoding
	// ...

	// Create lock
	unique_lock<mutex> lock(frameAvailableMutex);

	while (running) {
		// Wait for frames
		frameAvailableCV.wait(lock, [this] { return !frames.empty(); });
		for (auto& frame : frames) {

		}
	}
}