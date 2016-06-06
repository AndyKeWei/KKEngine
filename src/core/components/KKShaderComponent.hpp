//
//  KKShaderComponent.hpp
//  KKEngine
//
//  Created by kewei on 5/13/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#ifndef KKShaderComponent_hpp
#define KKShaderComponent_hpp

#include "KKComponent.h"
#include "KKUtil.h"
#include "KKMacros.h"

NS_KK_BEGIN

class KKProgram;

typedef CDelegate1<KKProgram *> ShaderDrawHandler;

class KKShaderComponent : public KKComponent {
    
    PROPERTIES_DECLARE
    PROPERTY_DECLARE_INIT(KKShaderComponent, KKProgram *, shaderProgram, nullptr)
    
public:
    
    ~KKShaderComponent();
    
    ShaderDrawHandler shaderDrawHandler;
    
    FAMILYID
};


NS_KK_END

#endif /* KKShaderComponent_hpp */
