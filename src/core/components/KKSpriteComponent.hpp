//
//  KKSpriteComponent.hpp
//  KKEngine
//
//  Created by kewei on 5/3/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#ifndef KKSpriteComponent_hpp
#define KKSpriteComponent_hpp

#include <stdio.h>
#include "KKComponent.h"
#include "KKGeometry.h"
#include "KKGL.h"

NS_KK_BEGIN

class KKTexture;
class KKTransform2DComponent;
class KKColorComponent;
class KKColorAdvanceComponent;
class KKBlendFuncComponent;
class KKShaderComponent;

class KKSpriteComponent : public KKComponent {
    
    PROPERTIES_DECLARE
    PROPERTY_DECLARE_INIT(KKSpriteComponent, KKTexture *, texture, nullptr)
    PROPERTY_DECLARE_IMPLEMENT_INIT(KKSpriteComponent, bool , textureRotated, false)
    PROPERTY_DECLARE_IMPLEMENT_INIT(KKSpriteComponent, bool, gray, false)
    PROPERTY_DECLARE(KKSpriteComponent, KKRect, textureRect)
    PROPERTY_READONLY_DECLARE_IMPLEMENT_INIT(KKSpriteComponent, bool, useBatch, false)
    PROPERTY_DECLARE(KKSpriteComponent, bool, flipX)
    PROPERTY_DECLARE(KKSpriteComponent, bool, flipY)
    PROPERTY_DECLARE_INIT(KKSpriteComponent, KKSize, untrimmedSize, KKSizeZero) //未切边的size
    PROPERTY_DECLARE_INIT(KKSpriteComponent, KKPoint, offsetPosFromBL, KKPointZero)
    PROPERTY_DECLARE_IMPLEMENT_INIT(KKSpriteComponent, float , brightness, 1.f)
    PROPERTY_DECLARE_IMPLEMENT_INIT(KKSpriteComponent, float , alphaTest, -1.f)
    
public:
    KKSpriteComponent(){}
    
    KKSpriteComponent(KKTexture *texture, KKRect textureRect):m_texture(texture), m_textureRect(textureRect){}
    
    virtual ~KKSpriteComponent();
    
    virtual void onActive();
    
    virtual void onDeactive();
    
    virtual void onInternalEvent(const char *event, void *info);
    
    //更新纹理坐标和顶点数据
    void updateCoordsAndVerts();
    
    void drawSprite();
    
    
    FAMILYID
    
private:
    bool mDirty {false};
    GLfloat mVertices[8];
    GLfloat mCoordinates[8];
    
    KKTransform2DComponent *mTransComp {nullptr};
    KKColorComponent *mColorComp {nullptr};
    KKColorAdvanceComponent *mColorAdvanceComp {nullptr};
    KKBlendFuncComponent *mBlendFuncComp {nullptr};
    KKShaderComponent *mShaderComp {nullptr};
};



NS_KK_END

#endif /* KKSpriteComponent_hpp */
