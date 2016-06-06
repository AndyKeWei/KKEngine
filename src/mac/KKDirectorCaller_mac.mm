//
//  KKDirectorCaller.m
//  KKEngine
//
//  Created by kewei on 7/17/14.
//  Copyright (c) 2014 kewei. All rights reserved.
//

#import "KKDirectorCaller_mac.h"
#include "KKMacros.h"
#include "KKApplication.h"
#include "KKAutoreleasePool.h"
#include "KKGLView.h"

NSThread *runningThread = 0;

static id s_sharedDirectorCaller;

@implementation KKDirectorCaller
@synthesize interval;

+ (id)sharedDirectorCaller
{
    if (s_sharedDirectorCaller == nil)
    {
        s_sharedDirectorCaller = [KKDirectorCaller new];
    }
    
    return s_sharedDirectorCaller;
}

- (instancetype)init
{
    self = [super init];
    
    displayLink = nullptr;
    
    return self;
}

+ (void)destroy
{
    s_sharedDirectorCaller = nil;
}

static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,
                                      const CVTimeStamp *inNow,
                                      const CVTimeStamp *inOutputTime,
                                      CVOptionFlags flagsIn,
                                      CVOptionFlags *flagsOut,
                                      void *displayLinkContext)
{

    USING_NS_KK
    static KKAutoreleasePool autoreleasePool;
    @autoreleasepool {
        
        do {
            [[KKGLView sharedGLView] lockOpenGLContext];
            if (!runningThread)
            {
                runningThread = [NSThread currentThread];
                USING_NS_KK
                if (!KKApplication::sharedApplication()->applicationDidFinishLaunching())
                {
                    KKLOG("application does not launching");
                    [NSThread exit];
                    break;
                }
            }
            [(__bridge KKDirectorCaller*)displayLinkContext doCaller:nil];
            [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:nil];
            [[KKGLView sharedGLView] unlockOpenGLContext];
            
        } while (false);
    }
    
    return kCVReturnSuccess;
}

- (void)startMainLoop
{
    if (displayLink) {
        CVDisplayLinkStop(displayLink);
        CVDisplayLinkRelease(displayLink);
        displayLink = nullptr;
    }
    
    // Create a display link capable of being used with all active displays
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    
    // Set the displayLink output callback function
    CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, (__bridge void *)self);
    
    KKGLView *glView = [KKGLView sharedGLView];
    CGLContextObj cglContextObj = (CGLContextObj)[[glView openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormatObj = (CGLPixelFormatObj)[[glView pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContextObj, cglPixelFormatObj);
    
    CVDisplayLinkStart(displayLink);
}

- (void)setAnimationInterval:(double)interval
{
    self.interval = interval;
}

- (void)swapBuffers
{
    [[[KKGLView sharedGLView] openGLContext] flushBuffer];
}

- (void)doCaller:(id)sender
{
    //TODO runLoop
    USING_NS_KK
    KKApplication::sharedApplication()->doCaller();
}

@end
