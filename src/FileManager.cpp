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
    FileManager::load(SeedBinary& sb,const std::string& fPath){

        std::ifstream fin(fPath.c_str(),std::ios::in | std::ios::binary );
        if (!fin) {
            std::cout<<"ERROR"<<std::endl;
            return 0;
        }

        fin.seekg(0, std::ios::end);
        std::streamoff eofPos = fin.tellg();
        if (eofPos < 0) return 0;
        fin.seekg(0, std::ios::beg);
        std::streamoff begPos = fin.tellg();
        if (begPos < 0 || eofPos < begPos) return 0;

        uint64_t size = eofPos - begPos;

        int idx=0;
        char ch[STANDARD_BINARY_SIZE];
        for (std::streamoff offset = 0; offset < size; offset += STANDARD_BINARY_SIZE, ++idx) {
            const std::streamsize readsize = static_cast<std::streamsize>(
                std::min<std::streamoff>(STANDARD_BINARY_SIZE, size - offset));
            fin.read(ch, readsize);
            if (fin.gcount() != readsize) return 0;
            if (!sb.setBinary(ch, static_cast<size_t>(readsize), idx)) return 0;

        }
        fin.close();

        return 1;
    }
    int
    FileManager::save(const SeedBinary& sb,const std::string& dstPath){

        std::ofstream fout(dstPath.c_str(),std::ios::out | std::ios_base::trunc | std::ios::binary );
        if (!fout) return 0;
        int max_index = 0;
        if (!sb.getFileIndex(max_index) || max_index < 0) return 0;
        for(int i=0;i<max_index;++i){
            bool isValid = false;
            size_t size = 0;
            char ch[STANDARD_BINARY_SIZE]{};
            if (!sb.getBinary(ch,size,i,isValid) || !isValid ||
                size > STANDARD_BINARY_SIZE) return 0;
            fout.write(ch, static_cast<std::streamsize>(size));
            if (!fout) return 0;
            if (max_index > 0) {
                std::cout<<"WRITING"<<i<<"/"<<max_index<<":"<<i*100/max_index<<"[%]"<<std::endl;
            }
        }
        fout.flush();
        if (!fout) return 0;
        fout.close();
        if (fout.fail()) return 0;

    return 1;
    }
}
