//
//  KKTouchDispatcher.cpp
//  KKEngine
//
//  Created by kewei on 28/03/2017.
//  Copyright © 2017 kewei. All rights reserved.
//

#include "KKTouchDispatcher.hpp"

NS_KK_BEGIN

KKTouchDispatcher KKTouchDispatcher::dispatcher;

void KKTouchDispatcher::hitTest(const KKPoint&)
{
    
}

void KKTouchDispatcher::touchesBegin(std::vector<KKTouch>& touches)
{
    if (touches.empty()) return;
    struct timeval now;
    gettimeofday(&now, 0);
    
    auto itr = mCompleteTouchInfos.begin();
    for (; itr != mCompleteTouchInfos.end();)
    {
        float delta = (now.tv_sec - (*itr).timestamp.tv_sec) + (now.tv_usec - (*itr).timestamp.tv_usec) / 1000000.0f;
        if (delta > 0.5f)
        {
            itr = mCompleteTouchInfos.erase(itr);
        }
        else
        {
            ++itr;
        }
    }
    
    KKPoint distance;
    auto touchesitr = touches.begin();
    if (mEventDispatching == -1)
    {
        mId = 1;
        KKPoint loc = touches[0].location;
        if (!mNeedsHitTest)
        {
            mNeedsHitTest = true;
            touches[0].tapCount = 1;
            for (auto itr : mCompleteTouchInfos)
            {
                KKPoint distance = loc - itr.location;
                if (distance.x+distance.x+distance.y*distance.y < 25)
                {
                    touches[0].tapCount = itr.tapCount+1;
                    mNeedsHitTest = false;
                    break;
                }
            }
        }
        
        if (mNeedsHitTest)
        {
            gui::KKView *lastView = mCurrView;
            KKEntity *lastEntity = mCurrEntity;
            hitTest(touches[0].location);
            if (lastView != mCurrView || lastEntity != mCurrEntity)
            {
                touches[0].tapCount = 1;
            }
        }
        
        ++touchesitr;
    }
    
//    for (; touchesitr != touches.end(); ++touchesitr)
//    {
//        (*touchesitr).tapCount = 1;
//        KKPoint loc = (*touchesitr).location;
//        for (auto itr : mCompleteTouchInfos)
//        {
//            
//        }
//    }
    if (!mCurrView || !mCurrEntity)
    {
        return;
    }
    KKTouchEvent event;
    event._id = mId++;
    event.touches = touches;
    event.type = kKKTouchesBegin;
    touchEventHandler(&event);
    do {
        if (mCurrView)
        {
            if (!mCurrView->multiTouchEnabled())
            {
                if (mEventDispatching != -1)
                    break;
                if (touches.size() > 1)
                {
                    event.touches.clear();
                    event.touches.push_back(touches[0]);
                }
            }
            mCurrView->handleTouchEvent(&event);
        }
        else
        {
            if (mCurrEntity)
            {
                if (!mCurrEntity->multiTouchEnabled())
                {
                    if (mEventDispatching != -1)
                    {
                        break;
                    }
                    if (touches.size() > 1)
                    {
                        event.touches.clear();
                        event.touches.push_back(touches[0]);
                    }
                }
                dispatchTouchEventToEntity(mCurrEntity, &event);
            }
        }
    } while (0);
    
    
    if (mEventDispatching == -1 && touches.size() > 1)
    {
        mEventDispatching = touches[0]._id;
    }
    
    for_each(touches.begin(), touches.end(), [ = ](KKTouch touch){
        
        mTouchEvents.insert(std::make_pair(touch._id, std::pair<int, struct timeval>(touch.tapCount, now)));
        
    });
}

void KKTouchDispatcher::touchesMove(std::vector<KKTouch>& touches)
{
    for (auto itr : touches)
    {
        auto res = mTouchEvents.find(itr._id);
        if (res != mTouchEvents.end())
        {
            itr.tapCount = res->second.first;
        }
    }
    
    if (!mCurrView && !mCurrEntity)
    {
        return;
    }
    KKTouchEvent event;
    event._id = mId++;
    event.touches = touches;
    event.type = kKKTouchesMove;
    touchEventHandler(&event);
    
    if (mCurrView)
    {
        if (!mCurrView->multiTouchEnabled())
        {
            event.touches.clear();
            for (auto itr : touches)
            {
                if (itr._id == mEventDispatching)
                {
                    event.touches.push_back(itr);
                    break;
                }
            }
            if (event.touches.empty())
            {
                return;
            }
        }
        mCurrView->handleTouchEvent(&event);
    }
    else
    {
        if (mCurrEntity)
        {
            if (!mCurrEntity->multiTouchEnabled())
            {
                event.touches.clear();
                for (auto itr : touches)
                {
                    if (itr._id == mEventDispatching)
                    {
                        event.touches.push_back(itr);
                        break;
                    }
                }
                if (event.touches.empty())
                {
                    return;
                }
            }
        }
        dispatchTouchEventToEntity(mCurrEntity, &event);
    }

}

