//
//  Player.h
//  seeds
//
//  Created by nn0cl on 2013/12/14.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#ifndef seeds_Player_h
#define seeds_Player_h

#include <cstdint>
#include <list>
#include <map>
#include <string>
#include <iostream>

#include "Position.h"
#include "Buff.h"
#include "Status.h"
#include "Logger.h"

class Player{
private:
    int64_t id;
    char name[64]{};
    Status status;
    std::list<Buff> buffs;
    Position position;
    
public:
    Player();
    Player(Player* player);
    Player(const Player& player);
    Player(int64_t playerId,const Status& status,const Position& position);
    ~Player();
    
    int64_t getPlayerId() const;
    
    bool setHp(long _hp);
    bool setMp(long _mp);
    void setStatus(const Status& _status);
    bool setBuff(const Buff& _buff);
    bool processBuffs();
    bool setPosition(const Position& position);
    Status& getStatus();
    const Status& getStatus() const;
    const Position& getPosition() const;
    Position& getPosition();
};


#endif
