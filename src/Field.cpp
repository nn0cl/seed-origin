//
//  Field.cpp
//  seeds
//
//  Created by nn0cl on 2013/12/15.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#include "Field.h"
#include "WorldInputQueue.h"

#include <cctype>
#include <cmath>
#include <limits>

namespace {
const uint64_t kAttackCooldownTicks = 1;
const uint64_t kSpellCooldownTicks = 2;
const float kSpellMpCostRatio = 0.1f;

bool spellMpCost(float power, long& cost) {
    const double raw = std::ceil(static_cast<double>(power) * kSpellMpCostRatio);
    if (!std::isfinite(raw) || raw < 1.0 ||
        raw > static_cast<double>(std::numeric_limits<long>::max())) {
        return false;
    }
    cost = static_cast<long>(raw);
    return true;
}

bool tickAfter(uint64_t tick, uint64_t duration, uint64_t& result) {
    if (tick > std::numeric_limits<uint64_t>::max() - duration) return false;
    result = tick + duration;
    return true;
}
}

Field::Field() : lastEtherHazard(0.0f) {
    
};

Field::~Field() {
    
};

Field*
Field::getInstance(){
    static Field fieldInstance;
    return &fieldInstance;
};

bool
Field::gainStatus(const Status& status){
    fieldStatus.gainHp(status.getHp());
    fieldStatus.gainMp(status.getMp());
    return true;
}

bool
Field::setActionQueue(Action action){
    Field* instance = Field::getInstance();
    instance->putActionQueue(action);
    return true;
};

bool
Field::setPosition(Position position){
    Field* instance = Field::getInstance();
    instance->putPositionQueue(position);
    return true;
};

bool
Field::setPlayer(Player player){
    Field* instance = Field::getInstance();
    return instance->putPlayer(player);
};

bool
Field::unsetPlayer(Player player){
    Field* instance = Field::getInstance();
    const int64_t playerId = player.getPlayerId();
    instance->nextAttackTick.erase(playerId);
    instance->nextSpellTick.erase(playerId);
    return instance->playerList.erase(playerId) > 0;
}

bool Field::setNpc(Npc npc){
    Field* instance = Field::getInstance();
    return instance->putNpc(npc);
}

bool Field::unsetNpc(Npc npc){
    Field* instance = Field::getInstance();
    return instance->npcList.erase(npc.getNpcId()) > 0;
}

bool
Field::queueMovement(int64_t playerId, float dx, float dy, float dz){
    std::map<int64_t,Player>::iterator playerItt = playerList.find(playerId);
    if (playerItt == playerList.end()) return false;
    Position next = playerItt->second.getPosition();
    next.movePosition(dx, dy, dz);
    positionQueue.push_back(next);
    return true;
}

bool
Field::hasPlayer(int64_t playerId) const {
    return playerList.find(playerId) != playerList.end();
}

const Player*
Field::findPlayer(int64_t playerId) const {
    std::map<int64_t,Player>::const_iterator found = playerList.find(playerId);
    return found == playerList.end() ? nullptr : &found->second;
}

world::EnvironmentEther& Field::environmentEther() {
    return fieldEther;
}

const world::EnvironmentEther& Field::environmentEther() const {
    return fieldEther;
}

float Field::environmentHazard() const {
    return lastEtherHazard;
}

Player*
Field::findPlayer(int64_t playerId) {
    std::map<int64_t,Player>::iterator found = playerList.find(playerId);
    return found == playerList.end() ? nullptr : &found->second;
}

const Npc* Field::findNpc(int64_t npcId) const {
    std::map<int64_t, Npc>::const_iterator found = npcList.find(npcId);
    return found == npcList.end() ? nullptr : &found->second;
}

Npc* Field::findNpc(int64_t npcId) {
    std::map<int64_t, Npc>::iterator found = npcList.find(npcId);
    return found == npcList.end() ? nullptr : &found->second;
}

