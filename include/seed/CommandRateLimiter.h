#ifndef SEED_COMMAND_RATE_LIMITER_H
#define SEED_COMMAND_RATE_LIMITER_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <map>

#include "NetworkCommand.h"

namespace server {

class CommandRateLimiter {
public:
    bool allow(int64_t sessionId, network::CommandType type);
    void beginFrame(uint64_t worldTick);
    void forgetSession(int64_t sessionId);
    void clear();

private:
    struct SessionCounters {
        std::array<std::size_t, 7> counts{};
    };

    static std::size_t commandIndex(network::CommandType type);
    static std::size_t limitFor(network::CommandType type);

    std::map<int64_t, SessionCounters> counters;
    uint64_t currentTick = 0;
};

}

#endif
