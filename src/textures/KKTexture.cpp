//
//  KKTexture.cpp
//  KKEngine
//
//  Created by kewei on 4/27/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#include "KKTexture.hpp"
#include "KKStringUtil.hpp"
#include "KKResourceManager.hpp"
#if defined(TARGET_IOS) || defined(TARGET_MACOS)
#include "semaphore_darwin.h"
#else
#include <semaphore.h>
#endif
#include <list>
#include <pthread.h>
#include "HLImageCodec.h"
#include "HLImageETC1Codec.h"
#include "HLImageJPEGCodec.h"
#include "HLImagePNGCodec.h"
#include "HLImageWEBPCodec.h"
#include "HLImagePVRCodec.h"
#include "KKShaderProgram.hpp"
#include "KKGLConfiguration.h"

#include <OpenThreads/ScopedLock>

NS_KK_BEGIN

struct PixelFormatDescription {
    GLuint glInternalFormat;
    GLuint glFormat;
    GLuint glType;
    GLuint bit;
    bool compressed;
};


PixelFormatDescription _pixelFormats[] = {
    {GL_BGRA,	GL_BGRA, GL_UNSIGNED_BYTE,					32, false},
    {GL_RGBA,	GL_RGBA, GL_UNSIGNED_BYTE,					32, false},
    {GL_RGB,	GL_RGB,	 GL_UNSIGNED_BYTE,					24, false},
    {GL_RGB,	GL_RGB,	 GL_UNSIGNED_SHORT_5_6_5,			16, false},
    {GL_ALPHA,	GL_ALPHA,	GL_UNSIGNED_BYTE,				8,	false},
    {GL_LUMINANCE,	GL_LUMINANCE,	GL_UNSIGNED_BYTE,		8,	false},
    {GL_LUMINANCE_ALPHA,	GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE,16,	false},
    {GL_RGBA,	GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4,			16, false},
    {GL_RGBA,	GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1,			16, false},
#ifdef GL_IMG_texture_compression_pvrtc
#if GL_IMG_texture_compression_pvrtc
    {GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, static_cast<GLuint>(-1), static_cast<GLuint>(-1),			4,	true},
    {GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG, static_cast<unsigned int>(-1), static_cast<GLuint>(-1),			2,	true},
    {GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, static_cast<GLuint>(-1), static_cast<GLuint>(-1),			4,	true},
    {GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG, static_cast<unsigned int>(-1), static_cast<GLuint>(-1),			2,	true},
#endif
#endif
#ifdef GL_OES_compressed_ETC1_RGB8_texture
#if GL_OES_compressed_ETC1_RGB8_texture
    {GL_ETC1_RGB8_OES, static_cast<GLuint>(-1), static_cast<GLuint>(-1),			8,	true}
#endif
#endif
};

std::map<std::string, KKTexture *> KKTexture::texCache;

std::vector<GLuint> KKTexture::texturesToRemove;

OpenThreads::Mutex KKTexture::mMutex;

//Asyncload
static sem_t* async_tex_avail = 0;
static OpenThreads::Mutex async_img_mutex;
static std::list<KKTexture *> img_loading_queue;

class ImageCodecWrapper : public KKObject {
    
public:
    HLImageCodec *codec {nullptr};
};

template <typename M>
class ScopedLock {
private:
    M &mMutex;
public:
    explicit ScopedLock(M &m):mMutex(m)
    {
        mMutex.lock();
    }
    ~ScopedLock()
    {
        mMutex.unlock();
    }
};

KKTexture::~KKTexture()
{
    //ScopedLock<OpenThreads::Mutex> lock(mMutex);
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);
    if (mFile.size() > 0)
    {
        texCache.erase(mFile);
    }
    if (mName != 0)
    {
        texturesToRemove.push_back(mName);
    }
    if (mAlphaTex != 0)
    {
        texturesToRemove.push_back(mName);
    }
}

