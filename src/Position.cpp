//
//  Position.cpp
//  seeds
//
//  Created by nn0cl on 2013/12/15.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#include "Position.h"

Position::Position(int64_t playerId, float x, float y, float z)
    : x(x), y(y), z(z), mapId(0), playerId(playerId) {
};

Position::Position(Position* position)
    : x(0), y(0), z(0), mapId(0), playerId(0) {
    if (position == nullptr) return;
    this->playerId = position->getPlayerId();
    this->x = position->getX();
    this->y = position->getY();
    this->z = position->getZ();
}

Position::~Position(){
    
};

void
Position::setPosition(const float x, const float y, const float z){
    this->x = x;
    this->y = y;
    this->z = z;
};

void
Position::movePosition(const float x, const float y, const float z){
    this->x += x;
    this->y += y;
    this->z += z;
};

float
Position::getX() const {
    return this->x;
};
float
Position::getY() const {
    return this->y;
};

float
Position::getZ() const {
    return this->z;
};

int64_t Position::getPlayerId() const {
    return this->playerId;
}
