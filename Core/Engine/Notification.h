#pragma once
#include <string>
#include <ctime>
using namespace std;

class Notification {
public:
	// Rendering style
	const int NOTIFICATION_DURATION = 5;
	const int NOTIFICATION_DISTANCE = 10;
	const float NOTIFICATION_BG_ALPHA = 0.3f;

	// Properties
	string title;
	string content;
	time_t displayTime;
	bool visible;
	
	Notification(string title, string content) : title(title), content(content), visible(true) {
		displayTime = time(0);
	}

	void Render();
};