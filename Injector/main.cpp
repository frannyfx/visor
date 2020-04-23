#include <iostream>
#include "util/process.h"

int main()
{
    int pid = GetPIDWithName("notepad.exe");
    InjectLibrary(pid, "C:\\Users\\blazi\\Desktop\\Visor\\Debug\\Core.dll");
    return 0;
}