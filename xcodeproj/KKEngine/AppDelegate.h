//
//  AppDelegate.h
//  KKEngine
//
//  Created by kewei on 7/11/14.
//  Copyright (c) 2014 kewei. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class KKGLView;
@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (weak) IBOutlet NSWindow *window;
@property (weak) IBOutlet KKGLView *glView;

@end

