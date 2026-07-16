//
//  FileManager.h
//  seeds
//
//  Created by Suguru Yonemura on 11/11/12.
//  Copyright (c) 2011年 __MyCompanyName__. All rights reserved.
//

#ifndef seeds_FileManager_h
#define seeds_FileManager_h

#include <iostream>
#include <string>
#include "SeedBinary.h"

namespace io{
    class FileManager{
    public:
        FileManager();
        ~FileManager();
        int load(SeedBinary& sb,const std::string& fPath);
        int save(const SeedBinary& sb,const std::string& dstPath);
    };
}

#endif
