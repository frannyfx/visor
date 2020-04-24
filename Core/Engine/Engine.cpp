#include <iostream>
#include <string>
#include "../Include/ImGui/imgui.h"
#include "Notification.h"
#include <vector>

using namespace std;

namespace Engine {
	vector<Notification> notifications = {};
	void Render() {
		ImGui::NewFrame();

		for (auto &notification : notifications) {
			notification.Render();
		}

		ImGui::Render();
	}

	void ShowNotification(const string& title, const string& content) {
		notifications.push_back(Notification(title, content));
	}
}