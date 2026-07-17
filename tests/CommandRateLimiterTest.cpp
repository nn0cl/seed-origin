#include <cassert>

#include "CommandRateLimiter.h"

namespace command_rate_limiter_tests {

void limits_commands_per_session_and_kind_per_world_frame() {
    server::CommandRateLimiter limiter;
    limiter.beginFrame(1);
    for (int i = 0; i < 4; ++i) {
        assert(limiter.allow(19701, network::CommandType::Chat));
    }
    assert(!limiter.allow(19701, network::CommandType::Chat));
    assert(limiter.allow(19701, network::CommandType::Attack));
    limiter.beginFrame(2);
    assert(limiter.allow(19701, network::CommandType::Chat));
}

void keeps_sessions_isolated_and_releases_counters() {
    server::CommandRateLimiter limiter;
    limiter.beginFrame(1);
    for (int i = 0; i < 4; ++i) {
        assert(limiter.allow(19702, network::CommandType::Chat));
    }
    assert(limiter.allow(19703, network::CommandType::Chat));
    limiter.forgetSession(19702);
    assert(limiter.allow(19702, network::CommandType::Chat));
}

} // namespace command_rate_limiter_tests
