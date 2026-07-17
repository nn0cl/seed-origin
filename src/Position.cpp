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

Position::Position(const Position* position)
    : x(0), y(0), z(0), mapId(0), playerId(0) {
    if (position == nullptr) return;
    this->playerId = position->getPlayerId();
    this->x = position->getX();
    this->y = position->getY();
    this->z = position->getZ();
}

void
Position::setPosition(const float newX, const float newY, const float newZ){
    this->x = newX;
    this->y = newY;
    this->z = newZ;
};

void
Position::movePosition(const float dx, const float dy, const float dz){
    this->x += dx;
    this->y += dy;
    this->z += dz;
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
