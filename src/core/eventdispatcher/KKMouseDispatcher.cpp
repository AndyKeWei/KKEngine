//
//  KKMouseDispatcher.cpp
//  KKEngine
//
//  Created by kewei on 27/03/2017.
//  Copyright © 2017 kewei. All rights reserved.
//

#include "KKMouseDispatcher.hpp"
//#include <stdlib.h>

NS_KK_BEGIN

KKMouseDispatcher KKMouseDispatcher::dispatcher;

void KKMouseDispatcher::hitTest(const KKPoint&)
{
    
}



void KKMouseDispatcher::onMouseEvent(KKMouseEvent& event)
{
    if (event.type == kKKMouseDown || event.type == kKKRightMouseDown)
    {
        struct timeval now;
        gettimeofday(&now, 0);
        auto delta = (now.tv_sec - timestamp.tv_sec) + (now.tv_usec - timestamp.tv_usec) / 1000000.0f;
        if (timestamp.tv_sec == 0  || delta > 0.5f)
        {
            mClickCount = 0;
            mNeedsHitTest = true;
        }
        gettimeofday(&timestamp, 0);
        
        KKPoint loc = event.location;
        if (!mNeedsHitTest && (abs((int)loc.x - (int)lastPoint.x) > 5 || abs((int)loc.y - (int)lastPoint.y) > 5))
        {
            mNeedsHitTest = true;
        }
        if (mNeedsHitTest) {
            hitTest(loc);
        }
        auto curView = mCurrView;
        auto curEntity = mCurrEntity;
        if (event.type == kKKMouseUp || event.type == kKKRightMouseUp)
        {
            lastPoint = event.location;
        }
        if (event.type == kKKMouseDown)
        {
            mClickCount++;
        }
        event.clickCount = mClickCount;
        if (!curView || !curEntity) {
            return;
        }
        
        ++mId;
        if (mId == INT_MAX) {
            mId = 1;
        }
#ifdef USE_TOUCH_EVENT
        KKTouchEvent touchEvent;
        touchEvent._id = mId;
        KKTouch touch;
        touch._id = 0;
        touch.location = event.location;
        touch.tapCount = event.clickCount;
        touchEvent.touches.push_back(touch);
        switch (event.type) {
            case kKKMouseDown:
                touchEvent.type = kKKTouchesBegin;
                break;
            case kKKMouseDrag:
                touchEvent.type = kKKTouchesMove;
                break;
            case kKKMouseUp:
                touchEvent.type = kKKTouchesEnd;
                break;
            case kKKMouseCancel:
                touchEvent.type = kKKTouchesCancel;
                break;
            default:
                break;
        }
#endif
    }
}
























NS_KK_END
