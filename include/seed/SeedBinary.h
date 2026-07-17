//
//  SeedBinary.h
//  seeds
//
//  Created by Suguru Yonemura on 11/10/30.
//  Copyright (c) 2011年 sumstates.net. All rights reserved.
//

#ifndef seeds_SeedBinary_h
#define seeds_SeedBinary_h

#include <cstddef>
#include <cstdint>
#include <map>

inline constexpr std::size_t STANDARD_BINARY_SIZE = 1024;

struct Binary {
    bool isValid;
    std::size_t size;
    char data[STANDARD_BINARY_SIZE];
};

class SeedBinary{
private:
    std::map<int,Binary> bData;
    int fIndex; //ファイルインデックスサイズ
public:
    /** Constructor */
    SeedBinary();
    /** Destructor */
    ~SeedBinary();
    /** バイナリーデータを指定されたインデックスへセットする。*/
    int setBinary(const char data[STANDARD_BINARY_SIZE],size_t size,int idx);
    /** 指定されたアドレスのバイナリーデータを取得する。 */
    int getBinary(char data[STANDARD_BINARY_SIZE],size_t& size,int idx,bool &isValid) const;
    /** ファイルインデックスサイズを取得する */
    int getFileIndex(int& fIndex) const;
};

#endif
