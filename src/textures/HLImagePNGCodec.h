//
//  HLImagePNGCodec.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-3-25.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLPNGCODEC_H__
#define __HLPNGCODEC_H__

#include "HLImageCodec.h"

NS_KK_BEGIN

class HLImagePNGCodec: public HLImageCodec
{
private:
    unsigned char* mData;
public:
    HLImagePNGCodec():mData(NULL) {}
    virtual ~HLImagePNGCodec();
    virtual bool loadWithFile(std::string file);
};

NS_KK_END

#endif
