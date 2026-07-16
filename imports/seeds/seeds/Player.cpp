//
//  Player.cpp
//  seeds
//
//  Created by nn0cl on 2013/12/14.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#include "Player.h"

Player::Player(){
    this->status = new Status(10.0f,10.0f);
    this->position = new Position(1,0.0f,0.0f,0.0f);
};

Player::Player(Player* player){
    this->setStatus(player->getStatus());
    this->setPosition(player->getPosition());

};


Player::Player(int playerId,const Status& status,const Position& position){
    this->status = new Status(status);
    this->position = new Position(position);
};


Player::~Player(){
    delete(status);
};

int64_t
Player::getPlayerId(){
    return this->id;
}

bool
Player::setBuff(Buff _buff){
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
    this->status = new Status(*_status);
};

bool
Player::processBuffs(){
    Status* bufStatus;
    std::list<Buff>::iterator it = this->buffs.begin();
    Logger::log(1,"processing all buffs");
    while(it != this->buffs.end()){
        struct timespec req = {1,0};
        nanosleep(&req,NULL);
        bufStatus = it->getStatus();
        this->status->gainHp(bufStatus->getHp());
        this->status->gainMp(bufStatus->getMp());
        ++it;
    }

    return true;
};

bool
Player::setPosition(Position* position){
    this->position->setPosition(position->getX(), position->getY(), position->getZ());
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
