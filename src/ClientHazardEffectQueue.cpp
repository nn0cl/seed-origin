#include "ClientHazardEffectQueue.h"

#include <cmath>
#include <sstream>

namespace client {
namespace {

bool parseFinite(const std::string& text, float& value) {
    std::istringstream input(text);
    input >> value;
    std::string trailing;
    return static_cast<bool>(input) && !(input >> trailing) &&
           std::isfinite(value);
}

bool parseHazard(const std::string& payload, float& severity,
                 float& instability, std::string& error) {
    const std::string prefix = "etherHazard=";
    const std::string first = "severity:";
    const std::string second = ";instability:";
    if (payload.compare(0, prefix.size(), prefix) != 0) {
        error = "world update is not an ether hazard event";
        return false;
    }
    const std::string fields = payload.substr(prefix.size());
    if (fields.compare(0, first.size(), first) != 0) {
        error = "hazard event is missing severity";
        return false;
    }
    const std::string::size_type separator = fields.find(second, first.size());
    if (separator == std::string::npos ||
        fields.find(';', separator + second.size()) != std::string::npos ||
        !parseFinite(fields.substr(first.size(), separator - first.size()),
                     severity) ||
        !parseFinite(fields.substr(separator + second.size()), instability) ||
        severity <= 0.0f || instability <= 1.0f) {
        error = "hazard event contains invalid fields";
        return false;
    }
    return true;
}

}

ClientHazardEffectQueue::ClientHazardEffectQueue() : effects(), sync() {}

HazardIngestResult ClientHazardEffectQueue::ingest(
    const network::WorldUpdate& update, std::string& error) {
    if (update.kind != network::UpdateKind::Event || update.eventId == 0 ||
        !network::validateWorldUpdate(update, error)) {
        if (error.empty()) error = "invalid WorldUpdate Event";
        return HazardIngestResult::Rejected;
    }
    float severity = 0.0f;
    float instability = 0.0f;
    if (!parseHazard(update.payload, severity, instability, error)) {
        return HazardIngestResult::Rejected;
    }
    const network::SyncResult result = sync.receive(update);
    if (result.decision == network::SyncDecision::IgnoreDuplicate)
        return HazardIngestResult::IgnoredDuplicate;
    if (result.decision == network::SyncDecision::RequestSnapshot)
        return HazardIngestResult::RequestSnapshot;
    if (result.decision != network::SyncDecision::Apply)
        return HazardIngestResult::Rejected;
    if (effects.size() >= MAX_HAZARD_EFFECTS) {
        error = "hazard effect queue is full";
        return HazardIngestResult::Backpressure;
    }
    effects.push_back({update.worldTick, update.sequence, update.eventId,
                       severity, instability});
    error.clear();
    return HazardIngestResult::Applied;
}

size_t ClientHazardEffectQueue::drain(
    size_t limit, std::vector<HazardEffect>& output) {
    output.clear();
    const size_t count = limit < effects.size() ? limit : effects.size();
    output.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        output.push_back(effects.front());
        effects.pop_front();
    }
    return count;
}

size_t ClientHazardEffectQueue::size() const { return effects.size(); }

const network::WorldUpdateSyncController&
ClientHazardEffectQueue::synchronization() const { return sync; }

}
