#include <iostream>
#include <string>
#include "../Include/ImGui/imgui.h"
#include "Notification.h"
#include <vector>

using namespace std;

namespace Engine {
	//ImDrawList drawList = ImDrawList(ImGui::GetDrawListSharedData());
	vector<Notification> notifications = {};
	void Render() {
		ImGui::NewFrame();

		for (vector<Notification>::iterator it = notifications.begin(); it != notifications.end();) {
			it->Render();

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