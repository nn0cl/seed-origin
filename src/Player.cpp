//
//  Player.cpp
//  seeds
//
//  Created by nn0cl on 2013/12/14.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#include "Player.h"

#include <cstring>

Player::Player()
    : id(0), authPlayerId(0), status(10, 10), position(1, 0.0f, 0.0f, 0.0f) {}

Player::Player(const Player& player)
    : id(player.id), authPlayerId(player.authPlayerId), name{},
      status(player.status), buffs(player.buffs), position(player.position) {
    std::memcpy(name, player.name, sizeof(name));
}


Player::Player(int64_t playerId, const Status& status, const Position& position)
    : id(playerId), authPlayerId(0), status(status), position(position) {}

Player::~Player() = default;

int64_t
Player::getPlayerId() const{
    return this->id;
}

int64_t
Player::getAuthPlayerId() const {
    return authPlayerId;
}

void
Player::setAuthPlayerId(int64_t issuedId) {
    if (authPlayerId > 0) return;
    authPlayerId = issuedId < 0 ? 0 : issuedId;
}

std::string
Player::getPlayerName() const {
    return std::string(name);
}

bool
Player::setPlayerName(const std::string& displayName) {
    if (displayName.empty() || displayName.size() >= sizeof(name)) return false;
    if (displayName.find(';') != std::string::npos ||
        displayName.find('=') != std::string::npos) {
        return false;
    }
    std::memset(name, 0, sizeof(name));
    std::memcpy(name, displayName.c_str(), displayName.size());
    return true;
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
Player::setPosition(const Position& newPosition){
    this->position.setPosition(newPosition.getX(), newPosition.getY(), newPosition.getZ());
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
