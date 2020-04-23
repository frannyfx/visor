#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <string>
#include <iostream>
#include "utils.h"

using namespace std;

string GetProcessName(const unsigned int &pid) {
	TCHAR processName[MAX_PATH] = TEXT("");
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

	if (hProcess != NULL) {
		HMODULE hMod;
		DWORD cbNeeded;
		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
			GetModuleBaseName(hProcess, hMod, processName, sizeof(processName) / sizeof(TCHAR));
		}

		CloseHandle(hProcess);
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

bool InjectLibrary(const int &pid, const string &libraryPath) {
	unsigned int pathLength = libraryPath.length() + 1;
	HANDLE injectionProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	if (injectionProcess == NULL) {
		cerr << "Failed to get target process handle." << endl;
		return false;
	}

	LPVOID allocation = VirtualAllocEx(injectionProcess, NULL, pathLength, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (allocation == NULL) {
		cerr << "Failed to allocate memory in the target process." << endl;
		return false;
	}

	int writeOK = WriteProcessMemory(injectionProcess, allocation, libraryPath.c_str(), pathLength, 0);
	if (writeOK == 0) {
		cerr << "Failed to write memory in the target process." << endl;
		return false;
	}

	DWORD threadId;
	LPTHREAD_START_ROUTINE loadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibrary(_T("kernel32")), "LoadLibraryA");
	HANDLE threadReturn = CreateRemoteThread(injectionProcess, NULL, 0, loadLibrary, allocation, 0, &threadId);
	if (threadReturn == NULL) {
		cerr << "Failed to create remote thread." << endl;
		return false;
	}

	cout << "Library injected." << endl;
	CloseHandle(injectionProcess);
	return true;
}