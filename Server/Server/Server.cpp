#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>

// Client
#include "VisorClient.h"

// Protocols
#include <Protocols/client.pb.h>
#include <Protocols/server.pb.h>

// Encoder
#include "../Encoder/Encoder.h"

using namespace std;
using namespace websocketpp;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

namespace Server {
	// Create types
	typedef websocketpp::server<config::asio> server;
	typedef server::message_ptr message_ptr;

	// Forward declaration
	void OnMessage(server* s, connection_hdl hdl, message_ptr msg);
	void OnOpen(server* s, connection_hdl hdl);
	void OnClose(server* s, connection_hdl hdl);
	void SendServerMessage(connection_hdl hdl, ServerMessage& message);

	DWORD WINAPI StartServer(LPVOID);

	// Handlers
	void HandleHello(connection_hdl hdl, ClientMessage& message);
	void HandleCaptureStarted(connection_hdl hdl, ClientMessage& message);
	void HandleFrame(connection_hdl hdl, ClientMessage& message);

	// Server
	string endpoint = "ws://localhost:1337";
	server visor_server;

	// Clients
	map <connection_hdl, VisorClient, owner_less<connection_hdl>> connectionList;

	string GetEndpoint() {
		return endpoint;
	}

	HANDLE Start() {
		HANDLE hThread = CreateThread(
			NULL,
			0,
			StartServer,
			NULL,
			0,
			NULL
		);

		return hThread;
	}

	DWORD WINAPI StartServer(LPVOID) {
		try {
			// Set logging settings
			visor_server.set_access_channels(log::alevel::all);
			visor_server.clear_access_channels(log::alevel::frame_payload);

			// Initialise ASIO
			visor_server.init_asio();

			// Register message handler
			visor_server.set_open_handler(bind(&OnOpen, &visor_server, ::_1));
			visor_server.set_message_handler(bind(&OnMessage, &visor_server, ::_1, ::_2));
			visor_server.set_close_handler(bind(&OnClose, &visor_server, ::_1));

			// Listen on port 1337
			visor_server.listen(1337);

			// Start the server accept loop
			visor_server.start_accept();

			cout << "Inhaled nicotine successfully." << endl;

			// Start the ASIO io_service run loop
			visor_server.run();
		}
		catch (websocketpp::exception const& exception) {
			cout << exception.what() << endl;
		}
		catch (...) {
			cout << "Error in Visor server." << endl;
		}

		return NULL;
	}

	void OnOpen(server* s, connection_hdl hdl) {
		cout << "Client connected!" << endl;
	}

	void OnClose(server* s, connection_hdl hdl) {
		VisorClient client = connectionList[hdl];
		cout << "Client injected in " << client.windowTitle << " disconnected." << endl;
		connectionList.erase(hdl);
	}

	void OnMessage(server* s, connection_hdl hdl, message_ptr msg) {
		cout << "Received message!" << endl;
		
		// Parse message
		ClientMessage message;
		if (!message.ParseFromString(msg->get_payload())) {
			cout << "Failed to parse client message." << endl;
			return;
		}

		cout << "Message parsed successfully - Message type: " << message.message_type() << endl;

		// Handle message
		switch (message.message_type()) {
		case ClientMessage_Type::ClientMessage_Type_HELLO:
			HandleHello(hdl, message);
			break;
		case ClientMessage_Type_CAPTURE_STARTED:
			HandleCaptureStarted(hdl, message);
			break;
		case ClientMessage_Type_FRAME:
			HandleFrame(hdl, message);
			break;
		default:
			cout << "Unhandled client message type." << endl;
			break;
		}
	}

	void SendServerMessage(connection_hdl hdl, ServerMessage& message) {
		lib::error_code error;

		// Serialise the message
		string data;
		message.SerializeToString(&data);

		visor_server.send(hdl, data, websocketpp::frame::opcode::binary);
		if (error) {
			cout << "Sending message failed: " << error.message() << endl;
		}
	}

	void HandleHello(connection_hdl hdl, ClientMessage& message) {
		// Don't add client if the payload is invalid or if the client has already been added.
		if (!message.has_hello() && connectionList.find(hdl) != connectionList.end())
			return;

		// Create client from information.
		VisorClient client = VisorClient(hdl, message.hello().pid(), message.hello().executable_path(), message.hello().window_title());

		// Add to connection list.
		cout << "Added client injected in " << client.windowTitle << endl;
		connectionList[hdl] = client;
		
		// Send configuration
		ServerMessage response;
		response.set_message_type(ServerMessage_Type_CONFIG);
		
		Config config;
		Config_CaptureConfig captureConfig;
		captureConfig.set_enabled(true);
		captureConfig.set_capture_mode(Config_CaptureConfig_CaptureMode_DVR);
		captureConfig.set_framerate(60);
		config.set_allocated_capture(&captureConfig);
		response.set_allocated_config(&config);
		SendServerMessage(hdl, response);

		config.release_capture();
		response.release_config();
		response.Clear();
	}

	void HandleCaptureStarted(connection_hdl hdl, ClientMessage& message) {
		if (!message.has_capture())
			return;

		// Get client
		VisorClient client = connectionList[hdl];
		if (client.pid == -1) {
			cout << "Client not found!" << endl;
			return;
		}

		// Set the resolution
		cout << "Source resolution: " << message.capture().width() << "x" << message.capture().height() << endl;
		client.SetSourceResolution(message.capture().width(), message.capture().height());

		// Spawn thread
		if (Encoder::Spawn(client)) {
			cout << "New encoder spawned successfully." << endl;
			
			// Send a message saying we're ready to receive frames
			ServerMessage response;
			response.set_message_type(ServerMessage_Type_ENCODER_READY);
			SendServerMessage(hdl, response);
			response.Clear();
		}
	}

	void HandleFrame(connection_hdl hdl, ClientMessage& message) {

	}
}