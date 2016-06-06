//
//  KKScissorComponent.hpp
//  KKEngine
//
//  Created by kewei on 1/17/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#ifndef KKScissorComponent_hpp
#define KKScissorComponent_hpp

#include "KKComponent.h"
#include "KKGeometry.h"
#include "KKGL.h"

NS_KK_BEGIN

class KKScissorComponent : public KKComponent {
    
    PROPERTIES_DECLARE
    PROPERTY_DECLARE_IMPLEMENT_INIT(KKScissorComponent, KKRect, scissorRect, KKRectZero)
    PROPERTY_READONLY_DECLARE_IMPLEMENT_INIT(KKScissorComponent, bool , scissorEnabled, false)
    PROPERTY_READONLY_DECLARE_IMPLEMENT_INIT(KKScissorComponent, bool, outside, false)
    
    GLint *scissorBox {nullptr};
public:
    KKScissorComponent(){}
    
    virtual ~KKScissorComponent(){
        KK_SAFE_DELETE(scissorBox);
    }
    
    virtual void visit();
    
    inline const GLint *getScissorBox() const
    {
        return scissorBox;
    }
    
    FAMILYID
};



NS_KK_END

#endif /* KKScissorComponent_hpp */
