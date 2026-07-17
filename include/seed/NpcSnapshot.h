#ifndef SEED_NPC_SNAPSHOT_H
#define SEED_NPC_SNAPSHOT_H

#include <cstdint>
#include <string>

struct NpcSnapshot {
    std::int64_t id;
    std::string type;
    float x;
    float y;
    float z;
    long hp;
    bool alive;
};

#endif
