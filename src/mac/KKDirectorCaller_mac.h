//
//  KKDirectorCaller.h
//  KKEngine
//
//  Created by kewei on 7/17/14.
//  Copyright (c) 2014 kewei. All rights reserved.
//

@interface KKDirectorCaller : NSObject {
    
    CVDisplayLinkRef displayLink;
    double interval;
    
}

@property (assign) double interval;

+ (id)sharedDirectorCaller;
+ (void)destroy;

- (void)startMainLoop;
- (void)doCaller:(id)sender;
- (void)setAnimationInterval:(double)interval;
- (void)swapBuffers;

@end

@interface NSObject(CADisplayLink)
+(id) displayLinKKithTarget: (id)arg1 selector:(SEL)arg2;
-(void) addToRunLoop: (id)arg1 forMode: (id)arg2;
-(void) setFrameInterval: (int)interval;
-(void) invalidate;
@end
