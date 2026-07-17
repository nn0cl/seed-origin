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
#include <cstdint>

#include <vector>
#include <cctype>
#include <cstdlib>
#include <climits>
#include <unistd.h>
#include "SeedBinary.h"
#include "FileManager.h"

namespace io{

    namespace {
        bool normalizeInputPath(const std::string& path, std::string& normalized) {
            if (path.empty() || path.find('\0') != std::string::npos) return false;
            char resolved[PATH_MAX];
            if (realpath(path.c_str(), resolved) == nullptr) return false;
            normalized.assign(resolved);
            return true;
        }

        bool normalizeOutputPath(const std::string& path, std::string& normalized) {
            if (path.empty() || path.find('\0') != std::string::npos) return false;

            const std::string::size_type separator = path.find_last_of('/');
            const std::string parent = separator == std::string::npos
                ? "." : path.substr(0, separator);
            const std::string filename = separator == std::string::npos
                ? path : path.substr(separator + 1);
            if (filename.empty() || filename == "." || filename == "..") return false;
            for (std::string::const_iterator it = filename.begin(); it != filename.end(); ++it) {
                if (!isalnum(static_cast<unsigned char>(*it)) && *it != '.' &&
                    *it != '_' && *it != '-') {
                    return false;
                }
            }

            char resolvedParent[PATH_MAX];
            if (realpath(parent.c_str(), resolvedParent) == nullptr) return false;
            normalized.assign(resolvedParent);
            normalized.append("/");
            normalized.append(filename);
            return true;
        }
    }

    FileManager::FileManager(){

    }
    FileManager::~FileManager(){

    }
    int
    FileManager::load(SeedBinary& sb,const std::string& fPath){

        std::string normalizedPath;
        if (!normalizeInputPath(fPath, normalizedPath)) return 0;

        std::ifstream fin(normalizedPath.c_str(),std::ios::in | std::ios::binary );
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

        const std::streamoff size = eofPos - begPos;

        int idx=0;
        char ch[STANDARD_BINARY_SIZE];
        for (std::streamoff offset = 0; offset < size; offset += STANDARD_BINARY_SIZE, ++idx) {
            if (idx == std::numeric_limits<int>::max()) return 0;
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

        std::string normalizedPath;
        if (!normalizeOutputPath(dstPath, normalizedPath)) return 0;

        std::ofstream fout(normalizedPath.c_str(),std::ios::out | std::ios_base::trunc | std::ios::binary );
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