KKTexture *KKTexture::getTexture(const std::string &file, bool asyncload)
{
    mMutex.lock();
    KKTexture *res = nullptr;
    auto itr = texCache.find(file);
    if (itr != texCache.end())
    {
        res = itr->second;
    }
    if (!res)
    {
        KKTexture *tex = new KKTexture();
        bool bLoad = false;
        if (asyncload)
        {
            bLoad = tex->loadWithFileAsync(file);
        }
        else
        {
            bLoad = tex->loadWithFile(file);
        }
        if (bLoad)
        {
            texCache.insert(std::make_pair(tex->mFile, tex));
            tex->autorelease();
            mMutex.unlock();
            return tex;
        }
        else
        {
            mMutex.unlock();
            delete tex;
            return nullptr;
        }
    }
    
    mMutex.unlock();
    
    return res;
}

void *imageloadingthread(void *p)
{
    while (1)
    {
        sem_wait(async_tex_avail);
        
        async_img_mutex.lock();
        KKTexture *tex = img_loading_queue.front();
        img_loading_queue.pop_front();
        async_img_mutex.unlock();
        
        HLImageCodec *codec = tex->loadImageCodec(tex->mFile);
        ImageCodecWrapper *codecWrapper = new ImageCodecWrapper;
        codecWrapper->codec = codec;
        codecWrapper->performFuncOnMainThread((CallFuncO)(&KKTexture::loadWithImageCodecOnMainThread), codecWrapper);
        codecWrapper->release();
        
        tex->release();
    }
    
    return 0;
}

void KKTexture::loadWithImageCodecOnMainThread(KKObject *param)
{
    mLoading = false;
    ImageCodecWrapper *codec = static_cast<ImageCodecWrapper*>(param);
    if (!codec->codec)
    {
        return;
    }
    bool res = loadWithImageCodec(codec->codec, mName);
    
    delete codec->codec;
    
    if (res && !strcasecmp(mFile.c_str()+mFile.length()-4, ".pkm"))
    {
        //针对android作的优化，是因为etc夸张方式的pkm不支持透明，需要把rgb和alpha单独出来，将alpha通道单独做成纹理
        loadExtraAlphaIfExsits();
    }
}

void KKTexture::loadExtraAlphaIfExsits()
{
    //加载额外的如果已经存在的alpha纹理
    std::string lowerCase(mFile);
    for (int i=0; i<lowerCase.length(); i++)
    {
        lowerCase[i] = tolower(lowerCase[i]);
    }
    std::string fname = mFile.substr(0,lowerCase.rfind(".pkm"));
    std::string alphafile = fname + "_alpha.pkm.ccz";
    HLImageCodec *codec = nullptr;
    if (KKResourceManager::getSingleton()->fileExists(alphafile.c_str()))
    {
        codec = new HLImageETC1Codec;
    }
    else
    {
        alphafile = fname + "_alpha.pkm";
        if (KKResourceManager::getSingleton()->fileExists(alphafile.c_str()))
        {
            codec = new HLImageETC1Codec;
        }
        else
        {
            alphafile = fname + "_alpha.jpg";
            if (KKResourceManager::getSingleton()->fileExists(alphafile.c_str()))
            {
                codec = new HLImageJPEGCodec;
            }
            else
            {
                alphafile = fname + "_alpha.png";
                if (KKResourceManager::getSingleton()->fileExists(alphafile.c_str()))
                {
                    codec = new HLImagePNGCodec;
                }
            }
        }
    }
    if (codec)
    {
        if (loadWithFile(alphafile))
        {
            loadWithImageCodec(codec, mAlphaTex);
        }
        delete codec;
    }
}

bool KKTexture::loadWithRawData(const void* data, KKTexturePixelFormat pixelFormat, unsigned int pixelsWidth, unsigned int pixelsHeight, float scale)
{
    mScale = scale;
    HLImageCodec codec;
    codec.format = pixelFormat;
    codec.width = pixelsWidth;
    codec.height = pixelsHeight;
    MipmapPtr mipmap;
    mipmap.address = (unsigned char *)data;
    mipmap.len = 0;
    codec.mipmaps.push_back(mipmap);
    return loadWithImageCodec(&codec, mName);
}

