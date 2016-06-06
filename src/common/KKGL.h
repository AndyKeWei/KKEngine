//
//  KKGL.h
//  KKEngine
//
//  Created by kewei on 1/8/16.
//  Copyright (c) 2016 kewei. All rights reserved.
//

#ifndef KKEngine_KKGL_h
#define KKEngine_KKGL_h

#include "GameEngineConfig.h"

#if defined(TARGET_ANDROID)

#elif defined(TARGET_IOS)


#elif defined(TARGET_MACOS)

#define glDeleteVertexArrays        glDeleteVertexArraysAPPLE
#define glBindVertexArray           glBindVertexArrayAPPLE
#define glGenVertexArrays           glGenVertexArraysAPPLE
#include <OpenGL/gl.h>

#endif

#include "KKGLStateCache.h"

#endif
