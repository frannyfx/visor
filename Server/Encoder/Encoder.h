#pragma once
#include "../Server/VisorClient.h"

namespace Encoder {
	bool Spawn(VisorClient client);
	void EncodeFrame(VisorClient client);
}