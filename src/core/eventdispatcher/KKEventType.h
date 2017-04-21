//
//  KKEventType.h
//  KKEngine
//
//  Created by kewei on 27/03/2017.
//  Copyright © 2017 kewei. All rights reserved.
//

#ifndef KKEventType_h
#define KKEventType_h

#include "KKMacros.h"
#include "KKGeometry.h"
#include <vector>

NS_KK_BEGIN

typedef struct {
    int64_t _id;
    KKPoint location;
    int tapCount;
} KKTouch;

typedef enum {
    kKKTouchesBegin = 1,
    kKKTouchesMove,
    kKKTouchesEnd,
    kKKTouchesCancel
} KKTouchEventType;

class KKTouchEvent {
    
public:
    int _id;
    KKTouchEventType type;
    std::vector<KKTouch> touches;
};

typedef enum {
    kKKMouseDown = 1,
    kKKMouseMove,
    kKKMouseDrag,
    kKKMouseUp,
    kKKRightMouseDown,
    kKKRightMouseDrag,
    kKKRightMouseUp,
    kKKMouseCancel
} KKMouseEventType;

class KKMouseEvent {
    
public:
    int _id;
    KKMouseEventType type;
    KKPoint location;
    uint32_t clickCount;
};

NS_KK_END

#endif /* KKEventType_h */
