//
//  KKColorComponent.hpp
//  KKEngine
//
//  Created by kewei on 5/4/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#ifndef KKColorComponent_hpp
#define KKColorComponent_hpp

#include "KKComponent.h"
#include "KKUtil.h"

NS_KK_BEGIN

class KKColorComponent : public KKComponent {
    
    PROPERTIES_DECLARE
    PROPERTY_DECLARE(KKColorComponent, color4B, color)
    
public:
    KKColorComponent() { m_color.r = m_color.g = m_color.b = m_color.a = 255; }
    
    FAMILYID
};


NS_KK_END

#endif /* KKColorComponent_hpp */
