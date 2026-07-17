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
#include <limits>
/** Constructor */
SeedBinary::SeedBinary()
{
    fIndex = 0;
}
/** Destructor */
SeedBinary::~SeedBinary() = default;
/** バイナリーデータを指定されたアドレスへセットする。*/
int 
SeedBinary::setBinary(const char data[STANDARD_BINARY_SIZE],size_t size,int idx){
    if (data == nullptr || size > STANDARD_BINARY_SIZE || idx < 0 ||
        idx == std::numeric_limits<int>::max()) {
        return 0;
    }

    Binary mData{};
    for(int i=0;i<(int)size;++i){
        mData.data[i] = data[i];
    }
    mData.size = size;
    mData.isValid = true;
    
    bData[idx] = mData;
    if (idx >= fIndex) fIndex = idx + 1;
    return 1;
}
/** 指定されたアドレスのバイナリーデータを取得する。 */
int 
SeedBinary::getBinary(char data[STANDARD_BINARY_SIZE],size_t& size,int idx,bool &isValid) const{
    if (data == nullptr || idx < 0) {
        return 0;
    }
 
    std::map<int,Binary>::const_iterator iter = bData.find(idx);
    if(iter == bData.end()){
        return 0;
    }
    for(std::size_t i = 0; i < STANDARD_BINARY_SIZE; ++i){
     data[i] = iter->second.data[i];
    }
    isValid = iter->second.isValid;
    size = iter->second.size;
    return 1;
}
int 
SeedBinary::getFileIndex(int& m_fIndex) const{
    m_fIndex = fIndex;
    return 1;
}
