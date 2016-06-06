//
//  HLImageJEPGCodec.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-3-25.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLIMAGEJPEGCODEC_H__
#define __HLIMAGEJPEGCODEC_H__

#include "HLImageCodec.h"

NS_KK_BEGIN

class HLImageJPEGCodec: public HLImageCodec
{
private:
    unsigned char* mData;
public:
    HLImageJPEGCodec():mData(NULL) {}
    virtual ~HLImageJPEGCodec();
    virtual bool loadWithFile(std::string file);
};

NS_KK_END

#endif
