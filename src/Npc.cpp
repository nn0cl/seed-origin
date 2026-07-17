#include "Npc.h"

Npc::Npc(std::int64_t npcId, const std::string& npcType,
         const Status& npcStatus, const Position& npcPosition)
    : id(npcId), type(npcType), status(npcStatus), position(npcPosition),
      alive(npcStatus.getHp() > 0) {}

std::int64_t Npc::getNpcId() const {
    return id;
}

const std::string& Npc::getNpcType() const {
    return type;
}

const Position& Npc::getPosition() const {
    return position;
}

Status& Npc::getStatus() {
    return status;
}

const Status& Npc::getStatus() const {
    return status;
}

bool Npc::isAlive() const {
    return alive && status.getHp() > 0;
}

bool Npc::setPosition(const Position& newPosition) {
    position.setPosition(newPosition.getX(), newPosition.getY(),
                         newPosition.getZ());
    return true;
}

bool Npc::applyDamage(long damage) {
    if (!isAlive() || damage <= 0) return false;
    status.gainHp(-damage);
    if (status.getHp() == 0) alive = false;
    return true;
}

bool Npc::respawn(const Status& newStatus, const Position& newPosition) {
    if (newStatus.getHp() <= 0) return false;
    status = newStatus;
    setPosition(newPosition);
    alive = true;
    return true;
}
