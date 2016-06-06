//
//  KKGLView.h
//  KKEngine
//
//  Created by kewei on 7/17/14.
//  Copyright (c) 2015 kewei. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface KKGLView : NSOpenGLView

+ (id)sharedGLView;

/** initializes the CCGLView with a frame rect and an OpenGL context */
- (id)initWithFrame:(NSRect)frameRect  shareContext:(NSOpenGLContext *)context;

/** uses and locks the OpenGL context */
- (void)lockOpenGLContext;

/** unlocks the openGL context */
- (void)unlockOpenGLContext;

/** returns the depth format of the view in BPP */
- (NSUInteger)depthFormat;

@end
