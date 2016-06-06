//
//  KKTransform2DComponent.hpp
//  KKEngine
//
//  Created by kewei on 1/15/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#ifndef KKTransform2DComponent_hpp
#define KKTransform2DComponent_hpp

//类似cocos2d-x  CCNode

//CCNode直接从CCObject继承而来，有如下几个特点：
//
//（1）可以包含其他CCNode节点，可以进行添加/获取/删除子节点操作。
//
//（2）可以执行周期性的回调任务。
//
//（3）可以执行动作。
//
//一些子类化的节点提供了更为丰富的特性和功能。

#include "KKMacros.h"
#include "KKGeometry.h"
#include "KKComponent.h"
#include "kazmath/mat4.h"
#include "KKEntityManager.hpp"
#include "KKSceneManager.hpp"

NS_KK_BEGIN

class KKTransform2DComponent : public  KKComponent {
    
    //friend class KKSceneManager;

    PROPERTIES_DECLARE
    PROPERTY_DECLARE_INIT(KKTransform2DComponent, KKPoint, position, KKPointZero) //设置节点(x, y)在OpenGL中的坐标
    PROPERTY_DECLARE_INIT(KKTransform2DComponent, float, vertexZ, 0.0f) //设置OpenGL Z轴顶点
    PROPERTY_DECLARE_INIT(KKTransform2DComponent, int, zOrder, 0) //设置节点Z轴顺序 当有多个节点在Z轴显示时 引擎会根据它们Z轴的大小决定绘制顺序 Z轴一样大的会覆盖
    PROPERTY_DECLARE_INIT(KKTransform2DComponent, KKPoint, anchorPoint, KKPointZero) //设置节点锚点的位置
    PROPERTY_DECLARE_INIT(KKTransform2DComponent, KKSize, size, KKSizeZero) //设置节点的尺寸
    PROPERTY_DECLARE_INIT(KKTransform2DComponent, float, scale, 1.0f) //设置节点比例因子 同时设置X轴、Y轴
    PROPERTY_DECLARE_INIT(KKTransform2DComponent, float, scaleX, 1.0f) //设置节点X轴比例因子
    PROPERTY_DECLARE_INIT(KKTransform2DComponent, float, scaleY, 1.0f) //设置节点Y轴比例因子
    PROPERTY_DECLARE_INIT(KKTransform2DComponent, float, scaleZ, 1.0f) //设置节点Y轴比例因子
    PROPERTY_DECLARE_INIT(KKTransform2DComponent, float, skewX, 0.0f) //设置X轴的倾斜角度
    PROPERTY_DECLARE_INIT(KKTransform2DComponent, float, skewY, 0.0f) //设置Y轴的倾斜角度
    PROPERTY_DECLARE_INIT(KKTransform2DComponent, float, rotation, 0.0f) //设置节点自转角度
    PROPERTY_DECLARE_INIT(KKTransform2DComponent, float, rotationX, 0.0f) //设置X节点自转角度
    PROPERTY_DECLARE_INIT(KKTransform2DComponent, float, rotationY, 0.0f) //设置Y节点自转角度
    PROPERTY_READONLY_DECLARE_INIT(KKTransform2DComponent, kmMat4*, transform, nullptr)
    PROPERTY_DECLARE(KKTransform2DComponent, kmMat4, additionalTransform)
    PROPERTY_DECLARE_INIT(KKTransform2DComponent, KKEntity *, parent, nullptr)
    PROPERTY_READONLY_DECLARE(KKTransform2DComponent, std::list<KKEntity *>, children)
    
private:
    bool _isTransformGLDirty {true};//transform dirty flag
    kmMat4* _nodeToWorldTransform {nullptr}; //当前节点往世界坐标转换对象
    kmMat4* _worldToNodeTransform {nullptr}; //世界坐标转换成当前节点对象
    std::list<KKEntity*> _childrenToAdd;
    bool _lockChildren {false};
    bool _processChildzOrder {false};
    bool _useAdditionalTransform{false};
    
