#ifndef SEED_WORLD_UPDATE_H
#define SEED_WORLD_UPDATE_H

#include <stddef.h>
#include <stdint.h>
#include <string>

namespace network {

static const size_t MAX_UPDATE_PAYLOAD = 16384;

enum class UpdateKind {
    Snapshot = 1,
    Event = 2
};

struct WorldUpdate {
    uint16_t version;
    UpdateKind kind;
    uint64_t worldTick;
    uint64_t sequence;
    uint64_t eventId;
    std::string payload;
};

bool validateWorldUpdate(const WorldUpdate& update, std::string& error);

}

#endif
