//
//  AppDelegate.m
//  KKEngine
//
//  Created by kewei on 7/11/14.
//  Copyright (c) 2014 kewei. All rights reserved.
//

#import "AppDelegate.h"
#include "AppDelegate.hpp"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    [self.window center];
    
    auto app = new KKApplication(new ApplicationDelegate());
    
    app->run();
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender;
{
    return YES;
}

@end
