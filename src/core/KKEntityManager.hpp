//
//  KKSprite.hpp
//  KKEngine
//
//  Created by kewei on 1/13/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#ifndef KKSprite_hpp
#define KKSprite_hpp

#include "KKMacros.h"
#include <list>
#include <map>
#include <vector>
#include <string>
#include "kazmath/GL/matrix.h"
#include "KKGeometry.h"
#include "KKComponent.h"
#include "KKEventType.h"
//#include "KKTouchDispatcher.hpp"

NS_KK_BEGIN


class KKComponent;
class KKEntity;
class KKEntityManager;
class KKSceneManager;

typedef CMultiDelegate1<float> KKEntityUpdateHandler;
typedef CMultiDelegate0 KKEntityDrawHandler;
typedef CMultiDelegate1<KKTouchEvent*> KKEntityTouchHandler;
#if defined(TARGET_MACOS)
typedef CMultiDelegate1<KKMouseEvent*> KKEntityMouseHandler;
#endif
typedef CMultiDelegate3<kmMat4*, const KKPoint&, bool*> KKEntityHitTestHandler;


class KKEntity {
    friend class KKEntityManager;
private:
    int mLastEvent = 0;
    bool mVisible {true};
    bool mUserInteractionEnabled {false};
    bool mHitTestEnabled {true};
    bool mMultiTouchEnabled {false};
    bool mEnabled {true};
    std::string mName {""};
    std::list<KKComponent *> mComponents;
    std::map<FamilyId, KKComponent *> mComponentsMap;
    
    KKEntityManager *mEntityMgr {nullptr};

    KKEntity(KKEntityManager *entityMgr) : mEntityMgr(entityMgr){
        char tmp[25] = {0};
        sprintf(tmp, "%lld", (unsigned long long)this);
        mName = std::string(tmp);
    }
    KKEntity(std::string name, KKEntityManager *entityMgr):mName(name), mEntityMgr(entityMgr){}
    
public:
    ~KKEntity();
    
    CMultiDelegate1<KKEntity *> onDestroyHandler;
    
    KKEntityUpdateHandler onUpdateHandler;
    KKEntityDrawHandler onDrawHandler;
    KKEntityTouchHandler onTouchEvent;
#if defined(TARGET_MACOS)
    KKEntityMouseHandler onMouseEvent;
#endif
    KKEntityHitTestHandler onHitTest;
    
    std::function<void()> onDraw;
    
    KKEntityManager *getEntityManager()
    {
        return mEntityMgr;
    }
    
    void setUserInteractionEnabled(bool b)
    {
        mUserInteractionEnabled = b;
    }
    
    bool userInteractionEnabled() 
    {
        return mUserInteractionEnabled;
    }
    
    void setHitTestEnabled(bool b)
    {
        mHitTestEnabled = b;
    }
    
    bool hitTestEnalbed()
    {
        return mHitTestEnabled;
    }
    
    void setMultiTouchEnabled(bool b)
    {
        mMultiTouchEnabled = b;
    }
    
    bool multiTouchEnabled()
    {
        return mMultiTouchEnabled;
    }
    
    bool isVisible()
    {
        return mVisible;
    }
    
    bool isEnabled()
    {
        return mEnabled;
    }
    
    void setVisible(bool b)
    {
        mVisible = b;
    }
    
    void setEnabled(bool b)
    {
        mEnabled = b;
    }
    
    std::string getName()
    {
        return mName;
    }
    
    const char * name()
    {
        return mName.c_str();
    }
    
    std::list<KKComponent *> & getComponents()
    {
        return mComponents;
    }
    
    std::map<FamilyId, KKComponent *> & getComponentsMap()
    {
        return mComponentsMap;
    }
    
    void onInternalEvent(const char *event, void *info) 
    {
        for_each(mComponents.begin(), mComponents.end(), [ = ](KKComponent *comp){
            comp->onInternalEvent(event, info);
        });
    }
    
    template<typename T> void addComponent(T *comp, int pos = -1);
    template<typename T> void addComponent(int pos = -1);
    template<typename T> bool hasComponent();
    template<typename T> T *getComponent();
    template<typename T> std::list<T *> getComponents();
    template<typename T> void removeComponent();
    template<typename T> T getProperty(std::string name)
    {
        std::list<KKComponent *>::iterator target = std::find_if(mComponents.begin(), mComponents.end(), [ = ](KKComponent *comp){
            return comp && comp->hasProperty(name);
        });
        
        if (target != mComponents.end())
        {
            return (*target)->getProperty<T>(name);
        }
        else
        {
            KKASSERT(0, "Property %s does not exist", name.c_str());
        }
    }
    template<typename T> void setProperty(std::string name, T val);
    bool hasProperty(const std::string &name)
    {
        auto target = std::find_if(mComponents.begin(), mComponents.end(), [ = ](KKComponent *comp){
            return comp && comp->hasProperty(name);
        });
        return target == mComponents.end() ? false : true;
    }
    
