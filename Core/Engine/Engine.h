#pragma once
#include <string>
#include "../Hook/GraphicsAPI.h"
#include <d3d9.h>
#include <d3d11.h>
#include "Texture.h"

using namespace std;

namespace Engine {
	void Initialise();
	void ShowNotification(const string& title, const string& body);
	void Render(GraphicsAPI graphicsAPI);
}