bool KKTexture::loadWithImageCodec(HLImageCodec *ic, GLuint& tex)
{
    if ((tex == mName) && (mName != 0))
    {
        return false;
    }
    else if (mAlphaTex)
    {
        return false;
    }
    
    mImageSize.width = ic->width / mScale;
    mImageSize.height = ic->height / mScale;
    //混合透明度
    //所以 Premultiplied Alpha 最重要的意义是使得带透明度图片纹理可以正常的进行线性插值。这样旋转、缩放或者非整数的纹理坐标才能正常显示，否则就会像上面的例子一样，在透明像素边缘附近产生奇怪的颜色
    mHasPremultipliedAlpha = ic->hasPremultipliedAlpha;
    
    bool compressed = _pixelFormats[ic->format].compressed;
    
    //有多少图片管线
    size_t m_uNumberOfMipmaps = ic->mipmaps.size();
    
    switch ( _pixelFormats[ic->format].bit/8) {
        case 1:
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //控制的是所读取的图像数据的行对齐方式  默认4个字节
            break;
        case 2:
            glPixelStorei(GL_UNPACK_ALIGNMENT, 2); //控制的是所读取的图像数据的行对齐方式  默认4个字节
            break;
        case 3:
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //控制的是所读取的图像数据的行对齐方式  默认4个字节
            break;
        case 4:
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4); //控制的是所读取的图像数据的行对齐方式  默认4个字节
            break;
        default:
            break;
    }
    //用来生成纹理的数量
    glGenTextures(1, &tex);
    //绑定纹理目标，后续的操作都是基于这个纹理
    kkglBindTexture(GL_TEXTURE_2D, tex);
    
    if (m_uNumberOfMipmaps == 1)
    {
        //纹理过滤
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    else
    {
        //纹理过滤
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    CHECK_GL_ERROR_DEBUG();
    
    if (compressed)
    {
        //纹理压缩过
        unsigned int width = ic->width;
        unsigned int height = ic->height;
        for (size_t mip = 0; mip < m_uNumberOfMipmaps; ++mip)
        {
            glCompressedTexImage2D(GL_TEXTURE_2D, (GLint)mip, _pixelFormats[ic->format].glInternalFormat, width, height, 0, ic->mipmaps[mip].len, ic->mipmaps[mip].address);
            width = MAX(width >> 1, 1);
            height = MAX(height >> 1, 1);
        }
    }
    else
    {
        unsigned int width = ic->width;
        unsigned int height = ic->height;
        for (size_t mip = 0; mip < m_uNumberOfMipmaps; ++mip)
        {
            glTexImage2D(GL_TEXTURE_2D, (GLint)mip, _pixelFormats[ic->format].glInternalFormat, width, height, 0, _pixelFormats[ic->format].glFormat, _pixelFormats[ic->format].glType, ic->mipmaps[mip].address);
            width = MAX(width >> 1, 1);
            height = MAX(height >> 1, 1);
        }
    }
    
    //控制的是所读取的图像数据的行对齐方式  默认4个字节
    glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
    CHECK_GL_ERROR_DEBUG();
    return true;
}

bool KKTexture::loadWithCubeMap(std::string name, std::string ext)
{
    if (mName != 0)
    {
        return false;
    }
    static const std::string suffixes[6] = {"_rt", "_lf", "_up", "_dn", "_fr", "_bk"};
    
    glGenTextures(1, &mName);
    kkglBindTexture(GL_TEXTURE_CUBE_MAP, mName);
    
    bool res = false;
    HLImageCodec *codec;
    if (std::string::npos != ext.find("pvr"))
    {
        codec = new HLImagePVRCodec;
    }
    else if (std::string::npos == ext.find("png"))
    {
        codec = new HLImagePNGCodec;
    }
    else
    {
        printf("unsupported image format");
        return false;
    }
    res = codec->loadWithFile(name + suffixes[0] + "." + ext);
    if (!res)
    {
        delete codec;
        return false;
    }
    res = loadWithImageCodec(codec, mName);
    
    switch( _pixelFormats[codec->format].bit/8 )
    {
        case 1: glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); break;
        case 2: glPixelStorei( GL_UNPACK_ALIGNMENT, 2 ); break;
        case 3: glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); break;
        case 4: glPixelStorei( GL_UNPACK_ALIGNMENT, 4 ); break;
    }
    
    mImageSize.width = codec->width / mScale;
    mImageSize.height = codec->height / mScale;
    
    KKGLConfiguration *configuration = KKGLConfiguration::getSingleton();
    
    bool compressed = _pixelFormats[codec->format].compressed;
    
    if (compressed && (!configuration->isSupportsPVRTC()))
    {
        KKLOG("WARNING: PVRTC textures are not supported");
        return false;
    }
    
    size_t m_uNumberOfMipmaps = codec->mipmaps.size();
    
    
    // Default: Anti alias.
    if (m_uNumberOfMipmaps == 1)
    {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    for (int i = 0; i < 6; ++i)
    {
        if (i > 0)
        {
            res = codec->loadWithFile(name + suffixes[i] + "." + ext);
            if (!res)
            {
                delete codec;
                return false;
            }
        }
        if (compressed)
        {
            for (size_t mip = 0; mip < m_uNumberOfMipmaps; ++mip)
            {
                glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, (GLint)mip, _pixelFormats[codec->format].glInternalFormat, codec->width, codec->height, 0, codec->mipmaps[mip].len, codec->mipmaps[mip].address);
            }
        }
        else
        {
            for (size_t mip = 0; mip < m_uNumberOfMipmaps; ++mip)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, (GLint)mip, _pixelFormats[codec->format].glInternalFormat, codec->width, codec->height, 0, _pixelFormats[codec->format].glFormat, _pixelFormats[codec->format].glType, codec->mipmaps[mip].address);
            }
        }
    }
    
    glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
    
    CHECK_GL_ERROR();
    
    delete codec;
    return res;
}

