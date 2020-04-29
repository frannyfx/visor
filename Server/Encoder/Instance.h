#pragma once
#include <websocketpp/server.hpp>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <Windows.h>
#include <chrono>
#include <iostream>

#include "../Server/VisorClient.h"

extern "C" {
#include <x264.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
}

#pragma comment(lib, "avcodec.lib")
#pragma comment (lib, "avformat.lib")
#pragma comment (lib, "swscale.lib")
#pragma comment (lib, "avutil.lib")

using namespace std;
using namespace websocketpp;
using namespace chrono;

namespace Encoder {
	class Instance {
	private:
		VisorClient client;
		bool running;
		mutex frameAvailableMutex;
		condition_variable frameAvailableCV;
		vector<vector<char16_t>> frames;
		time_point<system_clock> encodeStart;

		// Encoding
		SwsContext* swsContext;
		AVFormatContext* outputContext;
		AVCodecContext* context;
		AVStream* stream;

		DWORD WINAPI Run(LPVOID);

	public:
		void Start();
		void AddFrame();
		string GetFileName();
		Instance() : client(VisorClient()) {}
		Instance(VisorClient client) : client(client), running(false), outputContext(NULL), context(NULL), stream(NULL) {

		}

		Instance& operator=(const Instance& rhs) {
			if (this->client.pid == rhs.client.pid)
				return *this;

			client = rhs.client;
			running = rhs.running;
			return *this;
		}
	};
}