void
Field::processFrame(){

    //playerへのポジションキュー判定
    std::list<Position>::iterator positionItt = this->positionQueue.begin();
    while(positionItt != this->positionQueue.end()) {
        std::map<int64_t,Player>::iterator playerItt = playerList.find(positionItt->getPlayerId());
        if (playerItt != playerList.end()) {
            playerItt->second.setPosition(*positionItt);
        }
        ++positionItt;
    }
    this->positionQueue.clear();
    
    // Actionキューを処理取得
    std::list<Action>::iterator actionItt = this->actionQueue.begin();
    while(actionItt != this->actionQueue.end()) {
        if (!actionItt->isValid()) {
            ++actionItt;
            continue;
        }
        switch(actionItt->getActionType()){
            case 0:
                //0:Field,1:Player,3:self
                this->gainStatus(actionItt->getStatus());
                break;
            case 1:
                {
                    std::map<int64_t,Player>::iterator playerItt =
                        playerList.find(actionItt->getPlayerTo()->getPlayerId());
                    if (playerItt != playerList.end()) {
                        playerItt->second.getStatus().gainHp(actionItt->getStatus().getHp());
                        playerItt->second.getStatus().gainMp(actionItt->getStatus().getMp());
                    }
                }
                break;
            case 2:
            case 3:
                {
                    std::map<int64_t,Player>::iterator playerItt =
                        playerList.find(actionItt->getPlayerFrom()->getPlayerId());
                    if (playerItt != playerList.end()) {
                        playerItt->second.getStatus().gainHp(actionItt->getStatus().getHp());
                        playerItt->second.getStatus().gainMp(actionItt->getStatus().getMp());
                    }
                }
                break;
        }
        ++actionItt;
    }
    this->actionQueue.clear();
    
    //
    std::map<int64_t,Player>::iterator playerItt = this->playerList.begin();
    while(playerItt != this->playerList.end()){
        ++playerItt;
    }
};

bool Field::processInputs(const std::vector<server::WorldInput>& inputs) {
    std::vector<server::CombatResolution> ignored;
    return processInputs(inputs, ignored);
}

bool Field::processInputs(const std::vector<server::WorldInput>& inputs,
                          std::vector<server::CombatResolution>& resolutions) {
    return processInputs(inputs, resolutions, 0);
}

