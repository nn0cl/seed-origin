//
//  Status.h
//  seeds
//
//  Created by nn0cl on 2013/12/14.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#ifndef seeds_Status_h
#define seeds_Status_h

class Status {
private:
    long hp;
    long mp;
public:
    Status();
    Status(long hp,long mp);
    ~Status();
    Status(const Status& _status);
    bool setHp(const long& _hp);
    bool setMp(const long& _mp);
    long getHp() const;
    long getMp() const;
    bool gainHp(const long& _hp);
    bool gainMp(const long& _mp);
};

#endif
