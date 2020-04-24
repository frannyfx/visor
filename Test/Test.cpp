#include <iostream>
#include <Windows.h>
#include <string>
#include <tchar.h>

using namespace std;

int main()
{
    HMODULE myDLL = LoadLibrary(_T("C:\\Users\\blazi\\Desktop\\Visor\\x64\\Debug\\Core.dll"));
    if (myDLL == NULL) {
        cout << "FUCK" << endl;
        return 1;
    }

    while (true) {

    }
}
