//
//  Field.h
//  seeds
//
//  Created by nn0cl on 2013/12/15.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#ifndef seeds_Field_h
#define seeds_Field_h

#include <list>
#include <map>

#include "Position.h"
#include "Player.h"
#include "Action.h"

class Field {
private:
    static Field* fieldInstance;
    std::map<int,Player> playerList;
    std::list<Position> positionQueue;
    std::list<Action> actionQueue;
protected:
    Field();
public:
    ~Field();
    static Field* getInstance();
    bool gainStatus(Status& status);
    static bool setActionQueue(Action action);
    static bool setPosition(Position position);
    static bool setPlayer(Player player);
    static bool unsetPlayer(Player player);
    void processFrame();
    
    void putPositionQueue(Position position);
    void putActionQueue(Action action);
    void putPlayer(Player player);
};

#endif
