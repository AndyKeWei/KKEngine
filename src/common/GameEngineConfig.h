//
//  GameEngineConfig.h
//  KKEngine
//
//  Created by kewei on 9/24/14.
//  Copyright (c) 2014 kewei. All rights reserved.
//

#ifndef KKEngine_GameEngineConfig_h
#define KKEngine_GameEngineConfig_h

#ifndef ENABLE_GL_STATE_CACHE
#define ENABLE_GL_STATE_CACHE 1
#endif

#if defined(ANDROID)
#define TARGET_ANDROID
#elif defined(__IPHONE_OS_VERSION_MAX_ALLOWED)
#define TARGET_IOS
#elif defined(__MAC_OS_X_VERSION_MAX_ALLOWED)
#define TARGET_MACOS
#define USE_TOUCH_EVENT
#elif defined(WIN32)
#define TARGET_WIN32
#define USE_TOUCH_EVENT
#endif

#if defined(TARGET_IOS)
#define ENABLE_RETINA_RESOURCE_SUFFIX 0
#else
#define ENABLE_RETINA_RESOURCE_SUFFIX 0
#endif

#if defined(TARGET_ANDROID) || defined(TARGET_WIN32)
#define KK_USE_VAO 0
#else
#define KK_USE_VAO 1
#endif

#endif
