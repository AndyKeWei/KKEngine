//
//  KKObject.cpp
//  KKEngine
//
//  Created by kewei on 7/17/14.
//  Copyright (c) 2014 kewei. All rights reserved.
//

#include "KKObject.h"
#include "KKAutoreleasePool.h"
#include "KKFileUtil.hpp"
#include "KKUtil.h"
#include "KKResourceManager.hpp"
#include <OpenThreads/Mutex>

NS_KK_BEGIN

extern OpenThreads::Mutex scheduleSelectorMutex;
extern std::vector<ThreadScheduleObject> pendingScheduleArray;

void KKObject::performFuncOnMainThread(CallFuncO func, KKObject *param)
{
    if (currentIsMainThread())
    {
        (this->*func)(param);
        return;
    }
    scheduleSelectorMutex.lock();
    this->retain();
    
    if (param)
    {
        param->retain();
    }
    ThreadScheduleObject ob(this, func, param);
    pendingScheduleArray.push_back(ob);
    scheduleSelectorMutex.unlock();
}

void KKObject::autorelease()
{
    KKAutoreleasePool *pool = KKAutoreleasePool::currentPool();
    KKASSERT(pool != nullptr, "autorelease with no pool");
    if (pool) {
        pool->addObject(this);
    } else {
        KKLOG("warning: autorelease with no pool");
    }
}



KKString * KKString::stringWithContentsOfFile(const char *filename)
{
    KKFileData *data = KKResourceManager::getSingleton()->getFileData(filename);
    if (!data)
    {
        KKLOG("file (%s) does not exist", filename);
    }
    
    KKString *res = stringWithData(data->buffer, data->size);
    
    KK_SAFE_DELETE(data);
    
    return res;
}





NS_KK_END