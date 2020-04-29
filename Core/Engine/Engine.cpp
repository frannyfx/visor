#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../Include/ImGui/imgui.h"
#include "Notification.h"
#include "Engine.h"
#include "EngineResources.h"
#include "Fonts.h"

using namespace std;

namespace Engine {
	// On-screen elements
	vector<Notification> notifications = {};

	void Initialise() {
		// Setup ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();

		// Setup fonts
		Fonts::RegisterFont(Fonts::Font::BOLD, io.Fonts->AddFontFromFileTTF("C:\\Users\\blazi\\Desktop\\Visor\\Build\\Debug\\x64\\Fonts\\Rubik-Bold.ttf", 16.0f));
		Fonts::RegisterFont(Fonts::Font::REGULAR, io.Fonts->AddFontFromFileTTF("C:\\Users\\blazi\\Desktop\\Visor\\Build\\Debug\\x64\\Fonts\\Rubik-Regular.ttf", 14.0f));
	}

	void Render(GraphicsAPI graphicsAPI) {
		// Clear out old resources to prevent memory leaks
		EngineResources::Cleanup();

		// Begin new frame
		ImGui::NewFrame();

		// Offset
		int notificationOffset = 0;
		for (vector<Notification>::reverse_iterator it = notifications.rbegin(); it != notifications.rend(); it++) {
			ImVec2 offset = it->Render(graphicsAPI, notificationOffset);
			notificationOffset = offset.y;
		}

		for (vector<Notification>::iterator it = notifications.begin(); it != notifications.end();) {
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