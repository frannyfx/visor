#include <iostream>
#include "Util/Process.h"
#include <Windows.h>
#include <string>

using namespace std;

string processName = "GFXTest64.exe";

#if _WIN64
#define BOOST_DLL "\\boost_random-vc142-mt-gd-x64-1_72.dll"
#else
#define BOOST_DLL "\\boost_random-vc142-mt-gd-x32-1_72.dll"
#endif

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
    InjectLibrary(pid, parentDirectory + BOOST_DLL);
    InjectLibrary(pid, parentDirectory + "\\libprotobufd.dll");
    InjectLibrary(pid, parentDirectory + "\\libprotobuf-lited.dll");
    InjectLibrary(pid, parentDirectory + "\\Core.dll");
    return 0;
}
