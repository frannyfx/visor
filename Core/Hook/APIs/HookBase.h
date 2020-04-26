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

		void FinishInstall(bool s);
		
		HookBase() : installed(false), success(false) {}

	public:
		virtual void Install() = 0;
		virtual void Enable() = 0;
		virtual void Disable() = 0;
		virtual void Uninstall() = 0;
		void WaitForInstall();
		bool IsInstalled();
	};
}