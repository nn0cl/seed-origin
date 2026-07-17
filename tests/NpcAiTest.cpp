#include <cassert>

#include "NpcAi.h"
#include "WorldFrameApplier.h"

namespace npc_ai_tests {

void produces_a_deterministic_fixed_step_decision() {
    const server::NpcAiObservation observation = {
        19601, true, Position(19601, 0, 0, 0), Position(19602, 10, 4, 0), 2.0f};
    server::NpcAiDecision first = {0, 0, 0, 0};
    server::NpcAiDecision second = {0, 0, 0, 0};
    assert(server::NpcAiPlanner::plan(observation, first));
    assert(server::NpcAiPlanner::plan(observation, second));
    assert(first.npcId == 19601 && first.dx == 2.0f && first.dy == 0.0f);
    assert(first.dx == second.dx && first.dy == second.dy && first.dz == second.dz);
}

void queues_ai_decisions_without_mutating_the_field() {
    server::NpcAiInputQueue queue;
    const server::NpcAiDecision decision = {19602, 1.0f, 0.0f, 0.0f};
    assert(queue.enqueue(decision));
    assert(queue.pendingCount() == 1);
    const std::vector<server::NpcAiInput> inputs = queue.takeFrame();
    assert(inputs.size() == 1 && inputs[0].sequence == 1);
    assert(inputs[0].decision.npcId == 19602);
}

void applies_ai_input_only_through_the_world_frame() {
    Field* field = Field::getInstance();
    assert(field->setNpc(Npc(19603, "goblin", Status(80, 0),
                             Position(19603, 0, 0, 0))));
    server::NpcAiInputQueue queue;
    assert(queue.enqueue({19603, 3.0f, 0.0f, 0.0f}));
    server::WorldFrameApplier applier(*field, queue);
    ActionQueue actions;
    ServerTick tick(actions);
    const FrameActions frame = tick.advanceFrame();
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(applier.apply(frame, updates, error));
    assert(field->findNpc(19603)->getPosition().getX() == 3.0f);
}

} // namespace npc_ai_tests