    inline void processAddChildren()
    {
        if (_lockChildren)
        {
            return;
        }
        if (_processChildzOrder)
        {
            _processChildzOrder = false;
            _childrenToAdd.sort([](KKEntity *child1, KKEntity *child2)
            {
            
                if (child1 && child2)
                {
                    auto tranComp1 = child1->getComponent<KKTransform2DComponent>();
                    auto tranComp2 = child2->getComponent<KKTransform2DComponent>();
                    
                    if (tranComp1 && tranComp2)
                    {
                        return tranComp1->get_zOrder() < tranComp2->get_zOrder(); //从小到大排序
                    }
                    else
                    {
                        if (tranComp2)
                        {
                            return false;
                        }
                        return true;
                    }
                }
                else
                {
                    if (child2)
                    {
                        return false;
                    }
                    return true;
                }
            });
        }
        
        for (const auto & child : _childrenToAdd)
        {
            if (child)
            {
                int zOrder = 0;
                auto transComp = child->getComponent<KKTransform2DComponent>();
                if (transComp)
                {
                    zOrder = transComp->get_zOrder();
                }
                addChild(child, zOrder);
            }
        }
        _childrenToAdd.clear();
    }
    
    
public:
    KKTransform2DComponent()
    {
        m_transform = new kmMat4();
        _nodeToWorldTransform = new kmMat4();
        _worldToNodeTransform = new kmMat4();
    }
    
    ~KKTransform2DComponent()
    {
        KK_SAFE_DELETE(m_transform);
        KK_SAFE_DELETE(_nodeToWorldTransform);
        KK_SAFE_DELETE(_worldToNodeTransform);
    }
    
    void addChild(KKEntity *child , int zOrder)
    {
        KKASSERT(mEntity, "compnent not activated");
        KKASSERT(child, "Argument must be not-nil");
        KKASSERT(child->hasProperty("parent"), "Child should have parent property");
        KKASSERT(child->getProperty<KKEntity *>("parent")==nullptr, "Child already have parent");
        
        if (mEntity->hasProperty("useBatch"))
        {
            if (mEntity->getProperty<bool>("useBatch")) {
                KKLOG("Warning: Children will not use batch");
            }
        }
        
        auto transComp = child->getComponent<KKTransform2DComponent>();
        if (_lockChildren)
        {
            transComp->set_zOrder(zOrder);
            _childrenToAdd.push_back(child);
            return;
        }
        
        if (!transComp)
        {
            transComp->m_zOrder = zOrder = 0;
        }
        else
        {
            transComp->set_parent(mEntity);
        }
        
        if (m_children.size() == 0)
        {
            m_children.push_back(child);
        }
        else
        {
            auto back = m_children.back();
            if (back && back->hasComponent<KKTransform2DComponent>() && back->getProperty<int>("zOrder") <= zOrder)
            {
                m_children.push_back(child);
            }
            else
            {
                auto itr = m_children.begin();
                while (itr != m_children.end())
                {
                    if (*itr)
                    {
                        if ((*itr)->hasComponent<KKTransform2DComponent>())
                        {
                            if ((*itr)->getProperty<int>("zOrder") > zOrder)
                            {
                                m_children.insert(itr, child);
                                break;
                            }
                        }
                        ++itr;
                    }
                    else
                    {
                        itr = m_children.erase(itr);
                    }
                    
                }
                if (itr == m_children.end()) {
                    m_children.push_back(child);
                }
            }
        }
        
//        if (mEntity->hasComponent<HLBatchComponent>())
//        {
//            mEntity->getComponent<HLBatchComponent>()->addChild(child);
//        }
    }
    
    void addChild(KKEntity *child)
    {
        addChild(child, child->getProperty<int>("zOrder"));
    }
    
    void removeChild(KKEntity *child, bool cleanup = false)
    {
        if (!child)
        {
            return;
        }
        KKASSERT(child->hasProperty("parent"), "Child should have parent property");
        KKASSERT(child->getProperty<KKEntity *>("parent")!=nullptr, "Child must be have parent");

        if (child->hasComponent<KKTransform2DComponent>())
        {
            child->getComponent<KKTransform2DComponent>()->set_parent(nullptr);
        }
        
        auto itr = m_children.begin();
        while (itr != m_children.end())
        {
            if ((*itr) == child)
            {
                *itr = nullptr;
                break;
            }
            ++itr;
        }

        if (cleanup)
        {
            mEntity->getEntityManager()->destroyEntity(child);
        }
    }
    
    void removeFromParent(bool cleanup = false)
    {
        if (!m_parent)
        {
            return;
        }
        auto transComp = m_parent->getComponent<KKTransform2DComponent>();
        KKASSERT(transComp, "Parent should have nodegraph component");
        transComp->removeChild(mEntity, cleanup);
    }
    