    void removeComponent(KKComponent *comp);
    
    KKEntity *hitTest(kmMat4* preTransform, const KKPoint &point);
    
    const void dispatchTouchEvent(KKTouchEvent *event);
    
    KKEntity *getEntityToDispatchEvent(std::vector<KKPoint> location);
};

class KKEntityManager {
    friend class KKSceneManager;
private:
    std::list<KKEntity *> mEntitiesToDestroy;
    KKSceneManager *mSceneMgr {nullptr};
    
private:
    
    KKEntityManager(const KKEntityManager &);
    KKEntityManager& operator=(const KKEntityManager &);
    KKEntityManager(KKSceneManager *sceneMgr):mSceneMgr(sceneMgr){}
    
    void addComponent(KKEntity *entity, FamilyId familyId, KKComponent *comp, int pos = -1);
    
    std::multimap<FamilyId, KKEntity *> mEntityFamilyIdStore;
    std::map<std::string, KKEntity *> mEntityStore;

public:
    KKEntityManager(){}
    ~KKEntityManager();
    
    KKSceneManager *getSceneManager() { return mSceneMgr; }
    
    template<typename T = KKComponent> void getEntities(std::vector<KKEntity *> &result)
    {
        typedef std::multimap<FamilyId, KKEntity *>::iterator itrType;
        std::pair<itrType, itrType> iterPair = mEntityFamilyIdStore.equal_range(T::familyId);
        
        for(auto iter = iterPair.first; iter != iterPair.second; ++iter)
        {
            result.push_back(iter->second);
        }
    }
    
    KKEntity *getEntity(const std::string &name)
    {
        auto itr = mEntityStore.find(name);
        if (itr != mEntityStore.end())
        {
            return itr->second;
        }
        
        return nullptr;
    }
    
    template<typename T = KKComponent> void addComponent(KKEntity *entity, T *comp, int pos)
    {
        addComponent(entity, T::familyId, comp, pos);
    }
    
    void removeComponent(KKEntity *entity, KKComponent *comp);
    
    template<typename T = KKComponent> void removeComponent(KKEntity *entity, T *comp)
    {
        //remove component from entity
        removeComponent(entity, comp);
    }
    
    template<typename T = KKComponent> void removeComponent(KKEntity *entity)
    {
        // TODO assert component in enity
        removeComponent(entity, entity->getComponent<T>());
    }
    
    KKEntity *createEntity(std::string name)
    {
        auto itr = mEntityStore.find(name);
        if (itr != mEntityStore.end())
        {
            return nullptr;
        }
        KKEntity *e = new KKEntity(name, this);
        mEntityStore.insert(std::pair<std::string, KKEntity *>(name, e));
        
        return e;
    }
    
    KKEntity *createEntity(std::string name, const char *comps, ...);
    KKEntity *createAnonymousEntity(const char *comps, ...);
    
    inline void destroyEntity(std::string name)
    {
        destroyEntity(mEntityStore.find(name)->second);
    }
    
    void destroyEntity(KKEntity *entity);
    
private:
    void destroyEntities()
    {
        for (auto &child : mEntitiesToDestroy)
        {
            if (child)
            {
                KK_SAFE_DELETE(child);
            }
        }
        mEntitiesToDestroy.clear();
    }
};



//implement for entity
template<typename T> void KKEntity::addComponent(T *comp, int pos)
{
    mEntityMgr->addComponent<T>(this, comp, pos);
}

template<typename T> void KKEntity::addComponent(int pos)
{
    T *t = new T();
    mEntityMgr->addComponent(this, t, pos);
}

template<typename T> bool KKEntity::hasComponent()
{
    auto itr = mComponentsMap.find(T::familyId);
    return itr != mComponentsMap.end();
}

template<typename T> T * KKEntity::getComponent()
{
    auto itr = mComponentsMap.find(T::familyId);
    if (itr != mComponentsMap.end())
    {
        return  static_cast<T *>(itr->second);
    }
    return nullptr;
}

template<typename T> std::list<T *> KKEntity::getComponents()
{
    std::list<T *> comps;
    for_each(mComponents.begin(), mComponents.end(), [ = ](KKComponent *comp){
        if (comp->getFamilyId() == T::familyId)
        {
            comps.push_back(static_cast<T>(comp));
        }
    });
    
    return comps;
}

template<typename T> void KKEntity::removeComponent()
{
    mEntityMgr->removeComponent<T>(this);
}

template<typename T> void KKEntity::setProperty(std::string name, T val)
{
    auto itr = mComponents.begin();
    for (; itr != mComponents.end(); ++itr)
    {
        if ((*itr)->hasProperty(name))
        {
            (*itr)->setProperty<T>(name, val);
            return;
        }
    }
}

NS_KK_END
#endif /* KKSprite_hpp */
