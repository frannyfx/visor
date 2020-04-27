#include "../Include/ImGui/imgui.h"
#include <Windows.h>
#include <string>

using namespace std;

namespace Utils {
	float Clamp(float value, float min, float max) {
		if (value > max)
			value = max;
		
		if (value < min)
			value = min;

		return value;
	}

	ImU32 GetU32(const int r, const int g, const int b, const int a)
	{
		return ((a & 0xff) << 24) + ((b & 0xff) << 16) + ((g & 0xff) << 8) + (r & 0xff);
	}
	
	float Lerp(float a, float b, float f) {
		f = Clamp(f, 0, 1);
		return a + (b - a) * f;
	}

	string GetWindowTitle() {
		HWND hWindow = GetForegroundWindow();

		char windowTitle[256];
		GetWindowText(hWindow, windowTitle, sizeof(windowTitle));
		return string(windowTitle);
	}

	int GetPID() {
		return GetCurrentProcessId();
	}

	string GetExecutablePath() {
		char filename[MAX_PATH];
		GetModuleFileNameA(0, filename, sizeof(filename));
		return string(filename);
	}
}