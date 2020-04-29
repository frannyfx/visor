#pragma once
#include "../Server/VisorClient.h"

namespace Encoder {
	void Spawn(VisorClient client);
	void EncodeFrame(VisorClient client);
}