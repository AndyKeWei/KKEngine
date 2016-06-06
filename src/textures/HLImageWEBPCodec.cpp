//
//  HLImageWEBPCodec.cpp
//  HoolaiEngine
//
//  Created by zs_hoolai on 13-8-10.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLImageWEBPCodec.h"
#include "KKResourceManager.hpp"
#include "KKFileUtil.hpp"
#include "webp/decode.h"

NS_KK_BEGIN

bool HLImageWEBPCodec::mFastDecodeMode = false;

HLImageWEBPCodec::~HLImageWEBPCodec()
{
    if (mData)
    {
        delete []mData;
    }
}

void HLImageWEBPCodec::useFastDecodeMode(bool use)
{
    mFastDecodeMode = use;
}

bool HLImageWEBPCodec::loadWithFile(std::string file)
{
    KKFileData* filedata = KKResourceManager::getSingleton()->getFileData(file.c_str());
    if (!filedata)
    {
        KKLOG("file %s does not exist!\n", file.c_str());
        return false;
    }
    
    WebPDecoderConfig config;
    if (WebPInitDecoderConfig(&config) == 0) return false;
    if (WebPGetFeatures((uint8_t*)filedata->buffer, filedata->size, &config.input) != VP8_STATUS_OK) return false;
    if (config.input.width == 0 || config.input.height == 0) return false;
    
    hasPremultipliedAlpha = false;
    
    int bytesPerComp = 4;
    
    if (config.input.has_alpha)
    {
        format = kTexturePixelFormat_RGBA8888;
        config.output.colorspace = MODE_RGBA;
    }
    else
    {
        bytesPerComp = 3;
        format = kTexturePixelFormat_RGB888;
        config.output.colorspace = MODE_RGB;
    }
    
    width    = config.input.width;
    height   = config.input.height;
    
    int bufferSize = width * height * bytesPerComp;
    mData = new unsigned char[bufferSize];
    
    config.output.u.RGBA.rgba = (uint8_t*)mData;
    config.output.u.RGBA.stride = width * bytesPerComp;
    config.output.u.RGBA.size = bufferSize;
    config.output.is_external_memory = 1;
    
    if (mFastDecodeMode)
    {
        config.options.bypass_filtering = 1;
        config.options.use_threads = 1;
    }
    
    if (WebPDecode((uint8_t*)filedata->buffer, filedata->size, &config) != VP8_STATUS_OK)
    {
        delete filedata;
        delete []mData;
        mData = NULL;
        return false;
    }
    
    delete filedata;
    MipmapPtr mipmap;
    mipmap.address = mData;
    mipmap.len = 0; // only used by compressed texture
    mipmaps.push_back(mipmap);
    
    return true;
}

NS_KK_END