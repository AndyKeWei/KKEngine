//
//  KKUtil.cpp
//  
//
//  Created by kewei on 1/11/16.
//	Copyright (c) 2014 kw. All rights reserved.
//

#include "KKUtil.h"
#include "KKFileUtil.hpp"
#include <zlib.h>
#include <stdint.h>
#include "KKGeometry.h"
#include <iostream>

extern pthread_t mainthread;

NS_KK_BEGIN

bool currentIsMainThread()
{
    return pthread_equal(mainthread, pthread_self());
}

const char* getLanguageStr(KKLanguageType type)
{
    static const char* strs[] =
    {
        "en", "zh-Hant", "zh-Hans", "fr",
        "it", "de", "es", "ru", "ko", "ja",
        "hu", "pt", "ar", "tr", "pl"
    };
    if (type < 15)
    {
        return strs[type];
    }
    
    return "unknown";
}

NS_KK_END

float clampf(float value, float min_inclusive, float max_inclusive)
{
    if (min_inclusive > max_inclusive) {
        float tmp = min_inclusive;
        min_inclusive = max_inclusive;
        max_inclusive = tmp;
    }
    return value < min_inclusive ? min_inclusive : value < max_inclusive? value : max_inclusive;
}

struct CCZHeader {
    uint8_t            sig[4];                // signature. Should be 'CCZ!' 4 bytes
    uint16_t            compression_type;    // should 0
    uint16_t            version;            // should be 2 (although version type==1 is also supported)
    uint32_t             reserved;            // Reserverd for users.
    uint32_t            len;                // size of the uncompressed file
};

enum {
    CCZ_COMPRESSION_ZLIB,                // zlib format.
    CCZ_COMPRESSION_BZIP2,                // bzip2 format (not supported yet)
    CCZ_COMPRESSION_GZIP,                // gzip format (not supported yet)
    CCZ_COMPRESSION_NONE,                // plain (not supported yet)
};

//int inflateGZipFile(hoolai::KKFileData *data, unsigned char **out)
//{
//    int len;
//    unsigned int offset = 0;
//
//    HLASSERT(out, "");
//    HLASSERT(&*out, "");
//
//    gzFile inFile = gzopen(path, "rb");
//    if( inFile == NULL ) {
//        KKLOG("ZipUtils: error open gzip file: %s", path);
//        return 0;
//    }
//
//    /* 512k initial decompress buffer */
//    unsigned int bufferSize = 512 * 1024;
//    unsigned int totalBufferSize = bufferSize;
//
//    *out = (unsigned char*)malloc( bufferSize );
//    if( ! out )
//    {
//        KKLOG("ZipUtils: out of memory %s", path);
//        return 0;
//    }
//
//    for (;;) {
//        len = gzread(inFile, *out + offset, bufferSize);
//        if (len < 0)
//        {
//            KKLOG("ZipUtils: error in gzread %s", path);
//            free( *out );
//            *out = NULL;
//            return -1;
//        }
//        if (len == 0)
//        {
//            break;
//        }
//
//        offset += len;
//
//        // finish reading the file
//        if( (unsigned int)len < bufferSize )
//        {
//            break;
//        }
//
//        bufferSize *= 2;
//        totalBufferSize += bufferSize;
//        unsigned char *tmp = (unsigned char*)realloc(*out, totalBufferSize );
//
//        if( ! tmp )
//        {
//            KKLOG("ZipUtils: out of memory %s", path);
//            free( *out );
//            *out = NULL;
//            return 0;
//        }
//
//        *out = tmp;
//    }
//
//    if (gzclose(inFile) != Z_OK)
//    {
//        KKLOG("ZipUtils: gzclose failed %s", path);
//    }
//
//    return offset;
//}
USING_NS_KK
int inflateCCZFile(KKFileData *data, unsigned char **out)
{
    KKASSERT(out, "");
    KKASSERT(&*out, "");
    
    // load file into memory
    unsigned char* compressed = NULL;
    
    unsigned long fileLen = 0;
    //    char fullPath[MAX_PATH];
    //    hoolai::get_full_path(path, fullPath);
    //    hoolai::KKFileData data(fullPath, "rb");
    compressed = data->buffer;
    fileLen = data->size;
    
    if(NULL == compressed || 0 == fileLen)
    {
        KKLOG("Error loading CCZ compressed file");
        return 0;
    }
    
    struct CCZHeader *header = (struct CCZHeader*) compressed;
    
    // verify header
    if( header->sig[0] != 'C' || header->sig[1] != 'C' || header->sig[2] != 'Z' || header->sig[3] != '!' )
    {
        KKLOG("Invalid CCZ file");
        return 0;
    }
    
    // verify header version
    BigEndianToNative<sizeof(header->version)>(&header->version);
    //    unsigned int version = SWAP_INT16_BIG_TO_HOST(  );
    if( header->version > 2 )
    {
        KKLOG("Unsupported CCZ header format");
        return 0;
    }
    
    // verify compression format
    BigEndianToNative<sizeof(header->compression_type)>(&header->compression_type);
    if( header->compression_type != CCZ_COMPRESSION_ZLIB )
    {
        KKLOG("CCZ Unsupported compression method");
        return 0;
    }
    
    BigEndianToNative<sizeof(header->len)>(&header->len);
    
    *out = new unsigned char[header->len];
    if(! *out )
    {
        KKLOG("CCZ: Failed to allocate memory for texture");
        return 0;
    }
    
    
    unsigned long destlen = header->len;
    unsigned long source = (unsigned long) compressed + sizeof(*header);
    int ret = uncompress(*out, &destlen, (Bytef*)source, fileLen - sizeof(*header) );
    
    if( ret != Z_OK )
    {
        KKLOG("CCZ: Failed to uncompress data");
        free( *out );
        *out = NULL;
        return 0;
    }
    
    return header->len;
}

unsigned long nextPOT(unsigned long x)
{
    x = x - 1;
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >>16);
    return x + 1;
}

void EndianSwitch(int size, void* p)
{
    uint8_t* bytes = static_cast<uint8_t*>(p);
    for (int i = 0; i < size/2; ++i)
    {
        std::swap(bytes[i], bytes[size-i-1]);
    }
    
}

int pnpoly(int nvert, KKPoint *poly, KKPoint test)
{
    int i, j, c = 0;
    for (i = 0, j = nvert-1; i < nvert; j = i++) {
        if ( ((poly[i].y>test.y) != (poly[j].y>test.y)) &&
            (test.x < (poly[j].x-poly[i].x) * (test.y-poly[i].y) / (poly[j].y-poly[i].y) + poly[i].x) )
            c = !c;
    }
    return c;
}