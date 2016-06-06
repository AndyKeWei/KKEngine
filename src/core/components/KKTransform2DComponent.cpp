//
//  KKTransform2DComponent.cpp
//  KKEngine
//
//  Created by kewei on 1/15/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#include "KKTransform2DComponent.hpp"

NS_KK_BEGIN

INIT_FAMILYID(KKTransform2DComponent)

PROPERTIES_INITIAL(KKTransform2DComponent)
REGISTER_PROPERTY(KKTransform2DComponent, KKPoint, position) //设置节点(x, y)在OpenGL中的坐标
REGISTER_PROPERTY(KKTransform2DComponent, float, vertexZ) //设置OpenGL Z轴顶点
REGISTER_PROPERTY(KKTransform2DComponent, int, zOrder) //设置节点Z轴顺序 当有多个节点在Z轴显示时 引擎会根据它们Z轴的大小决定绘制顺序 Z轴大的会覆盖
REGISTER_PROPERTY(KKTransform2DComponent, KKPoint, anchorPoint) //设置节点锚点的位置
REGISTER_PROPERTY(KKTransform2DComponent, KKSize, size) //设置节点的尺寸
REGISTER_PROPERTY(KKTransform2DComponent, float, scale) //设置节点比例因子 同时设置X轴、Y轴
REGISTER_PROPERTY(KKTransform2DComponent, float, scaleX) //设置节点X轴比例因子
REGISTER_PROPERTY(KKTransform2DComponent, float, scaleY) //设置节点Y轴比例因子
REGISTER_PROPERTY(KKTransform2DComponent, float, scaleZ) //设置节点Y轴比例因子
REGISTER_PROPERTY(KKTransform2DComponent, float, skewX) //设置X轴的倾斜角度
REGISTER_PROPERTY(KKTransform2DComponent, float, skewY) //设置Y轴的倾斜角度
REGISTER_PROPERTY(KKTransform2DComponent, float, rotation) //设置节点自转角度
REGISTER_PROPERTY(KKTransform2DComponent, float, rotationX) //设置节点自转角度
REGISTER_PROPERTY(KKTransform2DComponent, float, rotationY) //设置节点自转角度
REGISTER_PROPERTY_READONLY(KKTransform2DComponent, kmMat4*, transform)
REGISTER_PROPERTY(KKTransform2DComponent, kmMat4, additionalTransform)
REGISTER_PROPERTY(KKTransform2DComponent, KKEntity *, parent)
REGISTER_PROPERTY_READONLY(KKTransform2DComponent, std::list<KKEntity *>, children)

KKPoint KKTransform2DComponent::get_position()
{
    return m_position;
}

void KKTransform2DComponent::set_position(KKPoint pos)
{
    if (m_position != pos)
    {
        m_position = pos;
        _isTransformGLDirty = true;
        mEntity->onInternalEvent("position_changed", nullptr);//观察者
        if (m_parent)
        {
            m_parent->onInternalEvent("child_position_changed", mEntity);
        }
    }
}

float KKTransform2DComponent::get_vertexZ()
{
    return m_vertexZ;
}

void KKTransform2DComponent::set_vertexZ(float z)
{
    if (m_vertexZ != z) {
        m_vertexZ = z;
        _isTransformGLDirty = true;
    }
}

int KKTransform2DComponent::get_zOrder()
{
    return m_zOrder;
}

void KKTransform2DComponent::set_zOrder(int zOrder)
{
    if (m_zOrder != zOrder) {
        
    }
}

KKPoint KKTransform2DComponent::get_anchorPoint()
{
    return m_anchorPoint;
}

void KKTransform2DComponent::set_anchorPoint(KKPoint anchorPoint)
{
    if (m_anchorPoint != anchorPoint)
    {
        m_anchorPoint = anchorPoint;
        _isTransformGLDirty = true;
    }
}

KKSize KKTransform2DComponent::get_size()
{
    return m_size;
}

void KKTransform2DComponent::set_size(cocos2d::KKSize size)
{
    if (m_size != size)
    {
        m_size = size;
        _isTransformGLDirty = true;
        
        mEntity->onInternalEvent("size_changed", nullptr);//观察者
        if (m_parent)
        {
            m_parent->onInternalEvent("child_size_changed", mEntity);
        }
    }
}

float KKTransform2DComponent::get_scale()
{
    KKASSERT(m_scaleX == m_scaleY, "get_scale ScaleX != ScaleY. Don.t know which one to return");
    return m_scaleX;
}

void KKTransform2DComponent::set_scale(float scale)
{
    if (m_scale != scale)
    {
        m_scaleX = m_scaleY = m_scaleZ = scale;
        _isTransformGLDirty = true;
        
        if (m_parent)
        {
            m_parent->onInternalEvent("child_scale_changed", mEntity);
        }
    }
}

