#include "Fonts.h"

namespace Engine::Fonts {
	map<Font, ImFont*> fonts;

	void RegisterFont(Font font, ImFont* instance) {
		fonts[font] = instance;
	}

	ImFont* GetFont(Font font) {
		return fonts[font];
	}
}