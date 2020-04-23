#pragma once
#include <string>
using namespace std;

int GetPIDWithName(const string &processName);
bool InjectLibrary(const int& pid, const string& libraryPath);