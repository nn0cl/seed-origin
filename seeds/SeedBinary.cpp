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
/** バイナリーデータを指定されたアドレスへセットする。*/
int 
SeedBinary::setBinary(const Binary data){
    strncpy(bData,data,sizeof(bData));
    isValid = true;
    return 0;
}
/** 指定されたアドレスのバイナリーデータを取得する。 */
int 
SeedBinary::getBinary(Binary &binary,bool &isValid){
    getBinary(binary);
    isValid = this->isValid;
    return 0;
}
int 
SeedBinary::getBinary(Binary &binary){
    strncpy(binary,bData,sizeof(binary));
    return 0;
}