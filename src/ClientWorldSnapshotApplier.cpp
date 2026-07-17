#include "ClientWorldSnapshotApplier.h"

#include <cmath>
#include <limits>
#include <map>
#include <sstream>
#include <string>

namespace client {
namespace {

bool parseFloat(const std::string& text, float& value) {
    std::istringstream input(text);
    input >> value;
    std::string trailing;
    if (!input || (input >> trailing) || !std::isfinite(value)) return false;
    return true;
}

bool parseLong(const std::string& text, long& value) {
    std::istringstream input(text);
    input >> value;
    std::string trailing;
    return input && !(input >> trailing);
}

bool parseInt64(const std::string& text, std::int64_t& value) {
    std::istringstream input(text);
    input >> value;
    std::string trailing;
    return input && !(input >> trailing);
}

bool parseNpcField(const std::string& key, const std::string& value,
                   std::map<int, NpcSnapshot>& npcs, std::string& error) {
    if (key == "npc.count") return true;
    if (key.size() < 8 || key.compare(0, 4, "npc.") != 0) return false;
    const std::string::size_type dot = key.find('.', 4);
    if (dot == std::string::npos || dot == 4 || dot + 1 >= key.size()) {
        error = "snapshot contains an invalid NPC field";
        return false;
    }
    int index = -1;
    std::istringstream indexInput(key.substr(4, dot - 4));
    indexInput >> index;
    std::string trailing;
    if (!indexInput || (indexInput >> trailing) || index < 0 || index >= 256) {
        error = "snapshot contains an invalid NPC index";
        return false;
    }
    const std::string field = key.substr(dot + 1);
    NpcSnapshot& npc = npcs[index];
    if (field == "id") {
        if (!parseInt64(value, npc.id)) error = "snapshot NPC id is invalid";
    } else if (field == "type") {
        if (value.empty() || value.size() > 32) error = "snapshot NPC type is invalid";
        npc.type = value;
    } else if (field == "x" || field == "y" || field == "z") {
        float parsed = 0.0f;
        if (!parseFloat(value, parsed)) error = "snapshot NPC position is invalid";
        if (field == "x") npc.x = parsed;
        if (field == "y") npc.y = parsed;
        if (field == "z") npc.z = parsed;
    } else if (field == "hp") {
        if (!parseLong(value, npc.hp) || npc.hp < 0) error = "snapshot NPC HP is invalid";
    } else if (field == "alive") {
        if (value != "1") error = "snapshot NPC alive state is invalid";
        npc.alive = true;
    } else {
        error = "snapshot contains an unknown NPC field";
    }
    return error.empty();
}

bool parsePayload(const std::string& payload, EnvironmentState& state,
                 std::string& error) {
    std::istringstream fields(payload);
    std::string field;
    long npcCount = 0;
    bool npcCountPresent = false;
    std::map<int, NpcSnapshot> parsedNpcs;
    bool present[5] = {false, false, false, false, false};
    float* values[5] = {&state.fire, &state.water, &state.earth,
                        &state.air, &state.hazard};
    const char* keys[5] = {"ether.fire", "ether.water", "ether.earth",
                           "ether.air", "ether.hazard"};
    while (std::getline(fields, field, ';')) {
        const std::string::size_type separator = field.find('=');
        if (separator == std::string::npos) {
            error = "snapshot payload contains a field without a value";
            return false;
        }
        const std::string key = field.substr(0, separator);
        if (key == "npc.count") {
            if (npcCountPresent || !parseLong(field.substr(separator + 1), npcCount) ||
                npcCount < 0 || npcCount >= 256) {
                error = "snapshot NPC count is invalid";
                return false;
            }
            npcCountPresent = true;
            continue;
        }
        if (key.compare(0, 4, "npc.") == 0) {
            if (!parseNpcField(key, field.substr(separator + 1), parsedNpcs, error)) {
                if (error.empty()) error = "snapshot NPC field is invalid";
                return false;
            }
            continue;
        }
        int index = -1;
        for (int i = 0; i < 5; ++i) {
            if (key == keys[i]) index = i;
        }
        if (index < 0 || present[index] ||
            !parseFloat(field.substr(separator + 1), *values[index])) {
            error = "snapshot payload contains an invalid or duplicate field";
            return false;
        }
        present[index] = true;
    }
    for (int i = 0; i < 5; ++i) {
        if (!present[i]) {
            error = "snapshot payload is missing an environment field";
            return false;
        }
    }
    if (state.hazard < 0.0f || state.hazard > 10.0f) {
        error = "snapshot hazard is outside the supported range";
        return false;
    }
    if (npcCountPresent && static_cast<long>(parsedNpcs.size()) != npcCount) {
        error = "snapshot NPC count does not match fields";
        return false;
    }
    if (npcCountPresent) {
        for (long index = 0; index < npcCount; ++index) {
            std::map<int, NpcSnapshot>::const_iterator found =
                parsedNpcs.find(static_cast<int>(index));
            if (found == parsedNpcs.end() || found->second.id <= 0 ||
                found->second.type.empty() || !std::isfinite(found->second.x) ||
                !std::isfinite(found->second.y) || !std::isfinite(found->second.z) ||
                found->second.hp < 0 || !found->second.alive) {
                error = "snapshot NPC fields are incomplete";
                return false;
            }
            state.npcs.push_back(found->second);
        }
    }
    return true;
}

}

ClientWorldSnapshotApplier::ClientWorldSnapshotApplier() : sync() {}

bool ClientWorldSnapshotApplier::applySnapshot(
    const network::WorldUpdate& update, ClientEnvironmentState& state,
    std::string& error) {
    if (update.kind != network::UpdateKind::Snapshot || update.eventId != 0) {
        error = "only a zero-event snapshot can replace client state";
        return false;
    }
    if (!network::validateWorldUpdate(update, error)) return false;
    if (update.sequence == std::numeric_limits<uint64_t>::max()) {
        error = "snapshot sequence cannot be incremented";
        return false;
    }
    EnvironmentState next = {};
    if (!parsePayload(update.payload, next, error)) return false;
    next.worldTick = update.worldTick;
    next.sequence = update.sequence;
    state.replace(next);
    sync.confirmSnapshot(update.sequence);
    error.clear();
    return true;
}

const network::WorldUpdateSyncController&
ClientWorldSnapshotApplier::synchronization() const {
    return sync;
}

}
