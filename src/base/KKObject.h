//
//  KKObject.h
//  KKEngine
//
//  Created by kewei on 7/17/14.
//  Copyright (c) 2014 kewei. All rights reserved.
//

#ifndef __KKEngine__KKObject__
#define __KKEngine__KKObject__
#include "KKMacros.h"
#include <string>
#include <vector>
#include <map>
#include "KKStringUtil.hpp"

#define kMaxStringLen (1024*100)

NS_KK_BEGIN

class KKObject;

typedef void (KKObject::*CallFuncO)(KKObject*);

struct ThreadScheduleObject
{
    KKObject *target;
    CallFuncO selector;
    KKObject *object;
    ThreadScheduleObject(KKObject *target, CallFuncO selector, KKObject *object)
    {
        this->target = target;
        this->selector = selector;
        this->object = object;
    }
};

typedef enum
{
    kKKObjectString,
    kKKObjectNumber,
    kKKObjectArray,
    kKKObjectDictionary,
    kKKObjectObject
} KKObjectType;

class KKObject {
protected:
    unsigned int _refCount{1};
    KKObjectType _type{kKKObjectObject};
public:
    KKObject(){}
    virtual ~KKObject(){}
    void retain()
    {
        _refCount++;
    }
    
    void release()
    {
        _refCount--;
        if (_refCount == 0) {
            delete this;
        }
    }
    
    void autorelease();
    
    unsigned int retainCount()
    {
        return _refCount;
    }
    
    KKObjectType getType()
    {
        return _type;
    }
    
    void performFuncOnMainThread(CallFuncO func, KKObject *param);
};

class KKString : public KKObject{
    
public:
    std::string _value = {""};
    KKString()
    {
        _type = kKKObjectString;
    }
    
    KKString(std::string &str):_value(str)
    {
        _type = kKKObjectString;
    }
    
    KKString(const char *str):_value(str)
    {
        _type = kKKObjectString;
    }
    
    KKString(KKString &str):_value(str._value)
    {
        _type = kKKObjectString;
    }
    
    KKString(unsigned char *pData, unsigned long len)
    {
        _value = std::string((const char*)pData, len);
    }
    
    KKString& operator= (const KKString &other)
    {
        _value = other._value;
        
        return *this;
    }
    
    const char *getCString() const
    {
        return _value.c_str();
    }
    
    int compare(const char *pStr) const
    {
        return strcmp(getCString(), pStr);
    }
    
    int length() const
    {
        return static_cast<int>(_value.length());
    }
    
    int intValue() const
    {
        if (length() == 0)
        {
            return 0;
        }
        return atoi(_value.c_str());
    }
    
    int uintValue() const
    {
        if (length() == 0)
        {
            return 0;
        }
        return (unsigned int)atoi(_value.c_str());
    }
    
    float floatValue() const
    {
        if (length() == 0)
        {
            return 0.f;
        }
        return (float)atof(_value.c_str());
    }
    
    double doubleValue() const
    {
        if (length() == 0)
        {
            return 0.f;
        }
        return atof(_value.c_str());
    }
    
    bool boolValue() const
    {
        if (length() == 0)
        {
            return false;
        }
        
        if (0 == strcmp(_value.c_str(), "0") || 0 == strcmp(_value.c_str(), "false"))
        {
            return false;
        }
        
        return true;
    }
    
    
    static KKString* stringWithFormat(const char *format, ...)
    {
        va_list ap;
        va_start(ap, format);
        char *pBuf = (char *)malloc(kMaxStringLen);
        if (pBuf != nullptr)
        {
            vsnprintf(pBuf, kMaxStringLen, format, ap);
            std::string str = pBuf;
            free(pBuf);
            KKString *ret = new KKString(str);
            ret->autorelease();
            va_end(ap);
            return ret;
        }
        va_end(ap);
        return nullptr;
    }
    
    static KKString* stringWithData(unsigned char *data, unsigned long len)
    {
        KKString *ret = nullptr;
        if (data && len > 0)
        {
            char *pStr = (char *)malloc(len+1);
            if (pStr) {
                memset(pStr, '\0', len+1);
                memcpy(pStr, (const char *)data, len);
                ret = new KKString(pStr);
                ret->autorelease();
                free(pStr);
            }
        }
        
        return ret;
    }
    
    static KKString *stringWithContentsOfFile(const char *filename);
};


NS_KK_END

#endif /* defined(__KKEngine__KKObject__) */
