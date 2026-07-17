//
//  Player.cpp
//  seeds
//
//  Created by nn0cl on 2013/12/14.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#include "Player.h"

Player::Player()
    : id(0), status(10, 10), position(1, 0.0f, 0.0f, 0.0f) {}

Player::Player(Player* player)
    : Player(player ? *player : Player()) {}

Player::Player(const Player& player)
    : id(player.id), status(player.status), buffs(player.buffs),
      position(player.position) {}


Player::Player(int64_t playerId,const Status& status,const Position& position){
    this->id = playerId;
    this->status = status;
    this->position = position;
};


Player::~Player() = default;

int64_t
Player::getPlayerId() const{
    return this->id;
}

bool
Player::setBuff(const Buff& _buff){
    this->buffs.push_back(_buff);
    return true;
};

bool
Player::setHp(long _hp){
    return this->status.setHp(_hp);
};

bool
Player::setMp(long _mp){
    return this->status.setMp(_mp);
};

void
Player::setStatus(const Status& _status){
    this->status = _status;
};

bool
Player::processBuffs(){
    std::list<Buff>::iterator it = this->buffs.begin();
    Logger::log(1,"processing all buffs");
    while(it != this->buffs.end()){
        Status& bufStatus = it->getStatus();
        this->status.gainHp(bufStatus.getHp());
        this->status.gainMp(bufStatus.getMp());
        ++it;
    }

    return true;
};

bool
Player::setPosition(const Position& position){
    this->position.setPosition(position.getX(), position.getY(), position.getZ());
    return true;
};

Status&
Player::getStatus(){
    return this->status;
};

const Status&
Player::getStatus() const {
    return this->status;
};

Position&
Player::getPosition(){
    return this->position;
}

const Position&
Player::getPosition() const {
    return this->position;
}
