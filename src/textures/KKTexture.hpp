//
//  KKTexture.hpp
//  KKEngine
//
//  Created by kewei on 4/27/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#ifndef KKTexture_hpp
#define KKTexture_hpp

#include "KKObject.h"
#include <OpenThreads/Mutex>
#include "KKGL.h"
#include "KKGeometry.h"

NS_KK_BEGIN

typedef enum {
    kTexturePixelFormat_BGRA8888,
    //! 32-bit texture: RGBA8888
    kTexturePixelFormat_RGBA8888,
    //! 24-bit texture: RGB888
    kTexturePixelFormat_RGB888,
    //! 16-bit texture without Alpha channel
    kTexturePixelFormat_RGB565,
    //! 8-bit textures used as masks
    kTexturePixelFormat_A8,
    //! 8-bit intensity texture
    kTexturePixelFormat_I8,
    //! 16-bit textures used as masks
    kTexturePixelFormat_AI88,
    //! 16-bit textures: RGBA4444
    kTexturePixelFormat_RGBA4444,
    //! 16-bit textures: RGB5A1
    kTexturePixelFormat_RGB5A1,
    //! 4-bit PVRTC-compressed texture: PVRTC4
#ifdef GL_IMG_texture_compression_pvrtc
#if GL_IMG_texture_compression_pvrtc
    kTexturePixelFormat_PVRTC4,
    //! 2-bit PVRTC-compressed texture: PVRTC2
    kTexturePixelFormat_PVRTC2,
    kTexturePixelFormat_PVRTC4_NOALPHA,
    kTexturePixelFormat_PVRTC2_NOALPHA,
#endif
#endif
#ifdef GL_OES_compressed_ETC1_RGB8_texture
#if GL_OES_compressed_ETC1_RGB8_texture
    kTexturePixelFormat_ETC1_RGB8,
#endif
#endif
} KKTexturePixelFormat;

class HLImageCodec;

class KKTexture :  public KKObject {
    
    friend void *imageloadingthread(void *p);
private:
    
    static std::map<std::string, KKTexture *> texCache;
    static std::vector<GLuint> texturesToRemove;
    static OpenThreads::Mutex mMutex;
    
    bool mLoading {false};
    std::string mFile;
    float mScale {1.0f};
    KKSize mImageSize;
    bool mHasPremultipliedAlpha {false};
    
    GLuint mName {0};
    GLuint mAlphaTex {0};
    
private:
    HLImageCodec * loadImageCodec(std::string file);
    
    bool loadWithFile(std::string file);
    
    bool loadWithFileAsync(std::string file);
    
    void loadExtraAlphaIfExsits();
    
    void loadWithImageCodecOnMainThread(KKObject *param);
    
public:
    KKTexture(){}
    virtual ~KKTexture();
    static void glPurgeTextures();
    
    bool loadWithImageCodec(HLImageCodec *ic, GLuint& tex);
    
    bool loadWithRawData(const void* data, KKTexturePixelFormat pixelFormat, unsigned int pixelsWidth, unsigned int pixelsHeight, float scale);
    
    bool loadWithCubeMap(std::string name, std::string ext);
    
    static KKTexture *getTexture(const std::string &file, bool asyncload = false);
    
    const GLuint getName() const
    {
        return mName;
    }
    
    const GLuint getAlphaTexture()
    {
        return mAlphaTex;
    }
    
    KKSize getImageSize()
    {
        return mImageSize;
    }
    
    float getScale() const
    {
        return mScale;
    }
    
    bool hasPremultipliedAlpha()
    {
        return mHasPremultipliedAlpha;
    }
    
    void drawAtPoint(KKPoint& point);
    void drawInRect(const KKRect& rect, bool repeate);
};


NS_KK_END

#endif /* KKTexture_hpp */
