//
//  KKMouseDispatcher.hpp
//  KKEngine
//
//  Created by kewei on 27/03/2017.
//  Copyright © 2017 kewei. All rights reserved.
//

#ifndef KKMouseDispatcher_hpp
#define KKMouseDispatcher_hpp

#include <stdio.h>
#include "KKMacros.h"
#include "KKGeometry.h"
#include "KKEventType.h"
#include <sys/time.h>
#include "KKEntityManager.hpp"
#include "KKView.hpp"
#include "KKDelegate.h"
#include "GameEngineConfig.h"

NS_KK_BEGIN

namespace gui {
    class KKView;
}

class KKMouseDispatcher {
    
private:
    static KKMouseDispatcher dispatcher;
    bool mNeedsHitTest = false;
    struct timeval timestamp;
    uint32_t mClickCount = 0;
    KKPoint lastPoint;
    int mId = 1;
    
    KKEntity *mCurrEntity = nullptr;
    gui::KKView *mCurrView = nullptr;
    
public:
    static KKMouseDispatcher *shareMouseDispatcher()
    {
        return &dispatcher;
    }
    
    CMultiDelegate1<KKMouseEvent*> mouseEventHandler;
    
    void hitTest(const KKPoint&);
    void onMouseEvent(KKMouseEvent&);
    
    void needsHitTest()
    {
        mNeedsHitTest = true;
    }
    
};


NS_KK_END

#endif /* KKMouseDispatcher_hpp */
