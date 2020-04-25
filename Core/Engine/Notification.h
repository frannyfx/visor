#pragma once
#include <string>
#include <chrono>
#include "../Hook/GraphicsAPI.h"

using namespace std;
using namespace std::chrono;

class Notification {
public:
	// Rendering style
	const int NOTIFICATION_DURATION = 3000;
	const int NOTIFICATION_FADE_DURATION = 300;
	const int NOTIFICATION_DISTANCE = 10;
	const float NOTIFICATION_MAX_ALPHA = 1;
	const float NOTIFICATION_BG_ALPHA = 0.7f;

	// Properties
	string title;
	string content;
	milliseconds displayTime;
	bool visible;
	
	Notification(string title, string content) : title(title), content(content), visible(true) {
		displayTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	}

	void Render(GraphicsAPI graphicsAPI);
	int GetTimeAlive();

	Notification& operator=(const Notification& other) {
		if (this != &other) {
			title = other.title;
			content = other.content;
			displayTime = other.displayTime;
			visible = other.visible;
		}

		return *this;
	}
};