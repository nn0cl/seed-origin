//
//  SeedBinaryTest.cpp
//  seeds
//
//  Created by Suguru Yonemura on 11/10/30.
//  Copyright (c) 2011年 sumstates.net. All rights reserved.
//

#include "SeedBinaryTest.h"

int 
SeedBinaryTest::utSeed()
{
    
    char inputData[STANDARD_BINARY_SIZE];
    strncpy(inputData,"AAbbcdcddslfkjadsf;lakjdfal;dfk",sizeof(inputData));
    
    SeedBinary* sdb = new SeedBinary();
    
    sdb->setBinary(inputData);
    
    char outputData[STANDARD_BINARY_SIZE];
    sdb->getBinary(outputData);
    
    std::cout<<outputData<<std::endl;
    
    return 0;
}