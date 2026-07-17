//
//  Field.h
//  seeds
//
//  Created by nn0cl on 2013/12/15.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#ifndef seeds_Field_h
#define seeds_Field_h

#include <list>
#include <map>
#include <cstdint>
#include <string>
#include <vector>

#include "Position.h"
#include "Player.h"
#include "Action.h"
#include "EnvironmentEther.h"
#include "Npc.h"
#include "NpcSnapshot.h"

namespace server {
class WorldInput;
struct CombatIntent;
struct SpellIntent;
struct CombatResolution {
    bool spell;
    uint64_t inputSequence;
    int64_t actorId;
    int64_t targetId;
    std::string element;
    std::string requestId;
    float basePower;
    float effectivePower;
    long damage;
    long remainingHp;
    long mpSpent;
    uint64_t cooldownUntil;
    float etherDelta[4];
    CombatResolution()
        : spell(false), inputSequence(0), actorId(0), targetId(0), element(),
          basePower(0.0f), effectivePower(0.0f), damage(0), remainingHp(0),
          mpSpent(0), cooldownUntil(0),
          etherDelta{0.0f, 0.0f, 0.0f, 0.0f} {}
};
}

class Field {
private:
    std::map<int64_t,Player> playerList;
    std::map<int64_t,Npc> npcList;
    std::list<Position> positionQueue;
    std::list<Action> actionQueue;
    Status fieldStatus;
    world::EnvironmentEther fieldEther;
    float lastEtherHazard;
protected:
    Field();
public:
    ~Field();
    static Field* getInstance();
    bool gainStatus(const Status& status);
    static bool setActionQueue(Action action);
    static bool setPosition(Position position);
    static bool setPlayer(Player player);
    static bool unsetPlayer(Player player);
    static bool setNpc(Npc npc);
    static bool unsetNpc(Npc npc);
    bool queueMovement(int64_t playerId, float dx, float dy, float dz);
    bool queueNpcMovement(int64_t npcId, float dx, float dy, float dz);
    bool hasPlayer(int64_t playerId) const;
    const Player* findPlayer(int64_t playerId) const;
    Player* findPlayer(int64_t playerId);
    const Npc* findNpc(int64_t npcId) const;
    Npc* findNpc(int64_t npcId);
    std::vector<NpcSnapshot> publicNpcSnapshots() const;
    world::EnvironmentEther& environmentEther();
    const world::EnvironmentEther& environmentEther() const;
    float environmentHazard() const;
    void processFrame();
    bool processInputs(const std::vector<server::WorldInput>& inputs);
    bool processInputs(const std::vector<server::WorldInput>& inputs,
                       std::vector<server::CombatResolution>& resolutions);
    bool processInputs(const std::vector<server::WorldInput>& inputs,
                       std::vector<server::CombatResolution>& resolutions,
                       uint64_t worldTick);
    
    void putPositionQueue(Position position);
    void putActionQueue(Action action);
    bool putPlayer(Player player);
    bool putNpc(Npc npc);

private:
    void applyAction(Action action);
    bool validateCombat(const server::CombatIntent& intent, std::string& error) const;
    bool validateSpell(const server::SpellIntent& intent, std::string& error) const;
    bool applyCombat(const server::CombatIntent& intent, std::string& error);
    bool applySpell(const server::SpellIntent& intent, std::string& error);
    const Position* findTargetPosition(int64_t targetId) const;
    bool targetIsAlive(int64_t targetId) const;
    bool applyDamageToTarget(int64_t targetId, long damage);
    std::map<int64_t, uint64_t> nextAttackTick;
    std::map<int64_t, uint64_t> nextSpellTick;
};

#endif
