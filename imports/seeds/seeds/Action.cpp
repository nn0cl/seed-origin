//
//  Action.cpp
//  seeds
//
//  Created by nn0cl on 2013/12/15.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#include "Action.h"

Action::Action(int actionType,Player* playerFrom,Player* playerTo,const Status status){
    this->actionType = actionType;
    this->playerFrom = new Player(*playerFrom);
    this->playerTo = new Player(*playerTo);
    this->status = status;
}

Action::~Action() {
    delete(this->playerFrom);
    delete(this->playerTo);
};

int
Action::getActionType(){
    return this->actionType;
}

Player*
Action::getPlayerFrom(){
    return this->playerFrom;
};

Player*
Action::getPlayerTo(){
    return this->playerTo;
};

Status*
Action::getStatus(){
    return &status;
};