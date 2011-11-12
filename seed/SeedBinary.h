//
//  SeedBinary.h
//  seeds
//
//  Created by Suguru Yonemura on 11/10/30.
//  Copyright (c) 2011年 sumstates.net. All rights reserved.
//

#ifndef seeds_SeedBinary_h
#define seeds_SeedBinary_h

#define STANDARD_BINARY_SIZE 4096

class SeedBinary{
private:
    typedef char Binary[STANDARD_BINARY_SIZE];
    bool isValid;
    Binary bData;
public:
    /** Constructor */
    SeedBinary();
    /** Destructor */
    ~SeedBinary();
    /** バイナリーデータを指定されたアドレスへセットする。*/
    int setBinary(const Binary data);
    /** 指定されたアドレスのバイナリーデータを取得する。 */
    int getBinary(Binary &binary,bool &isValid);
    int getBinary(Binary &binary);
};

#endif
