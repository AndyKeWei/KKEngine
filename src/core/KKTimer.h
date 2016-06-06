//
//  KKTimer.h
//  
//
//  Created by kewei on 1/7/14.
//
//

#ifndef ____KKTimer__
#define ____KKTimer__

#include "KKMacros.h"
#include "KKSingleton.h"
#include "KKDelegate.h"
#include <list>

NS_KK_BEGIN


class KKTimer {
    
    typedef CDelegate2<KKTimer *, float> KKTimerDelegate;
public:
    int priority {0};
    KKTimerDelegate delegate;
public:
    KKTimer(float interval, bool repeate = false, int priority = 0);
    ~KKTimer();
    
    void update(float dt);
    void cancel() {canceled = true;}
private:
    bool canceled {false};
    bool repeate {false};
    float interval {0};
    float elapsed {0};
    float delta {0};
};


class KKTimerManager : public KKSingleton<KKTimerManager> {
    
    friend class KKSingleton<KKTimerManager>;
    friend class KKTimer;
public:
    void update(float dt);
    void cancelAllTimers();
    
private:
    std::list<KKTimer *> mTimers;
    std::list<KKTimer *> mTimersToAdd;
};

NS_KK_END


#endif /* defined(____KKTimer__) */
