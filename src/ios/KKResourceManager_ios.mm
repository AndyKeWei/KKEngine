//
//  KKResourceManager_ios.cpp
//  KKEngine
//
//  Created by kewei on 3/16/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#include <stdio.h>
#include "KKResourceManager.hpp"

NS_KK_BEGIN


void KKResourceManager::initResourcePathsNative()
{
    NSString* path = [[NSBundle mainBundle] resourcePath];
    addFileSearchPath([path UTF8String]);
    NSString *docPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString* dataPath = [docPath stringByAppendingPathComponent:@"data"];
    addFileSearchPath([dataPath UTF8String]);
    mWritablePath = [dataPath UTF8String];
    mWritablePath += "/";
    if (![[NSFileManager defaultManager] fileExistsAtPath:dataPath])
    {
        [[NSFileManager defaultManager] createDirectoryAtPath:dataPath withIntermediateDirectories:NO attributes:nil error:nil];
        uint8_t attrValue = 1;
        setxattr([dataPath UTF8String], "com.apple.MobileBackup", &attrValue, sizeof(attrValue), 0, 0);
    }
}

KKFileData* KKResourceManager::getFileDataInAppBundle(const char* file)
{
    NSString* path = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:[NSString stringWithUTF8String:file]];
    return getFileData([path UTF8String]);
}

bool KKResourceManager::fileInAppBundle(const char* file)
{
    NSString* path = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:[NSString stringWithUTF8String:file]];
    return [[NSFileManager defaultManager] fileExistsAtPath:path];
}

const char* KKResourceManager::getBundleFullPath()
{
    static NSString* path = [[[NSBundle mainBundle] resourcePath] stringByAppendingString:@"/"];
    return [path UTF8String];
}




NS_KK_END