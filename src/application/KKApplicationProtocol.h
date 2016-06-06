//
//  KKApplicationProtocol.h
//  KKEngine
//
//  Created by kewei on 7/11/14.
//  Copyright (c) 2014 kewei. All rights reserved.
//

#ifndef KKEngine_KKApplicationProtocol_h
#define KKEngine_KKApplicationProtocol_h

#include "KKMacros.h"

NS_KK_BEGIN

class KKApplicationDelegate {
    
public:
    
    virtual ~KKApplicationDelegate() {}
    
    virtual bool applicationDidFinishLaunching() = 0;
    
    virtual void applicationDidEnterBackground() = 0;
    
    virtual void applicationWillEnterForeground() = 0;
    
    virtual void applicationWillTerminate() = 0;
};


NS_KK_END

#endif
