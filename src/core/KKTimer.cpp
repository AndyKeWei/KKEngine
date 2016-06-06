//
//  KKTimer.cpp
//  
//
//  Created by kewei on 1/7/16.
//
//

#include "KKTimer.h"

NS_KK_BEGIN

KKTimer::KKTimer(float interval, bool repeate, int priority):interval(interval), repeate(repeate), priority(priority)
{
    KKTimerManager::getSingleton()->mTimersToAdd.push_back(this);
}

KKTimer::~KKTimer()
{
    do {
        auto &timers = KKTimerManager::getSingleton()->mTimers;
        auto res = std::find(timers.begin(), timers.end(), this);
        if (res == timers.end())
        {
            auto &timersToAdd = KKTimerManager::getSingleton()->mTimersToAdd;
            res = std::find(timersToAdd.begin(), timersToAdd.end(), this);
            if (res == timersToAdd.end())
            {
                break;
            }
        }
        *res = nullptr;
    } while (0);
}

void KKTimer::update(float dt)
{
    if (canceled) {
        delete this;
        return;
    }
    elapsed += dt;
    delta += dt;
    if (elapsed >= interval)
    {
        delegate(this, delta);
        if (repeate && !canceled)
        {
            delta = 0;
            if (interval)
            {
                do {
                    elapsed -= interval;
                } while (elapsed > interval);
            }
            else
            {
                elapsed = 0;
            }
        }
        else
        {
            delete this;
        }
    }
}

void KKTimerManager::update(float dt)
{
    if (mTimersToAdd.begin() != mTimersToAdd.end()) {
        mTimers.insert(mTimers.end(), mTimersToAdd.begin(), mTimersToAdd.end());
        mTimersToAdd.clear();
        mTimers.sort([](const KKTimer *timer1, const KKTimer *timer2){
            if (timer1 == nullptr || timer2 == nullptr)
            {
                return  false;
            }
            return timer1->priority > timer2->priority;
        });
    }
    
    auto itr = mTimers.begin();
    while (itr != mTimers.end())
    {
        if (*itr == nullptr)
        {
            itr = mTimers.erase(itr);
        }
        else
        {
            (*itr)->update(dt);
            ++itr;
        }
    }
}

void KKTimerManager::cancelAllTimers()
{
    for_each(mTimersToAdd.begin(), mTimersToAdd.end(), [ = ](KKTimer *timer){
        delete timer;
    });
    mTimersToAdd.clear();
    for_each(mTimers.begin(), mTimers.end(), [ = ](KKTimer *timer){
        if (timer)
        {
            timer->cancel();
        }
    });
}




NS_KK_END