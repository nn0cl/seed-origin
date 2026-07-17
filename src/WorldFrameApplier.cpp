#include "WorldFrameApplier.h"

namespace server {

WorldFrameApplier::WorldFrameApplier(Field& field)
    : field(field), updateBuilder() {}

bool WorldFrameApplier::apply(const FrameActions& frame,
                              std::vector<network::WorldUpdate>& updates,
                              std::string& error) {
    for (std::vector<QueuedAction>::const_iterator it = frame.actions.begin();
         it != frame.actions.end(); ++it) {
        if (!it->getAction().isValid()) {
            updates.clear();
            error = "world frame contains an invalid action";
            return false;
        }
    }
    if (!updateBuilder.build(frame, updates, error)) return false;

    for (std::vector<QueuedAction>::const_iterator it = frame.actions.begin();
         it != frame.actions.end(); ++it) {
        field.putActionQueue(it->getAction());
    }
    field.processFrame();
    error.clear();
    return true;
}

}
