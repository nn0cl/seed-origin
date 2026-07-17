//
//  Logger.h
//  seeds
//
//  Created by nn0cl on 2013/12/14.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#ifndef seeds_Logger_h
#define seeds_Logger_h

#include <iostream>
#include <string>

class Logger{
public:
    static void log(int _id,std::string str) {
       (void)_id;
       //std::cout << _id << str << std::endl;
        std::cout << str << std::endl;
    }
};

#endif