void KKTouchDispatcher::touchesEnd(std::vector<KKTouch>& touches)
{
    CompleteTouchInfo info;
    for (auto itr : touches)
    {
        auto res = mTouchEvents.find(itr._id);
        if (res != mTouchEvents.end())
        {
            itr.tapCount = res->second.first;
            info.timestamp = res->second.second;
        }
        
        if (itr._id == mEventDispatching)
        {
            lastPoint = itr.location;
        }
        
        info.location = itr.location;
        info.tapCount = itr.tapCount;
        mCompleteTouchInfos.push_back(info);
    }
    
    for (auto itr : touches)
    {
        auto res = mTouchEvents.find(itr._id);
        if (res != mTouchEvents.end())
        {
            mTouchEvents.erase(res);
        }
    }
    
    if (!mCurrView && !mCurrEntity)
    {
        if (mTouchEvents.empty())
        {
            mEventDispatching = -1;
        }
        return;
    }
    KKTouchEvent event;
    event._id = mId++;
    event.touches = touches;
    event.type = kKKTouchesEnd;
    touchEventHandler(&event);
    
    do {
        
        if (mCurrView)
        {
            if (!mCurrView->multiTouchEnabled())
            {
                event.touches.clear();
                for (auto itr : touches)
                {
                    if (itr._id == mEventDispatching)
                    {
                        event.touches.push_back(itr);
                        break;
                    }
                }
                if (event.touches.empty())
                {
                    break;
                }
            }
            mCurrView->handleTouchEvent(&event);
        }
        else
        {
            if (mCurrEntity)
            {
                if (!mCurrEntity->multiTouchEnabled())
                {
                    event.touches.clear();
                    for (auto itr : touches)
                    {
                        if (itr._id == mEventDispatching)
                        {
                            event.touches.push_back(itr);
                            break;
                        }
                    }
                    if (event.touches.empty())
                    {
                        break;
                    }
                }
            }
            dispatchTouchEventToEntity(mCurrEntity, &event);
        }
        
    } while (0);
    
    if (mTouchEvents.empty())
    {
        mEventDispatching = -1;
    }
}

void KKTouchDispatcher::touchesCancel(std::vector<KKTouch>& touches)
{
    for (auto itr : touches)
    {
        auto res = mTouchEvents.find(itr._id);
        if (res != mTouchEvents.end())
        {
            itr.tapCount = res->second.first;
        }
        
        if (itr._id == mEventDispatching)
        {
            lastPoint = itr.location;
        }
    }
    for (auto itr : touches)
    {
        auto res = mTouchEvents.find(itr._id);
        if (res != mTouchEvents.end())
        {
            mTouchEvents.erase(res);
        }
    }
    
    if (!mCurrView && !mCurrEntity)
    {
        if (mTouchEvents.empty())
        {
            mEventDispatching = -1;
        }
        return;
    }
    KKTouchEvent event;
    event._id = mId++;
    event.touches = touches;
    event.type = kKKTouchesCancel;
    touchEventHandler(&event);
    
    do {
        if (mCurrView)
        {
            if (!mCurrView->multiTouchEnabled())
            {
                event.touches.clear();
                for (auto itr : touches)
                {
                    if (itr._id == mEventDispatching)
                    {
                        event.touches.push_back(itr);
                        break;
                    }
                }
                if (event.touches.empty())
                {
                    break;
                }
            }
            mCurrView->handleTouchEvent(&event);
        }
        else
        {
            if (mCurrEntity)
            {
                if (!mCurrEntity->multiTouchEnabled())
                {
                    event.touches.clear();
                    for (auto itr : touches)
                    {
                        if (itr._id == mEventDispatching)
                        {
                            event.touches.push_back(itr);
                            break;
                        }
                    }
                    if (event.touches.empty())
                    {
                        break;
                    }
                }
            }
            dispatchTouchEventToEntity(mCurrEntity, &event);
        }
    } while (0);
    
    if (mTouchEvents.empty())
    {
        mEventDispatching = - 1;
    }
}

NS_KK_END
