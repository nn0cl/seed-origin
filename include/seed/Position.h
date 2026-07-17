//
//  Position.h
//  seeds
//
//  Created by nn0cl on 2013/12/15.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#ifndef seeds_Position_h
#define seeds_Position_h

#include <cstdint>

class Position{
private:
    float x;
    float y;
    float z;
    int mapId;
    int64_t playerId;

public:
    Position(int64_t playerId, float x, float y, float z);
    Position(const Position* position);
    ~Position() = default;
    void setPosition(const float x, const float y, const float z);
    void movePosition(const float x, const float y, const float z);
    int64_t getPlayerId() const;
    float getX() const;
    float getY() const;
    float getZ() const;
};

#endif
