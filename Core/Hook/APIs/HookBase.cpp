#pragma once
#include "HookBase.h"
#include <iostream>

using namespace std;

void Hook::HookBase::WaitForInstall() {
	unique_lock lock(installationMutex);
	installedCV.wait_for(lock, 500ms, [this] { return installed; });
}

bool Hook::HookBase::IsEnabled() {
	return enabled;
}

bool Hook::HookBase::IsInstalled() {
	return installed;
}

bool Hook::HookBase::IsSuccessful() {
	return success;
}

void Hook::HookBase::FinishInstall(bool s) {
	installed = true;
	success = s;
	installedCV.notify_all();
}