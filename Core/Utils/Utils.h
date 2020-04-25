#pragma once
#include "../Include/ImGui/imgui.h"

namespace Utils {
	float Clamp(float value, float min, float max);
	ImU32 GetU32(const int r, const int g, const int b, const int a);
}