bool Field::processInputs(const std::vector<server::WorldInput>& inputs,
                          std::vector<server::CombatResolution>& resolutions,
                          uint64_t worldTick) {
    resolutions.clear();
    std::map<int64_t, long> projectedMp;
    std::map<int64_t, uint64_t> projectedAttackTick = nextAttackTick;
    std::map<int64_t, uint64_t> projectedSpellTick = nextSpellTick;
    for (std::vector<server::WorldInput>::const_iterator it = inputs.begin();
         it != inputs.end(); ++it) {
        if (it->kind() == server::WorldInputKind::Movement &&
            !hasPlayer(it->movement().sessionId)) {
            return false;
        }
        if (it->kind() == server::WorldInputKind::Movement &&
            !server::isValidMovementDelta(it->movement().dx,
                                          it->movement().dy,
                                          it->movement().dz)) {
            return false;
        }
        if (it->kind() == server::WorldInputKind::Action &&
            !it->action().isValid()) {
            return false;
        }
        if (it->kind() == server::WorldInputKind::Combat) {
            std::string error;
            if (!validateCombat(it->combat(), error)) return false;
        }
        if (it->kind() == server::WorldInputKind::Spell) {
            std::string error;
            if (!validateSpell(it->spell(), error)) return false;
        }
        if (it->kind() == server::WorldInputKind::Combat) {
            uint64_t nextTick = 0;
            if (!tickAfter(worldTick, kAttackCooldownTicks, nextTick) ||
                (projectedAttackTick.count(it->combat().attackerId) != 0 &&
                 worldTick < projectedAttackTick[it->combat().attackerId])) {
                return false;
            }
            projectedAttackTick[it->combat().attackerId] = nextTick;
        }
        if (it->kind() == server::WorldInputKind::Spell) {
            long cost = 0;
            uint64_t nextTick = 0;
            if (!spellMpCost(it->spell().power, cost) ||
                !tickAfter(worldTick, kSpellCooldownTicks, nextTick) ||
                (projectedSpellTick.count(it->spell().casterId) != 0 &&
                 worldTick < projectedSpellTick[it->spell().casterId])) {
                return false;
            }
            const Player* caster = findPlayer(it->spell().casterId);
            const long available = projectedMp.count(it->spell().casterId) != 0
                ? projectedMp[it->spell().casterId] : caster->getStatus().getMp();
            if (available < cost) return false;
            projectedMp[it->spell().casterId] = available - cost;
            projectedSpellTick[it->spell().casterId] = nextTick;
        }
        if (it->kind() == server::WorldInputKind::Movement) {
            const Player* player = findPlayer(it->movement().sessionId);
            const Position& position = player->getPosition();
            const float x = position.getX() + it->movement().dx;
            const float y = position.getY() + it->movement().dy;
            const float z = position.getZ() + it->movement().dz;
            if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z) ||
                std::fabs(x) > server::MAX_WORLD_COORDINATE ||
                std::fabs(y) > server::MAX_WORLD_COORDINATE ||
                std::fabs(z) > server::MAX_WORLD_COORDINATE) {
                return false;
            }
        }
    }
    const std::map<int64_t, Player> playersBefore = playerList;
    const std::map<int64_t, Npc> npcsBefore = npcList;
    const world::EnvironmentEther etherBefore = fieldEther;
    const std::map<int64_t, uint64_t> attackTicksBefore = nextAttackTick;
    const std::map<int64_t, uint64_t> spellTicksBefore = nextSpellTick;
    const float hazardBefore = lastEtherHazard;
    for (std::vector<server::WorldInput>::const_iterator it = inputs.begin();
         it != inputs.end(); ++it) {
        if (it->kind() == server::WorldInputKind::Movement) {
            std::map<int64_t,Player>::iterator playerItt =
                playerList.find(it->movement().sessionId);
            Position next = playerItt->second.getPosition();
            next.movePosition(it->movement().dx, it->movement().dy, it->movement().dz);
            playerItt->second.setPosition(next);
        } else if (it->kind() == server::WorldInputKind::Action) {
            applyAction(it->action());
        } else if (it->kind() == server::WorldInputKind::Combat) {
            const Player* targetBefore = findPlayer(it->combat().targetId);
            const long hpBefore = targetBefore->getStatus().getHp();
            std::string error;
            if (!applyCombat(it->combat(), error)) {
                playerList = playersBefore;
                npcList = npcsBefore;
                fieldEther = etherBefore;
                nextAttackTick = attackTicksBefore;
                nextSpellTick = spellTicksBefore;
                lastEtherHazard = hazardBefore;
                resolutions.clear();
                return false;
            }
            const Player* targetAfter = findPlayer(it->combat().targetId);
            server::CombatResolution resolution;
            resolution.inputSequence = it->sequence();
            resolution.actorId = it->combat().attackerId;
            resolution.targetId = it->combat().targetId;
            resolution.basePower = it->combat().power;
            resolution.requestId = it->combat().requestId;
            resolution.effectivePower = it->combat().power;
            resolution.damage = hpBefore - targetAfter->getStatus().getHp();
            resolution.remainingHp = targetAfter->getStatus().getHp();
            resolution.mpSpent = 0;
            tickAfter(worldTick, kAttackCooldownTicks, resolution.cooldownUntil);
            nextAttackTick[it->combat().attackerId] = resolution.cooldownUntil;
            resolutions.push_back(resolution);
        } else if (it->kind() == server::WorldInputKind::Spell) {
            const Player* targetBefore = findPlayer(it->spell().targetId);
            const long hpBefore = targetBefore->getStatus().getHp();
            const float before[4] = {
                fieldEther.value(world::EtherAttribute::Fire),
                fieldEther.value(world::EtherAttribute::Water),
                fieldEther.value(world::EtherAttribute::Earth),
                fieldEther.value(world::EtherAttribute::Air)};
            world::EtherAttribute spellAttribute = world::EtherAttribute::Fire;
            fieldEther.attributeForElement(it->spell().element, spellAttribute);
            const float effectivePower =
                it->spell().power * fieldEther.conductivity(spellAttribute);
            std::string error;
            if (!applySpell(it->spell(), error)) {
                playerList = playersBefore;
                npcList = npcsBefore;
                fieldEther = etherBefore;
                nextAttackTick = attackTicksBefore;
                nextSpellTick = spellTicksBefore;
                lastEtherHazard = hazardBefore;
                resolutions.clear();
                return false;
            }
            long mpCost = 0;
            if (!spellMpCost(it->spell().power, mpCost)) {
                playerList = playersBefore;
                npcList = npcsBefore;
                fieldEther = etherBefore;
                nextAttackTick = attackTicksBefore;
                nextSpellTick = spellTicksBefore;
                lastEtherHazard = hazardBefore;
                resolutions.clear();
                return false;
            }
            Player* caster = findPlayer(it->spell().casterId);
            caster->getStatus().gainMp(-mpCost);
            const Player* targetAfter = findPlayer(it->spell().targetId);
            server::CombatResolution resolution;
            resolution.spell = true;
            resolution.inputSequence = it->sequence();
            resolution.actorId = it->spell().casterId;
            resolution.targetId = it->spell().targetId;
            resolution.element = it->spell().element;
            resolution.requestId = it->spell().requestId;
            resolution.basePower = it->spell().power;
            resolution.damage = hpBefore - targetAfter->getStatus().getHp();
            resolution.effectivePower = effectivePower;
            resolution.remainingHp = targetAfter->getStatus().getHp();
            resolution.mpSpent = mpCost;
            tickAfter(worldTick, kSpellCooldownTicks, resolution.cooldownUntil);
            nextSpellTick[it->spell().casterId] = resolution.cooldownUntil;
            const float after[4] = {
                fieldEther.value(world::EtherAttribute::Fire),
                fieldEther.value(world::EtherAttribute::Water),
                fieldEther.value(world::EtherAttribute::Earth),
                fieldEther.value(world::EtherAttribute::Air)};
            for (size_t i = 0; i < 4; ++i) {
                resolution.etherDelta[i] = after[i] - before[i];
            }
            resolutions.push_back(resolution);
        }
    }
    fieldEther.decay();
    lastEtherHazard = fieldEther.adverseEffectSeverity();
    return true;
}

