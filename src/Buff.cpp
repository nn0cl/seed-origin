//
//  Buff.cpp
//  seeds
//
//  Created by nn0cl on 2013/12/14.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//


#include "Buff.h"

Buff::Buff(int _id, std::string _buffName, long _lifetime,long _startTime){
    this->id = _id;
    this->lifetime = _lifetime;
    this->buffName = _buffName;
    this->startTime = _startTime;
    
    std::ostringstream stream;
    stream << "Initializing...";
    stream << "ID:";
    stream << this->id;
    stream << "BuffName:";
    stream << this->buffName;
    Logger::log(1,stream.str());
    

};

Buff::~Buff() = default;

Status&
Buff::getStatus(){
    
    std::ostringstream stream;
    stream << "buffer proceeded ID:";
    stream << this->id;
    stream << " BufferName:";
    stream << this->buffName;
    Logger::log(1,stream.str());
    
    return this->status;
};

void
Buff::setStatus(const Status& status) {
    this->status = status;
};
