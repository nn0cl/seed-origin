//
//  FileManager.cpp
//  seeds
//
//  Created by Suguru Yonemura on 11/11/12.
//  Copyright (c) 2011年 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdint.h>

#include <vector>
#include "SeedBinary.h"
#include "FileManager.h"

namespace io{

    FileManager::FileManager(){

    }
    FileManager::~FileManager(){

    }
    int
    FileManager::load(SeedBinary& sb,std::string& fPath,char* rw){

        std::ifstream fin(fPath.c_str(),std::ios::in | std::ios::binary );
        if (!fin) {
            std::cout<<"ERROR"<<std::endl;
            return 0;
        }

        fin.seekg(0, std::ios::end);
        std::streamoff eofPos = fin.tellg();
        fin.seekg(0, std::ios::beg);
        std::streamoff begPos = fin.tellg();

        uint64_t size = eofPos - begPos;

        int idx=0;
        char ch[STANDARD_BINARY_SIZE];
        for (std::streamoff offset = 0; offset < size; offset += STANDARD_BINARY_SIZE, ++idx) {
            const std::streamsize readsize = static_cast<std::streamsize>(
                std::min<std::streamoff>(STANDARD_BINARY_SIZE, size - offset));
            fin.read(ch, readsize);
            if (fin.gcount() != readsize) return 0;
            sb.setBinary(ch, static_cast<size_t>(readsize), idx);

        }
        fin.close();

        return 1;
    }
    int
    FileManager::save(SeedBinary& sb,std::string& dstPath){

        std::ofstream fout(dstPath.c_str(),std::ios::out | std::ios_base::trunc | std::ios::binary );
        if (!fout) return 0;
        int max_index;
        if (!sb.getFileIndex(max_index)) return 0;
        for(int i=0;i<max_index;++i){
            bool isValid;
            size_t size;
            char ch[1024];
            sb.getBinary(ch,size,i,isValid);
            if(isValid==true){
                fout.write(ch,size);
            }
            if (max_index > 0) {
                std::cout<<"WRITING"<<i<<"/"<<max_index<<":"<<i*100/max_index<<"[%]"<<std::endl;
            }
        }
        fout.close();

    return 1;
    }
}
