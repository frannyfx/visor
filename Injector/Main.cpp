#include <iostream>
#include "Util/Process.h"
#include <Windows.h>
#include <string>

using namespace std;

string processName = "hl.exe";

#if _WIN64
#define MIN_HOOK_NAME "\\MinHook.x64.dll"
#else
#define MIN_HOOK_NAME "\\MinHook.x86.dll"
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
    //parentDirectory = "C:\\Users\\blazi\\Desktop\\Visor\\Build\\Debug\\Win32";
    cout << "Injecting libraries from " << parentDirectory << " into " << processName << " (" << pid << ")." << endl;
    InjectLibrary(pid, parentDirectory + "\\capstone.dll");
    InjectLibrary(pid, parentDirectory + "\\PolyHook_2.dll");
    InjectLibrary(pid, parentDirectory + MIN_HOOK_NAME);
    InjectLibrary(pid, parentDirectory + "\\Core.dll");
    return 0;
}
