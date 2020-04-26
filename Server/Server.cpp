#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>

using namespace std;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

namespace Server {
	// Create types
	typedef websocketpp::server<websocketpp::config::asio> server;
	typedef server::message_ptr message_ptr;

	// Forward declaration
	void OnMessage(server* s, websocketpp::connection_hdl hdl, message_ptr msg);
	void OnOpen(server* s, websocketpp::connection_hdl hdl);
	DWORD WINAPI StartServer(LPVOID);

	HANDLE Start() {
		cout << "Inhaling nicotine..." << endl;
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
		server echo_server;
		try {
			// Set logging settings
			echo_server.set_access_channels(websocketpp::log::alevel::all);
			echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

			// Initialise ASIO
			echo_server.init_asio();

			// Register message handler
			echo_server.set_open_handler(bind(&OnOpen, &echo_server, ::_1));
			echo_server.set_message_handler(bind(&OnMessage, &echo_server, ::_1, ::_2));

			// Listen on port 1337
			echo_server.listen(1337);

			// Start the server accept loop
			echo_server.start_accept();

			cout << "Inhaled nicotine successfully." << endl;

			// Start the ASIO io_service run loop
			echo_server.run();
		}
		catch (websocketpp::exception const& exception) {
			cout << exception.what() << endl;
		}
		catch (...) {
			cout << "Error in Visor server." << endl;
		}

		return NULL;
	}

	void OnOpen(server* s, websocketpp::connection_hdl hdl) {
		cout << "Client connected!" << endl;
	}

	void OnMessage(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
		cout << "Received message with HDL: " << hdl.lock().get() << " - Message: " << msg->get_payload() << endl;

		try {
			s->send(hdl, msg->get_payload(), msg->get_opcode());
		}
		catch (websocketpp::exception const& exception) {
			cout << "Sending message failed." << endl;
		}
	}
}