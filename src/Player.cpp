//
//  Player.cpp
//  seeds
//
//  Created by nn0cl on 2013/12/14.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#include "Player.h"

Player::Player()
    : id(0), status(new Status(10, 10)), position(new Position(1, 0.0f, 0.0f, 0.0f)) {}

Player::Player(Player* player)
    : Player(player ? *player : Player()) {}

Player::Player(const Player& player)
    : id(player.id), status(new Status(*player.status)), buffs(player.buffs),
      position(new Position(*player.position)) {}


Player::Player(int playerId,const Status& status,const Position& position){
    this->id = playerId;
    this->status = new Status(status);
    this->position = new Position(position);
};


Player::~Player(){
    delete status;
    delete position;
};

int64_t
Player::getPlayerId(){
    return this->id;
}

bool
Player::setBuff(const Buff& _buff){
    this->buffs.push_back(_buff);
    return true;
};

bool
Player::setHp(long _hp){
    return this->status->setHp(_hp);
};

bool
Player::setMp(long _mp){
    return this->status->setMp(_mp);
};

void
Player::setStatus(Status* _status){
    if (_status == NULL) return;
    if (this->status == NULL) this->status = new Status(*_status);
    else *this->status = *_status;
};

bool
Player::processBuffs(){
    Status* bufStatus;
    std::list<Buff>::iterator it = this->buffs.begin();
    Logger::log(1,"processing all buffs");
    while(it != this->buffs.end()){
        bufStatus = it->getStatus();
        this->status->gainHp(bufStatus->getHp());
        this->status->gainMp(bufStatus->getMp());
        ++it;
    }

    return true;
};

bool
Player::setPosition(Position* position){
    if (position == NULL) return false;
    if (this->position == NULL) this->position = new Position(*position);
    else this->position->setPosition(position->getX(), position->getY(), position->getZ());
    return true;
};

Status*
Player::getStatus(){
    return this->status;
};

Position*
Player::getPosition(){
    return this->position;
}
