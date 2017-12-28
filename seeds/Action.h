//
//  Action.h
//  seeds
//
//  Created by nn0cl on 2013/12/15.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#ifndef seeds_Action_h
#define seeds_Action_h

#include "Player.h"
#include "Status.h"

class Action {
private:
    int actionType; //0:Field,1:Player,3:self
    Player* playerFrom;
    Player* playerTo;
    Status status;
public:
    Action(int actionType,Player* playerFrom,Player* playerTo,const Status status);
    ~Action();
    int getActionType();
    Player* getPlayerFrom();
    Player* getPlayerTo();
    Status* getStatus();
};


#endif
