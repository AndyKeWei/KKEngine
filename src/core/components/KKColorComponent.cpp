//
//  KKColorComponent.cpp
//  KKEngine
//
//  Created by kewei on 5/4/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#include "KKColorComponent.hpp"

NS_KK_BEGIN

INIT_FAMILYID(KKColorComponent)
PROPERTIES_INITIAL(KKColorComponent)

REGISTER_PROPERTY(KKColorComponent, color4B, color)

void KKColorComponent::set_color(color4B color)
{
    m_color = color;
}


color4B KKColorComponent::get_color()
{
    return m_color;
}



NS_KK_END