float KKTransform2DComponent::get_scaleX()
{
    return m_scaleX;
}


void KKTransform2DComponent::set_scaleX(float scaleX)
{
    if (m_scaleX != scaleX)
    {
        m_scaleX = scaleX;
        _isTransformGLDirty = true;
        
        if (m_parent)
        {
            m_parent->onInternalEvent("child_scalex_changed", mEntity);
        }
    }
}


float KKTransform2DComponent::get_scaleY()
{
    return m_scaleY;
}


void KKTransform2DComponent::set_scaleY(float scaleY)
{
    if (m_scaleY != scaleY)
    {
        m_scaleY = scaleY;
        _isTransformGLDirty = true;
        
        if (m_parent)
        {
            m_parent->onInternalEvent("child_scaley_changed", mEntity);
        }
    }
}

float KKTransform2DComponent::get_scaleZ()
{
    return m_scaleZ;
}


void KKTransform2DComponent::set_scaleZ(float scaleZ)
{
    if (m_scaleZ != scaleZ)
    {
        m_scaleZ = scaleZ;
        _isTransformGLDirty = true;
        
        if (m_parent)
        {
            m_parent->onInternalEvent("child_scalez_changed", mEntity);
        }
    }
}

float KKTransform2DComponent::get_skewX()
{
    return m_skewX;
}


void KKTransform2DComponent::set_skewX(float skewX)
{
    if (m_skewX != skewX)
    {
        m_skewX = skewX;
        _isTransformGLDirty = true;
        
        if (m_parent)
        {
            m_parent->onInternalEvent("child_skewx_changed", mEntity);
        }
    }
}


float KKTransform2DComponent::get_skewY()
{
    return m_skewY;
}


void KKTransform2DComponent::set_skewY(float skewY)
{
    if (m_skewY != skewY)
    {
        m_skewY = skewY;
        _isTransformGLDirty = true;
        
        if (m_parent)
        {
            m_parent->onInternalEvent("child_skewy_changed", mEntity);
        }
    }
}


float KKTransform2DComponent::get_rotation()
{
    return m_rotation;
}

float KKTransform2DComponent::get_rotationX()
{
    return m_rotationX;
}

float KKTransform2DComponent::get_rotationY()
{
    return m_rotationY;
}

void KKTransform2DComponent::set_rotation(float rotation)
{
    if (m_rotation != rotation)
    {
        m_rotation = rotation;
        _isTransformGLDirty = true;
        
        if (m_parent)
        {
            m_parent->onInternalEvent("child_rotation_changed", mEntity);
        }
    }
}

void KKTransform2DComponent::set_rotationX(float rotation)
{
    if (m_rotationX != rotation)
    {
        m_rotationX = rotation;
        _isTransformGLDirty = true;
        
        if (m_parent)
        {
            m_parent->onInternalEvent("child_rotationX_changed", mEntity);
        }
    }
}

void KKTransform2DComponent::set_rotationY(float rotation)
{
    if (m_rotationY != rotation)
    {
        m_rotationY = rotation;
        _isTransformGLDirty = true;
        
        if (m_parent)
        {
            m_parent->onInternalEvent("child_rotationY_changed", mEntity);
        }
    }
}

kmMat4 KKTransform2DComponent::get_additionalTransform()
{
    return m_additionalTransform;
}

void KKTransform2DComponent::set_additionalTransform(kmMat4 additionalTransform)
{
    m_additionalTransform = additionalTransform;
    //kmMat4IsIdentity用于初始化“单位矩阵(Indentify Matrix)”，所谓"单位矩阵"，指的是对脚线上元素都为1的矩阵。从kmMat4IsIdentity的实现，我们也可以看出这一点：
    if (!kmMat4IsIdentity(&m_additionalTransform))
    {
        _useAdditionalTransform = true;
    }
    _isTransformGLDirty = true;
}