HLImageCodec * KKTexture::loadImageCodec(std::string file)
{
    std::string lowerCase(file);
    StringUtil::StringToLower(lowerCase);
    
    mScale = 1.f / KKResourceManager::getSingleton()->getResourceScale();
    
    HLImageCodec *codec;
    
    mFile = file;
    
    if (std::string::npos != lowerCase.rfind(".pvr"))
    {
        codec = new HLImagePVRCodec;
    }
    else if (std::string::npos != lowerCase.rfind(".png"))
    {
        if (KKResourceManager::getSingleton()->fileExists(file.c_str()))
        {
            codec = new HLImagePNGCodec;
        }
        else
        {
            KKLOG("file %s does not exist!", file.c_str());
            file = file.substr(0, file.length() - 3) + "webp";
            codec = new HLImageWEBPCodec;
        }
    }
    else if (std::string::npos != lowerCase.rfind(".jpg") || std::string::npos != lowerCase.rfind(".jpeg"))
    {
        codec = new HLImageJPEGCodec;
    }
    else if (std::string::npos != lowerCase.rfind(".webp"))
    {
        codec = new HLImageWEBPCodec;
    }
    else if (std::string::npos != lowerCase.rfind(".pkm"))
    {
        codec = new HLImageETC1Codec;
    }
    else
    {
        KKLOG("unsupported image format: %s", file.c_str());
        return nullptr;
    }
    
    if (!codec->loadWithFile(file))
    {
        delete codec;
        return nullptr;
    }
    
    return codec;
}

bool KKTexture::loadWithFile(std::string file)
{
    HLImageCodec *codec = loadImageCodec(file);
    if (!codec)
    {
        return false;
    }
    
    bool res = loadWithImageCodec(codec, mName);
    delete codec;
    if (res && (!strcasecmp(mFile.c_str()+mFile.length()-4, ".pkm") || !strcasecmp(mFile.c_str()+mFile.length()-8, ".pkm.ccz")))
    {
        //针对android作的优化，是因为etc夸张方式的pkm不支持透明，需要把rgb和alpha单独出来，将alpha通道单独做成纹理
        loadExtraAlphaIfExsits();
    }
    return res;
}

