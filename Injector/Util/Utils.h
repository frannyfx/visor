#pragma once
#include <string>
#include <tchar.h>

using namespace std;

string GetFilenameWithoutExtension(const string &filename);
string ConvertTCHARArrayToString(const TCHAR arr[]);