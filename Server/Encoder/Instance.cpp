#include "Instance.h"

#include <x264.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>

void Encoder::Instance::Start() {
	av_register_all();

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

	return NULL;
}