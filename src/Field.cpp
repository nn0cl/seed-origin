//
//  Field.cpp
//  seeds
//
//  Created by nn0cl on 2013/12/15.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#include "Field.h"
#include "WorldInputQueue.h"

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
Field::gainStatus(const Status& status){
    fieldStatus.gainHp(status.getHp());
    fieldStatus.gainMp(status.getMp());
    return true;
}

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
    Field* instance = Field::getInstance();
    return instance->playerList.erase(player.getPlayerId()) > 0;
}

bool
Field::queueMovement(int64_t playerId, float dx, float dy, float dz){
    std::map<int64_t,Player>::iterator playerItt = playerList.find(playerId);
    if (playerItt == playerList.end()) return false;
    Position next = playerItt->second.getPosition();
    next.movePosition(dx, dy, dz);
    positionQueue.push_back(next);
    return true;
}

bool
Field::hasPlayer(int64_t playerId) const {
    return playerList.find(playerId) != playerList.end();
}

void
Field::processFrame(){

    //playerへのポジションキュー判定
    std::list<Position>::iterator positionItt = this->positionQueue.begin();
    while(positionItt != this->positionQueue.end()) {
        std::map<int64_t,Player>::iterator playerItt = playerList.find(positionItt->getPlayerId());
        if (playerItt != playerList.end()) {
            playerItt->second.setPosition(*positionItt);
        }
        ++positionItt;
    }
    this->positionQueue.clear();
    
    // Actionキューを処理取得
    std::list<Action>::iterator actionItt = this->actionQueue.begin();
    while(actionItt != this->actionQueue.end()) {
        if (!actionItt->isValid()) {
            ++actionItt;
            continue;
        }
        switch(actionItt->getActionType()){
            case 0:
                //0:Field,1:Player,3:self
                this->gainStatus(actionItt->getStatus());
                break;
            case 1:
                {
                    std::map<int64_t,Player>::iterator playerItt =
                        playerList.find(actionItt->getPlayerTo()->getPlayerId());
                    if (playerItt != playerList.end()) {
                        playerItt->second.getStatus().gainHp(actionItt->getStatus().getHp());
                        playerItt->second.getStatus().gainMp(actionItt->getStatus().getMp());
                    }
                }
                break;
            case 2:
            case 3:
                {
                    std::map<int64_t,Player>::iterator playerItt =
                        playerList.find(actionItt->getPlayerFrom()->getPlayerId());
                    if (playerItt != playerList.end()) {
                        playerItt->second.getStatus().gainHp(actionItt->getStatus().getHp());
                        playerItt->second.getStatus().gainMp(actionItt->getStatus().getMp());
                    }
                }
                break;
        }
        ++actionItt;
    }
    this->actionQueue.clear();
    
    //
    std::map<int64_t,Player>::iterator playerItt = this->playerList.begin();
    while(playerItt != this->playerList.end()){
        ++playerItt;
    }
};

bool Field::processInputs(const std::vector<server::WorldInput>& inputs) {
    for (std::vector<server::WorldInput>::const_iterator it = inputs.begin();
         it != inputs.end(); ++it) {
        if (it->kind() == server::WorldInputKind::Movement &&
            !hasPlayer(it->movement().sessionId)) {
            return false;
        }
        if (it->kind() == server::WorldInputKind::Action &&
            !it->action().isValid()) {
            return false;
        }
    }
    for (std::vector<server::WorldInput>::const_iterator it = inputs.begin();
         it != inputs.end(); ++it) {
        if (it->kind() == server::WorldInputKind::Movement) {
            std::map<int64_t,Player>::iterator playerItt =
                playerList.find(it->movement().sessionId);
            Position next = playerItt->second.getPosition();
            next.movePosition(it->movement().dx, it->movement().dy, it->movement().dz);
            playerItt->second.setPosition(next);
        } else {
            applyAction(it->action());
        }
    }
    return true;
}

void Field::applyAction(Action action) {
    if (!action.isValid()) return;
    switch (action.getActionType()) {
        case 0:
            gainStatus(action.getStatus());
            break;
        case 1: {
            std::map<int64_t,Player>::iterator playerItt =
                playerList.find(action.getPlayerTo()->getPlayerId());
            if (playerItt != playerList.end()) {
                playerItt->second.getStatus().gainHp(action.getStatus().getHp());
                playerItt->second.getStatus().gainMp(action.getStatus().getMp());
            }
            break;
        }
        case 2:
        case 3: {
            std::map<int64_t,Player>::iterator playerItt =
                playerList.find(action.getPlayerFrom()->getPlayerId());
            if (playerItt != playerList.end()) {
                playerItt->second.getStatus().gainHp(action.getStatus().getHp());
                playerItt->second.getStatus().gainMp(action.getStatus().getMp());
            }
            break;
        }
    }
}

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
