#include <iostream>
#include "util/process.h"
#include <Windows.h>

string processName = "notepad.exe";

int main()
{
    // Get running directory
    char filename[MAX_PATH];
    GetModuleFileNameA(0, filename, sizeof(filename));
    string filenameStr = (string)filename;
    string parentDirectory = filenameStr.substr(0, filenameStr.find_last_of("\\"));

    // Get the PID of the process
    int pid = GetPIDWithName(processName);
    if (pid == 0) {
        cerr << "Could not get handle on process " << processName << "\." << endl;
        return 1;
    }

    cout << "Found process " << processName << " (" << pid << ")." << endl;

    // Get library path and inject
    string libraryPath = parentDirectory + "\\Core.dll";
    cout << "Attempting to inject library " << libraryPath << " into " << processName << " (" << pid << ")." << endl;
    InjectLibrary(pid, libraryPath);
    return 0;
}
