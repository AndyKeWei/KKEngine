//
//  KKFileUtil.cpp
//  KKEngine
//
//  Created by kewei on 3/14/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#include "KKFileUtil.hpp"
#include <string.h>
#include <ctype.h>

NS_KK_BEGIN

KKFileData::KKFileData(const char *filename, const char *mode)
{
    FILE *fp = fopen(filename, mode);
    if (fp) {
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        if (strchr(mode, 'b')) {
            buffer = new unsigned char[size];
            memset(buffer, '\0', size+1);
            size = fread(buffer, sizeof(unsigned char), size, fp);
        }
        else
        {
            buffer = new unsigned char[size+1];
            memset(buffer, '\0', size+1);
            size = fread(buffer, sizeof(unsigned char), size, fp);
        }
        fclose(fp);
    } else {
        KKLOG("read file %s error .", filename);
    }
}


void KKFileData::read(void *dst, size_t size)
{
    if (postion + size > this->size)
    {
        size = this->size - postion;
    }
    memcpy(dst, &buffer[postion], size);
    postion += size;
}

int KKFileData::readLine(void *dst)
{
    int pos = postion;
    if (pos >= size) {
        return -1;
    }
    for (; pos < size && buffer[pos] != '\n'; ++pos);
    int len = pos - postion;
    if (buffer[pos-1] == '\r') {
        --len;
    }
    memcpy(dst, &buffer[postion], len);
    postion = pos+1;
    return len;
}

void get_file_path(const char *filepath, char *path)
{
    const char *t = nullptr;
    unsigned int p;
#ifdef TARGET_WIN32
    strcpy(path, filepath);
    for (char *t1 = path; *t1 != '\0'; ++t1) {
        if (*t1 == '\\') {
            *t1 = '/';
        }
    }
    filepath = (const char *)path;
#endif
    t = strchr(filepath, '/');
    if (t) {
        p = (t - filepath) + 1;
        strncpy(path, filepath, p);
        path[ p ] = 0;
    }
    else
    {
        path[0] = '\0';
    }
}

void get_full_path(const char *file, char *fullpath)
{
#ifdef TARGET_WIN32
    if (file[1]==':'&&(file[2]=='\\'||file[2]=='/'))
    {
        strcpy(fullpath, file);
        return;
    }
#else
    if (file[0] == '/') {
        strcpy(fullpath, file);
        return;
    }
#endif
    strcpy(fullpath, getenv("FILESYSTEM"));
    strcat(fullpath, file);
}

float get_file_scale(const char *file)
{
#if ENABLE_RETINA_RESOURCE_SUFFIX
    char fullpath[PATH_MAX];
    get_full_path(file, fullpath);
    for (int i = strlen(fullpath) - 1; i > 3; i--) //(ie: icon@2x.png)
    {
        if (fullpath[i] == '.')
        {
            if (fullpath[i - 1] == 'x' && fullpath[i - 2] == '2' && fullpath[i - 3] == '@')
                return 2.f;
            else
                return 1.f;
        }
    }
#endif
    return 1.f;
}

void get_full_path_from_relative_file(const char* file, const char* relativefilepath, char* fullpath)
{
    get_file_path(relativefilepath, fullpath);
    strcat(fullpath, file);
}

void get_file_name(const char *filepath, char *filename)
{
    const char *p = strrchr(filepath, '/');
    if (p) {
        strcpy(filename, p+1);
    } else {
        strcpy(filename, filepath);
    }
}

void get_file_extension(const char *filepath, char *ext, unsigned char uppercase)
{
    const char *p = strrchr(filepath, '.');
    if (p) {
        strcpy(ext, p+1);
        if (uppercase) {
            unsigned int i = 0;
            size_t l = strlen(ext);
            while (i != l) {
                ext[i] = toupper(ext[i]);
                ++i;
            }
        }
    }
}


NS_KK_END