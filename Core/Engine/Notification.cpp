#include <iostream>
#include <chrono>
#include "Notification.h"
#include "../Utils/Utils.h"
#include "EngineResources.h"
#include "Fonts.h"

using namespace std;
using namespace std::chrono;

int Notification::GetTimeAlive() {
	// Get time alive
	milliseconds timeAlive = duration_cast<milliseconds>(system_clock::now().time_since_epoch() - displayTime);
	return timeAlive.count();
}

ImVec2 Notification::Render(GraphicsAPI graphicsAPI, int offset) {
	// Get time alive, we will perform animations with it
	int timeAlive = GetTimeAlive();

	// Get style to change alpha
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowBorderSize = 0;
	style.FrameBorderSize = 0;
	style.WindowRounding = 20;
	style.WindowPadding = ImVec2(30, 15);

	// Animations in
	float alpha = NOTIFICATION_MAX_ALPHA;
	int topOffset = NOTIFICATION_DISTANCE + offset;

	if (timeAlive < NOTIFICATION_FADE_DURATION) {
		alpha = Utils::Clamp((float)timeAlive / (float)NOTIFICATION_FADE_DURATION, 0, 1) * NOTIFICATION_MAX_ALPHA;
		topOffset = Utils::Lerp(-1 * NOTIFICATION_SIZE.y + offset, NOTIFICATION_DISTANCE + offset, (float)timeAlive / (float)NOTIFICATION_FADE_DURATION);
	}

	// Animations out
	if (timeAlive > NOTIFICATION_DURATION - NOTIFICATION_FADE_DURATION) {
		alpha = (1 - Utils::Clamp((float)(timeAlive - (NOTIFICATION_DURATION - NOTIFICATION_FADE_DURATION)) / (float)NOTIFICATION_FADE_DURATION, 0, 1)) * NOTIFICATION_MAX_ALPHA;
	}

	// Set alpha
	float previousAlpha = style.Alpha;
	style.Alpha = alpha;

	// Window size to return
	ImVec2 nextOffset = ImVec2(0, NOTIFICATION_SIZE.y + topOffset);

	// Set window position and size
	ImVec2 window_pos = ImVec2(NOTIFICATION_DISTANCE, topOffset);
	ImVec2 window_pos_pivot = ImVec2(0, 0);
	ImGui::SetNextWindowSize(NOTIFICATION_SIZE);
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

	// Set window background colour
	ImGui::PushStyleColor(ImGuiCol_WindowBg,  Utils::GetU32(30, 30, 30, NOTIFICATION_BG_ALPHA * 255));
	
	// Create window
	if (ImGui::Begin(title.c_str(), &visible, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) {
		ImGui::Columns(2, 0, false);
		ImGui::SetColumnWidth(0, NOTIFICATION_ICON_SIZE.x + 20);

		// Render image centered vertically.
		ImGui::SetCursorPosY((NOTIFICATION_SIZE.y - NOTIFICATION_ICON_SIZE.y) / 2);
		EngineResources::RenderTexture(graphicsAPI, TextureID::VISOR_LOGO, NOTIFICATION_ICON_SIZE);

		// Get fonts
		ImFont* bold = Engine::Fonts::GetFont(Engine::Fonts::Font::BOLD);
		ImFont* regular = Engine::Fonts::GetFont(Engine::Fonts::Font::REGULAR);
		
		int textWindowWidth = NOTIFICATION_SIZE.x - NOTIFICATION_ICON_SIZE.x - 60;
		float textHeight = bold->CalcTextSizeA(bold->FontSize, FLT_MAX, textWindowWidth, title.c_str()).y;
		textHeight += regular->CalcTextSizeA(regular->FontSize, FLT_MAX, textWindowWidth, content.c_str()).y;
		textHeight += 5;

		ImGui::NextColumn();
		ImGui::SetCursorPosY((NOTIFICATION_SIZE.y - textHeight) / 2);
		ImGui::PushFont(bold);
		ImGui::TextWrapped(title.c_str());
		ImGui::PopFont();
		ImGui::PushFont(regular);
		ImGui::TextWrapped(content.c_str());
		ImGui::PopFont();
	}

	// Cleanup
	ImGui::PopStyleColor();
	ImGui::End();
	style.Alpha = previousAlpha;

	// Return window size for offset
	return nextOffset;
}