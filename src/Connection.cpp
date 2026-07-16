//
//  Connection.cpp
//  seeds
//
//  Created by nn0cl on 2013/12/23.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#include "Connection.h"
#include <unistd.h>

void
Connection::open() {

    int sock0;
    struct sockaddr_in addr;
    int yes = 1;

    sock0 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock0 < 0) {
        perror("socket");
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = INADDR_ANY;

    setsockopt(sock0,
               SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));

    if (bind(sock0, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        perror("bind");
        ::close(sock0);
        return;
    }

    if (listen(sock0, 5) != 0) {
        perror("listen");
        ::close(sock0);
        return;
    }

    // This legacy method only prepares the listener; request handling is not implemented.
    ::close(sock0);
};

void
Connection::closeSocket() {

};
