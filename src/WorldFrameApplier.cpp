#include "WorldFrameApplier.h"

namespace server {

WorldFrameApplier::WorldFrameApplier(Field& field)
    : field(field), movementQueue(nullptr), updateBuilder() {}

WorldFrameApplier::WorldFrameApplier(Field& field, MovementIntentQueue& movementQueue)
    : field(field), movementQueue(&movementQueue), updateBuilder() {}

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
    std::vector<MovementIntent> intents;
    if (movementQueue != nullptr) {
        intents = movementQueue->takeFrame();
        for (std::vector<MovementIntent>::const_iterator it = intents.begin();
             it != intents.end(); ++it) {
            if (!field.hasPlayer(it->sessionId)) {
                movementQueue->restoreFrame(intents);
                updates.clear();
                error = "movement intent session is not present in the field";
                return false;
            }
        }
    }
    if (!updateBuilder.build(frame, updates, error)) {
        if (movementQueue != nullptr) movementQueue->restoreFrame(intents);
        return false;
    }

    if (movementQueue != nullptr) {
        for (std::vector<MovementIntent>::const_iterator it = intents.begin();
             it != intents.end(); ++it) {
            if (!field.queueMovement(it->sessionId, it->dx, it->dy, it->dz)) {
                movementQueue->restoreFrame(intents);
                updates.clear();
                error = "movement intent could not be queued in the field";
                return false;
            }
        }
    }

    for (std::vector<QueuedAction>::const_iterator it = frame.actions.begin();
         it != frame.actions.end(); ++it) {
        field.putActionQueue(it->getAction());
    }
    field.processFrame();
    error.clear();
    return true;
}

}
