//
//  KKApplication_mac.cpp
//  KKEngine
//
//  Created by kewei on 7/11/14.
//  Copyright (c) 2014 kewei. All rights reserved.
//

#include "KKApplication.h"
#include "KKDirector.h"
#include "KKAutoreleasePool.h"
#import "KKDirectorCaller_mac.h"

NS_KK_BEGIN


int KKApplication::run()
{
    static KKAutoreleasePool autoreleasePool;
    [[KKDirectorCaller sharedDirectorCaller] startMainLoop];
    
    return 0;
}
 
void KKApplication::setAnimationInterval(double interval)
{
    [[KKDirectorCaller sharedDirectorCaller] setAnimationInterval:interval];
}

void KKApplication::setWinSize(float width, float height)
{
    KKDirector::getSingleton()->reshapeProjection(KKSize(width, height));
}


void KKApplication::doCaller()
{
    //TODO runLoop
    //KKLOG("caller now");
    KKDirector::getSingleton()->mainLoop();
}


void KKApplication::swapBuffers()
{
    [[KKDirectorCaller sharedDirectorCaller] swapBuffers];
}

KKLanguageType KKApplication::getCurrentLanguage() 
{
    if (mCurrLanguage == kLanguageUnknown)
    {
        // get the current language and country config
        NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
        NSArray *languages = [defaults objectForKey:@"AppleLanguages"];
        NSString *currentLanguage = [languages objectAtIndex:0];
        
        // get the current language code.(such as English is "en", Chinese is "zh" and so on)
        NSDictionary* temp = [NSLocale componentsFromLocaleIdentifier:currentLanguage];
        NSString * languageCode = [temp objectForKey:NSLocaleLanguageCode];
        
        KKLanguageType ret = kLanguageEnglish;
        if ([currentLanguage isEqualToString:@"zh-Hant"])
        {
            ret = kLanguageChineseTaiwan;
        }
        else if ([languageCode isEqualToString:@"zh"])
        {
            ret = kLanguageChinese;
        }
        else if ([languageCode isEqualToString:@"en"])
        {
            ret = kLanguageEnglish;
        }
        else if ([languageCode isEqualToString:@"fr"]){
            ret = kLanguageFrench;
        }
        else if ([languageCode isEqualToString:@"it"]){
            ret = kLanguageItalian;
        }
        else if ([languageCode isEqualToString:@"de"]){
            ret = kLanguageGerman;
        }
        else if ([languageCode isEqualToString:@"es"]){
            ret = kLanguageSpanish;
        }
        else if ([languageCode isEqualToString:@"ru"]){
            ret = kLanguageRussian;
        }
        else if ([languageCode isEqualToString:@"ko"]){
            ret = kLanguageKorean;
        }
        else if ([languageCode isEqualToString:@"ja"]){
            ret = kLanguageJapanese;
        }
        else if ([languageCode isEqualToString:@"hu"]){
            ret = kLanguageHungarian;
        }
        else if ([languageCode isEqualToString:@"pt"])
        {
            ret = kLanguagePortuguese;
        }
        else if ([languageCode isEqualToString:@"ar"])
        {
            ret = kLanguageArabic;
        }
        
        if (!mSupportLanguages.empty() && mSupportLanguages.size() > 0) {
            if (std::find(mSupportLanguages.begin(), mSupportLanguages.end(), ret) != mSupportLanguages.end())
            {
                mCurrLanguage = ret;
            }
            else
            {
                mCurrLanguage = mSupportLanguages[0];
            }
        }
        else
        {
            mCurrLanguage = ret;
        }
    }
    return mCurrLanguage;
}

NS_KK_END