bool Field::validateCombat(const server::CombatIntent& intent, std::string& error) const {
    const Player* attacker = findPlayer(intent.attackerId);
    const Position* targetPosition = findTargetPosition(intent.targetId);
    if (attacker == nullptr || targetPosition == nullptr ||
        !targetIsAlive(intent.targetId)) {
        error = "combat actor or target is not present";
        return false;
    }
    const float dx = attacker->getPosition().getX() - targetPosition->getX();
    const float dy = attacker->getPosition().getY() - targetPosition->getY();
    const float dz = attacker->getPosition().getZ() - targetPosition->getZ();
    if (dx * dx + dy * dy + dz * dz > 10000.0f) {
        error = "combat target is out of range";
        return false;
    }
    if (!std::isfinite(intent.power) || intent.power <= 0.0f || intent.power > 100000.0f) {
        error = "combat power is invalid";
        return false;
    }
    error.clear();
    return true;
}

bool Field::validateSpell(const server::SpellIntent& intent, std::string& error) const {
    if (intent.element.empty() || intent.element.size() > 32) {
        error = "spell element is invalid";
        return false;
    }
    if (!fieldEther.isKnownElement(intent.element)) {
        error = "spell element is unknown";
        return false;
    }
    for (std::string::const_iterator it = intent.element.begin();
         it != intent.element.end(); ++it) {
        if (!std::isalnum(static_cast<unsigned char>(*it)) && *it != '_' && *it != '-') {
            error = "spell element contains invalid characters";
            return false;
        }
    }
    const server::CombatIntent combat = {
        intent.casterId, intent.targetId, intent.power, intent.requestId};
    return validateCombat(combat, error);
}

