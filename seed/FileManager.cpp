//
//  FileManager.cpp
//  seeds
//
//  Created by Suguru Yonemura on 11/11/12.
//  Copyright (c) 2011年 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <fstream.h>
#include <stdint.h>

#include <vector>
#include "Seedbinary.h"
#include "FileManager.h"

namespace io{
    
    FileManager::FileManager(){
        
    }
    FileManager::~FileManager(){
        
    }
    int 
    FileManager::sublime(char *buff,std::string& fPath,char* rw){
        
        ifstream fin(fPath.c_str(),std::ios::in | std::ios::binary );
        if(fin.eof()){
            std::cout<<"ERROR"<<std::endl;
            return -1;
        }
               
        fin.seekg(0, fstream::end);
        uint64_t eofPos = fin.tellg();
        fin.clear();
        
        fin.seekg(0, std::fstream::beg);
        uint64_t begPos = fin.tellg();
        fin.clear();

        uint64_t size = eofPos - begPos;
        std::vector<SeedBinary> sVec;
        
        for(int idx=0;idx*STANDARD_BINARY_SIZE*sizeof(char)<size;idx++){
          char ch[STANDARD_BINARY_SIZE];
          fin.seekg (idx*STANDARD_BINARY_SIZE*sizeof(char));  //ポインタの位置を移動
          fin.read(ch,STANDARD_BINARY_SIZE*sizeof(char));  //文字列ではないデータを読みこむ
            SeedBinary msb;
            msb.setBinary(ch);
            
            sVec.push_back(msb);
        }
        
        fin.close();
        
        return 0;        
    }

    
}