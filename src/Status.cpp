//
//  Status.cpp
//  seeds
//
//  Created by nn0cl on 2013/12/14.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#include "Status.h"

Status::Status() : hp(0), mp(0) {}

Status::~Status() = default;


Status::Status(const long _hp, const long _mp)
    : hp(_hp < 0 ? 0 : _hp), mp(_mp < 0 ? 0 : _mp) {}

Status::Status(const Status& _status) = default;


bool
Status::setHp(const long& _hp){
    this->hp = _hp < 0 ? 0 : _hp;
    return true;
};

bool
Status::setMp(const long& _mp){
    this->mp = _mp < 0 ? 0 : _mp;
    return true;
};

long
Status::getHp() const{
    return this->hp;
};

long
Status::getMp() const {
    return this->mp;
};

bool
Status::gainHp(const long& _hp){
    this->hp += _hp;
    if (this->hp < 0) this->hp = 0;
    return true;
};

bool
Status::gainMp(const long& _mp){
    this->mp += _mp;
    if (this->mp < 0) this->mp = 0;
    return true;
};