bool Field::applyCombat(const server::CombatIntent& intent, std::string& error) {
    if (!validateCombat(intent, error)) return false;
    const long damage = static_cast<long>(intent.power);
    if (!applyDamageToTarget(intent.targetId, damage)) {
        error = "combat target is not alive";
        return false;
    }
    error.clear();
    return true;
}

bool Field::applySpell(const server::SpellIntent& intent, std::string& error) {
    if (!validateSpell(intent, error)) return false;
    float effectivePower = 0.0f;
    if (!fieldEther.resolveSpell(intent.element, intent.power, effectivePower, error)) return false;
    const long damage = static_cast<long>(effectivePower);
    if (!applyDamageToTarget(intent.targetId, damage)) {
        error = "spell target is not alive";
        return false;
    }
    error.clear();
    return true;
}

void Field::applyAction(Action action) {
    if (!action.isValid()) return;
    switch (action.getActionType()) {
        case 0:
            gainStatus(action.getStatus());
            break;
        case 1: {
            std::map<int64_t,Player>::iterator playerItt =
                playerList.find(action.getPlayerTo()->getPlayerId());
            if (playerItt != playerList.end()) {
                playerItt->second.getStatus().gainHp(action.getStatus().getHp());
                playerItt->second.getStatus().gainMp(action.getStatus().getMp());
            }
            break;
        }
        case 2:
        case 3: {
            std::map<int64_t,Player>::iterator playerItt =
                playerList.find(action.getPlayerFrom()->getPlayerId());
            if (playerItt != playerList.end()) {
                playerItt->second.getStatus().gainHp(action.getStatus().getHp());
                playerItt->second.getStatus().gainMp(action.getStatus().getMp());
            }
            break;
        }
    }
}

void
Field::putPositionQueue(Position position){
    this->positionQueue.push_back(position);
};

void
Field::putActionQueue(Action action){
    this->actionQueue.push_back(action);
};

bool
Field::putPlayer(Player player){
    if (player.getPlayerId() <= 0 || findNpc(player.getPlayerId()) != nullptr) {
        return false;
    }
    this->playerList[player.getPlayerId()] = player;
    return true;
}

bool Field::putNpc(Npc npc){
    if (npc.getNpcId() <= 0 || hasPlayer(npc.getNpcId()) ||
        findNpc(npc.getNpcId()) != nullptr) {
        return false;
    }
    npcList.emplace(npc.getNpcId(), std::move(npc));
    return true;
}

const Position* Field::findTargetPosition(int64_t targetId) const {
    const Player* player = findPlayer(targetId);
    if (player != nullptr) return &player->getPosition();
    const Npc* npc = findNpc(targetId);
    return npc == nullptr ? nullptr : &npc->getPosition();
}

bool Field::targetIsAlive(int64_t targetId) const {
    const Player* player = findPlayer(targetId);
    if (player != nullptr) return player->getStatus().getHp() > 0;
    const Npc* npc = findNpc(targetId);
    return npc != nullptr && npc->isAlive();
}

bool Field::applyDamageToTarget(int64_t targetId, long damage) {
    Player* player = findPlayer(targetId);
    if (player != nullptr) {
        if (player->getStatus().getHp() <= 0 || damage <= 0) return false;
        player->getStatus().gainHp(-damage);
        return true;
    }
    Npc* npc = findNpc(targetId);
    return npc != nullptr && npc->applyDamage(damage);
}
