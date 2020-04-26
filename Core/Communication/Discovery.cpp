#include <iostream>
#include <tchar.h>
#include <Windows.h>
#include "Discovery.h"
#include "../Engine/Engine.h"
#include "Client.h"

using namespace std;

#define BUFSIZE 256

namespace Discovery {
	// Forward declaration
	DWORD WINAPI Run(LPVOID);

	// Pipe params
	bool running = false;
	string endpoint = "";
	LPCTSTR lpszPipeName = TEXT("\\\\.\\pipe\\visor_discovery");

	string GetServerEndpoint() {
		return endpoint;
	}

	void Start() {
		if (running) {
			cout << "Cannot start pipe client as it's already running." << endl;
			return;
		}

		running = true;
		HANDLE hThread = CreateThread(
			NULL,
			0,
			Run,
			NULL,
			0,
			NULL
		);

		if (hThread == NULL) {
			cout << "Spawning pipe client thread failed." << endl;
			return;
		}
		else CloseHandle(hThread);
	}

	void HandleResult(bool result, string receivedEndpoint = "") {
		if (!result) {
			Engine::ShowNotification("Something went wrong.", "Please restart your game and try again.");
			return;
		}

		Client::Open(receivedEndpoint);
		cout << receivedEndpoint << endl;
	}

	void Stop() {
		running = false;
	}

	DWORD WINAPI Run(LPVOID) {
		HANDLE hPipe = NULL;
		TCHAR chBuf[512];
		BOOL fSuccess = FALSE;
		DWORD  cbRead, dwMode;

		// Repeatedly attempt to connect
		while (running) {
			hPipe = CreateFile(
				lpszPipeName,
				GENERIC_READ,
				0,
				NULL,
				OPEN_EXISTING,
				0,
				NULL
			);

			if (hPipe != INVALID_HANDLE_VALUE)
				break;

			if (GetLastError() != ERROR_PIPE_BUSY) {
				cout << "Could not open discovery pipe." << endl;
				HandleResult(false);
				return NULL;
			}

			if (!WaitNamedPipe(lpszPipeName, 5000)) {
				cout << "Discovery pipe connection timed out." << endl;
				HandleResult(false);
				return NULL;
			}
		}

		// Let's read
		cout << "Waiting to receive server endpoint..." << endl;
		fSuccess = ReadFile(
			hPipe,
			chBuf,
			BUFSIZE * sizeof(TCHAR),
			&cbRead,
			NULL
		);

		if (fSuccess) {
			wstring receivedEndpoint(chBuf);
			HandleResult(true, string(receivedEndpoint.begin(), receivedEndpoint.end()));
			return NULL;
		}
		
		HandleResult(false);
		CloseHandle(hPipe);
		return NULL;
	}
}