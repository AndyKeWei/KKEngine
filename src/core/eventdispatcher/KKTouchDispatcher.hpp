//
//  KKTouchDispatcher.hpp
//  KKEngine
//
//  Created by kewei on 28/03/2017.
//  Copyright © 2017 kewei. All rights reserved.
//

#ifndef KKTouchDispatcher_hpp
#define KKTouchDispatcher_hpp

#include <stdio.h>
#include "KKEntityManager.hpp"
#include "KKGeometry.h"
#include "KKMacros.h"
#include "KKView.hpp"
#include <sys/time.h>
#include "KKDelegate.h"
#include "KKEventType.h"
#include <vector>
#include <map>

NS_KK_BEGIN

namespace gui {
    class KKView;
}

class KKTouchDispatcher {
    
private:
    KKEntity *mCurrEntity = nullptr;
    gui::KKView *mCurrView = nullptr;
    static KKTouchDispatcher dispatcher;
    bool mNeedsHitTest = false;
    uint32_t mClickCount = 0;
    int64_t mEventDispatching = -1;
    KKPoint lastPoint;
    std::map<int64_t, std::pair<int, struct timeval> > mTouchEvents;
    
    typedef struct {
        int tapCount;
        KKPoint location;
        struct timeval timestamp;
    } CompleteTouchInfo;
    std::list<CompleteTouchInfo> mCompleteTouchInfos;
    int mId = 1;
    
    KKTouchDispatcher() {}
    
public:
    static KKTouchDispatcher *sharedTouchDispatcher ()
    {
        return &dispatcher;
    }
    
    CMultiDelegate1<KKTouchEvent*> touchEventHandler;
    
    void needsHitTest()
    {
        this->mNeedsHitTest = true;
    }
    
    void hitTest(const KKPoint&);
    void touchesBegin(std::vector<KKTouch>&);
    void touchesMove(std::vector<KKTouch>&);
    void touchesEnd(std::vector<KKTouch>&);
    void touchesCancel(std::vector<KKTouch>&);
    
    void dispatchTouchEventToEntity(KKEntity *entity, KKTouchEvent *event)
    {
        auto isDispatchToCurrEntity = (mCurrEntity == entity);
        auto *tmp = mCurrEntity;
        mCurrEntity = entity;
        bool touchHandled = false;
        //玩家有可能自己通过设置addTouchEventListener进行了事件分发，所以需要判断onTouchEvent, 如果有设置，需要通过onTouchEvent拦截
        KKEntityTouchHandler::ListDelegateIterator itr = mCurrEntity->onTouchEvent.begin();
        for (; itr != mCurrEntity->onTouchEvent.end(); itr++)
        {
            if (*itr)
            {
                //需要通过onTouchEvent拦截事件
                (*itr)->invoke(event);
                touchHandled = true;
                if (isDispatchToCurrEntity && !mCurrEntity) // entity may be destroyed during event handling
                {
                    return;
                }
                else if (!mCurrEntity)
                {
                    mCurrEntity = tmp;
                    return;
                }
            }
        }
        if (!touchHandled)
        {
            //如果没有onTouchEvent拦截事件， 分发touch事件
            mCurrEntity->dispatchTouchEvent(event);
        }
        
        mCurrEntity = tmp; //还原当前entity
    }
    
    void cancelCurrentEventHandler() {
        mCurrView = nullptr;
        mCurrEntity = nullptr;
        mNeedsHitTest = true;
    }
    
    void clearCurrentTarget()
    {
        if (mCurrView || mCurrEntity)
        {
            std::vector<KKTouch> dispatchTouches;
            for (auto itr : mTouchEvents)
            {
                dispatchTouches.push_back({itr.first, KKPointZero, itr.second.first});
            }
            if (!dispatchTouches.empty())
            {
                touchesCancel(dispatchTouches);
            }
            cancelCurrentEventHandler();
        }
    }
};

NS_KK_END

#endif /* KKTouchDispatcher_hpp */
