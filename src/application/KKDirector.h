//
//  KKDirector.h
//  KKEngine
//
//  Created by kewei on 7/17/14.
//  Copyright (c) 2014 kewei. All rights reserved.
//

#ifndef __KKEngine__KKDirector__
#define __KKEngine__KKDirector__

#include "KKMacros.h"
#include "KKSingleton.h"
#if defined(TARGET_WIN32)
#include "hltime.h"
#else
#include <sys/time.h>
#endif
#include "KKGeometry.h"
#include "KKGL.h"
#include "KKSceneManager.hpp"
#include <vector>
#include "KKClassRegister.hpp"
#include "KKTransform2DComponent.hpp"
#include "KKSpriteComponent.hpp"
#include "KKColorComponent.hpp"
#include "KKColorAdvanceComponent.hpp"
#include "KKClippingComponent.hpp"
#include "KKScissorComponent.hpp"
#include "KKShaderComponent.hpp"

NS_KK_BEGIN

class KKDirector : public KKSingleton<KKDirector>{
    
    friend class KKSingleton<KKDirector>;

private:
    KKSize mWinSize = {0,0};
    KKSize mDesignWinSize = KKSizeZero;
    KKSize mWinSizeInPixels = KKSizeZero;
    
    float mViewPortScaleFactor = {0};
    float mContentScaleFactor = {1};
    
    struct timeval mLastUpdate;
    float mDeltaTime;
    bool mNextDeltaTimeZero {true};
    
    bool mShowStats = {false};
    
    KKSceneManager *mCurrentSceneMgr {nullptr}; //当前scene
    KKSceneManager *mNextSceneMgr {nullptr};//下一个执行的scene
    
    std::vector<KKSceneManager *> mSceneMgrStack;
    std::vector<KKSceneManager *> mSceneToDestroy;
    
private:
    
    KKDirector();
    
    inline void registerComponents() const
    {
        REGISTER_CLASS(KKTransform2DComponent);
        REGISTER_CLASS(KKSpriteComponent);
        REGISTER_CLASS(KKColorComponent);
        REGISTER_CLASS(KKColorAdvanceComponent);
        REGISTER_CLASS(KKClippingComponent);
        REGISTER_CLASS(KKScissorComponent);
        REGISTER_CLASS(KKShaderComponent);
    }
    
    void setProjection();
    
    void calculateDeltaTime()
    {
        static timeval now;
        gettimeofday(&now, 0);
        if (mNextDeltaTimeZero)
        {
            mDeltaTime = 0;
            mNextDeltaTimeZero = false;
        }
        else
        {
            mDeltaTime = (now.tv_sec - mLastUpdate.tv_sec) + (now.tv_usec - mLastUpdate.tv_usec) / 1000000.0f;
            mDeltaTime = MAX(mDeltaTime, 0);
        }
        mLastUpdate = now;
    }
public:
    void mainLoop();
    void drawScene();
    
    CMultiDelegate0 onRenderBegin;
    CMultiDelegate0 onRenderEnd;
    
    
    void setDesignWinSize(const KKSize &size)
    {
        mDesignWinSize = size;
        setProjection();
    }
    
    void setDesignWinWidth(float width)
    {
        setDesignWinSize(KKSize(width, 0));
    }
    
    void setDesignWinHeight(float height)
    {
        setDesignWinSize(KKSize(0, height));
    }
    
    void setContentScaleFactor(float scaleFactor)
    {
#ifdef TARGET_IOS
        mContentScaleFactor = scaleFactor;
#else
        mContentScaleFactor = 1;
#endif
    }
    
    float getContentScaleFactor()
    {
#ifdef TARGET_IOS
        return mContentScaleFactor;
#else
        return 1;
#endif
    }
    
    
    void reshapeProjection(const KKSize &winSize)
    {
        mWinSize = winSize;
        mWinSizeInPixels = winSize;
        setProjection();
    }
    
    KKSize getWinSize() const
    {
        return mWinSize;
    }
    
    KKSize &getWinSizeInPixels()
    {
        return mWinSizeInPixels;
    }
    
    float getWinSizeScale() const
    {
        float scale = 1.f;
        if (mDesignWinSize == KKSizeZero)
        {
            return 1.f;
        }
        if (mDesignWinSize.width != 0 && mDesignWinSize.height != 0)
        {
            scale = MAX(mDesignWinSize.width/mWinSizeInPixels.width, mDesignWinSize.height/mWinSizeInPixels.height);
        }
        else if (mDesignWinSize.width == 0) // width fill and heigth scale
        {
            scale = mDesignWinSize.height/mWinSizeInPixels.height;
        }
        else if (mDesignWinSize.height == 0) // height fill and width scale
        {
            scale = mDesignWinSize.width/mWinSizeInPixels.width;
        }
        
        return scale;
    }
    
    KKPoint convertToGLPoint(const KKPoint &p) const;
    KKPoint convertToScreenPoint(const KKPoint &p) const;
    
    void setViewPort()
    {
        //gl windows reshape
        KKSize pixelsSize = mWinSize / mViewPortScaleFactor;
        GLint viewPortX = (mWinSizeInPixels.width - pixelsSize.width) / 2;
        GLint viewPortY = (mWinSizeInPixels.height - pixelsSize.height) / 2;
        GLint viewPortWidth = pixelsSize.width;
        GLint viewPortHeight = pixelsSize.height;
        glViewport(viewPortX, viewPortY, viewPortWidth, viewPortHeight);
    }
    
    void setGLDefaultValues()
    {
        
    }
    
    KKSceneManager *getCurrentSceneManager() const
    {
        return mCurrentSceneMgr;
    }
    
    void runWithSceneManager(KKSceneManager *scene)
    {
        KKASSERT(scene, "Running scene should be not-nil");
        KKASSERT(mCurrentSceneMgr==nullptr, "Running runWithSceneManager method twice");
        pushSceneManager(scene);
    }
    
    void pushSceneManager(KKSceneManager *scene)
    {
        KKASSERT(scene, "Running scene should be not-nil");
        mSceneMgrStack.push_back(scene);
        mNextSceneMgr = scene;
    }
    
    void popSceneManager()
    {
        KKSceneManager *currentSceneMgr = mSceneMgrStack.back();
        mSceneMgrStack.pop_back();
        if (mSceneMgrStack.size() <= 0)
        {
            abort(); //exit game
        }
        mNextSceneMgr = mSceneMgrStack.back();
        mSceneToDestroy.push_back(currentSceneMgr);
    }
    
    void setNextSceneManager()
    {
        if (mCurrentSceneMgr)
        {
            auto itr = std::find(mSceneToDestroy.begin(), mSceneToDestroy.end(), mCurrentSceneMgr);
            if (itr != mSceneToDestroy.end())
            {
                //TODO destroy currentScene
                mCurrentSceneMgr->onDeactive(mCurrentSceneMgr);
            }
        }
        for_each(mSceneToDestroy.begin(), mSceneToDestroy.end(), [ = ](KKSceneManager *sceneMgr){
            
            if (sceneMgr)
            {
                sceneMgr->onDestroy(sceneMgr);
            }
            KK_SAFE_DELETE(sceneMgr);
        });
        mSceneToDestroy.clear();
        
        mCurrentSceneMgr = mNextSceneMgr;
        mNextSceneMgr = nullptr;
        if (mCurrentSceneMgr)
        {
            mCurrentSceneMgr->onActive(mCurrentSceneMgr);
        }
    }
};

NS_KK_END


#endif /* defined(__KKEngine__KKDirector__) */
