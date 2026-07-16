//
//  File.cpp
//  seeds
//
//  Created by Suguru Yonemura on 11/10/30.
//  Copyright (c) 2011年 sumstates.net. All rights reserved.
//

#include <iostream>
#include "FileManager.h"

int main(int argc,char** argv)
{

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " INPUT OUTPUT\n";
        return 2;
    }
    io::FileManager fm;
    std::string fPath = argv[1];
    std::string dstPath = argv[2];
    SeedBinary sb;
    return fm.load(sb, fPath) && fm.save(sb, dstPath) ? 0 : 1;
}
