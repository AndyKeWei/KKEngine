//
//  KKEntityManager.cpp
//  KKEngine
//
//  Created by kewei on 1/13/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#include "KKEntityManager.hpp"
#include "KKSceneManager.hpp"
#include "KKTouchDispatcher.hpp"

NS_KK_BEGIN

KKEntity::~KKEntity()
{
    for_each(mComponents.begin(), mComponents.end(), [ = ](KKComponent *comp){
        KK_SAFE_DELETE(comp);
    });
}

KKEntity *KKEntity::hitTest(kmMat4* preTransform, const KKPoint &point)
{
    if (!mHitTestEnabled)
    {
        return nullptr;
    }
    kmMat4 transform;
    if (hasProperty("transform"))
    {
        //合并矩阵
        kmMat4Multiply(&transform, preTransform, getProperty<kmMat4*>("transform"));
    }
    else
    {
        //矩阵赋值
        kmMat4Assign(&transform, preTransform);
    }
    
    //事件先判断zOrder >= 0 的subEntity, 然后再到自己, 最后还需判断zOrder < 0 的subEntity
    bool hasProp = hasProperty("children");
    if (hasProp)
    {
        auto children = getProperty<std::list<KKEntity *>>("children");
        for (auto itr = children.rbegin(); itr != children.rend(); itr++)
        {
            if (*itr != nullptr)
            {
                if ((*itr)->hasProperty("zOrder") && (*itr)->getProperty<int>("zOrder") < 0)
                {
                    //zOrder < 0 说明子entity绘制在本身下文，故不必判断事件
                    break;
                }
                KKEntity *res = (*itr)->hitTest(&transform, point);
                if (res)
                {
                    return  res;
                }
            }
        }
    }
    
    //self event
    if (mUserInteractionEnabled)
    {
        if (!onHitTest.empty())
        {
            bool ret;
            onHitTest(&transform, point, &ret);
            return ret ? this : nullptr;
        }
        else
        {
            auto size = getProperty<KKSize>("size");
            if (hasProperty("size") && size !=KKSizeZero)
            {
                kmMat4 transform1;
                kmMat4Inverse(&transform1, &transform);
                float localX = (float)((double)transform1.mat[0]*point.x+(double)transform1.mat[4]*point.x+(double)transform1.mat[12]);
                float localY = (float)((double)transform1.mat[1]*point.y+(double)transform1.mat[5]*point.y+(double)transform1.mat[13]);
                if (localX>=0&&localY>=0&&localX<=size.width&&localY<=size.height)
                {
                    return this;
                }
                
            }
        }
    }
    
    //还需判断zOrder < 0 的subEntity
    if (hasProp)
    {
        auto children = getProperty<std::list<KKEntity *>>("children");
        for (auto itr = children.rbegin(); itr != children.rend(); itr++)
        {
            if (*itr != nullptr)
            {
                if ((*itr)->hasProperty("zOrder") && (*itr)->getProperty<int>("zOrder") >= 0)
                {
                    //zOrder < 0 说明子entity绘制在本身下文，故不必判断事件
                    continue;
                }
                KKEntity *res = (*itr)->hitTest(&transform, point);
                if (res)
                {
                    return  res;
                }
            }
        }
    }
    
    return  nullptr;
}

const void KKEntity::dispatchTouchEvent(KKTouchEvent *event)
{
    if (mLastEvent == event->_id) //already dispatched
    {
        return;
    }
    mLastEvent = event->_id;
    std::vector<KKPoint> location;
    for (auto itr : event->touches)
    {
        location.push_back(itr.location);
    }
    auto entity = getEntityToDispatchEvent(location);
    if (entity)
    {
        KKTouchDispatcher::sharedTouchDispatcher()->dispatchTouchEventToEntity(entity, event);
    }
}


KKEntity *KKEntity::getEntityToDispatchEvent(std::vector<KKPoint> location)
{
    auto *sceneMgr = getEntityManager()->getSceneManager();
    KKEntity *e = nullptr;
    for (auto itr : location)
    {
        e = sceneMgr->hitTest(itr);
        if (e) {
            return e;
        }
    }
    
    return nullptr;
}


KKEntityManager::~KKEntityManager()
{
    for_each (mSceneMgr->entities.begin(), mSceneMgr->entities.end(), [ = ](KKEntity *e){
        destroyEntity(e);
    });
    
    //destroy other entities
    auto itr = mEntityStore.begin();
    while (itr != mEntityStore.end())
    {
        destroyEntity(itr->second);
        itr = mEntityStore.begin();
    }
    destroyEntities();
    
}

