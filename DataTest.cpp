//
//  DataTest.cpp
//  seed_base
//
//  Created by Yonemura Suguru on 11/10/25.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "Data.h"

int
main(int argc,char** argv)
{
    //typdef for local
    typedef std::map<int64_t,int64_t> Node;
    
    std::cout<<"Data Input Start"<<std::endl;
    Node cMap;
    Data* dt = new Data();
    for(int64_t i=0;i<1000;++i){
        int64_t key = i;
        int64_t val = 100+i;
        std::cout<<"key:"<<key<<"value:"<<val<<std::endl;
        dt->setParams(cMap,key,val);    
    }
    dt->setMap(1,cMap);

    Node rMap;
    dt->getMap(1,rMap);
    int64_t value;
    for(int64_t i=0;i<100;++i){
        dt->getParams(cMap,i,value);
        std::cout<<value<<std::endl;
    }

    
    return 0;
}