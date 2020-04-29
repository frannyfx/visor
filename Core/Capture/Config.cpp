#include "Config.h"

namespace VisorConfig {
	Instance* Instance::GetInstance() {
		static Instance instance;
		return &instance;
	}

	void Instance::SetConfig(Config c) {
		hasConfig = true;
		config = c;
	}

	bool Instance::HasConfig() {
		return hasConfig;
	}

	Config Instance::GetConfig() {
		return config;
	}
}