void KKEntityManager::addComponent(KKEntity *entity, FamilyId familyId, KKComponent *comp, int pos)
{
    KKASSERT(entity, "does not add component for empty entity");
    KKASSERT(comp!=nullptr, "does not add empty component for entity");
    entity->onInternalEvent(COMPONENT_WILL_ADD, comp);
    mEntityFamilyIdStore.insert(std::pair<FamilyId, KKEntity *>(familyId, entity));
    if (pos >= entity->mComponents.size())
        pos = -1;
    if (pos == -1)
    {
        entity->getComponents().push_back(comp);
    }
    else
    {
        auto itr = entity->mComponents.begin();
        std::advance(itr, pos);
        entity->getComponents().insert(itr, comp);
    }
    entity->mComponentsMap.insert(std::pair<FamilyId, KKComponent *>(familyId, comp));
    comp->active(entity);
}

void KKEntityManager::removeComponent(KKEntity *entity, KKComponent *comp)
{
    KKASSERT(!entity, "does not add component for empty entity");
    KKASSERT(!comp, "does not add empty component for entity");
    auto itr = std::find(entity->mComponents.begin(), entity->mComponents.end(), comp);
    KKASSERT(itr != entity->mComponents.end(), "entity does not contain specified component");
    (*itr)->dective();
    entity->onInternalEvent(COMPONENT_WILL_REMOVE, comp);
    entity->mComponents.erase(itr);
    auto itr1 = entity->mComponentsMap.find((*itr)->getFamilyId());
    if (itr1 != entity->mComponentsMap.end() && itr1->second == comp)
    {
        entity->mComponentsMap.erase(itr1);
    }
    
    typedef std::multimap<FamilyId, KKEntity *>::iterator itrType;
    std::pair<itrType, itrType> iterPair = mEntityFamilyIdStore.equal_range(comp->getFamilyId());
    for(auto iter = iterPair.first; iter != iterPair.second; ++iter)
    {
        if (iter->second == entity)
        {
            mEntityFamilyIdStore.erase(iter);
            break;
        }
    }
    comp->release();
}

void KKEntityManager::destroyEntity(KKEntity *entity)
{
    if (!entity)
        return;
    if (std::find(mEntitiesToDestroy.begin(), mEntitiesToDestroy.end(), entity) != mEntitiesToDestroy.end())
        return;
    mEntitiesToDestroy.push_back(entity);
    
    auto itr = mEntityStore.find(entity->name());
    if (itr == mEntityStore.end() || itr->second != entity)
    {
        KKASSERT(0, "cannot find names %s entity", entity->name());
    }
    entity->onDestroyHandler(entity);
    
    //deactive component
    auto components = entity->mComponents;
    for (auto & component : components)
    {
        component->dective();
    }
    
    
    auto itr1 = std::find(mSceneMgr->entities.begin(), mSceneMgr->entities.end(), entity);
    if (itr1 != mSceneMgr->entities.end())
    {
        *itr1 = nullptr;
    }
    
    mEntityStore.erase(entity->getName());
    auto itr2 = mEntityFamilyIdStore.begin();
    while (itr2 != mEntityFamilyIdStore.end()) {
        if (itr2->second == entity)
        {
            itr2 = mEntityFamilyIdStore.erase(itr2);
        }
        else
        {
            ++itr2;
        }
    }
}

KKEntity *KKEntityManager::createEntity(std::string name, const char *comps, ...)
{
    if (mEntityStore.find(name) != mEntityStore.end())
    {
        KKASSERT(0, "entity named (%s) is duplicate", name.c_str());
    }
    
    KKEntity *entity = new KKEntity(name, this);
    mEntityStore.insert(std::pair<std::string, KKEntity *>(name, entity));
    
    va_list params;
    va_start(params, comps);
    while (comps)
    {
        KKComponent *component = static_cast<KKComponent *>(g_factory.construct(comps));
        if (!component)
        {
            KKLOG("%s not found", comps);
        }
        addComponent(entity, StringUtil::BKDRHash(comps), component);
        
        comps = va_arg(params, const char *);
    }
    
    va_end(params);
    
    return entity;
}

KKEntity *KKEntityManager::createAnonymousEntity(const char *comps, ...)
{
    KKEntity *entity = new KKEntity(this);
    
    va_list params;
    va_start(params, comps);
    while (comps)
    {
        KKComponent *component = static_cast<KKComponent *>(g_factory.construct(comps));
        if (!component)
        {
            KKLOG("%s not found", comps);
        }
        addComponent(entity, StringUtil::BKDRHash(comps), component);
        
        comps = va_arg(params, const char *);
    }
    
    va_end(params);
    
    return entity;
}


NS_KK_END
