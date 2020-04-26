#pragma once
#include "HookBase.h"
#include <iostream>

using namespace std;

void Hook::HookBase::WaitForInstall() {
	unique_lock lock(installationMutex);
	installedCV.wait_for(lock, 500ms, [this] {return installed; });
	cout << "Done waiting!" << endl;
}

bool Hook::HookBase::IsInstalled() {
	return installed;
}

void Hook::HookBase::FinishInstall(bool s) {
	installed = true;
	success = s;
	installedCV.notify_all();
}