//
//  Position.cpp
//  seeds
//
//  Created by nn0cl on 2013/12/15.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#include "Position.h"

Position::Position(int playerId, float x, float y, float z) {
    this->playerId = playerId;
    this->x = x;
    this->y = y;
    this->z = z;
};

Position::Position(Position* position){
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
Position::getX(){
    return this->x;
};
float
Position::getY(){
    return this->y;
};

float
Position::getZ(){
    return this->z;
};

int Position::getPlayerId() {
    return this->getPlayerId();
}