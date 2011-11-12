//
//  SeedBinary.h
//  seeds
//
//  Created by Suguru Yonemura on 11/10/30.
//  Copyright (c) 2011年 sumstates.net. All rights reserved.
//

#ifndef seeds_SeedBinary_h
#define seeds_SeedBinary_h

#define STANDARD_BINARY_SIZE 1024
#include <stdint.h>
#include <map>

typedef struct{
    bool isValid;
    size_t size;
    char data[STANDARD_BINARY_SIZE];
} Binary;

class SeedBinary{
private:
    std::map<int,Binary> bData;
    int fIndex; //ファイルインデックスサイズ
public:
    /** Constructor */
    SeedBinary();
    /** Destructor */
    ~SeedBinary();
    //SeedBinary(const SeedBinary& cpy);
    /** バイナリーデータを指定されたインデックスへセットする。*/
    int setBinary(char data[STANDARD_BINARY_SIZE],size_t size,int idx);
    /** 指定されたアドレスのバイナリーデータを取得する。 */
    int getBinary(char data[STANDARD_BINARY_SIZE],size_t& size,int &idx,bool &isValid);
    /** ファイルインデックスサイズを取得する */
    int getFileIndex(int& fIndex);
};

#endif
