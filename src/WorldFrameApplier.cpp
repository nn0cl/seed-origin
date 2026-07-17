#include "WorldFrameApplier.h"

#include <cmath>

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

bool WorldFrameApplier::apply(const WorldFrameInputs& frame,
                              std::vector<network::WorldUpdate>& updates,
                              std::string& error) {
    for (std::vector<WorldInput>::const_iterator it = frame.inputs.begin();
         it != frame.inputs.end(); ++it) {
        if (it->kind() == WorldInputKind::Action) {
            if (!it->action().isValid()) {
                updates.clear();
                error = "world input contains an invalid action";
                return false;
            }
        } else if (it->kind() == WorldInputKind::Movement) {
            if (!field.hasPlayer(it->movement().sessionId)) {
                updates.clear();
                error = "world input movement session is not present in the field";
                return false;
            }
        } else if (it->kind() == WorldInputKind::Combat) {
            if (it->combat().attackerId <= 0 || it->combat().targetId <= 0 ||
                !std::isfinite(it->combat().power) || it->combat().power <= 0.0f) {
                updates.clear();
                error = "world input combat intent is invalid";
                return false;
            }
        } else if (it->spell().casterId <= 0 || it->spell().targetId <= 0 ||
                   it->spell().element.empty() || it->spell().element.size() > 32 ||
                   !std::isfinite(it->spell().power) || it->spell().power <= 0.0f) {
            updates.clear();
            error = "world input spell intent is invalid";
            return false;
        }
    }
    if (!updateBuilder.build(frame, updates, error)) return false;
    if (!field.processInputs(frame.inputs)) {
        updates.clear();
        error = "world input could not be applied in the field";
        return false;
    }
    if (field.environmentEther().hasAdverseEffect() &&
        !updateBuilder.appendHazard(frame.worldTick,
                                    field.environmentHazard(),
                                    field.environmentEther().instability(),
                                    updates, error)) {
        updates.clear();
        return false;
    }
    error.clear();
    return true;
}

}
