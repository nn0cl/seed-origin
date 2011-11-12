//
//  SeedBinary.cpp
//  seeds
//
//  Created by Suguru Yonemura on 11/10/30.
//  Copyright (c) 2011年 sumstates.net. All rights reserved.
//

#include <iostream>
#include "SeedBinary.h"

/** Constructor */
SeedBinary::SeedBinary()
{

}
/** Destructor */
SeedBinary::~SeedBinary(){
    
}

SeedBinary::SeedBinary(const SeedBinary& cpy){
    isValid = new bool;
    bData = cpy.bData;
}
/** バイナリーデータを指定されたアドレスへセットする。*/
int 
SeedBinary::setBinary(char data[STANDARD_BINARY_SIZE]){
    for(int i=0;i<STANDARD_BINARY_SIZE;++i){
        bData.data[i] = data[i];
    }
    isValid = true;
    return 0;
}
/** 指定されたアドレスのバイナリーデータを取得する。 */
int 
SeedBinary::getBinary(char* data,uint64_t idx,bool &isValid){
    getBinary(data);
    isValid = isValid;
    
    return 0;
}
int 
SeedBinary::getBinary(char* data){
    for(int i;i<STANDARD_BINARY_SIZE;++i){
        data[i] = bData.data[i];
    }
    return 0;
}