    void removeAllChildren(bool cleaup = false)
    {
//        if (cleaup)
//        {
//            for_each(m_children.begin(), m_children.end(), [ = ](KKEntity *entity){
//                
//                if (entity)
//                {
//                    mEntity->getEntityManager()->destroyEntity(entity);
//                }
//                
//            });
//        }
        
        if (cleaup)
        {
            for (const auto & entity:m_children)
            {
                if (entity)
                {
                    mEntity->getEntityManager()->destroyEntity(entity);
                }
            }
        }
        
        //TODO batch for entity
//        if (mEntity->hasComponent<HLBatchComponent>())
//        {
//            mEntity->getComponent<HLBatchComponent>()->removeAllChildren(cleanup);
//        }
        m_children.clear();
    }
    
    std::list<KKEntity *> & getChildrenRef()
    {
        return m_children;
    }
    
    kmMat4 *nodeToWorldTransform()
    {
        
        kmMat4 *transform = get_transform();
        kmMat4Assign(_nodeToWorldTransform, transform); //获取nodeToWorld矩阵
        auto p = m_parent;
        if (nullptr != p)
        {
            if (p->hasComponent<KKTransform2DComponent>())
            {
                kmMat4Multiply(_nodeToWorldTransform, p->getComponent<KKTransform2DComponent>()->nodeToWorldTransform(), _nodeToWorldTransform);//矩阵相乘
                p = p->getComponent<KKTransform2DComponent>()->get_parent();
            }
        }
        else
        {
            KKASSERT(mEntity, "compnent not activated");
            KKSceneManager *sceneMgr = mEntity->getEntityManager()->getSceneManager();
            kmMat4Multiply(_nodeToWorldTransform, _nodeToWorldTransform, sceneMgr->currentCameraMatrix());
        }
        
//        kmMat4* transform = get_transform();
//        kmMat4Assign(_nodeToWorldTransform, transform);
//        
//        KKEntity* parent = m_parent;
//        if (parent != NULL)
//        {
//            if (parent->hasComponent<KKTransform2DComponent>())
//            {
//                kmMat4Multiply(_nodeToWorldTransform, parent->getComponent<KKTransform2DComponent>()->nodeToWorldTransform(),_nodeToWorldTransform);
//                parent = parent->getComponent<KKTransform2DComponent>()->get_parent();
//            }
//        }
//        else
//        {
//            KKSceneManager* sceneMgr = mEntity->getEntityManager()->getSceneManager();
//            kmMat4Multiply(_nodeToWorldTransform, _nodeToWorldTransform, sceneMgr->currentCameraMatrix());
//        }
        
        return _nodeToWorldTransform;
    }
    
    kmMat4 *worldToNodeTransform()
    {
        kmMat4Inverse(_worldToNodeTransform, nodeToWorldTransform()); //反转矩阵
        return _worldToNodeTransform;
    }
    
    KKPoint convertToNodeSpace(const KKPoint &worldPoint)
    {
        kmMat4 *transform = worldToNodeTransform();
        
        return KKPoint((float)((double)transform->mat[0]*worldPoint.x+(double)transform->mat[4]*worldPoint.y+transform->mat[12]), (float)((double)transform->mat[1]*worldPoint.x+(double)transform->mat[5]*worldPoint.y+transform->mat[13]));
        
        //TODO cocos2d-x
        //        kmVec3 vec3 = {worldPoint, worldPoint, 0};
        //        kmVec3 ret;
        //        kmVec3Transform(&ret, &vec3, transform);
        //        return KKPoint(ret.x, ret.y);
    }
    
    KKPoint convertToNodeSpaceAR(const KKPoint &worldPoint)
    {
        KKPoint nodePoint = convertToNodeSpace(worldPoint);
        return nodePoint - m_anchorPoint;
    }
    
    KKPoint convertToWorldSpace(const KKPoint &nodePoint)
    {
        kmMat4 *transform = nodeToWorldTransform();
        
        return KKPoint((float)((double)transform->mat[0]*nodePoint.x+(double)transform->mat[4]*nodePoint.y+transform->mat[12]), (float)((double)transform->mat[1]*nodePoint.x+(double)transform->mat[5]*nodePoint.y+transform->mat[13]));
        
        //TODO cocos2d-x
//        kmVec3 vec3 = {nodePoint.x, nodePoint.y, 0};
//        kmVec3 ret;
//        kmVec3Transform(&ret, &vec3, transform);
//        return KKPoint(ret.x, ret.y);
    }
    
    KKPoint converToWorldSpaceAR(const KKPoint &nodePoint)
    {
        KKPoint pt =  nodePoint + m_anchorPoint;
        return convertToWorldSpace(pt);
    }
    
    
    FAMILYID
    
};

NS_KK_END

#endif /* KKTransform2DComponent_hpp */
