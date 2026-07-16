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
#include <string.h>

namespace io{
    class FileManager{
    public:
        FileManager();
        ~FileManager();
        int sublime(char *buff,std::string& fPath,char* rw);
    };
}

#endif
