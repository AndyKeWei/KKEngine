//
//  KKColorAdvanceComponent.hpp
//  KKEngine
//
//  Created by kewei on 5/4/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#ifndef KKColorAdvanceComponent_hpp
#define KKColorAdvanceComponent_hpp

#include "KKComponent.h"
#include "KKUtil.h"

NS_KK_BEGIN

class KKColorAdvanceComponent : public KKComponent {
    
    PROPERTIES_DECLARE
    PROPERTY_DECLARE_IMPLEMENT(KKColorAdvanceComponent, color4F, colorPercent)
    PROPERTY_DECLARE_IMPLEMENT(KKColorAdvanceComponent, color4F, colorAmount)
    
public:
    KKColorAdvanceComponent()
    {
        m_colorPercent.r = m_colorPercent.g = m_colorPercent.b = m_colorPercent.a = 1;
        m_colorAmount.r = m_colorAmount.g = m_colorAmount.b = m_colorAmount.a = 0;
    }
    
    FAMILYID
};


NS_KK_END

#endif /* KKColorAdvanceComponent_hpp */
