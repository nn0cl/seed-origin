#include "WorldFrameUpdateBuilder.h"

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
        const network::WorldUpdate update = {
            network::CURRENT_PROTOCOL_VERSION,
            network::UpdateKind::Event,
            frame.worldTick,
            updateSequence++,
            eventId++,
            payload.str()
        };
        if (!network::validateWorldUpdate(update, error)) {
            updates.clear();
            return false;
        }
        updates.push_back(update);
    }
    error.clear();
    return true;
}

uint64_t WorldFrameUpdateBuilder::nextSequence() const {
    return updateSequence;
}

}
