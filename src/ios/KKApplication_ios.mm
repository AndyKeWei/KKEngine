//
//  KKApplication_ios.cpp
//  KKEngine
//
//  Created by kewei on 3/16/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#include <stdio.h>
#include "KKApplication.h"

NS_KK_BEGIN

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
        else if ([languageCode isEqualToString:@"zh"] || [languageCode isEqualToString:@"zh-Hans"])
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
        else if ([languageCode isEqualToString:@"pt"] || [languageCode isEqualToString:@"pt-BR"] || [languageCode isEqualToString:@"pt-PT"]){
            ret = kLanguagePortuguese;
        }
        else if ([languageCode isEqualToString:@"ar"]){
            ret = kLanguageArabic;
        }
        else if ([languageCode isEqualToString:@"tr"] || [languageCode isEqualToString:@"tr-CY"] || [languageCode isEqualToString:@"tr-TR"]){
            ret = kLanguageTurkish;
        }
        else if ([languageCode isEqualToString:@"pl"] || [languageCode isEqualToString:@"pl-PL"]){
            ret = kLanguagePolish;
        }
        
        if (!mSupportLanguages.empty() && mSupportLanguages.size() > 0) {
            if (std::find(mSupportLanguages.begin(), mSupportLanguages.end(), ret) != mSupportLanguages.end()) {
                mCurrLanguage = ret;
            }
            else
            {
                mCurrLanguage = mSupportLanguages[0];
            }
        }
    }
    return mCurrLanguage;
}

NS_KK_END