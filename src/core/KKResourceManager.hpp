//
//  KKResourceManager.hpp
//  KKEngine
//
//  Created by kewei on 3/14/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#ifndef KKResourceManager_hpp
#define KKResourceManager_hpp

#include <stdio.h>
#include "KKSingleton.h"
#include "KKMacros.h"
#include <vector>
#include <map>
#include <string>

NS_KK_BEGIN

class KKResourcePackageLoader;
class KKFileData;

class KKResourceManager : public KKSingleton<KKResourceManager> {
    
    friend class KKSingleton<KKResourceManager>;
    friend class KKResourcePackageLoader;
private:
    float mResourceScale = {1.0f};
    unsigned long mResourceVersion = {0};
    std::vector<std::string> mFileSearchPaths;
    std::map<std::string, KKResourcePackageLoader *> mResLoaders;
    
    KKResourcePackageLoader *mResLoader = {nullptr};
    
    std::string mWritablePath;
    std::string mResPackageFile;
public:
    KKFileData *getFileData(const char *filename);
    bool fileExists(const char *filename);
    void addResourcePackage(const char *filename);
    const char *getSysPathForFile(const char *filename);
    void getSysPathForFile(const char *filename, char path[]);
    unsigned long getResourceVersion();
    void setResourceScale(float scale);
    float getResourceScale();
    
    void addFileSearchPath(const char *path);
    
    void updateResourcePackage(const char *file, const char *patchfile);
    void removeResourcePackage(const char *file);
    unsigned long getResourceVersionOfPackage(const char *package) const;
    bool hasResourcePackage(const char *package) const;
    
#if defined(TARGET_IOS)
    const char* getBundleFullPath();
#endif
    
    //initialize
    void initializeResPath(){ initResourcePaths(); }
private:
    virtual ~KKResourceManager();
    
    void initResourcePaths() { initResourcePathsNative(); }
    void initResourcePathsNative();
    KKFileData *getFileDataInAppBundle(const char *filename);
    bool fileInAppBundle(const char *filename);
};




NS_KK_END

#endif /* KKResourceManager_hpp */
