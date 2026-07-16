//
//  FileManagerTest.cpp
//  seeds
//
//  Created by Suguru Yonemura on 11/11/12.
//  Copyright (c) 2011年 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "FileManagerTest.h"

int fileManagerTest()
{
    io::FileManager* fm = new io::FileManager();
    char buff[4096];
    //strcpy(buff,NULL);
    std::string fPath = "/Users/nn0cl/Pictures/Konachan.com - 72141 blame cibo killy monochrome.jpg";
    char mode[4096];
    strcpy(mode,"rb");
    fm->sublime(buff, fPath, mode);
    
    delete fm;
    return 0;
}