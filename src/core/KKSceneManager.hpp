//
//  KKSceneManager.hpp
//  KKEngine
//
//  Created by kewei on 1/13/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#ifndef KKSceneManager_hpp
#define KKSceneManager_hpp

#include "KKMacros.h"
#include "KKEntityManager.hpp"
#include <list>
#include "KKGLMatrixStack.h"

#if defined(TARGET_MACOS)
#include <OpenThreads/ReadWriteMutex>
#endif

NS_KK_BEGIN

class KKEntityManager;
class KKEntity;

class KKSceneManager {
    typedef CMultiDelegate1<KKSceneManager *> SceneHandler;
    
public:
    SceneHandler onActive;
    SceneHandler onDeactive;
    SceneHandler onDestroy;
    
private:
#if defined(TARGET_MACOS)
    mutable OpenThreads::ReadWriteMutex mMutex;
#endif
    KKEntityManager *mEntityMgr {nullptr};
    
    KKEntity *mActiveCamera {nullptr};
    
    kmMat4 mCameraMatrix;
    
    kmMat4 mOldCameraMatrix;
    
    float mCameraChangeProgress {1};
    
public:
    KKSceneManager()
    {
        mEntityMgr = new KKEntityManager(this);
        kmMat4Identity(&mCameraMatrix); //初始化
        kmMat4Identity(&mOldCameraMatrix); //初始化
    }
    
    ~KKSceneManager();
    
    KKEntityManager *getEntityManager() 
    {
        return mEntityMgr;
    }
    
    void addEntity(KKEntity *entity)
    {
#if defined(TARGET_MACOS)
        OpenThreads::ScopedWriteLock lock(mMutex);
#endif
        entities.push_back(entity);
    }
    
    void draw();
    void drawEntity(KKEntity *entity);
    
    kmMat4 *currentCameraMatrix()
    {
        return &mCameraMatrix;
    }
    
    KKEntity *hitTest(const KKPoint&);
public:
    std::list<KKEntity *> entities;
    
};


NS_KK_END

#endif /* KKSceneManager_hpp */
