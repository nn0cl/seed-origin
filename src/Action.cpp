//
//  Action.cpp
//  seeds
//
//  Created by nn0cl on 2013/12/15.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#include "Action.h"

Action::Action(int actionType,Player* playerFrom,Player* playerTo,const Status status)
    : actionType(actionType),
      playerFrom(playerFrom ? *playerFrom : Player()),
      playerTo(playerTo ? *playerTo : Player()),
      status(status) {}

Action::~Action() = default;

int
Action::getActionType(){
    return this->actionType;
}

Player*
Action::getPlayerFrom(){
    return &this->playerFrom;
};

Player*
Action::getPlayerTo(){
    return &this->playerTo;
};

Status&
Action::getStatus(){
    return status;
};
