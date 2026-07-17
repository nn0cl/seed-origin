#include <cassert>

#include "ClientHazardEffectQueue.h"

namespace client_hazard_effect_tests {

network::WorldUpdate hazard(uint64_t sequence, uint64_t eventId) {
    return {1, network::UpdateKind::Event, 8, sequence, eventId,
            "etherHazard=severity:2;instability:3"};
}

void preserves_order_and_drains_by_display_limit() {
    client::ClientHazardEffectQueue queue;
    std::string error;
    assert(queue.ingest(hazard(1, 1), error) == client::HazardIngestResult::Applied);
    assert(queue.ingest(hazard(2, 2), error) == client::HazardIngestResult::Applied);
    std::vector<client::HazardEffect> effects;
    assert(queue.drain(1, effects) == 1);
    assert(effects[0].sequence == 1);
    assert(queue.size() == 1);
}

void rejects_duplicates_gaps_and_non_hazard_events() {
    client::ClientHazardEffectQueue queue;
    std::string error;
    assert(queue.ingest(hazard(1, 1), error) == client::HazardIngestResult::Applied);
    assert(queue.ingest(hazard(1, 1), error) == client::HazardIngestResult::IgnoredDuplicate);
    assert(queue.ingest(hazard(3, 3), error) == client::HazardIngestResult::RequestSnapshot);
    network::WorldUpdate other = hazard(2, 2);
    other.payload = "action=move";
    assert(queue.ingest(other, error) == client::HazardIngestResult::Rejected);
}

} // namespace client_hazard_effect_tests
