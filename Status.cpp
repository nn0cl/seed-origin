//
//  Status.cpp
//  seeds
//
//  Created by nn0cl on 2013/12/14.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#include "Status.h"

Status::Status(){
    
};

Status::~Status(){
};


Status::Status(const long _hp, const long _mp){
    this->hp = _hp;
    this->mp = _mp;
};

Status::Status(const Status& _status){
    this->hp = _status.hp;
    this->mp = _status.mp;
};


bool
Status::setHp(const long& _hp){
    long tmp = this->hp + _hp;
    this->hp = tmp < 0.0f ? 0 : tmp;
    
    this->hp = _hp;
    return true;
};

bool
Status::setMp(const long& _mp){
    long tmp = this->mp + _mp;
    this->mp = tmp < 0.0f ? 0 : tmp;
    
    this->hp = _mp;
    return true;
};

long
Status::getHp(){
    return this->hp;
};

long
Status::getMp() {
    return this->mp;
};

bool
Status::gainHp(const long& _hp){
    this->hp += _hp;
    return true;
};

bool
Status::gainMp(const long& _mp){
    this->mp += _mp;
    return true;
};