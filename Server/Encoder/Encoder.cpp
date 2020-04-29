#include "Encoder.h"
#include "Instance.h"

namespace Encoder {
	map<int, Instance> encoders;

	void Spawn(VisorClient client) {
		// Don't add an encoder for a PID which already has one
		if (encoders.find(client.pid) != encoders.end())
			return;

		// Create new instance
		Instance clientInstance(client);
		encoders[client.pid] = clientInstance;

		// Start encoder
		clientInstance.Start();
	}
}