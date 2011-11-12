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
    FileManager::load(SeedBinary& sb,std::string& fPath,char* rw){
        
        ifstream fin(fPath.c_str(),std::ios::in | std::ios::binary );
        if(fin.eof()){
            std::cout<<"ERROR"<<std::endl;
            return 0;
        }
               
        fin.seekg(0, fstream::end);
        uint64_t eofPos = fin.tellg();
        fin.clear();
        
        fin.seekg(0, std::fstream::beg);
        uint64_t begPos = fin.tellg();
        fin.clear();
        
        uint64_t size = eofPos - begPos;
        
        int idx=0;
        char ch[STANDARD_BINARY_SIZE];
        size_t readsize = STANDARD_BINARY_SIZE*sizeof(char);;
        for(idx=0;idx*STANDARD_BINARY_SIZE*sizeof(char)<size;++idx){
            memset(ch,NULL,STANDARD_BINARY_SIZE);
            fin.seekg((size_t)(idx*STANDARD_BINARY_SIZE*sizeof(char)));  //ポインタの位置を移動
            if((idx+1)*STANDARD_BINARY_SIZE*sizeof(char)>=size){
                readsize = size-idx*STANDARD_BINARY_SIZE*sizeof(char);
            }
            fin.read(ch,readsize);  //文字列ではないデータを読みこむ
            if( fin.fail() ){
                printf( "ERROR:\n");
                break;
            }
            sb.setBinary(ch,readsize,idx);

        }
        fin.close();
                
        return 1;        
    }    
    int
    FileManager::save(SeedBinary& sb,std::string& dstPath){
        
        ofstream fout(dstPath.c_str(),std::ios::out | std::ios_base::trunc | std::ios::binary );
        int max_index;
        sb.getFileIndex(max_index);
        for(int i=0;i<max_index;++i){
            bool isValid;
            size_t size;
            char ch[1024];
            sb.getBinary(ch,size,i,isValid);
            if(isValid==true){
                fout.write(ch,size);
            }
            std::cout<<"WRITING"<<i<<"/"<<max_index<<":"<<i*100/max_index<<"[%]"<<std::endl;
        }
        fout.close();
        
    return 1;
    }
}