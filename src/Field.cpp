//
//  Field.cpp
//  seeds
//
//  Created by nn0cl on 2013/12/15.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#include "Field.h"

Field::Field() {
    
};

Field::~Field() {
    
};

Field*
Field::getInstance(){
    static Field fieldInstance;
    return &fieldInstance;
};

bool
Field::gainStatus(Status& status){
    
    return true;
};

bool
Field::setActionQueue(Action action){
    Field* instance = Field::getInstance();
    instance->putActionQueue(action);
    return true;
};

bool
Field::setPosition(Position position){
    Field* instance = Field::getInstance();
    instance->putPositionQueue(position);
    return true;
};

bool
Field::setPlayer(Player player){
    Field* instance = Field::getInstance();
    instance->putPlayer(player);
    return true;
};

bool
Field::unsetPlayer(Player player){
    
    return true;
};

void
Field::processFrame(){

    //playerへのポジションキュー判定
    std::list<Position>::iterator positionItt = this->positionQueue.begin();
    while(positionItt != this->positionQueue.end()) {
        std::map<int,Player>::iterator playerItt = playerList.find(positionItt->getPlayerId());
        if (playerItt != playerList.end()) {
            playerItt->second.setPosition(*positionItt);
        }
        ++positionItt;
    }
    this->positionQueue.clear();
    
    // Actionキューを処理取得
    std::list<Action>::iterator actionItt = this->actionQueue.begin();
    while(actionItt != this->actionQueue.end()) {
        switch(actionItt->getActionType()){
            case 0:
                //0:Field,1:Player,3:self
                this->gainStatus(actionItt->getStatus());
                break;
            case 1:
                break;
            case 2:
                break;
        }
        ++actionItt;
    }
    this->actionQueue.clear();
    
    //
    std::map<int,Player>::iterator playerItt = this->playerList.begin();
    while(playerItt != this->playerList.end()){
        ++playerItt;
    }
};

void
Field::putPositionQueue(Position position){
    this->positionQueue.push_back(position);
};

void
Field::putActionQueue(Action action){
    this->actionQueue.push_back(action);
};

void
Field::putPlayer(Player player){
    this->playerList[player.getPlayerId()] = player;
}
