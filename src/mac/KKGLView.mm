//
//  KKGLView.m
//  KKEngine
//
//  Created by kewei on 7/17/14.
//  Copyright (c) 2014 kewei. All rights reserved.
//

#import "KKGLView.h"
#include "KKApplication.h"

static KKGLView *view = 0;

@implementation KKGLView

- (id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    view = self;
    return self;
}

- (id)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    view = self;
    return self;
}

- (id) initWithFrame:(NSRect)frameRect shareContext:(NSOpenGLContext*)context
{
    NSOpenGLPixelFormatAttribute attribs[] =
    {
        NSOpenGLPFAAccelerated,
        //		NSOpenGLPFANoRecovery,
        //		NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAColorSize, 32,
        NSOpenGLPFADepthSize, 24,
        
#if 0
        // Must specify the 3.2 Core Profile to use OpenGL 3.2
        NSOpenGLPFAOpenGLProfile,
        NSOpenGLProfileVersion3_2Core,
#endif
        
        0
    };
    
    NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
    if (!pixelFormat) {
        NSLog(@"No OpenGL pixel format");
    }
    
    if (self = [super initWithFrame:frameRect pixelFormat:pixelFormat]) {
        
        if (context) {
            [self setOpenGLContext:context];
        }
    }
    
    return self;
}

- (NSUInteger) depthFormat
{
    return 24;
}

- (void) update
{
    // XXX: Should I do something here ?
    [super update];
}

- (void) prepareOpenGL
{
    // XXX: Initialize OpenGL context
    
    [super prepareOpenGL];
    
    // Make this openGL context current to the thread
    // (i.e. all openGL on this thread calls will go to this context)
    [[self openGLContext] makeCurrentContext];
    
    // Synchronize buffer swaps with vertical refresh rate
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    //	GLint order = -1;
    //	[[self openGLContext] setValues:&order forParameter:NSOpenGLCPSurfaceOrder];
}

- (void) reshape
{
    [self lockOpenGLContext];
    
    NSRect rect = [self bounds];
    
    USING_NS_KK
    KKApplication::sharedApplication()->setWinSize(rect.size.width, rect.size.height);
    
    [self unlockOpenGLContext];
}

- (void)lockOpenGLContext
{
    NSOpenGLContext *glContext = [self openGLContext];
    NSAssert(glContext, @"FATAL: could not get openGL context");
    
    [glContext makeCurrentContext];
    CGLLockContext((CGLContextObj)[glContext CGLContextObj]);
}

-(void) unlockOpenGLContext
{
    NSOpenGLContext *glContext = [self openGLContext];
    NSAssert( glContext, @"FATAL: could not get openGL context");
    
    CGLUnlockContext((CGLContextObj)[glContext CGLContextObj]);
}

+ (id) sharedGLView
{
    return view;
}


@end











