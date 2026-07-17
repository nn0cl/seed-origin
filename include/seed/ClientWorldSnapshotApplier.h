#ifndef SEED_CLIENT_WORLD_SNAPSHOT_APPLIER_H
#define SEED_CLIENT_WORLD_SNAPSHOT_APPLIER_H

#include <string>

#include "ClientEnvironmentState.h"
#include "WorldUpdate.h"
#include "WorldUpdateSync.h"

namespace client {

class ClientWorldSnapshotApplier {
public:
    ClientWorldSnapshotApplier();

    bool applySnapshot(const network::WorldUpdate& update,
                       ClientEnvironmentState& state, std::string& error);
    const network::WorldUpdateSyncController& synchronization() const;

private:
    network::WorldUpdateSyncController sync;
};

}

#endif
