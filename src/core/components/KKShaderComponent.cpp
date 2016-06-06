//
//  KKShaderComponent.cpp
//  KKEngine
//
//  Created by kewei on 5/13/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#include "KKShaderComponent.hpp"
#include "KKShaderProgram.hpp"

NS_KK_BEGIN

INIT_FAMILYID(KKShaderComponent)
PROPERTIES_INITIAL(KKShaderComponent)

REGISTER_PROPERTY(KKShaderComponent, KKProgram *, shaderProgram)

KKShaderComponent::~KKShaderComponent()
{
    KK_SAFE_RELEASE(m_shaderProgram);
}

KKProgram * KKShaderComponent::get_shaderProgram()
{
    return m_shaderProgram;
}

void KKShaderComponent::set_shaderProgram(KKProgram *shaderProgram)
{
    shaderProgram->retain();
    KK_SAFE_RELEASE(m_shaderProgram);
    m_shaderProgram = shaderProgram;
}


NS_KK_END