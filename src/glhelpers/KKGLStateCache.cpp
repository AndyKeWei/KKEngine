//
//  KKGLStateCache.cpp
//  
//
//  Created by kewei on 1/11/16.
//	Copyright (c) 2014 kw. All rights reserved.
//

#include "KKGLStateCache.h"
#include <map>
#include "KKUtil.h"

NS_KK_BEGIN

#if ENABLE_GL_STATE_CACHE
GLuint _kkCurrentProgram = -1;
GLenum _kkGLBlendFunSFactor = 0;
GLenum _kkGLBlendFunDFactor = 0;
GLenum _kkGLActiveTexture = 0;
float _kkGLLineWidth = 1;
GLenum _kkGLBlendEquation = GL_FUNC_ADD;
GLuint _kkTextures[32] = {0};
std::map<GLenum, uint8_t> _kkGLEnabledFeatures;
GLint _scissorBox[4] = {0};
GLboolean _kkGLDepthMask = {true};
#endif

void kkglEnable(GLenum cap)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glEnable(cap);
    }
    else if ((_kkGLEnabledFeatures.find(cap)) == _kkGLEnabledFeatures.end())
    {
        glEnable(cap);
        _kkGLEnabledFeatures.insert(std::make_pair(cap, 1));
    }
#else
    glEnable(cap);
#endif
}

bool kkglIsEnabled(GLenum cap)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        return glIsEnabled(cap);
    }
    return (_kkGLEnabledFeatures.find(cap)) != _kkGLEnabledFeatures.end();
#else
    return glIsEnabled(cap);
#endif
}

void kkglDisable(GLenum cap)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        return glDisable(cap);
    }
    else if ((_kkGLEnabledFeatures.find(cap)) != _kkGLEnabledFeatures.end())
    {
        glDisable(cap);
        _kkGLEnabledFeatures.erase(cap);
    }
#else
    glDisable(cap);
#endif
}

void kkglDepthMask(GLboolean flag)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread()) {
        glDepthMask(flag);
    }
    else if (flag != _kkGLDepthMask)
    {
        glDepthMask(flag);
        _kkGLDepthMask = flag;
    }
#else
    glDepthMask(flag);
#endif
}

void kkglBlendFunc(GLenum sfactor, GLenum dfactor)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glBlendFunc(sfactor, dfactor);
    }
    else if (_kkGLBlendFunSFactor != sfactor || _kkGLBlendFunDFactor != dfactor)
    {
        glBlendFunc(sfactor, dfactor);
        _kkGLBlendFunSFactor = sfactor;
        _kkGLBlendFunDFactor = dfactor;
        
    }
#else
    glBlendFunc(sfactor, dfactor);
#endif
}

void kkglUseProgram(GLuint program)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glUseProgram(program);
    }
    else if (_kkCurrentProgram != program)
    {
        glUseProgram(program);
        _kkCurrentProgram = program;
    }
#else
    glUseProgram(program);
#endif
    
}

void kkglActiveTexture(GLenum texture)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glActiveTexture(texture);
    }
    else if (_kkGLActiveTexture != (texture - GL_TEXTURE0))
    {
        glActiveTexture(texture);
        _kkGLActiveTexture = (texture - GL_TEXTURE0);
    }
#else
    glActiveTexture(texture);
#endif
}

void kkglBindTexture(GLenum target, GLuint texture)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glBindTexture(target, texture);
    }
    else if (_kkTextures[_kkGLActiveTexture] != texture)
    {
        glBindTexture(target, texture);
        _kkTextures[_kkGLActiveTexture] = texture;
    }
#else
    glBindTexture(target, texture);
#endif
}

void kkglScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glScissor(x, y, width, height);
    }
    else if (_scissorBox[0]!=x||_scissorBox[1]!=y||_scissorBox[2]!=width||_scissorBox[3]!=height)
    {
        _scissorBox[0] = x;
        _scissorBox[1] = y;
        _scissorBox[2] = width;
        _scissorBox[3] = height;
        glScissor(x, y, width, height);
    }
#else
    glScissor(x, y, width, height);
#endif
}

void kkglGetScissorBox(GLint *out)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glGetIntegerv(GL_SCISSOR_BOX, out);
    }
    else
    {
        out[0] = _scissorBox[0];
        out[1] = _scissorBox[1];
        out[2] = _scissorBox[2];
        out[3] = _scissorBox[3];
    }
#else
    glGetIntegerv(GL_SCISSOR_BOX, out);
#endif
}

void kkglBlendEquation(GLenum mode)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glBlendEquation(mode);
    }
    else if (mode != _kkGLBlendEquation)
    {
        _kkGLBlendEquation = mode;
        glBlendEquation(mode);
    }
#else
    glBlendEquation(mode);
#endif
}

void kkglLineWidth(GLfloat width)
{
#if ENABLE_GL_STATE_CACHE
    if (!currentIsMainThread())
    {
        glLineWidth(width);
    }
    else if (width != _kkGLLineWidth)
    {
        glLineWidth(width);
        _kkGLLineWidth = width;
    }
#else
    glLineWidth(width);
#endif
}


NS_KK_END