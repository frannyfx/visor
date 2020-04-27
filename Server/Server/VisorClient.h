#pragma once
#include <string>
#include <websocketpp/server.hpp>

using namespace std;
using namespace websocketpp;

class VisorClient {
private:
	connection_hdl hdl;
	bool isRecording;

public:
	int pid;
	string executablePath;
	string windowTitle;

	VisorClient() : pid(-1) {}
	
	VisorClient(connection_hdl hdl, int pid, string executablePath, string windowTitle) : pid(pid), executablePath(executablePath), windowTitle(windowTitle) {
		isRecording = false;
	}
};