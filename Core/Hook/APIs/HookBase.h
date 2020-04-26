#pragma once
#include <mutex>
#include <condition_variable>

using namespace std;

namespace Hook {
	class HookBase {
	protected:
		bool installed;
		bool success;
		mutex installationMutex;
		condition_variable installedCV;
		void WaitForInstall();
	public:
		void Install();
		void Uninstall();
	};
}