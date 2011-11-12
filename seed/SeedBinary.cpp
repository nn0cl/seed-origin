//
//  SeedBinary.cpp
//  seeds
//
//  Created by Suguru Yonemura on 11/10/30.
//  Copyright (c) 2011年 sumstates.net. All rights reserved.
//

#include <iostream>
#include "SeedBinary.h"
#include <map>
/** Constructor */
SeedBinary::SeedBinary()
{
    fIndex = 0;
}
/** Destructor */
SeedBinary::~SeedBinary(){
    
}
/*
SeedBinary::SeedBinary(const SeedBinary& cpy){
    isValid = new bool;
    bData = cpy.bData;
}*/
/** バイナリーデータを指定されたアドレスへセットする。*/
int 
SeedBinary::setBinary(char data[STANDARD_BINARY_SIZE],size_t size,int idx){
    
    Binary mData;
    for(int i=0;i<(int)size;++i){
        mData.data[i] = data[i];
    }
    mData.size = size;
    mData.isValid = true;
    
    std::map<int,Binary>::iterator iter = bData.find(idx);
    if(iter != bData.end()){
        bData.erase(iter++);
    }
    bData.insert(std::pair<int,Binary>(idx,mData));
    ++fIndex;
    return 1;
}
/** 指定されたアドレスのバイナリーデータを取得する。 */
int 
SeedBinary::getBinary(char data[STANDARD_BINARY_SIZE],size_t& size,int& idx,bool &isValid){
 
    std::map<int,Binary>::iterator iter = bData.find(idx);
    if(iter == bData.end()){
        return 0;
    }
    for(int i=0;i<STANDARD_BINARY_SIZE;++i){
     data[i] = iter->second.data[i];
    }
    isValid = iter->second.isValid;
    size = iter->second.size;
    return 1;
}
int 
SeedBinary::getFileIndex(int& m_fIndex){
    if(fIndex == 0){
        return 0;
    }
    
    m_fIndex = fIndex;
    return 1;
}