//
//  KKMacros.h
//  KKEngine
//
//  Created by kewei on 7/11/14.
//  Copyright (c) 2014 kewei. All rights reserved.
//

#ifndef KKEngine_KKMacros_h
#define KKEngine_KKMacros_h

#include "GameEngineConfig.h"

#if __cplusplus < 201103L
    #error "should be user c++11 compiler"
#endif

//namespace

#ifdef __cplusplus
    #define NS_KK_BEGIN         namespace cocos2d {
    #define NS_KK_END         }
    #define USING_NS_KK       using namespace cocos2d;

    #define NS_KK_BEGIN_GUI     namespace gui {
    #define NS_KK_END_GUI     } 
    #define USING_NS_KK_GUI       using namespace cocos2d::gui;
#else
    #define NS_KK_BEGIN
    #define NS_KK_END
    #define USING_NS_KK
    #define NS_KK_BEGIN_GUI
    #define NS_KK_END_GUI
    #define USING_NS_KK_GUI
#endif


//math

#define RANDOM_MINUS1_1() ((2.0f*((float)rand()/RAND_MAX))-1.0f)

#define RANDOM_0_1() ((float)rand()/RAND_MAX)

#define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) * 0.01745329252f) //角度速算成弧度

#define RADIANS_TO_DEGREES(__ANGLE__) ((__ANGLE__) * 57.29577951f) //弧度速算成角度

#define HOST_IS_BIG_ENDIAN (bool)(*(unsigned short *)"\0\xff" < 0x100)


//log

#include <assert.h>

#ifdef DEBUG
#if defined(TARGET_ANDROID)
#define KKLOG(...)  __android_log_print(ANDROID_LOG_DEBUG,"cocos2d",__VA_ARGS__)
#elif defined(TARGET_WIN32)
void log_win32(const char *, ...);
#define KKLOG(format, ...)      log_win32(format, ##__VA_ARGS__)
#else
#define KKLOG(format, ...)      {printf("cocos2d: "); printf(format, ##__VA_ARGS__); printf("\n"); fflush(stdout);}
#endif
#define KKASSERT(cond, msg, ...)   if (!(cond)){printf(msg,  ##__VA_ARGS__);abort();}
#else
#define KKLOG(format, ...);
#define KKASSERT(cond, msg, ...)
#endif

#ifndef MIN
#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#endif

#ifndef MAX
#define MAX(x,y) (((x) < (y)) ? (y) : (x))
#endif

#define MAX_CHAR	64
#ifndef MAX_PATH
#ifdef PATH_MAX
#define MAX_PATH PATH_MAX
#else
#define MAX_PATH	512
#endif
#endif

#define KK_SAFE_DELETE(p)           do { delete (p); (p) = nullptr; } while(0)
#define KK_SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)
#define KK_SAFE_FREE(p)             do { if(p) { free(p); (p) = nullptr; } } while(0)
#define KK_SAFE_RELEASE(p)          do { if(p) { (p)->release(); } } while(0)
#define KK_SAFE_RELEASE_NULL(p)     do { if(p) { (p)->release(); (p) = nullptr; } } while(0)
#define KK_SAFE_RETAIN(p)           do { if(p) { (p)->retain(); } } while(0)
#define KK_BREAK_IF(cond)           if(cond) break


#if !defined(DEBUG) || DEBUG == 0
#define CHECK_GL_ERROR_DEBUG()
#else
#define CHECK_GL_ERROR_DEBUG() \
do { \
GLenum __error = glGetError(); \
if(__error) { \
KKLOG("OpenGL error 0x%04X in file %s function %s line %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
} \
} while (false)
#endif

#ifndef CHECK_GL_ERROR
#define CHECK_GL_ERROR  CHECK_GL_ERROR_DEBUG
#endif

// new callbacks based on C++11
#define CC_CALLBACK_0(__selector__,__target__, ...) std::bind(&__selector__,__target__, ##__VA_ARGS__)
#define CC_CALLBACK_1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
#define CC_CALLBACK_2(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define CC_CALLBACK_3(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)


/* Define NULL pointer value */
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif


#define COMPONENT_WILL_ADD "component_will_add"
#define COMPONENT_WILL_REMOVE "component_will_remove"

#define FRIEND_CLASS(CLASS) \
template<typename CLASS> class CLASS { \
friend CLASS; \
}

#if defined(TARGET_IOS)
#define CONTENT_SCALE_FACTOR  cocos2d::KKDirector2D::getSingleton()->getContentScaleFactor()
#else
#define CONTENT_SCALE_FACTOR  1
#endif


#endif
