#ifndef SEED_NPC_H
#define SEED_NPC_H

#include <cstdint>
#include <string>

#include "Position.h"
#include "Status.h"

class Npc {
public:
    Npc(std::int64_t npcId, const std::string& npcType,
        const Status& status, const Position& position);

    std::int64_t getNpcId() const;
    const std::string& getNpcType() const;
    const Position& getPosition() const;
    Status& getStatus();
    const Status& getStatus() const;
    bool isAlive() const;
    bool setPosition(const Position& position);
    bool applyDamage(long damage);

private:
    std::int64_t id;
    std::string type;
    Status status;
    Position position;
    bool alive;
};

#endif
