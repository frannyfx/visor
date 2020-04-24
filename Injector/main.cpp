#include <iostream>
#include "Util/Process.h"
#include <Windows.h>
#include <string>

using namespace std;

string processName = "GFXTest64.exe";

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
        cerr << "Could not get handle on process " << processName << endl;
        return 1;
    }

    cout << "Found process " << processName << " (" << pid << ")." << endl;

    // Get library path and inject
    //string libraryPath = "C:\\Users\\spydas\\source\\repos\\visor\\Debug\\Core.dll";
    cout << "Injecting libraries from " << parentDirectory << " into " << processName << " (" << pid << ")." << endl;

    InjectLibrary(pid, parentDirectory + "\\capstone.dll");
    InjectLibrary(pid, parentDirectory + "\\PolyHook_2.dll");
    InjectLibrary(pid, parentDirectory + "\\FW1FontWrapper.dll");
    InjectLibrary(pid, parentDirectory + "\\Core.dll");
    return 0;
}
