#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <string>
#include <iostream>
#include "Utils.h"

using namespace std;

string GetProcessName(const unsigned int pid) {
	TCHAR processName[MAX_PATH] = TEXT("");
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

	if (hProcess != NULL) {
		HMODULE hMod;
		DWORD cbNeeded;
		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
			GetModuleBaseName(hProcess, hMod, processName, sizeof(processName) / sizeof(TCHAR));
			CloseHandle(hProcess);
		}
	}

	return ConvertTCHARArrayToString(processName);
}

int GetPIDWithName(const string &processName) {
	// Remove .exe
	string processed = GetFilenameWithoutExtension(processName) + ".exe";

	// Get processes
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
		return 1;
	}

	cProcesses = cbNeeded / sizeof(DWORD);
	for (unsigned int i = 0; i < cProcesses; i++) {
		if (aProcesses[i] != 0) {
			// Check if this matches
			if (processed.compare(GetProcessName(aProcesses[i])) == 0) {
				return aProcesses[i];
			}
		}
	}

	return 0;
}

bool InjectLibrary(const unsigned int pid, const string &libraryPath) {
	// Open a handle to the process
	HANDLE injectionProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	if (injectionProcess == NULL) {
		cerr << "Failed to get target process handle." << endl;
		return false;
	}

	// Allocate memory for the path of the library
	LPVOID allocation = VirtualAllocEx(injectionProcess, 0, libraryPath.length() + 1, MEM_COMMIT, PAGE_READWRITE);
	if (allocation == NULL) {
		cerr << "Failed to allocate memory in the target process." << endl;
		return false;
	}

	// Write the path to the memory
	int writeOK = WriteProcessMemory(injectionProcess, allocation, libraryPath.c_str(), libraryPath.length() + 1, 0);
	if (writeOK == 0) {
		cerr << "Failed to write memory in the target process." << endl;
		return false;
	}

	// Call load library and create a threaed
	LPTHREAD_START_ROUTINE loadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA");
	HANDLE threadReturn = CreateRemoteThread(injectionProcess, 0, 0, loadLibrary, allocation, 0, 0);
	if (threadReturn == NULL) {
		cerr << "Failed to create remote thread." << endl;
		return false;
	}

	// Wait for the thread to exit
	WaitForSingleObject(threadReturn, INFINITE);
	cout << "Library injected in process " << pid << "." << endl;

	// Free up the virtual memory we assigned earlier and close proc handle
	VirtualFreeEx(injectionProcess, allocation, 0, MEM_RELEASE);
	return true;
}