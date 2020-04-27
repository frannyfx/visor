#pragma once
#include <websocketpp/server.hpp>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <Windows.h>

using namespace std;
using namespace websocketpp;

namespace Encoder {
	class Instance {
	private:
		connection_hdl hdl;
		bool running;
		mutex frameAvailableMutex;
		condition_variable frameAvailableCV;
		vector<vector<uint8_t>> frames;

		DWORD WINAPI Run(LPVOID);

	public:
		void Start();
		void AddFrame();
		Instance(connection_hdl hdl) : hdl(hdl), running(false) {

		}
	};
}