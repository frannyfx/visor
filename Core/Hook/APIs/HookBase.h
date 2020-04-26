#pragma once
#include <mutex>
#include <condition_variable>

using namespace std;

namespace Hook {
	class HookBase {
	protected:
		bool installed;
		bool success;
		bool enabled;
		mutex installationMutex;
		condition_variable installedCV;
		
		HookBase() : installed(false), success(false), enabled(false) {}

	public:
		virtual void Install() = 0;
		virtual void Enable() = 0;
		virtual void Disable() = 0;
		virtual void Uninstall() = 0;
		void FinishInstall(bool s);
		void WaitForInstall();
		bool IsEnabled();
		bool IsInstalled();
		bool IsSuccessful();
	};
}