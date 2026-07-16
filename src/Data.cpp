//
//  Data.cpp
//  seed_base
//
//  Created by Yonemura Suguru on 11/10/25.
//  Copyright 2011年 sumstates.net. All rights reserved.
//

#include <iostream>
#include "Data.h"

int
Data::setMap(int64_t key,Node& node)
{
    dataMap[key] = node;
    return 0;
}

int 
Data::setParams(Node &node,int64_t key,int64_t value)
{    
    node.insert(std::pair<int64_t,int64_t>(key,value));
    return 0;
}
int 
Data::getMap(int64_t key,Node& node)
{
    std::map<int64_t,Node>::iterator itData = dataMap.find(key);
    if(itData == dataMap.end()){
        std::cout<<"no data found!" << std::endl;
        return -1;
    }
    
    node = itData->second;
    return 0;
    
}

int
Data::getParams(Node &node,int64_t key,int64_t &value)
{
    Node::iterator itData = node.find(key);
    if (itData == node.end()) {
        return -1;
    }
    value = itData->second;
    return 0;
}
