#include "CommandRateLimiter.h"

namespace server {

std::size_t CommandRateLimiter::commandIndex(network::CommandType type) {
    return static_cast<std::size_t>(type);
}

std::size_t CommandRateLimiter::limitFor(network::CommandType type) {
    switch (type) {
        case network::CommandType::Move: return 20;
        case network::CommandType::Chat: return 4;
        case network::CommandType::Attack: return 4;
        case network::CommandType::CastSpell: return 4;
        case network::CommandType::Disconnect: return 2;
        case network::CommandType::Login: return 0;
    }
    return 0;
}

bool CommandRateLimiter::allow(int64_t sessionId, network::CommandType type) {
    if (sessionId <= 0) return false;
    const std::size_t index = commandIndex(type);
    const std::size_t limit = limitFor(type);
    if (index >= SessionCounters{}.counts.size() || limit == 0) return false;
    SessionCounters& session = counters[sessionId];
    if (session.counts[index] >= limit) return false;
    ++session.counts[index];
    return true;
}

void CommandRateLimiter::beginFrame(uint64_t worldTick) {
    if (worldTick == currentTick) return;
    currentTick = worldTick;
    for (std::map<int64_t, SessionCounters>::iterator it = counters.begin();
         it != counters.end(); ++it) {
        it->second.counts.fill(0);
    }
}

void CommandRateLimiter::forgetSession(int64_t sessionId) {
    counters.erase(sessionId);
}

void CommandRateLimiter::clear() {
    counters.clear();
    currentTick = 0;
}

}
