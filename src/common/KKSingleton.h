//
//  KKSingleton.h
//  KKEngine
//
//  Created by kewei on 7/11/14.
//  Copyright (c) 2014 kewei. All rights reserved.
//

#ifndef KKEngine_KKSingleton_h
#define KKEngine_KKSingleton_h

#include "KKMacros.h"

NS_KK_BEGIN

template <typename T>
class KKSingleton {
private:
    KKSingleton(const KKSingleton<T> &);
    
    KKSingleton& operator=(const KKSingleton<T> &);
    
public:
    KKSingleton(void){};
    
    virtual ~KKSingleton(void){};
    
public:
    friend T;
    static T* getSingleton(void)
    {
        static T instance;
        
        return &instance;
    }
};

NS_KK_END


#endif
