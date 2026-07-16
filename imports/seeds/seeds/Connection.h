//
//  Connection.h
//  seeds
//
//  Created by nn0cl on 2013/12/23.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#ifndef __seeds__Connection__
#define __seeds__Connection__

#include <iostream>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class Connection {
public:
    void open();
    void closeSocket();
};

#endif /* defined(__seeds__Connection__) */
