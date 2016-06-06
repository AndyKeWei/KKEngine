//
//  KKColorAdvanceComponent.cpp
//  KKEngine
//
//  Created by kewei on 5/4/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#include "KKColorAdvanceComponent.hpp"

NS_KK_BEGIN

INIT_FAMILYID(KKColorAdvanceComponent)
PROPERTIES_INITIAL(KKColorAdvanceComponent)

REGISTER_PROPERTY(KKColorAdvanceComponent, color4F, colorPercent)

REGISTER_PROPERTY(KKColorAdvanceComponent, color4F, colorAmount)



NS_KK_END