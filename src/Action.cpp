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
      status(status),
      valid(actionType == 0 ||
            (actionType == 1 && playerFrom != nullptr && playerTo != nullptr) ||
            ((actionType == 2 || actionType == 3) && playerFrom != nullptr)) {}

Action::~Action() = default;

int
Action::getActionType(){
    return this->actionType;
}

bool
Action::isValid() const{
    return valid;
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
