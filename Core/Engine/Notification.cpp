#include <iostream>
#include <chrono>
#include "Notification.h"
#include "../Include/ImGui/imgui.h"
#include "../Utils/Utils.h"
#include "EngineResources.h"

using namespace std;
using namespace std::chrono;

int Notification::GetTimeAlive() {
	// Get time alive
	milliseconds timeAlive = duration_cast<milliseconds>(system_clock::now().time_since_epoch() - displayTime);
	return timeAlive.count();
}

void Notification::Render(GraphicsAPI graphicsAPI) {
	// Get time alive, we will perform animations with it
	int timeAlive = GetTimeAlive();

	// Get style to change alpha
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowBorderSize = 0;
	style.FrameBorderSize = 0;
	style.WindowRounding = 20;
	style.WindowPadding = ImVec2(30, 15);

	// Fade in
	float alpha = NOTIFICATION_MAX_ALPHA;
	if (timeAlive < NOTIFICATION_FADE_DURATION) {
		alpha = Utils::Clamp((float)timeAlive / (float)NOTIFICATION_FADE_DURATION, 0, 1) * NOTIFICATION_MAX_ALPHA;
	}

	// Fade out
	if (timeAlive > NOTIFICATION_DURATION - NOTIFICATION_FADE_DURATION) {
		alpha = (1 - Utils::Clamp((float)(timeAlive - (NOTIFICATION_DURATION - NOTIFICATION_FADE_DURATION)) / (float)NOTIFICATION_FADE_DURATION, 0, 1)) * NOTIFICATION_MAX_ALPHA;
	}

	// Set alpha
	float previousAlpha = style.Alpha;
	style.Alpha = alpha;

	ImVec2 window_pos = ImVec2(NOTIFICATION_DISTANCE, NOTIFICATION_DISTANCE);
	ImVec2 window_pos_pivot = ImVec2(0, 0);
	ImGui::PushStyleColor(ImGuiCol_WindowBg,  Utils::GetU32(30, 30, 30, NOTIFICATION_BG_ALPHA * 255));
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	if (ImGui::Begin(title.c_str(), &visible, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) {
		EngineResources::RenderTexture(graphicsAPI, TextureID::VISOR_LOGO, ImVec2(40, 40));
		ImGui::SameLine();
		ImGui::Text(title.c_str());
	}

	ImGui::PopStyleColor();
	ImGui::End();

	// Reset alpha
	style.Alpha = previousAlpha;
}