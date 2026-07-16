//
//  Connection.cpp
//  seeds
//
//  Created by nn0cl on 2013/12/23.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#include "Connection.h"
#include <unistd.h>

Connection::Connection() : listenerSocket(-1) {}

Connection::~Connection() {
    closeSocket();
}

bool
Connection::open(uint16_t port) {
    if (listenerSocket >= 0) return false;

    int sock0 = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {};
    int yes = 1;

    if (sock0 < 0) {
        perror("socket");
        return false;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR,
                   (const char *)&yes, sizeof(yes)) != 0) {
        ::close(sock0);
        return false;
    }

    if (bind(sock0, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        perror("bind");
        ::close(sock0);
        return false;
    }

    if (listen(sock0, 5) != 0) {
        perror("listen");
        ::close(sock0);
        return false;
    }

    listenerSocket = sock0;
    return true;
}

bool
Connection::closeSocket() {
    if (listenerSocket < 0) return true;
    const int result = ::close(listenerSocket);
    listenerSocket = -1;
    return result == 0;
}

bool
Connection::isOpen() const {
    return listenerSocket >= 0;
}
