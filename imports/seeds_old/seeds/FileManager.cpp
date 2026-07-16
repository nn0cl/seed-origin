//
//  FileManager.cpp
//  seeds
//
//  Created by Suguru Yonemura on 11/11/12.
//  Copyright (c) 2011年 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "FileManager.h"
namespace io{
    
    FileManager::FileManager(){
        
    }
    FileManager::~FileManager(){
        
    }
    int 
    FileManager::sublime(char *buff,std::string& fPath,char* rw){
    	FILE *fp;
        size_t size;       /* 読み込めたデータサイズ */
        fp = fopen( fPath.c_str(), rw );  /* ビットマップファイルを開く */
        if(fp!=NULL){
            int i=0;
            size = fread( buff, 1, 5000, fp );  /* 5000バイト分読み込む */
            for(i=0; i<size; ++i)  /* 実際に読み込めた分だけ繰り返す */
            {
                printf( "%d", buff[i] );  /* 10進整数として表示 */
            }
        }else {
            std::cout<<"file read faild"<<std::endl;
        }
        fclose( fp );
        
        return 1;        
    }

    
}