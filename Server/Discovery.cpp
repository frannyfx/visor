#include <iostream>
#include <Windows.h>
#include "Discovery.h"

using namespace std;

#define BUFSIZE 256
namespace Discovery {
    // Forward declaration
    DWORD WINAPI StartDiscovery(LPVOID);
    DWORD WINAPI InstanceThread(LPVOID);

    // Pipe data
	bool running = true;
    LPCTSTR lpvMessage = TEXT("ws://localhost:1337");
    LPCTSTR lpszPipename = TEXT("\\\\.\\pipe\\visor_discovery");

    HANDLE Start() {
        cout << "Hitting JUUL" << endl;
        HANDLE hThread = CreateThread(
            NULL,
            0,
            StartDiscovery,
            NULL,
            0,
            NULL
        );

        return hThread;
    }

    DWORD WINAPI StartDiscovery(LPVOID) {
        HANDLE hPipe = NULL;
        HANDLE hThread = NULL;
        DWORD  dwThreadId = 0;

        while (running) {
            hPipe = CreateNamedPipe(
                lpszPipename,             // pipe name 
                PIPE_ACCESS_DUPLEX,       // read/write access 
                PIPE_TYPE_MESSAGE |       // message type pipe 
                PIPE_READMODE_MESSAGE |   // message-read mode 
                PIPE_WAIT,                // blocking mode 
                PIPE_UNLIMITED_INSTANCES, // max. instances  
                BUFSIZE,                  // output buffer size 
                BUFSIZE,                  // input buffer size 
                0,                        // client time-out 
                NULL);                    // default security attribute 

            if (hPipe == INVALID_HANDLE_VALUE) {
                cout << "Invalid pipe handle." << endl;
                return NULL;
            }

            bool connected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
            if (connected) {
                cout << "Client connected." << endl;
                hThread = CreateThread(
                    NULL,
                    0,
                    InstanceThread,
                    (LPVOID)hPipe,
                    0,
                    &dwThreadId
                );

                if (hThread == NULL) {
                    cout << "Spawning pipe thread failed." << endl;
                    return NULL;
                }
                else CloseHandle(hThread);
            }
            else CloseHandle(hPipe);
        }

        return NULL;
    }

    DWORD WINAPI InstanceThread(LPVOID lpvParam) {
        HANDLE hHeap = GetProcessHeap();
        TCHAR* pchReply = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));

        DWORD cbWritten = 0;
        BOOL fSuccess = FALSE;
        HANDLE hPipe = NULL;

        if (lpvParam == NULL) {
            cout << "Invalid pipe server thread spawned." << endl;
            if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
            return (DWORD)-1;
        }

        if (pchReply == NULL) {
            cout << "Heap allocation for reply buffer failed." << endl;
            return (DWORD)-1;
        }

        cout << "Pipe server thread created successfully." << endl;
        hPipe = (HANDLE)lpvParam;

        fSuccess = WriteFile(
            hPipe,
            lpvMessage,
            BUFSIZE * sizeof(TCHAR),
            &cbWritten,
            NULL
        );

        if (!fSuccess) {
            if (GetLastError() == ERROR_BROKEN_PIPE) cout << "Client disconnected." << endl;
        }

        cout << "Sent client discovery information successfully." << endl;
    }
}