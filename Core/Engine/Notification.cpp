#include <iostream>
#include "Notification.h"
#include "../Include/ImGui/imgui.h"
using namespace std;

void Notification::Render() {
	ImVec2 window_pos = ImVec2(NOTIFICATION_DISTANCE, NOTIFICATION_DISTANCE);
	ImVec2 window_pos_pivot = ImVec2(0, 0);
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowBgAlpha(NOTIFICATION_BG_ALPHA);
	if (ImGui::Begin(title.c_str(), &visible, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) {
		ImGui::Text(title.c_str());
		ImGui::Text(content.c_str());
	}

	ImGui::End();
}