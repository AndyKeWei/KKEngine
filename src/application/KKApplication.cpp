//
//  KKApplication.cpp
//  KKEngine
//
//  Created by kewei on 7/11/14.
//  Copyright (c) 2014 kewei. All rights reserved.
//

#include "KKApplication.h"

extern bool appWillTerminate;

NS_KK_BEGIN

static KKApplication* pSharedApplication{nullptr};

KKApplication::KKApplication(KKApplicationDelegate *delegate)
{
    _delegate = delegate;
    if (!pSharedApplication) {
        pSharedApplication = this;
    }
}

KKApplication::~KKApplication()
{
    pSharedApplication = nullptr;
    delete _delegate;
}

KKApplication* KKApplication::sharedApplication()
{
    //    HLASSERT(pSharedApplication, "no application created");
    return pSharedApplication;
}

bool KKApplication::applicationDidFinishLaunching()
{
    return _delegate->applicationDidFinishLaunching();
}

void KKApplication::applicationDidEnterBackground()
{
    _delegate->applicationDidEnterBackground();
}

void KKApplication::applicationWillEnterForeground()
{
    _delegate->applicationWillEnterForeground();
}

void KKApplication::applicationWillTerminate()
{
    appWillTerminate = true;
    _delegate->applicationWillTerminate();
}


NS_KK_END