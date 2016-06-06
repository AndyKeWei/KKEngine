//
//  KKImageCodec.h
//  KKEngine
//
//  Created by kewei on 4/27/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#ifndef HLImageCodec_h
#define HLImageCodec_h

#include "KKTexture.hpp"

// HLImageCodec class  图片解码器

NS_KK_BEGIN

struct MipmapPtr {
    const unsigned char *address;
    unsigned int len;
};

class HLImageCodec {
    
public:
    virtual ~HLImageCodec() {}
    KKTexturePixelFormat format;
    std::vector<MipmapPtr> mipmaps;
    
    unsigned int width {0};
    unsigned int height {0};
    bool hasPremultipliedAlpha {false};
    virtual bool loadWithFile(std::string file) {return false;}
};


NS_KK_END


#endif /* HLImageCodec_h */
