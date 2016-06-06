//
//  HLETCCodec.h
//  HoolaiEngine
//
//  Created by zs_hoolai on 13-9-2.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLIMAGEETC1CODEC_H__
#define __HLIMAGEETC1CODEC_H__

#include "HLImageCodec.h"

NS_KK_BEGIN

class HLImageETC1Codec: public HLImageCodec
{
private:
    unsigned char* mData;
public:
    HLImageETC1Codec():mData(NULL) {}
    virtual ~HLImageETC1Codec();
    virtual bool loadWithFile(std::string file);
};

NS_KK_END


#endif
