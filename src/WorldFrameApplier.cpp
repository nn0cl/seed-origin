#include "WorldFrameApplier.h"

#include <cmath>

namespace server {

WorldFrameApplier::WorldFrameApplier(Field& field)
    : field(field), movementQueue(nullptr), npcAiInputQueue(nullptr), updateBuilder() {}

WorldFrameApplier::WorldFrameApplier(Field& field, MovementIntentQueue& movementQueue)
    : field(field), movementQueue(&movementQueue), npcAiInputQueue(nullptr), updateBuilder() {}

WorldFrameApplier::WorldFrameApplier(Field& field, NpcAiInputQueue& npcAiInputQueue)
    : field(field), movementQueue(nullptr), npcAiInputQueue(&npcAiInputQueue),
      updateBuilder() {}

WorldFrameApplier::WorldFrameApplier(Field& field,
                                     MovementIntentQueue& movementQueue,
                                     NpcAiInputQueue& npcAiInputQueue)
    : field(field), movementQueue(&movementQueue),
      npcAiInputQueue(&npcAiInputQueue), updateBuilder() {}

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
    std::vector<NpcAiInput> npcInputs;
    if (npcAiInputQueue != nullptr) {
        npcInputs = npcAiInputQueue->takeFrame();
        for (std::vector<NpcAiInput>::const_iterator it = npcInputs.begin();
             it != npcInputs.end(); ++it) {
            const Npc* npc = field.findNpc(it->decision.npcId);
            if (npc == nullptr || !npc->isAlive() ||
                !isValidMovementDelta(it->decision.dx, it->decision.dy,
                                      it->decision.dz)) {
                if (movementQueue != nullptr) movementQueue->restoreFrame(intents);
                npcAiInputQueue->restoreFrame(npcInputs);
                updates.clear();
                error = "NPC AI input is invalid for the world frame";
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
                if (npcAiInputQueue != nullptr) npcAiInputQueue->restoreFrame(npcInputs);
                updates.clear();
                error = "movement intent could not be queued in the field";
                return false;
            }
        }
    }

    if (npcAiInputQueue != nullptr) {
        for (std::vector<NpcAiInput>::const_iterator it = npcInputs.begin();
             it != npcInputs.end(); ++it) {
            if (!field.queueNpcMovement(it->decision.npcId, it->decision.dx,
                                        it->decision.dy, it->decision.dz)) {
                if (movementQueue != nullptr) movementQueue->restoreFrame(intents);
                npcAiInputQueue->restoreFrame(npcInputs);
                updates.clear();
                error = "NPC AI input could not be queued in the field";
                return false;
            }
        }
    }

    for (std::vector<QueuedAction>::const_iterator it = frame.actions.begin();
         it != frame.actions.end(); ++it) {
        field.putActionQueue(it->getAction());
    }
    field.processFrame(frame.worldTick);
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
        } else if (it->kind() == WorldInputKind::Spell &&
                   (it->spell().casterId <= 0 || it->spell().targetId <= 0 ||
                   it->spell().element.empty() || it->spell().element.size() > 32 ||
                   !std::isfinite(it->spell().power) || it->spell().power <= 0.0f)) {
            updates.clear();
            error = "world input spell intent is invalid";
            return false;
        } else if (it->kind() == WorldInputKind::Chat &&
                   (it->chat().senderId <= 0 || it->chat().audience.empty() ||
                    it->chat().message.empty())) {
            updates.clear();
            error = "world input chat intent is invalid";
            return false;
        }
    }
    if (!updateBuilder.build(frame, updates, error)) return false;
    std::vector<CombatResolution> resolutions;
    if (!field.processInputs(frame.inputs, resolutions, frame.worldTick)) {
        updates.clear();
        error = "world input could not be applied in the field";
        return false;
    }
    for (std::vector<CombatResolution>::const_iterator it = resolutions.begin();
         it != resolutions.end(); ++it) {
        if (!updateBuilder.appendCombatResolution(frame.worldTick, *it,
                                                   updates, error)) {
            updates.clear();
            return false;
        }
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
