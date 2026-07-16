//
//  File.cpp
//  seeds
//
//  Created by Suguru Yonemura on 11/10/30.
//  Copyright (c) 2011年 sumstates.net. All rights reserved.
//

#include <iostream>
#include "SeedBinaryTest.h"
#include "DataTest.h"
#include "FileManagerTest.h"

int main(int argc,char** argv)
{
    
    io::FileManager* fm = new io::FileManager();
    std::string fPath = "/Users/nn0cl/Downloads/Amo's style Rena.mp4";
    std::string dstPath = "/Users/nn0cl/Pictures/amoamo.mp4";
    char mode[4096];
    strcpy(mode,"rb");
    SeedBinary sb;
    fm->load(sb, fPath, mode);
    fm->save(sb, dstPath);
    delete fm;
    
    return 0;
    
    sleep(10);

    int tType = 1;
    int r = 0;
    
    SeedBinaryTest* sbt =new SeedBinaryTest();
    DataTest* dt = new DataTest();
    std::cout<<"This is a test Data"<<std::endl;
    
    if(tType== 0 ){
        r = dt->utData();
        if(r!=0){
            std::cout<<"ERROR " << std::endl;
        }
    }else if(tType==1){
        r = sbt->utSeed();
        if(r!=0){
            std::cout<<"ERROR" << std::endl;
        }
        
    }
    
    std::cout<<"End"<<std::endl;
     
    return 0;
}