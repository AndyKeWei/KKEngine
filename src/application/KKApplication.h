//
//  KKApplication.h
//  KKEngine
//
//  Created by kewei on 7/11/14.
//  Copyright (c) 2014 kewei. All rights reserved.
//

#ifndef __KKEngine__KKApplication__
#define __KKEngine__KKApplication__

#include "KKMacros.h"
#include "KKApplicationProtocol.h"
#include <vector>

NS_KK_BEGIN

/**
 @brief Enum the language type supported now
 */
typedef enum LanguageType
{
    kLanguageEnglish = 0,
    kLanguageChineseTaiwan,
    kLanguageChinese,
    kLanguageFrench,
    kLanguageItalian,
    kLanguageGerman,
    kLanguageSpanish,
    kLanguageRussian,
    kLanguageKorean,
    kLanguageJapanese,
    kLanguageHungarian,
    kLanguagePortuguese,
    kLanguageArabic,
    kLanguageTurkish,
    kLanguagePolish,
    kLanguageUnknown = 9999
} KKLanguageType;

class KKApplication {
private:
    KKApplicationDelegate *_delegate = {nullptr};
    std::vector<KKLanguageType> mSupportLanguages;
    KKLanguageType mCurrLanguage = {kLanguageUnknown};
    
public:
    KKApplication(KKApplicationDelegate *delegate);
    
    ~KKApplication();
    
    static KKApplication* sharedApplication();
    
    bool applicationDidFinishLaunching();
    
    void applicationDidEnterBackground();
    
    void applicationWillEnterForeground();
    
    void applicationWillTerminate();
    
    int run();
    
    void setAnimationInterval(double interval);
    
    void setWinSize(float width, float height);
    
    void doCaller();
    
    void swapBuffers();
    
    KKLanguageType getCurrentLanguage();
    
    void setSupportedLanguages(std::vector<KKLanguageType> & languages) { mSupportLanguages = languages; }
};



NS_KK_END

#endif /* defined(__KKEngine__KKApplication__) */
