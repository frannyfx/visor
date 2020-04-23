#include <iostream>
#include "util/process.h"

int main()
{
    int pid = GetPIDWithName("notepad.exe");
    InjectLibrary(pid, "C:\\Users\\blazi\\Desktop\\Visor\\x64\\Debug\\Core.dll");
    return 0;
}