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
#include <libavutil/imgutils.h>
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
		vector<uint8_t*> frames;
		time_point<system_clock> encodeStart;

		// Encoding
		FILE* file;
		SwsContext* swsContext;
		AVFormatContext* outputContext;
		AVCodecContext* context;
		AVStream* stream;
		static DWORD WINAPI Run(void* param);

	public:
		bool Start();
		void AddFrame();
		string GetFileName();
		Instance() : client(VisorClient()) {}
		Instance(VisorClient client) : client(client), running(false), outputContext(NULL), context(NULL), stream(NULL), file(NULL), swsContext(NULL) {

		}

		Instance& operator=(const Instance& rhs) {
			if (this->client.pid == rhs.client.pid)
				return *this;

			client = rhs.client;
			running = rhs.running;
			frames = rhs.frames;
			encodeStart = rhs.encodeStart;
			file = rhs.file;
			swsContext = rhs.swsContext;
			outputContext = rhs.outputContext;
			context = rhs.context;
			stream = rhs.stream;
			return *this;
		}
	};
}