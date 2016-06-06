//
//  KKFileManager.hpp
//  KKEngine
//
//  Created by kewei on 3/14/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#ifndef KKFileManager_hpp
#define KKFileManager_hpp

#include <stdio.h>
#include "KKMacros.h"

NS_KK_BEGIN

class KKFileData {
private:
    unsigned int postion = {0};
public:
    unsigned char *buffer = {nullptr};
    unsigned long size = {0};
public:
    KKFileData(){}
    KKFileData(const char *filename, const char *mode);
    
    ~KKFileData()
    {
        if (buffer) {
            delete buffer;
        }
    }
    
    void seek(unsigned int pos)
    {
        postion = pos;
    }
    
    void read(void *dst, size_t size);
    int readLine(void *dst);
};

void get_file_path(const char *filepath, char *path);
void get_full_path(const char *file, char *fullpath);
float get_file_scale(const char *file);
void get_full_path_from_relative_file(const char* file, const char* relativefilepath, char* fullpath);
void get_file_name(const char *filepath, char *filename);
void get_file_extension(const char *filepath, char *ext, unsigned char uppercase);


NS_KK_END

#endif /* KKFileManager_hpp */
