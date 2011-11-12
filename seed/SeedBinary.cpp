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
    strncpy(bData.data,data,sizeof(bData.data)/sizeof(char));
        isValid = true;
    return 0;
}
/** 指定されたアドレスのバイナリーデータを取得する。 */
int 
SeedBinary::getBinary(char* &data,bool &isValid){
    getBinary(data);
    isValid = isValid;
    
    return 0;
}
int 
SeedBinary::getBinary(char* &data){
    strncpy(data,bData.data,sizeof(data));
    return 0;
}
