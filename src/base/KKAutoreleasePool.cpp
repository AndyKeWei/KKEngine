//
//  KKAutoreleasePool.cpp
//  KKEngine
//
//  Created by kewei on 7/17/14.
//  Copyright (c) 2014 kewei. All rights reserved.
//

#include "KKAutoreleasePool.h"
#include <map>
#include <pthread.h>
#include <OpenThreads/Mutex>

bool appWillTerminate = false;

NS_KK_BEGIN

std::map<pthread_t, std::list<KKAutoreleasePool *> > poolManager;

KKAutoreleasePool::KKAutoreleasePool()
{
    pthread_t thread = pthread_self();
    auto itr = poolManager.find(thread);
    if (itr != poolManager.end())
    {
        itr->second.push_back(this);
    }
    else
    {
        std::list<KKAutoreleasePool *> l;
        l.push_back(this);
        static OpenThreads::Mutex mutex;
        mutex.lock();
        poolManager.insert(std::make_pair(thread, l));
        mutex.unlock();
    }
}

KKAutoreleasePool::~KKAutoreleasePool()
{
    purgePool();
    if (appWillTerminate) {
        return;
    }
    pthread_t thread = pthread_self();
    auto itr = poolManager.find(thread);
    KKASSERT(itr != poolManager.end(), "autoreleasepool may dealloc in wrong thread");
    std::list<KKAutoreleasePool *>& l = itr->second;
    
    auto itr1 = std::find(l.begin(), l.end(), this);
    KKASSERT(itr1 != l.end(), "autoreleasepool may dealloc in wrong thread");
    l.erase(itr1);
}

KKAutoreleasePool *KKAutoreleasePool::currentPool()
{
    pthread_t thread = pthread_self();
    auto itr = poolManager.find(thread);
    if (itr != poolManager.end()) {
        return itr->second.back();
    }
    
    return nullptr;
}

void KKAutoreleasePool::purgePool()
{
//    for_each(ObjectList.begin(), ObjectList.end(), [ = ](KKObject *o){
//        if (o)
//        {
//            o->release();
//        }
//    });
    
    for (auto &itr : ObjectList)
    {
        if (itr)
        {
            itr->release();
        }
    }
    
    ObjectList.clear();
}

void KKAutoreleasePool::addObject(KKObject *o)
{
    ObjectList.push_back(o);
}






NS_KK_END