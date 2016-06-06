//
//  KKDirector.cpp
//  KKEngine
//
//  Created by kewei on 7/17/15.
//  Copyright (c) 2014 kewei. All rights reserved.
//

#include "KKDirector.h"
#include "KKTimer.h"
#include "KKApplication.h"
#include "KKAutoreleasePool.h"
#include "KKGLMatrixStack.h"
#include "KKTexture.hpp"


pthread_t mainthread;

extern bool appWillTerminate;

NS_KK_BEGIN

std::vector<ThreadScheduleObject> pendingScheduleArray;
OpenThreads::Mutex scheduleSelectorMutex;

KKDirector::KKDirector():
#ifdef TARGET_IOS
mContentScaleFactor(1),
#endif
mShowStats(false)
{
    mNextDeltaTimeZero = true;
    
    registerComponents();
}

void KKDirector::setProjection()
{
    if (mDesignWinSize == KKSizeZero)
    {
        mViewPortScaleFactor = 1;
    }
    else
    {
        float scale;
        if (mDesignWinSize.width != 0 && mDesignWinSize.height != 0)
        {
            scale = MAX(mDesignWinSize.width/mWinSizeInPixels.width, mDesignWinSize.height/mWinSizeInPixels.height);
            mWinSize = mDesignWinSize;
        }
        else if (mDesignWinSize.width == 0)// width fill and heigth scale
        {
            scale = mDesignWinSize.height/mWinSizeInPixels.height;
            mWinSize = mWinSizeInPixels * scale;
        }
        else if (mDesignWinSize.height == 0)// height fill and width scale
        {
            scale = mDesignWinSize.width/mWinSizeInPixels.width;
            mWinSize = mWinSizeInPixels * scale;
        }
        mViewPortScaleFactor = scale;
    }
    setViewPort();
    KKGLMatrixStack *matrixStack = KKGLMatrixStack::currentMatrixStack();
    matrixStack->matrixMode(KM_GL_PROJECTION);
    matrixStack->loadIdentity();
    kmMat4 orthoMatrix;
#ifdef NVCARD
    //OpenGL支持两种类型的投影变换，即透视投影和正投影, 通过参数生成出投影矩阵变量的值，此处使用的是正交投影矩阵。
    kmMat4OrthographicProjection(&orthoMatrix, -0.01, mWinSize.width-0.01, -0.01, mWinSize.height-0.01, -1024, 1024);
#else
    kmMat4OrthographicProjection(&orthoMatrix, 0, mWinSize.width, 0, mWinSize.height, -1024, 1024);
#endif
    matrixStack->multMatrix(&orthoMatrix);
    matrixStack->matrixMode(KM_GL_MODELVIEW);
    matrixStack->loadIdentity();
    
    // 启动混合并设置混合因子
    //混合是什么呢？混合就是把两种颜色混在一起。具体一点，就是把某一像素位置原来的颜色和将要画上去的颜色，通过某种方式混在一起，从而实现特殊的效果
    //注意：只有在RGBA模式下，才可以使用混合功能，颜色索引模式下是无法使用混合功能的
    //kkglEnable(GL_BLEND);
//    源因子和目标因子是可以通过glBlendFunc函数来进行设置的。glBlendFunc有两个参数，前者表示源因子，后者表示目标因子。这两个参数可以是多种值，下面介绍比较常用的几种。
//    GL_ZERO：      表示使用0.0作为因子，实际上相当于不使用这种颜色参与混合运算。
//    GL_ONE：       表示使用1.0作为因子，实际上相当于完全的使用了这种颜色参与混合运算。
//    GL_SRC_ALPHA：表示使用源颜色的alpha值来作为因子。
//    GL_DST_ALPHA：表示使用目标颜色的alpha值来作为因子。
//    GL_ONE_MINUS_SRC_ALPHA：表示用1.0减去源颜色的alpha值来作为因子。
//    GL_ONE_MINUS_DST_ALPHA：表示用1.0减去目标颜色的alpha值来作为因子
    //kkglBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //禁用深度缓冲
    //如果先绘制一个不透明的物体，再在其背后绘制半透明物体，本来后面的半透明物体将不会被显示（被不透明的物体遮住了），但如果禁用深度缓冲，则它仍然将会显示，并进行混合
    //glDepthMask(GL_FALSE)
    //kkglDisable(GL_DEPTH_TEST);
    // set other opengl default values
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}


void KKDirector::mainLoop()
{
    if (appWillTerminate) {
        return;
    }
    
    static bool initialized = false;
    if (!initialized)
    {
        initialized = true;
        mainthread = pthread_self();
    }
    
    drawScene();
    
    KKAutoreleasePool::currentPool()->purgePool();
    KKTexture::glPurgeTextures();
}

void KKDirector::drawScene()
{
    calculateDeltaTime();
    
    if (mNextSceneMgr)
    {
        setNextSceneManager();
    }
    
    KKTimerManager::getSingleton()->update(mDeltaTime);
    //GL_COLOR_BUFFER_BIT 清空屏幕颜色
    //GL_DEPTH_BUFFER_BIT 清空深度缓冲
    onRenderBegin();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    KKGLMatrixStack *matrixStack = KKGLMatrixStack::currentMatrixStack();
    
    matrixStack->pushMatrix();
    if (mCurrentSceneMgr)
    {
        mCurrentSceneMgr->draw();
    }
    matrixStack->popMatrix();
    
    onRenderEnd();
    
    //swap buffers
    KKApplication::sharedApplication()->swapBuffers();
}

KKPoint KKDirector::convertToGLPoint(const KKPoint &p) const
{
    if (mDesignWinSize == KKSizeZero)
    {
        return KKPoint(p.x, mWinSizeInPixels.height - p.y);
    }
    else
    {
        float scale = getWinSizeScale();
        KKSize winSize = mWinSize/scale;
        KKSize pixelsSize = mWinSizeInPixels;
        KKPoint point = p;
        point.y = pixelsSize.height - p.y;
        point.x += (pixelsSize.width - winSize.width) / 2.f;
        point.y += (pixelsSize.height - winSize.height) / 2.f;
        return point * scale;
    }
}

KKPoint KKDirector::convertToScreenPoint(const KKPoint &p) const
{
    if (mDesignWinSize == KKSizeZero)
    {
        return KKPoint(p.x, mWinSizeInPixels.height - p.y);
    }
    else
    {
        float scale = getWinSizeScale();
        KKSize winSize = mWinSize/scale;
        KKSize pixelsSize = mWinSizeInPixels;
        KKPoint point = p;
        point /= scale;
        point.y = pixelsSize.height - point.y;
        point.x -= (pixelsSize.width - winSize.width) / 2.f;
        point.y -= (pixelsSize.height - winSize.height) / 2.f;
        return point / CONTENT_SCALE_FACTOR;
    }
}


NS_KK_END
