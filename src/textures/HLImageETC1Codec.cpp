//
//  HLETCCodec.cpp
//  HoolaiEngine
//
//  Created by zs_hoolai on 13-9-2.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLImageETC1Codec.h"
#include "KKGLConfiguration.h"
#include "KKUtil.h"

NS_KK_BEGIN

typedef struct {
    char magic[4];
    uint16_t version;
    uint16_t type;
    uint16_t extWidth;
    uint16_t extHeight;
    uint16_t orignWith;
    uint16_t orignHeight;
}PKMHeader;

#define ETC_PKM_HEADER_SIZE 16

HLImageETC1Codec::~HLImageETC1Codec()
{
    if (mData)
    {
        delete []mData;
    }
}

bool HLImageETC1Codec::loadWithFile(std::string file)
{
#ifdef GL_OES_compressed_ETC1_RGB8_texture
#if GL_OES_compressed_ETC1_RGB8_texture
    if (!KKGLConfiguration::getSingleton()->isSupportsETC())
    {
        KKLOG("Unsupported ETC format. Use the Legacy format until the new format is supported");
        return false;
    }
    
    std::string lowerCase(file);
    for (int i=0; i<lowerCase.length(); i++)
    {
        lowerCase[i] = tolower(lowerCase[i]);
    }
    
    KKFileData* filedata = KKResourceManager::getSingleton()->getFileData(file.c_str());
    if (!filedata)
    {
        KKLOG("file %s does not exist!\n", file.c_str());
        return false;
    }
    
    if (lowerCase.find(".ccz") != std::string::npos)
    {
        unsigned char* data = NULL;
        int len = inflateCCZFile(filedata, &data);
        if (!len)
        {
            KKLOG("file %s load error", file.c_str());
            if (data)
                delete []data;
            return false;
        }
        delete []filedata->buffer;
        filedata->buffer = data;
        filedata->size = len;
    }
    
    format = kTexturePixelFormat_ETC1_RGB8;
    hasPremultipliedAlpha = false;
    PKMHeader* header = reinterpret_cast<PKMHeader*>(filedata->buffer);
    if (header->magic[0] != 'P' || header->magic[1] != 'K' || header->magic[2] != 'M' || header->magic[3] != ' ')
    {
        KKLOG("data are not in valid PKM format: %s", file.c_str());
        return false;
    }
    BigEndianToNative<2>(&(header->orignWith));
    BigEndianToNative<2>(&(header->orignHeight));
    width = header->orignWith;
    height = header->orignHeight;
    mData = new unsigned char[filedata->size-ETC_PKM_HEADER_SIZE];
    memcpy(mData, filedata->buffer+ETC_PKM_HEADER_SIZE, filedata->size-ETC_PKM_HEADER_SIZE);
    
    MipmapPtr mipmap;
    mipmap.address = mData;
    mipmap.len = filedata->size-ETC_PKM_HEADER_SIZE;
    mipmaps.push_back(mipmap);
    
    delete filedata;
    
    return true;
#else
    KKLOG("Unsupported ETC format. Use the Legacy format until the new format is supported");
    return false;
#endif
#else
    KKLOG("Unsupported ETC format. Use the Legacy format until the new format is supported");
    return false;
#endif
}

NS_KK_END
