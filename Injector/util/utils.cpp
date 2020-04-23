#include <string>
#include <tchar.h>

using namespace std;

string GetFilenameWithoutExtension(const string &filename) {
	size_t dotIndex = filename.find_last_of(".");
	return filename.substr(0, dotIndex);
}

string ConvertTCHARArrayToString(const TCHAR arr[]) {
	wstring arr_w(arr);
	string arr_s(arr_w.begin(), arr_w.end());
	return arr_s;
}