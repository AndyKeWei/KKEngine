//
//  KKAutoreleasePool.h
//  KKEngine
//
//  Created by kewei on 7/17/14.
//  Copyright (c) 2014 kewei. All rights reserved.
//

#ifndef __KKEngine__KKAutoreleasePool__
#define __KKEngine__KKAutoreleasePool__

#include "KKMacros.h"
#include "KKObject.h"
#include <list>

NS_KK_BEGIN

class KKAutoreleasePool {
    
    std::list<KKObject *> ObjectList;
    std::list<int> aa;
public:
    KKAutoreleasePool();
    ~KKAutoreleasePool();
    
    static KKAutoreleasePool *currentPool();
    
    void purgePool();
    
    void addObject(KKObject *);
};


NS_KK_END

#endif /* defined(__KKEngine__KKAutoreleasePool__) */
