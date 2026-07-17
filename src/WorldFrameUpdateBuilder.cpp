#include "WorldFrameUpdateBuilder.h"

#include <cmath>
#include <limits>
#include <sstream>

namespace server {

WorldFrameUpdateBuilder::WorldFrameUpdateBuilder()
    : updateSequence(1), eventId(1) {}

bool WorldFrameUpdateBuilder::build(const FrameActions& frame,
                                    std::vector<network::WorldUpdate>& updates,
                                    std::string& error) {
    updates.clear();
    if (frame.actions.size() > network::MAX_UPDATE_PAYLOAD) {
        error = "world frame contains too many actions";
        return false;
    }
    if (frame.actions.size() > std::numeric_limits<size_t>::max() - updates.size()) {
        error = "world update count overflow";
        return false;
    }
    if (frame.actions.size() >= std::numeric_limits<uint64_t>::max() - updateSequence ||
        frame.actions.size() >= std::numeric_limits<uint64_t>::max() - eventId) {
        error = "world update identity exhausted";
        return false;
    }

    updates.reserve(frame.actions.size());
    for (std::vector<QueuedAction>::const_iterator it = frame.actions.begin();
         it != frame.actions.end(); ++it) {
        std::ostringstream payload;
        payload << "action=" << it->getAction().getActionType()
                << ";actionSequence=" << it->getSequence();
        if (!appendEvent(frame.worldTick, payload.str(), updates, error)) return false;
    }
    error.clear();
    return true;
}

bool WorldFrameUpdateBuilder::build(const WorldFrameInputs& frame,
                                    std::vector<network::WorldUpdate>& updates,
                                    std::string& error) {
    updates.clear();
    if (frame.inputs.size() > network::MAX_UPDATE_PAYLOAD) {
        error = "world frame contains too many inputs";
        return false;
    }
    updates.reserve(frame.inputs.size());
    for (std::vector<WorldInput>::const_iterator it = frame.inputs.begin();
         it != frame.inputs.end(); ++it) {
        std::ostringstream payload;
        if (it->kind() == WorldInputKind::Action) {
            payload << "action=" << it->action().getActionType();
        } else if (it->kind() == WorldInputKind::Movement) {
            payload << "movement=session:" << it->movement().sessionId
                    << ";dx=" << it->movement().dx
                    << ";dy=" << it->movement().dy
                    << ";dz=" << it->movement().dz;
        } else if (it->kind() == WorldInputKind::Combat) {
            payload << "combat=attacker:" << it->combat().attackerId
                    << ";target:" << it->combat().targetId
                    << ";power:" << it->combat().power;
        } else if (it->kind() == WorldInputKind::Spell) {
            payload << "spell=caster:" << it->spell().casterId
                    << ";target:" << it->spell().targetId
                    << ";element:" << it->spell().element
                    << ";power:" << it->spell().power;
        } else {
            payload << "chat=sender:" << it->chat().senderId
                    << ";audience:" << it->chat().audience
                    << ";message:" << it->chat().message;
        }
        payload << ";inputSequence=" << it->sequence();
        if (!appendEvent(frame.worldTick, payload.str(), updates, error)) return false;
    }
    error.clear();
    return true;
}

bool WorldFrameUpdateBuilder::appendHazard(
    uint64_t worldTick, float severity, float instability,
    std::vector<network::WorldUpdate>& updates, std::string& error) {
    if (!std::isfinite(severity) || !std::isfinite(instability) ||
        severity <= 0.0f || instability <= 1.0f) {
        error = "world hazard is invalid";
        return false;
    }
    std::ostringstream payload;
    payload << "etherHazard=severity:" << severity
            << ";instability:" << instability;
    return appendEvent(worldTick, payload.str(), updates, error);
}

bool WorldFrameUpdateBuilder::appendEvent(uint64_t worldTick,
                                          const std::string& payload,
                                          std::vector<network::WorldUpdate>& updates,
                                          std::string& error) {
    if (updateSequence == std::numeric_limits<uint64_t>::max() ||
        eventId == std::numeric_limits<uint64_t>::max()) {
        error = "world update identity exhausted";
        updates.clear();
        return false;
    }
    const network::WorldUpdate update = {
        network::CURRENT_PROTOCOL_VERSION,
        network::UpdateKind::Event,
        worldTick,
        updateSequence++,
        eventId++,
        payload
    };
    if (!network::validateWorldUpdate(update, error)) {
        updates.clear();
        return false;
    }
    updates.push_back(update);
    return true;
}

uint64_t WorldFrameUpdateBuilder::nextSequence() const {
    return updateSequence;
}

}
