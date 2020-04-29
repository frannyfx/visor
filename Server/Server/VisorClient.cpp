#include "VisorClient.h"

void VisorClient::SetRecording(bool recording) {
	isRecording = recording;
}

void VisorClient::SetSourceResolution(int width, int height) {
	sourceWidth = width;
	sourceHeight = height;
}

int VisorClient::GetWidth() {
	return sourceWidth;
}

int VisorClient::GetHeight() {
	return sourceHeight;
}