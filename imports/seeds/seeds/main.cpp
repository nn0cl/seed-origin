//
//  main.cpp
//  seeds
//
//  Created by nn0cl on 2013/12/14.
//  Copyright (c) 2013年 nn0cl. All rights reserved.
//

#include <sstream>
#include <string>
#include <iostream>

#include "Player.h"
#include "Status.h"
#include "Field.h"

void run() {
    Player *player  = new Player();

    Buff* mbuff[1024];
    for(int i = 0;i<10;i++){
        std::ostringstream stream;
        stream << "[[BuffName:";
        stream << i;
        stream << "]]";
        std::cout << stream.str() << std::endl;
        mbuff[i] = new Buff(i,stream.str(),0,0);
        Status *bufStatus = new Status(10.0f,10.0f);
        
        mbuff[i]->setStatus(*bufStatus);
        player->setBuff(*mbuff[i]);
    }
    
    while(player->processBuffs()){
        struct timespec  req = {1,0};
        nanosleep(&req,NULL);
    }
}

int main(int argc, const char * argv[])
{
    
    Field* mFields = Field::getInstance();
    
    while(1){
        mFields->processFrame();
    }

    return 0;
}