kmMat4 * KKTransform2DComponent::get_transform()
{
//    if (_isTransformGLDirty) {
//        kmMat4Identity(m_transform); //kmMat4Identify用于初始化转换矩阵
//        if (m_position != KKPointZero)
//        {
//            //构建平移矩阵
//            kmMat4Translation(m_transform, m_position.x, m_position.y, 0);
//        }
//        if (m_rotation != 0)
//        {
//            kmMat4 rotaionMatrix; //旋转矩阵
//            kmMat4RotationZ(&rotaionMatrix, -DEGREES_TO_RADIANS(m_rotation));
//            kmMat4Multiply(m_transform, m_transform, &rotaionMatrix);
//        }
//        bool needsSkewMatrix = ( m_skewX || m_skewY );
//        if (needsSkewMatrix)
//        {
//            kmMat4 skewMatrix = { 1, (float)tanf(DEGREES_TO_RADIANS(m_skewY)), 0, 0,
//                (float)tanf(DEGREES_TO_RADIANS(m_skewX)), 1, 0, 0,
//                0,  0,  1, 0,
//                0,  0,  0, 1};  //倾斜矩阵
//            
//            kmMat4Multiply(m_transform, m_transform, &skewMatrix);
//        }
//        bool needsScaleMatrix = !(m_scaleX == 1 && m_scaleY == 1);
//        if (needsScaleMatrix)
//        {
//            kmMat4 scaleMatrix;//放大缩小矩阵
//            kmMat4Scaling(&scaleMatrix, m_scaleX, m_scaleY, 1);
//            kmMat4Multiply(m_transform, m_transform, &scaleMatrix);
//        }
//        if (m_anchorPoint != KKPointZero || m_vertexZ)
//        {
//            kmMat4 mat;
//            kmMat4Translation(&mat, -m_anchorPoint.x, -m_anchorPoint.y, m_vertexZ);
//            kmMat4Multiply(m_transform, m_transform, &mat);
//        }
//        
//        _isTransformGLDirty = false;
//    }
//    
//    return m_transform;
    
    if (_isTransformGLDirty)
    {
        kmMat4Identity(m_transform);
        kmMat4 tmpMat;
        float x = m_position.x;
        float y = m_position.y;
        float z = m_vertexZ;
        
        float cx = 1, sx = 0, cy = 1, sy = 0;
        if (m_rotation)
        {
            //旋转矩阵
            float radiansX = -DEGREES_TO_RADIANS(m_rotation);
            float radiansY = -DEGREES_TO_RADIANS(m_rotation);
            cx = cosf(radiansX);
            sx = sinf(radiansX);
            cy = cosf(radiansY);
            sy = sinf(radiansY);
        }
        bool needsSkewMatrix = ( m_skewX || m_skewY );//倾斜矩阵
        
        KKPoint anchorPoint = m_anchorPoint;
        anchorPoint.x *= m_scaleX;
        anchorPoint.y *= m_scaleY;
        
        if (! needsSkewMatrix && anchorPoint != KKPointZero)
        {
            x += cy * -anchorPoint.x + -sx * -anchorPoint.y;
            y += sy * -anchorPoint.x +  cx * -anchorPoint.y;
        }
        m_transform->mat[0] = cy * m_scaleX;
        m_transform->mat[1] = sy * m_scaleX;
        m_transform->mat[4] = -sx * m_scaleY;
        m_transform->mat[5] = cx * m_scaleY;
        m_transform->mat[10] = m_scaleZ;
        m_transform->mat[12] = x;
        m_transform->mat[13] = y;
        m_transform->mat[14] = z;
        
        kmMat4Translation(&tmpMat, anchorPoint.x, anchorPoint.y, 0);
        kmMat4Multiply(m_transform, m_transform, &tmpMat);
        
        if (m_rotationY)
        {
            kmMat4RotationY(&tmpMat, DEGREES_TO_RADIANS(m_rotationY));
            kmMat4Multiply(m_transform, m_transform, &tmpMat);
        }
        if (m_rotationX)
        {
            kmMat4RotationX(&tmpMat, DEGREES_TO_RADIANS(m_rotationX));
            kmMat4Multiply(m_transform, m_transform, &tmpMat);
        }
        
        kmMat4Translation(&tmpMat, -anchorPoint.x, -anchorPoint.y, 0);
        kmMat4Multiply(m_transform, m_transform, &tmpMat);
        
        if (needsSkewMatrix)
        {
            kmMat4Identity(&tmpMat);
            tmpMat.mat[1] = (float)tanf(DEGREES_TO_RADIANS(m_skewY));
            tmpMat.mat[4] = (float)tanf(DEGREES_TO_RADIANS(m_skewX));
            kmMat4Multiply(m_transform, m_transform, &tmpMat);
            if (m_anchorPoint != KKPointZero)
            {
                m_transform->mat[12] += m_transform->mat[0] * -m_anchorPoint.x + m_transform->mat[4] * - m_anchorPoint.y;
                m_transform->mat[13] += m_transform->mat[1] * -m_anchorPoint.x + m_transform->mat[5] * - m_anchorPoint.y;
            }
        }
        
        if (_useAdditionalTransform)
        {
            //乘以附加矩阵
            kmMat4Multiply(m_transform, m_transform, &m_additionalTransform);
        }
        
        _isTransformGLDirty = false;
    }
    
    return m_transform;
}

KKEntity *KKTransform2DComponent::get_parent()
{
    return m_parent;
}

void KKTransform2DComponent::set_parent(cocos2d::KKEntity *entity)
{
    m_parent = entity;
}

std::list<KKEntity *> KKTransform2DComponent::get_children()
{
    return m_children;
}


NS_KK_END