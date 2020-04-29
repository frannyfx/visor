#pragma once
#include <map>
#include "../Include/ImGui/imgui.h"

using namespace std;

namespace Engine::Fonts {
	enum class Font {
		REGULAR = 0,
		BOLD = 1,
		BLACK = 2,
		LIGHT = 3
	};

	void RegisterFont(Font font, ImFont* instance);
	ImFont* GetFont(Font font);
}