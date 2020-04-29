#include "Encoder.h"
#include "Instance.h"

namespace Encoder {
	map<int, Instance> encoders;

	bool Spawn(VisorClient client) {
		// Don't add an encoder for a PID which already has one
		if (encoders.find(client.pid) != encoders.end())
			return false;

		// Create new instance
		Instance clientInstance(client);
		encoders[client.pid] = clientInstance;

		// Start encoder
		return clientInstance.Start();
	}
}