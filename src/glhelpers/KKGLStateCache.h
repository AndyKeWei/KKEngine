//
//  KKGLStateCache.h
//  
//
//  Created by kewei on 1/11/16.
//	Copyright (c) 2014 kw. All rights reserved.
//

#ifndef _KKGLStateCache_cpp
#define _KKGLStateCache_cpp

#include "KKMacros.h"
#include "KKGL.h"

NS_KK_BEGIN

void kkglEnable(GLenum cap);
bool kkglIsEnabled(GLenum cap);
void kkglDisable(GLenum cap);
void kkglDepthMask(GLboolean flag);//深度缓冲区是否禁用(只读)
void kkglBlendFunc(GLenum sfactor, GLenum dfactor);//sfactor 源因子, dfactor 目标因子
void kkglUseProgram(GLuint program);
void kkglActiveTexture(GLenum texture);
void kkglBindTexture(GLenum target, GLuint texture);
void kkglScissor(GLint x, GLint y, GLsizei width, GLsizei height);
void kkglGetScissorBox(GLint *out);
void kkglBlendEquation(GLenum mode);
void kkglLineWidth(GLfloat width);


NS_KK_END

#endif /* defined(_KKGLStateCache_cpp) */
