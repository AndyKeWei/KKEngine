//
//  KKComponent.h
//  KKEngine
//
//  Created by kewei on 1/12/16.
//	Copyright (c) 2014 kw. All rights reserved.
//

#ifndef KKEngine_KKComponent_h
#define KKEngine_KKComponent_h

#include "KKMacros.h"
#include "KKObject.h"
#include "KKTimer.h"
#include <map>
#include "KKStringUtil.hpp"
#include "KKClassRegister.hpp"

#define PROPERTIES_DECLARE \
public: \
static std::map<std::string, cocos2d::PropertyBase *> _properties; \
virtual std::map<std::string, cocos2d::PropertyBase *>& properties() {return _properties;}

#define PROPERTIES_INITIAL(CLASS) \
std::map<std::string, cocos2d::PropertyBase *> CLASS::_properties;

#define PROPERTY_DECLARE(CLASS, TYPE, NAME) \
public:\
TYPE m_##NAME; \
static cocos2d::KKPropertyInfo<TYPE> prop_##NAME; \
public:\
TYPE get_##NAME(); \
void set_##NAME(TYPE NAME);

#define PROPERTY_DECLARE_INIT(CLASS, TYPE, NAME, INITVALUE) \
public:\
TYPE m_##NAME{INITVALUE}; \
static cocos2d::KKPropertyInfo<TYPE> prop_##NAME; \
public:\
TYPE get_##NAME(); \
void set_##NAME(TYPE NAME);

#define PROPERTY_READONLY_DECLARE(CLASS, TYPE, NAME) \
private:\
TYPE m_##NAME; \
static cocos2d::KKPropertyInfo<TYPE> prop_##NAME; \
public:\
TYPE get_##NAME();

#define PROPERTY_READONLY_DECLARE_INIT(CLASS, TYPE, NAME, INITVALUE) \
private:\
TYPE m_##NAME{INITVALUE}; \
static cocos2d::KKPropertyInfo<TYPE> prop_##NAME; \
public:\
TYPE get_##NAME();

#define PROPERTY_DECLARE_IMPLEMENT(CLASS, TYPE, NAME) \
private:\
TYPE m_##NAME; \
static cocos2d::KKPropertyInfo<TYPE> prop_##NAME; \
public:\
TYPE get_##NAME() \
{ \
return m_##NAME; \
} \
void set_##NAME(TYPE NAME) \
{ \
m_##NAME = NAME; \
}

#define PROPERTY_DECLARE_IMPLEMENT_INIT(CLASS, TYPE, NAME, INITVALUE) \
private:\
TYPE m_##NAME{INITVALUE}; \
static cocos2d::KKPropertyInfo<TYPE> prop_##NAME; \
public:\
TYPE get_##NAME() \
{ \
return m_##NAME; \
} \
void set_##NAME(TYPE NAME) \
{ \
m_##NAME = NAME; \
}

#define PROPERTY_READONLY_DECLARE_IMPLEMENT(CLASS, TYPE, NAME) \
private:\
TYPE m_##NAME; \
static cocos2d::KKPropertyInfo<TYPE> prop_##NAME; \
public:\
TYPE get_##NAME() \
{ \
return m_##NAME; \
}

#define PROPERTY_READONLY_DECLARE_IMPLEMENT_INIT(CLASS, TYPE, NAME, INITVALUE) \
private:\
TYPE m_##NAME{INITVALUE}; \
static cocos2d::KKPropertyInfo<TYPE> prop_##NAME; \
public:\
TYPE get_##NAME() \
{ \
return m_##NAME; \
}


#define FAMILYID \
public: \
static const cocos2d::FamilyId familyId; \
virtual cocos2d::FamilyId getFamilyId() \
{ \
return familyId; \
}

#define INIT_FAMILYID(CLASS) const cocos2d::FamilyId CLASS::familyId = KK_STRING_BKDRHASH(CLASS);

#define REGISTER_PROPERTY_DECLARE(CLASS, TYPE, NAME) static cocos2d::KKPropertyInfo<TYPE> prop_##NAME;