bool KKTexture::loadWithFileAsync(std::string file)
{
    mLoading = true;
    std::string lowerCase(file);
    StringUtil::StringToLower(lowerCase);
    
    if (!KKResourceManager::getSingleton()->fileExists(file.c_str()))
    {
        KKLOG("file %s does not exist!", file.c_str());
        if (std::string::npos != lowerCase.rfind(".png"))
        {
            file = file.substr(0, file.length() - 3) + "webp";
            if (!KKResourceManager::getSingleton()->fileExists(file.c_str()))
            {
                KKLOG("file %s does not exist!", file.c_str());
                return false;
            }
        }
    }
    
    if (async_tex_avail == 0)
    {
        async_tex_avail = new sem_t;
        sem_init(async_tex_avail, 0, 0);
        pthread_t tid;
        pthread_create(&tid, nullptr, imageloadingthread, this);
    }
    
    mFile = file;
    async_img_mutex.lock();
    this->retain();
    img_loading_queue.push_back(this);
    async_img_mutex.unlock();
    sem_post(async_tex_avail);
    
    return true;
}

void KKTexture::glPurgeTextures()
{
    //ScopedLock<OpenThreads::Mutex> lock(mMutex);
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);
    
    if (!texturesToRemove.empty())
    {
        glDeleteTextures((GLsizei)texturesToRemove.size(), &texturesToRemove[0]);
        texturesToRemove.clear();
    }
    CHECK_GL_ERROR();
}

void KKTexture::drawAtPoint(KKPoint& point)
{
    if (mName == 0)
    {
        return;
    }
    
    GLfloat    coordinates[8] = {
        0.f, 1.f,
        1.f, 1.f,
        0.f, 0.f,
        1.f, 0.f
    };
    GLfloat     vertices[] = {
        point.x,                   point.y,                    /*0.0f,*/
        point.x+mImageSize.width,        point.y,                    /*0.0f,*/
        point.x,                   point.y+mImageSize.height,        /*0.0f,*/
        point.x+mImageSize.width,        point.y+mImageSize.height,        /*0.0f*/ };
    //    GLubyte *colors = new GLubyte[16];
    //    memset(colors, 255, 16*sizeof(GLbyte));
    KKProgram *program = KKProgramManager::currentManager()->getProgram(kShaderPositionTexture);
    program->use();
    program->setUniformForModelViewProjectionMatrix();
    //    program->getUniforms()[0].setUniform1f(1.f); // brightness
    
    kkglBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    kkglBindTexture(GL_TEXTURE_2D, mName);
    
    glEnableVertexAttribArray( kVertexAttrib_Position ); //启用顶点属性数组
    glEnableVertexAttribArray( kVertexAttrib_TexCoords );
    glEnableVertexAttribArray( kVertexAttrib_Color );
    
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, coordinates);
    //    glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, colors);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    //    delete colors;
    
    CHECK_GL_ERROR_DEBUG();
}

void KKTexture::drawInRect(const KKRect& rect, bool repeate)
{
    if (mName == 0)
    {
        return;
    }
    
    float maxS = 1.f;
    float maxT = 1.f;
    if (repeate)
    {
        //        maxS = rect.size.width / mImageSize.width;
        //        maxT = rect.size.height / mImageSize.height;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    GLfloat    coordinates[] = {
        0.0f,    maxT,
        maxS,     maxT,
        0.0f,    0.0f,
        maxS,     0.0f };
    
    GLfloat    vertices[] = {    rect.origin.x,        rect.origin.y,                            /*0.0f,*/
        rect.origin.x + rect.size.width,        rect.origin.y,                            /*0.0f,*/
        rect.origin.x,                            rect.origin.y + rect.size.height,        /*0.0f,*/
        rect.origin.x + rect.size.width,        rect.origin.y + rect.size.height,        /*0.0f*/ };
    
    KKProgram* program = KKProgramManager::currentManager()->getProgram(kShaderPositionTexture);
    glEnableVertexAttribArray(kVertexAttrib_Position);
    glEnableVertexAttribArray(kVertexAttrib_TexCoords);
    program->use();
    program->setUniformForModelViewProjectionMatrix();
    
    kkglBlendEquation(GL_FUNC_ADD);
    kkglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    kkglBindTexture(GL_TEXTURE_2D, mName);
    
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, coordinates);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    CHECK_GL_ERROR_DEBUG();
}





NS_KK_END