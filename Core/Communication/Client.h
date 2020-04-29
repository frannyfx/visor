#pragma once
#include <string>
#include <iostream>

using namespace std;

// Protocols
#include <Protocols/client.pb.h>
#include <Protocols/server.pb.h>

namespace Client {
	void Open(string endpoint);
	void SendClientMessage(ClientMessage& message);
}