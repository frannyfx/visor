#pragma once
#include <Protocols/server.pb.h>

namespace VisorConfig {
	class Instance {
	private:
		// Singleton
		static Instance* instance;
		Instance() : hasConfig(false) {};

		Config config;
		bool hasConfig;

	public:
		static Instance* GetInstance();
		bool HasConfig();
		Config GetConfig();
		void SetConfig(Config config);
	};
}