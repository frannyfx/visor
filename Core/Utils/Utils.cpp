#include "../Include/ImGui/imgui.h"

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
}