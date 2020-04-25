#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../Include/ImGui/imgui.h"
#include "Notification.h"
#include "Engine.h"
#include "EngineResources.h"

using namespace std;

namespace Engine {
	// On-screen elements
	vector<Notification> notifications = {};

	void Render(GraphicsAPI graphicsAPI) {
		// Clear out old resources to prevent memory leaks
		EngineResources::Cleanup();

		// Begin new frame
		ImGui::NewFrame();

		for (vector<Notification>::iterator it = notifications.begin(); it != notifications.end();) {
			it->Render(graphicsAPI);

			// Remove notifications and only increment when the notification still has time to live.
			if (it->GetTimeAlive() > it->NOTIFICATION_DURATION) {
				it = notifications.erase(it);
			}
			else {
				++it;
			}
		}

		ImGui::Render();
	}

	void ShowNotification(const string& title, const string& content) {
		notifications.push_back(Notification(title, content));
	}
}