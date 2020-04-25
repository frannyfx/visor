#include <iostream>
#include <Windows.h>
#include "Server.h"

using namespace std;

#define BUFSIZE 512
namespace Server {
    // Forward declaration
    DWORD WINAPI InstanceThread(LPVOID);

    // Pipe data
	bool running = true;
    LPCTSTR lpvMessage = TEXT("http://localhost:1337");
    LPCTSTR lpszPipename = TEXT("\\\\.\\pipe\\visor_discovery");

    void Server::Start() {
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
                return;
            }

            cout << "Waiting for client to connect..." << endl;
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
                    return;
                }
                else CloseHandle(hThread);
            }
            else CloseHandle(hPipe);
        }
    }

    DWORD WINAPI InstanceThread(LPVOID lpvParam) {
        HANDLE hHeap = GetProcessHeap();
        TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));
        TCHAR* pchReply = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));

        DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
        BOOL fSuccess = FALSE;
        HANDLE hPipe = NULL;

        if (lpvParam == NULL) {
            cout << "Invalid pipe server thread spawned." << endl;
            if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
            if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
            return (DWORD)-1;
        }

        if (pchRequest == NULL) {
            cout << "Heap allocation for request buffer failed." << endl;
            if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
            return (DWORD)-1;
        }

        if (pchReply == NULL) {
            cout << "Heap allocation for reply buffer failed." << endl;
            if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
            return (DWORD)-1;
        }

        cout << "Pipe server thread created successfully." << endl;
        hPipe = (HANDLE)lpvParam;

        while (true) {
            fSuccess = WriteFile(
                hPipe,
                lpvMessage,
                BUFSIZE * sizeof(TCHAR),
                &cbWritten,
                NULL
            );

            if (!fSuccess || cbBytesRead == 0) {
                if (GetLastError() == ERROR_BROKEN_PIPE)
                    cout << "Client disconnected." << endl;
                else
                    cout << "Reading pipe failed." << endl;
                break;
            }

            cout << "Received message from client: " << pchRequest << endl;
        }
    }
}