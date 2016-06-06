//
//  HLImagePVRCodec.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-3-25.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLIMAGEPVRCODEC_H__
#define __HLIMAGEPVRCODEC_H__

#include "HLImageCodec.h"

#if defined(TARGET_WIN32)
typedef SSIZE_T ssize_t;
#endif

NS_KK_BEGIN

class HLImagePVRCodec: public HLImageCodec
{
private:
    unsigned char* mData;
    bool loadWithPVRv2Data(const unsigned char* data, ssize_t len);
    bool loadWithPVRv3Data(const unsigned char* data, ssize_t len);
public:
    HLImagePVRCodec():mData(NULL) {}
    virtual ~HLImagePVRCodec();
    virtual bool loadWithFile(std::string file);
    static void setPVRImagesHavePremultipliedAlpha(bool haveAlphaPremultiplied);
};

NS_KK_END

#endif
