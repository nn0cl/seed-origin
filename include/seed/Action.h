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
    Player playerFrom;
    Player playerTo;
    Status status;
    bool valid;
public:
    Action(int actionType, const Player* playerFrom, const Player* playerTo,
           const Status status);
    ~Action();
    int getActionType() const;
    bool isValid() const;
    const Player& getPlayerFrom() const;
    const Player& getPlayerTo() const;
    const Status& getStatus() const;
};


#endif
