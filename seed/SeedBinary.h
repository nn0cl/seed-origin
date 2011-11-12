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

typedef struct{
    int64_t idx;
    char data[STANDARD_BINARY_SIZE];
} Binary;

class SeedBinary{
private:
    bool isValid;
    Binary bData;
public:
    /** Constructor */
    SeedBinary();
    /** Destructor */
    ~SeedBinary();
    SeedBinary(const SeedBinary& cpy);
    /** バイナリーデータを指定されたアドレスへセットする。*/
    int setBinary(char data[STANDARD_BINARY_SIZE]);
    /** 指定されたアドレスのバイナリーデータを取得する。 */
    int getBinary(char* &data,bool &isValid);
    int getBinary(char* &data);
};

#endif
