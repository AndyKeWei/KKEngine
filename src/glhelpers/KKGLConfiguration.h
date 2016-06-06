//
//  KKGLConfiguration.h
//  HoolaiEngine2D
//
//  Created by zhao shuan on 12-5-28.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __KKGLConfiguration_H__
#define __KKGLConfiguration_H__

#include "KKMacros.h"
#include "KKGL.h"
#include <string>
#include "KKSingleton.h"
#include "KKFileUtil.hpp"

NS_KK_BEGIN

class KKGLConfiguration: public KKSingleton<KKGLConfiguration>
{
    friend class KKSingleton<KKGLConfiguration>;
private:
    GLint _maxTextureSize;
    GLint _maxModelviewStackDepth;
    bool _supportsPVRTC;
    bool _supportsETC;
    bool _supportsNPOT;
    bool _supportsBGRA8888;
    bool _supportsDiscardFramebuffer;
    bool _supportsShareableVAO;
    GLint _maxSamplesAllowed;
    GLint _maxTextureUnits;
    std::string _glExtensions;
    KKGLConfiguration();
public:
    int getMaxTextureSize()
    {
        return _maxTextureSize;
    }
    
    int getMaxModelviewStackDepth()
    {
        return _maxModelviewStackDepth;
    }
    
    int getTaxTextureUnits()
    {
        return _maxTextureUnits;
    }
    
    bool isSupportsNPOT()
    {
        return _supportsNPOT;
    }
    
    bool isSupportsPVRTC()
    {
        return _supportsPVRTC;
    }
    
    bool isSupportsETC()
    {
        return _supportsETC;
    }
    
    bool isSupportsBGRA8888()
    {
        return _supportsBGRA8888;
    }
    
    bool isSupportsDiscardFramebuffer()
    {
        return _supportsDiscardFramebuffer;
    }
    
    
    bool isSupportsShareableVAO()
    {
        return _supportsShareableVAO;
    }
    
    bool checkForGLExtension(const std::string& searchName);
};

NS_KK_END

#endif
