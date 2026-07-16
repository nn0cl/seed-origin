//
//  Buff.h
//  seeds
//
//  Created by nn0cl on 2013/12/14.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#ifndef seeds_Buff_h
#define seeds_Buff_h

#include <stdio.h>
#include <sstream>
#include <string>
#include "Status.h"
#include "Logger.h"

class Buff{
private:
    int id;
    std::string buffName;
    // 有効期限
    long lifetime;
    // 効果開始日時
    long startTime;
    
    Status* status;
    
public:
    Buff(int _id,
         std::string _buffName,
         long _lifetime,
         long _startTime
         );
    ~Buff();
    
    void setStatus(const Status& status);
    Status* getStatus();
};

#endif
