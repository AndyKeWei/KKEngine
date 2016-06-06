//
//  KKUtil.h
//  
//
//  Created by kewei on 1/11/16.
//	Copyright (c) 2014 kw. All rights reserved.
//

#ifndef _KKUtil_cpp
#define _KKUtil_cpp

#include "KKMacros.h"
#include "KKGL.h"
#include "KKApplication.h"
#include <pthread.h>


NS_KK_BEGIN

typedef struct _color4B
{
    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
    //    _color4B(GLubyte r, GLubyte g, GLubyte b, GLubyte a): r(r),g(g),b(b),a(a) {}
    //    _color4B():r(255),g(255),b(255),a(255) {}
} color4B;

const color4B HLBlackColor4B = {0, 0, 0, 255};
const color4B HLDarkGrayColor4B = {85, 85, 85, 255};
const color4B HLWhiteColor4B = {255, 255, 255, 255};
const color4B HLGrayColor4B = {63, 63, 63, 255};
const color4B HLRedColor4B = {255, 0, 0, 255};
const color4B HLGreenColor4B = {0, 255, 0, 255};
const color4B HLBlueColor4B = {0, 0, 255, 255};
const color4B HLYellowColor4B = {255, 255, 0, 255};
const color4B HLOrangeColor4B = {255, 63, 0, 255};
const color4B HLDarkYellowColor4B = {250, 205, 109, 255};
const color4B HLBrownColor4B = {65,56,36, 255};
const color4B HLPurpleColor4B = {160, 39, 240, 255};

typedef struct _color4F
{
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;
} color4F;

const color4F HLBlackColor4F = {0.f, 0.f, 0.f, 1.f};
const color4F HLDarkGrayColor4F = {0.333f, 0.333f, 0.333f, 1.f};
const color4F HLWhiteColor4F = {1.f, 1.f, 1.f, 1.f};
const color4F HLGrayColor4F = {0.25f, 0.25f, 0.25f, 1.f};
const color4F HLRedColor4F = {1.f, 0.f, 0.f, 1.f};
const color4F HLGreenColor4F = {0.f, 1.f, 0.f, 1.f};
const color4F HLBlueColor4F = {0.f, 0.f, 1.f, 1.f};
const color4F HLYellowColor4F = {1.f, 1.f, 0.f, 1.f};
const color4F HLOrangeColor4F = {1.f, 0.25, 0.f, 1.f};
const color4F HLPurpleColor4F = { 0.627f, 0.125f, 0.941f, 1.f};

typedef struct _vertex3F
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    //    _vertex3F(GLfloat x, GLfloat y, GLfloat z):x(x),y(y),z(z){}
    //    _vertex3F():x(0),y(0),z(0){}
} vertex3F;

typedef struct _tex2F
{
    GLfloat u;
    GLfloat v;
    //    _tex2F(GLfloat u, GLfloat v):u(u),v(v){}
    //    _tex2F():u(0),v(0){}
} tex2F;

typedef struct _V3F_C4B_T2F
{
    vertex3F vertices;
    color4B colors;
    tex2F texCoords;
} V3F_C4B_T2F;

typedef struct _Quad3
{
    vertex3F tl;
    vertex3F bl;
    vertex3F tr;
    vertex3F br;
} Quad3;

typedef struct _V3F_C4B_T2F_Quad
{
    // top left
    V3F_C4B_T2F tl;
    // bottom left
    V3F_C4B_T2F bl;
    // top right
    V3F_C4B_T2F tr;
    // bottom right
    V3F_C4B_T2F br;
} V3F_C4B_T2F_Quad;

int utf8_strlen (const char * p, int max);
int utf8_strlen2 (const char * p);

class KKFileData;

bool currentIsMainThread();

const char* getLanguageStr(KKLanguageType type);

NS_KK_END

float clampf(float value, float min_inclusive, float max_inclusive);

//int inflateGZipFile(const char *path, unsigned char **out);
int inflateCCZFile(cocos2d::KKFileData* data, unsigned char **out);
unsigned long nextPOT(unsigned long x);

void EndianSwitch(int size, void* p);

template <int size>
void NativeToBigEndian(void* p)
{
    if (!HOST_IS_BIG_ENDIAN)
    {
        EndianSwitch(size, p);
    }
}

template <int size>
void NativeToLittleEndian(void* p)
{
    if (HOST_IS_BIG_ENDIAN)
    {
        EndianSwitch(size, p);
    }
}

template <int size>
void BigEndianToNative(void* p)
{
    NativeToBigEndian<size>(p);
}

template <int size>
void LittleEndianToNative(void* p)
{
    NativeToLittleEndian<size>(p);
}

namespace cocos2d
{
    class KKPoint;
}
USING_NS_KK
int pnpoly(int nvert, KKPoint *poly, KKPoint test);

#ifdef TARGET_WIN32
float roundf(float x);
#endif


#endif /* defined(_KKUtil_cpp) */
