#include "Capture.h"
#include "../Communication/Client.h"
#include "Config.h"
#include <sstream>


namespace Capture {
	// Source resolution
	int width = 0;
	int height = 0;

	// Flags
	bool starting = false;
	bool recording = false;
	bool serverReady = false;
	bool firstFrame = true;

	// Timing
	time_point<steady_clock> captureStart;
	time_point<steady_clock> lastFrame;

	// Frames
	vector<LPD3DXBUFFER> frames;
	vector<LPD3DXBUFFER> waitingFrames;

	// Instance
	VisorConfig::Instance* instance = VisorConfig::Instance::GetInstance();

	// Forward declarations
	DWORD WINAPI Run(LPVOID);

	void Start() {
		if (width == 0 && height == 0) {
			cout << "Resolution not yet set, waiting for setting..." << endl;
			starting = true;
			return;
		}

		if (recording) {
			cout << "Already recording." << endl;
			return;
		}

		if (!instance->HasConfig()) {
			cout << "Have not yet received config from server, cannot start." << endl;
			return;
		}

		captureStart = high_resolution_clock::now();
		recording = true;

		HANDLE hThread = CreateThread(
			NULL,
			0,
			Run,
			NULL,
			0,
			NULL
		);

		// Send capture configuration
		ClientMessage message;
		message.set_message_type(ClientMessage_Type_CAPTURE_STARTED);

		ClientMessage_Capture capture;
		capture.set_width(width);
		capture.set_height(height);
		message.set_allocated_capture(&capture);

		Client::SendClientMessage(message);
	}

	void SetResolution(int w, int h) {
		if (recording) {
			cout << "Cannot set resolution while recording." << endl;
			return;
		}

		width = w;
		height = h;

		// If previously wanted to start but had no resolution, start now.
		if (starting) Start();
	}

	void AddFrame(LPD3DXBUFFER &buffer) {
		if (!recording)
			return;

		waitingFrames.push_back(buffer);
		firstFrame = false;
		lastFrame = high_resolution_clock::now();
	}

	DWORD WINAPI Run(LPVOID) {
		while (recording) {
			// Wait for frames to encode
			while (waitingFrames.empty()) {}
			
			frames.swap(waitingFrames);
			waitingFrames.clear();

			for (vector<LPD3DXBUFFER>::iterator it = frames.begin(); it != frames.end();) {
				LPD3DXBUFFER p = *it;
				void* pBuffer = p->GetBufferPointer();
				DWORD size = p->GetBufferSize();
				cout << pBuffer << endl;
				cout << size << endl;

				// Create message
				ClientMessage message;
				message.set_message_type(ClientMessage_Type_FRAME);

				ostringstream ss;
				ss.write((char*)pBuffer, size);
				message.set_frame(ss.str());
				
				Client::SendClientMessage(message);

				// Release buffer
				if (it != frames.end()) it = frames.erase(it);
				p->Release();
			}
		}

		return NULL;
	}

	bool ShouldCapture() {
		// Don't begin capturing unless the server is ready to receive frames.
		if (!serverReady)
			return false;

		// Skip timing calculation if this is the first frame we're encoding.
		if (firstFrame)
			return true;

		float deltaTime = (float)1000 / (float)(instance->GetConfig().capture().framerate());
		duration timeSinceLastFrame = duration_cast<milliseconds>(high_resolution_clock::now() - lastFrame);
		return recording && timeSinceLastFrame.count() >= deltaTime;
	}

	void ReceivedReady() {
		serverReady = true;
	}
}