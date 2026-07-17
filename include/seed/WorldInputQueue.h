#ifndef SEED_WORLD_INPUT_QUEUE_H
#define SEED_WORLD_INPUT_QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include <deque>
#include <mutex>
#include <string>
#include <set>
#include <vector>

#include "Action.h"
#include "MovementIntentQueue.h"

namespace server {

enum class WorldInputKind {
    Action,
    Movement,
    Combat,
    Spell,
    Chat
};

struct CombatIntent {
    int64_t attackerId;
    int64_t targetId;
    float power;
    std::string requestId;
};

struct SpellIntent {
    int64_t casterId;
    int64_t targetId;
    std::string element;
    float power;
    std::string requestId;
};

struct ChatIntent {
    int64_t senderId;
    std::string audience;
    std::string message;
};

class WorldInput {
public:
    WorldInput(uint64_t sequence, const Action& action);
    WorldInput(uint64_t sequence, const MovementIntent& movement);
    WorldInput(uint64_t sequence, const CombatIntent& combat);
    WorldInput(uint64_t sequence, const SpellIntent& spell);
    WorldInput(uint64_t sequence, const ChatIntent& chat);

    uint64_t sequence() const;
    WorldInputKind kind() const;
    const Action& action() const;
    const MovementIntent& movement() const;
    const CombatIntent& combat() const;
    const SpellIntent& spell() const;
    const ChatIntent& chat() const;

private:
    uint64_t inputSequence;
    WorldInputKind inputKind;
    Action inputAction;
    MovementIntent inputMovement;
    CombatIntent inputCombat;
    SpellIntent inputSpell;
    ChatIntent inputChat;
};

class WorldInputQueue {
public:
    static const size_t MAX_PENDING_INPUTS = 8192;

    WorldInputQueue();
    bool enqueueAction(const Action& action);
    bool enqueueMovement(int64_t sessionId, float dx, float dy, float dz);
    bool enqueueCombat(int64_t attackerId, int64_t targetId, float power);
    bool enqueueCombat(int64_t attackerId, int64_t targetId, float power,
                       const std::string& requestId);
    bool enqueueSpell(int64_t casterId, int64_t targetId,
                      const std::string& element, float power);
    bool enqueueSpell(int64_t casterId, int64_t targetId,
                      const std::string& element, float power,
                      const std::string& requestId);
    bool enqueueChat(int64_t senderId, const std::string& audience,
                     const std::string& message);
    std::vector<WorldInput> takeFrame();
    void clear();
    size_t pendingCount() const;

private:
    mutable std::mutex mutex;
    std::deque<WorldInput> pending;
    std::set<std::string> acceptedCombatRequests;
    uint64_t nextSequence;
};

}

#endif
