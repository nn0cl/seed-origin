//
//  Status.cpp
//  seeds
//
//  Created by nn0cl on 2013/12/14.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#include "Status.h"
#include <limits>

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
    const long maxValue = std::numeric_limits<long>::max();
    if (_hp > 0) {
        if (this->hp > maxValue - _hp) {
            this->hp = maxValue;
        } else {
            this->hp += _hp;
        }
    } else if (_hp < 0) {
        const long magnitudeWithoutOne = -(_hp + 1);
        if (this->hp <= magnitudeWithoutOne) {
            this->hp = 0;
        } else {
            this->hp -= magnitudeWithoutOne;
            this->hp -= 1;
        }
    }
    return true;
};

bool
Status::gainMp(const long& _mp){
    const long maxValue = std::numeric_limits<long>::max();
    if (_mp > 0) {
        if (this->mp > maxValue - _mp) {
            this->mp = maxValue;
        } else {
            this->mp += _mp;
        }
    } else if (_mp < 0) {
        const long magnitudeWithoutOne = -(_mp + 1);
        if (this->mp <= magnitudeWithoutOne) {
            this->mp = 0;
        } else {
            this->mp -= magnitudeWithoutOne;
            this->mp -= 1;
        }
    }
    return true;
};