#define REGISTER_PROPERTY(CLASS, TYPE, NAME) cocos2d::KKPropertyInfo<TYPE> CLASS::prop_##NAME(#TYPE, CLASS::_properties, #NAME, (void (KKComponent::*)(TYPE))&CLASS::set_##NAME, (TYPE (KKComponent::*)())&CLASS::get_##NAME);
//#define REGISTER_PROPERTY_REF(CLASS, TYPE, NAME) cocos2d::KKPropertyInfo<TYPE&> CLASS::prop_##NAME(#TYPE, CLASS::_properties, #NAME, (void (KKComponent::*)(TYPE&))&CLASS::set_##NAME, (TYPE& (KKComponent::*)())&CLASS::get_##NAME);
#define REGISTER_PROPERTY_READONLY(CLASS, TYPE, NAME) cocos2d::KKPropertyInfo<TYPE> CLASS::prop_##NAME(#TYPE, CLASS::_properties, #NAME, NULL, (TYPE (KKComponent::*)())&CLASS::get_##NAME);
//#define REGISTER_PROPERTY_READONLY_REF(CLASS, TYPE, NAME) cocos2d::KKPropertyInfo<TYPE&> CLASS::prop_##NAME(#TYPE, CLASS::_properties, #NAME, NULL, (TYPE& (KKComponent::*)())&CLASS::get_##NAME);

NS_KK_BEGIN

typedef unsigned int FamilyId;

class KKEntity;
class KKComponent;

typedef struct _PropertyBase
{
    std::string type;
} PropertyBase;

template <typename T>
struct KKPropertyInfo : public  PropertyBase{
    void (KKComponent::*setter)(T);
    T (KKComponent::*getter)();
    KKPropertyInfo(std::string type, std::map<std::string, PropertyBase *>& props, std::string name, void (KKComponent::*setter)(T), T (KKComponent::*getter)())
    {
        this->type = type;
        this->setter = setter;
        this->getter = getter;
        props.insert(std::make_pair(name, reinterpret_cast<PropertyBase*>(this)));
    }
};

template <class T = KKComponent>
class KKRegisterComponent {
    
public:
    friend T;
    virtual FamilyId getFamilyId()
    {
        return KK_STRING_BKDRHASH(T);
    }
    static FamilyId familyId()
    {
        return KK_STRING_BKDRHASH(T);
    }
};

class KKComponent : public KKObject {
    friend class KKEntityManager;
protected:
    KKEntity *mEntity {nullptr};
    
    void active(KKEntity *entity)
    {
        KKASSERT(!mEntity, "Component is already attached to an entity!");
        mEntity = entity;
        onActive();
    }
    
    void dective()
    {
        onDeactive();
        mEntity = nullptr;
    }

    
public:
    KKComponent(){}
    
    virtual ~KKComponent(){}
    
    virtual void onActive() {}
    virtual void onDeactive() {}
    virtual FamilyId getFamilyId() = 0;
    
    virtual std::map<std::string, PropertyBase *>& properties() = 0;
    
    virtual void onInternalEvent(const char *event, void *info) {}

    
    KKEntity *getEntity() const
    {
        return mEntity;
    }
    
    bool hasProperty(const std::string &name)
    {
        return properties().find(name) != properties().end();
    }
    
    template<typename T> T getProperty(const std::string &name)
    {
        KKPropertyInfo<T> * info = reinterpret_cast<KKPropertyInfo<T>*>(properties().find(name)->second);
        KKASSERT(info != nullptr, "property %s does not exist", name.c_str());
        return (this->*info->getter)();
    }
    
    template<typename T> void setProperty(const std::string &name, T value)
    {
        KKPropertyInfo<T> * info = reinterpret_cast<KKPropertyInfo<T>*>(properties().find(name)->second);
        KKASSERT(info != nullptr, "property %s does not exist", name.c_str());
        (this->*info->setter)(value);
    }
};

NS_KK_END

#endif  /* defined(KKEngine_KKComponent_h) */
