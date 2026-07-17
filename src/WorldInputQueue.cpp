#include "WorldInputQueue.h"

#include <cctype>
#include <cmath>
#include <limits>

namespace server {

WorldInput::WorldInput(uint64_t sequence, const Action& action)
    : inputSequence(sequence), inputKind(WorldInputKind::Action), inputAction(action),
      inputMovement({0, 0, 0.0f, 0.0f, 0.0f}), inputCombat({0, 0, 0.0f}),
      inputSpell({0, 0, std::string(), 0.0f}), inputChat({0, std::string(), std::string()}) {}

WorldInput::WorldInput(uint64_t sequence, const MovementIntent& movement)
    : inputSequence(sequence), inputKind(WorldInputKind::Movement), inputAction(0, nullptr, nullptr, Status()),
      inputMovement(movement), inputCombat({0, 0, 0.0f}),
      inputSpell({0, 0, std::string(), 0.0f}), inputChat({0, std::string(), std::string()}) {}

WorldInput::WorldInput(uint64_t sequence, const CombatIntent& combat)
    : inputSequence(sequence), inputKind(WorldInputKind::Combat), inputAction(0, nullptr, nullptr, Status()),
      inputMovement({0, 0, 0.0f, 0.0f, 0.0f}), inputCombat(combat),
      inputSpell({0, 0, std::string(), 0.0f}), inputChat({0, std::string(), std::string()}) {}

WorldInput::WorldInput(uint64_t sequence, const ChatIntent& chat)
    : inputSequence(sequence), inputKind(WorldInputKind::Chat),
      inputAction(0, nullptr, nullptr, Status()),
      inputMovement({0, 0, 0.0f, 0.0f, 0.0f}), inputCombat({0, 0, 0.0f}),
      inputSpell({0, 0, std::string(), 0.0f}), inputChat(chat) {}

WorldInput::WorldInput(uint64_t sequence, const SpellIntent& spell)
    : inputSequence(sequence), inputKind(WorldInputKind::Spell), inputAction(0, nullptr, nullptr, Status()),
      inputMovement({0, 0, 0.0f, 0.0f, 0.0f}), inputCombat({0, 0, 0.0f}),
      inputSpell(spell), inputChat({0, std::string(), std::string()}) {}

uint64_t WorldInput::sequence() const { return inputSequence; }
WorldInputKind WorldInput::kind() const { return inputKind; }
const Action& WorldInput::action() const { return inputAction; }
const MovementIntent& WorldInput::movement() const { return inputMovement; }
const CombatIntent& WorldInput::combat() const { return inputCombat; }
const SpellIntent& WorldInput::spell() const { return inputSpell; }
const ChatIntent& WorldInput::chat() const { return inputChat; }

WorldInputQueue::WorldInputQueue() : nextSequence(1) {}

bool WorldInputQueue::enqueueAction(const Action& action) {
    if (!action.isValid()) return false;
    std::lock_guard<std::mutex> lock(mutex);
    if (pending.size() >= MAX_PENDING_INPUTS ||
        nextSequence == std::numeric_limits<uint64_t>::max()) return false;
    pending.push_back(WorldInput(nextSequence++, action));
    return true;
}

bool WorldInputQueue::enqueueMovement(int64_t sessionId, float dx, float dy, float dz) {
    if (sessionId <= 0 || !isValidMovementDelta(dx, dy, dz)) return false;
    std::lock_guard<std::mutex> lock(mutex);
    if (pending.size() >= MAX_PENDING_INPUTS ||
        nextSequence == std::numeric_limits<uint64_t>::max()) return false;
    pending.push_back(WorldInput(nextSequence++, {0, sessionId, dx, dy, dz}));
    return true;
}

bool WorldInputQueue::enqueueCombat(int64_t attackerId, int64_t targetId, float power) {
    if (attackerId <= 0 || targetId <= 0 || !std::isfinite(power) || power <= 0.0f) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex);
    if (pending.size() >= MAX_PENDING_INPUTS ||
        nextSequence == std::numeric_limits<uint64_t>::max()) return false;
    pending.push_back(WorldInput(nextSequence++, {attackerId, targetId, power}));
    return true;
}

bool WorldInputQueue::enqueueSpell(int64_t casterId, int64_t targetId,
                                   const std::string& element, float power) {
    if (casterId <= 0 || targetId <= 0 || element.empty() || element.size() > 32 ||
        !std::isfinite(power) || power <= 0.0f) return false;
    for (std::string::const_iterator it = element.begin(); it != element.end(); ++it) {
        if (!std::isalnum(static_cast<unsigned char>(*it)) && *it != '_' && *it != '-') {
            return false;
        }
    }
    std::lock_guard<std::mutex> lock(mutex);
    if (pending.size() >= MAX_PENDING_INPUTS ||
        nextSequence == std::numeric_limits<uint64_t>::max()) return false;
    pending.push_back(WorldInput(nextSequence++, {casterId, targetId, element, power}));
    return true;
}

bool WorldInputQueue::enqueueChat(int64_t senderId, const std::string& audience,
                                  const std::string& message) {
    if (senderId <= 0 || audience.empty() || audience.size() > 16 ||
        message.empty() || message.size() > 512) return false;
    for (std::string::const_iterator it = audience.begin(); it != audience.end(); ++it) {
        if (!std::isalnum(static_cast<unsigned char>(*it)) && *it != '_' && *it != '-')
            return false;
    }
    for (std::string::const_iterator it = message.begin(); it != message.end(); ++it) {
        if (static_cast<unsigned char>(*it) < 0x20 || *it == '\x7f') return false;
    }
    std::lock_guard<std::mutex> lock(mutex);
    if (pending.size() >= MAX_PENDING_INPUTS ||
        nextSequence == std::numeric_limits<uint64_t>::max()) return false;
    pending.push_back(WorldInput(nextSequence++, {senderId, audience, message}));
    return true;
}

std::vector<WorldInput> WorldInputQueue::takeFrame() {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<WorldInput> frame;
    frame.reserve(pending.size());
    while (!pending.empty()) {
        frame.push_back(pending.front());
        pending.pop_front();
    }
    return frame;
}

void WorldInputQueue::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    pending.clear();
}

size_t WorldInputQueue::pendingCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    return pending.size();
}

}
