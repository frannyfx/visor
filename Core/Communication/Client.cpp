#include "Client.h"

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

// Protocols
#include <Protocols/client.pb.h>
#include <Protocols/server.pb.h>

#include "../Engine/Engine.h"
#include "../Utils/Utils.h";

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

namespace Client {
	// Create types
	typedef websocketpp::client<websocketpp::config::asio_client> client;
	typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

	// Forward declaration
	void OnMessage(client* visor_client, websocketpp::connection_hdl hdl, message_ptr msg);
	void OnOpen(client* visor_client, websocketpp::connection_hdl hdl);
	void SendClientMessage(ClientMessage& message);

	// Client
	string endpoint;
	client visor_client;
	websocketpp::connection_hdl handle;

	void Open(string e) {
		endpoint = e;
		cout << "Attempting to connect to " << endpoint << endl;

		try {
			// Set logging to be pretty verbose (everything except message payloads)
			visor_client.set_access_channels(websocketpp::log::alevel::all);
			visor_client.clear_access_channels(websocketpp::log::alevel::frame_payload);

			// Initialise ASIO
			visor_client.init_asio();

			// Register handlers
			visor_client.set_open_handler(bind(&OnOpen, &visor_client, ::_1));
			visor_client.set_message_handler(bind(&OnMessage, &visor_client, ::_1, ::_2));

			// Attempt to connect
			websocketpp::lib::error_code error;
			client::connection_ptr connection = visor_client.get_connection(endpoint, error);
			if (error) {
				cout << "Could not create connection. " << error.message() << endl;
				return;
			}

			visor_client.connect(connection);
			visor_client.run();
		}
		catch (websocketpp::exception const& e) {
			cout << e.what() << endl;
		}
	}

	void OnMessage(client* visor_client, websocketpp::connection_hdl hdl, message_ptr msg) {
		cout << "Message received with HDL: " << hdl.lock().get() << " - Message: " << msg->get_payload() << endl;
	}

	void OnOpen(client* visor_client, websocketpp::connection_hdl hdl) {
		cout << "Successfully opened connection." << endl;
		handle = hdl;

		// Send message with game details
		ClientMessage message;
		message.set_message_type(ClientMessage_Type_HELLO);
		
		ClientMessage_Hello hello;
		hello.set_pid(Utils::GetPID());
		hello.set_executable_path(Utils::GetExecutablePath());
		hello.set_window_title(Utils::GetWindowTitle());
		message.set_allocated_hello(&hello);
		SendClientMessage(message);
	}

	void SendClientMessage(ClientMessage &message) {
		websocketpp::lib::error_code error;

		// Serialise the message
		string data;
		message.SerializeToString(&data);

		// Release message
		message.release_bookmark();
		message.release_frame();
		message.release_hello();
		message.Clear();

		visor_client.send(handle, data, websocketpp::frame::opcode::binary);
		if (error) {
			cout << "Sending message failed: " << error.message() << endl;
		}
	}
}