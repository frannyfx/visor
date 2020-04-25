#include <iostream>
#include <tchar.h>
#include <Windows.h>
#include <string>
#include "Client.h"

using namespace std;

namespace Client {
	// Forward declaration
	DWORD WINAPI Run(LPVOID);

	bool running = false;
	string pipeName = "\\\\.\\pipe\\visor";

	void Client::Start() {
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

	DWORD WINAPI Run(LPVOID) {
		HANDLE hPipe = NULL;
		TCHAR chBuf[512];
		BOOL fSuccess = FALSE;
		DWORD  cbRead, cbToWrite, cbWritten, dwMode;
		wstring wstr(pipeName.begin(), pipeName.end());

		// Repeatedly attempt to connect
		while (running) {
			hPipe = CreateFile(
				wstr.c_str(),
				GENERIC_READ | GENERIC_WRITE,
				0,
				NULL,
				OPEN_EXISTING,
				0,
				NULL
			);

			if (hPipe != INVALID_HANDLE_VALUE)
				break;

			if (GetLastError() != ERROR_PIPE_BUSY) {
				cout << "Could not open pipe." << endl;
				return NULL;
			}

			if (!WaitNamedPipe(wstr.c_str(), 5000)) {
				cout << "Pipe connection timed out." << endl;
				return NULL;
			}
		}

		// Let's read
		cout << "Connected " << hPipe << endl;
		dwMode = PIPE_READMODE_MESSAGE;
		fSuccess = SetNamedPipeHandleState(
			hPipe,
			&dwMode,
			NULL,
			NULL
		);

		if (!fSuccess) {
			cout << "Could not change the pipe's handle state." << endl;
			return NULL;
		}

		cbToWrite = (lstrlen(wstr.c_str()) + 1) * sizeof(TCHAR);
		cout << "Sending message " << wstr.c_str() << endl;

		fSuccess = WriteFile(
			hPipe,
			wstr.c_str(),
			cbToWrite,
			&cbWritten,
			NULL
		);

		cout << "Message sent." << endl;
		CloseHandle(hPipe);

		return NULL;
	}
}