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
#include <stdint.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

enum class AcceptStatus {
    Accepted,
    NoPendingClient,
    Failed
};

class Connection {
private:
    int listenerSocket;
public:
    Connection();
    ~Connection();
    bool open(uint16_t port);
    AcceptStatus acceptClient(int& clientSocket);
    bool closeSocket();
    bool isOpen() const;
};

#endif /* defined(__seeds__Connection__) */
