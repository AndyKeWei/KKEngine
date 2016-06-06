//
//  KKSpriteComponent.cpp
//  KKEngine
//
//  Created by kewei on 5/3/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#include "KKSpriteComponent.hpp"
#include "KKTexture.hpp"
#include "KKEntityManager.hpp"
#include "KKTransform2DComponent.hpp"
#include "KKResourceManager.hpp"
#include "KKShaderProgram.hpp"
#include "KKStringUtil.hpp"
#include "KKColorComponent.hpp"
#include "KKColorAdvanceComponent.hpp"
#include "KKShaderComponent.hpp"

NS_KK_BEGIN

INIT_FAMILYID(KKSpriteComponent)

PROPERTIES_INITIAL(KKSpriteComponent)
REGISTER_PROPERTY(KKSpriteComponent, KKTexture *, texture)
REGISTER_PROPERTY(KKSpriteComponent, bool, textureRotated)
REGISTER_PROPERTY(KKSpriteComponent, bool, gray)
REGISTER_PROPERTY(KKSpriteComponent, KKRect, textureRect)
REGISTER_PROPERTY_READONLY(KKSpriteComponent, bool, useBatch)
REGISTER_PROPERTY(KKSpriteComponent, bool, flipX)
REGISTER_PROPERTY(KKSpriteComponent, bool, flipY)
REGISTER_PROPERTY(KKSpriteComponent, KKSize, untrimmedSize)
REGISTER_PROPERTY(KKSpriteComponent, KKPoint, offsetPosFromBL)
REGISTER_PROPERTY(KKSpriteComponent, float, brightness)
REGISTER_PROPERTY(KKSpriteComponent, float, alphaTest)


KKSpriteComponent::~KKSpriteComponent()
{
    KK_SAFE_RELEASE(m_texture);
}

void KKSpriteComponent::onActive()
{
    mEntity->onDrawHandler += newDelegate(this, &KKSpriteComponent::drawSprite);
    mTransComp = mEntity->getComponent<KKTransform2DComponent>();
    mColorComp = mEntity->getComponent<KKColorComponent>();
    mColorAdvanceComp = mEntity->getComponent<KKColorAdvanceComponent>();
    mShaderComp = mEntity->getComponent<KKShaderComponent>();
}

void KKSpriteComponent::onDeactive()
{
    mEntity->onDrawHandler -= newDelegate(this, &KKSpriteComponent::drawSprite);
}

void KKSpriteComponent::drawSprite()
{
    //draw sprite
    if (!mEntity)
    {
        return;
    }
    if (!m_texture)
    {
        return;
    }
    if (m_texture->getName() == 0)
    {
        return;
    }
    if (m_useBatch)
    {
        return;
    }
    if (mDirty)
    {
        mDirty = false;
        //更新纹理坐标和顶点数据
        updateCoordsAndVerts();
    }
    
    //shader
    KKProgram *program = nullptr;
    
    if (mShaderComp)
    {
        program = mEntity->getComponent<KKShaderComponent>()->get_shaderProgram();
    }
    
    if (!program)
    {
        if (m_alphaTest >= 0)
        {
            //有alpha通道
            program = KKProgramManager::currentManager()->getProgram(kShaderPositionTextureColorAlphaTest);
        }
        else if (mColorAdvanceComp)
        {
            if (m_gray)
            {
                //gray
                program = KKProgramManager::currentManager()->getProgram(m_texture->getAlphaTexture()?kShaderPositionTextureColorAdvanceGrayWithExtraAlpha:kShaderPositionTextureColorAdvanceGray);
            }
            else
            {
                program = KKProgramManager::currentManager()->getProgram(m_texture->getAlphaTexture()?kShaderPositionTextureColorAdvanceWithExtraAlpha:kShaderPositionTextureColorAdvance);
            }
        }
        else
        {
            if (m_gray)
            {
                //gray
                program = KKProgramManager::currentManager()->getProgram(m_texture->getAlphaTexture()?kShaderPositionTextureColorGrayWithExtraAlpha:kShaderPositionTextureColorGray);
            }
            else
            {
                program = KKProgramManager::currentManager()->getProgram(m_texture->getAlphaTexture()?kShaderPositionTextureColorWithExtraAlpha:kShaderPositionTextureColor);
            }
        }
    }
    
    //use
    program->use();
    
    // 视图转换
    program->setUniformForModelViewProjectionMatrix();
    
    if (mShaderComp)
    {
        mShaderComp->shaderDrawHandler(program);
    }
    else
    {
    
        // 需要 精讲
        auto& uniforms = program->getUniforms();
        
        for (std::vector<KKProgramUniform>::iterator it = uniforms.begin(); it != uniforms.end(); ++it)
        {
            KKProgramUniform& uniform = *it;
            if (!strcmp(uniform.name, kUniformBrightness))
            {
                //设置shader属性值, brightness
                uniform.setUniform1f(m_brightness);
            }
            else if (!strcmp(uniform.name, kUniformColorPercent))
            {
                color4F color = mColorAdvanceComp->get_colorPercent();
                uniform.setUniform4fv((GLfloat*)&color, 1);
            }
            else if (!strcmp(uniform.name, kUniformColorAmount))
            {
                color4F color = mColorAdvanceComp->get_colorAmount();
                uniform.setUniform4fv((GLfloat*)&color, 1);
            }
            else if (!strcmp(uniform.name, kUniformAlphaThreshold))
            {
                
                uniform.setUniform1f(m_alphaTest);
            }
        }
    }
    
    if (m_texture->getAlphaTexture())
    {
        // bug on nexus 10 and some other samsong devices
        GLint sampler1 = glGetUniformLocation(program->getProgram(), kUniformSamplerAlpha_s);//u_texture_alpha
        glUniform1i(sampler1, 1);
    }
    
    if (mBlendFuncComp)
    {
        //混色
    }
    else
    {
        if (m_texture->hasPremultipliedAlpha())
        {
            //所以 Premultiplied Alpha 最重要的意义是使得带透明度图片纹理可以正常的进行线性插值。这样旋转、缩放或者非整数的纹理坐标才能正常显示，否则就会像上面的例子一样，在透明像素边缘附近产生奇怪的颜色
            //混合
            //第一个参数 源因子， 第二个参数： 目标因子
            kkglBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            kkglBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        }
    }
    
    if (m_alphaTest >= 0 && m_texture->getAlphaTexture())
    {
        //如果有alpha通道
        //绑定纹理目标，后续的操作都是基于这个纹理
        kkglBindTexture(GL_TEXTURE_2D, m_texture->getAlphaTexture());
    }
    else
    {
        // 绑定纹理 后续使用的都是相关的纹理
        kkglBindTexture(GL_TEXTURE_2D, m_texture->getName());
        if (m_texture->getAlphaTexture())
        {
            //如果有alpha通道
            //GPU有N个纹理单元（具体数目依赖你的显卡能力），每个纹理单元（GL_TEXTURE0、GL_TEXTURE1等）都有GL_TEXTURE_1D、GL_TEXTURE_2D等
            //glActiveTexture 并不是激活纹理单元，而是选择当前活跃的纹理单元
            kkglActiveTexture(GL_TEXTURE1);
            kkglBindTexture(GL_TEXTURE_2D, m_texture->getAlphaTexture());
            kkglActiveTexture(GL_TEXTURE0);
        }
    }
    
    //开启顶点属性数组
    glEnableVertexAttribArray( kVertexAttrib_Position ); //顶点坐标数组
    glEnableVertexAttribArray( kVertexAttrib_TexCoords ); //纹理坐标数组
    
    if (!mColorAdvanceComp)
    {
        glEnableVertexAttribArray( kVertexAttrib_Color ); //顶点颜色数组
    }
    
    //设置数组属性
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, mVertices);
    glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, mCoordinates);
    
    GLubyte *colors = nullptr;
    if (!mColorAdvanceComp || m_gray)
    {
        colors = new GLubyte[32];
        if (mEntity->hasComponent<KKColorComponent>())
        {
            color4B color = mEntity->getComponent<KKColorComponent>()->get_color();
            for (int i = 0; i < 8; i++)
            {
                memcpy(colors+sizeof(color4B)*i, &color, sizeof(color4B));
            }
        }
        else
        {
            memset(colors, 255, 32*sizeof(GLubyte));
        }
        // (GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr)
        glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, colors);
    }
    
    //开始绘制
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    KK_SAFE_DELETE(colors);
    
    if (mBlendFuncComp)
    {
        // 混合还原
        kkglBlendEquation(GL_FUNC_ADD);
    }
    
    CHECK_GL_ERROR();
}

//更新纹理坐标和顶点数据
void KKSpriteComponent::updateCoordsAndVerts()
{
    if (m_textureRect == KKRectZero)
    {
        m_textureRect.size = m_texture->getImageSize() * m_texture->getScale();
    }
    
    static KKSize size;
    size = KKSizeZero;
    static KKSize imageSize;
    imageSize = m_texture->getImageSize() * m_texture->getScale();
    
    // 2d矩阵
    if (mTransComp)
    {
        size = mTransComp->get_size();
    }
    
    //
    if (size == KKSizeZero)
    {
        if (m_untrimmedSize != KKSizeZero)
            size = m_untrimmedSize * KKResourceManager::getSingleton()->getResourceScale();
        else
            size = m_textureRect.size * KKResourceManager::getSingleton()->getResourceScale();
    }
    
    static KKSize untrimmedSize;
    untrimmedSize = m_untrimmedSize;
    if (untrimmedSize == KKSizeZero)
    {
        untrimmedSize = m_textureRect.size;
    }
    
    KKSize drawSize = m_textureRect.size;
    if (m_textureRotated)
    {
        float tmp = drawSize.width;
        drawSize.width = drawSize.height;
        drawSize.height = tmp;
    }
    
    float left = m_textureRect.origin.x/imageSize.width;
    float right = left + drawSize.width/imageSize.width;
    float top = m_textureRect.origin.y/imageSize.height;
    float bottom = top + drawSize.height/imageSize.height;
    if ((m_flipX && !m_textureRotated) || (m_flipY && m_textureRotated))
    {
        float tmp = left;
        left = right;
        right = tmp;
    }
    if ((m_flipY && !m_textureRotated) || (m_flipX && m_textureRotated)) {
        float tmp = top;
        top = bottom;
        bottom = tmp;
    }
    
    if (m_textureRotated)
    {
        mCoordinates[0] = left;
        mCoordinates[1] = top;
        mCoordinates[2] = left;
        mCoordinates[3] = bottom;
        mCoordinates[4] = right;
        mCoordinates[5] = top;
        mCoordinates[6] = right;
        mCoordinates[7] = bottom;
    }
    else
    {
        mCoordinates[0] = left;
        mCoordinates[1] = bottom;
        mCoordinates[2] = right;
        mCoordinates[3] = bottom;
        mCoordinates[4] = left;
        mCoordinates[5] = top;
        mCoordinates[6] = right;
        mCoordinates[7] = top;
    }
    
    drawSize = m_textureRect.size;
    KKPoint offset;
    //    if (m_untrimmedSize != HLSizeZero) {
    offset = m_offsetPosFromBL;
    if (m_flipX)
    {
        offset.x = untrimmedSize.width - offset.x - drawSize.width;
    }
    if (m_flipY)
    {
        offset.y = untrimmedSize.height - offset.y - drawSize.height;
    }
    offset.x = offset.x*(size.width/untrimmedSize.width);
    offset.y = offset.y*(size.height/untrimmedSize.height);
    drawSize.width *= (size.width/untrimmedSize.width);
    drawSize.height *= (size.height/untrimmedSize.height);
    //    }
    
    // sprite的坐标 2个三角形
    mVertices[0] = offset.x;
    mVertices[1] = offset.y;
    mVertices[2] = offset.x+drawSize.width;
    mVertices[3] = offset.y;
    mVertices[4] = offset.x;
    mVertices[5] = offset.y+drawSize.height;
    mVertices[6] = offset.x+drawSize.width;
    mVertices[7] = offset.y+drawSize.height;
}

KKRect KKSpriteComponent::get_textureRect()
{
    return m_textureRect;
}

void KKSpriteComponent::set_textureRect(KKRect textureRect)
{
    m_textureRect = textureRect;
    mDirty = true;
    //TODO useBatch
    if (m_useBatch && mTransComp && mTransComp->get_parent())
    {
        mTransComp->get_parent()->onInternalEvent("child_textureRect_changed", mEntity);
    }
}

KKTexture *KKSpriteComponent::get_texture()
{
    return m_texture;
}

void KKSpriteComponent::set_texture(KKTexture *texture)
{
    if (texture == m_texture)
    {
        return;
    }
    if (texture)
    {
        texture->retain();
    }
    KK_SAFE_RELEASE(m_texture);
    
    m_texture = texture;
    m_textureRect.origin = KKPointZero;
    if (m_texture)
    {
        m_textureRect.size = m_texture->getImageSize() * m_texture->getScale();
    }
    //TODO 需要重新更新纹理
    mDirty = true;
    
    //TODO useBatch
    if (m_useBatch && mTransComp && mTransComp->get_parent())
    {
        mTransComp->get_parent()->onInternalEvent("child_texture_changed", mEntity);
    }
}

bool KKSpriteComponent::get_flipX()
{
    return m_flipX;
}

void KKSpriteComponent::set_flipX(bool flipX)
{
    m_flipX = flipX;
    mDirty = true;
    if (m_useBatch && mTransComp && mTransComp->get_parent())
    {
        mTransComp->get_parent()->onInternalEvent("child_flipX_changed", mEntity);
    }
}

bool KKSpriteComponent::get_flipY()
{
    return m_flipY;
}

void KKSpriteComponent::set_flipY(bool flipY)
{
    m_flipY = flipY;
    mDirty = true;
    if (m_useBatch && mTransComp && mTransComp->get_parent())
    {
        mTransComp->get_parent()->onInternalEvent("child_flipY_changed", mEntity);
    }
}

KKSize KKSpriteComponent::get_untrimmedSize()
{
    return m_untrimmedSize;
}

void KKSpriteComponent::set_untrimmedSize(KKSize untrimmedSize)
{
    m_untrimmedSize = untrimmedSize;
    mDirty = true;
    if (m_useBatch && mTransComp && mTransComp->get_parent())
    {
        mTransComp->get_parent()->onInternalEvent("child_untrimmedSize_changed", mEntity);
    }
}

KKPoint KKSpriteComponent::get_offsetPosFromBL()
{
    return m_offsetPosFromBL;
}

void KKSpriteComponent::set_offsetPosFromBL(KKPoint offsetPosFromBL)
{
    m_offsetPosFromBL = offsetPosFromBL;
    mDirty = true;
    if (m_useBatch && mTransComp && mTransComp->get_parent())
    {
        mTransComp->get_parent()->onInternalEvent("child_offsetPosFromBL_changed", mEntity);
    }
}


void KKSpriteComponent::onInternalEvent(const char *event, void *info)
{
    KKString str(event);
    
    if (!str.compare("size_changed"))
    {
        mDirty = true;
    }
    else if (!str.compare(COMPONENT_WILL_ADD))
    {
        KKComponent *component = (KKComponent *)info;
        if (component->getFamilyId() == KKTransform2DComponent::familyId)
        {
            if (!mTransComp)
            {
                mTransComp = (KKTransform2DComponent *)component;
            }
        }
        else if (component->getFamilyId() == KKColorComponent::familyId)
        {
            if (!mColorComp)
            {
                mColorComp = (KKColorComponent *)component;
            }
        }
        else if (component->getFamilyId() == KKColorAdvanceComponent::familyId)
        {
            if (!mColorAdvanceComp)
            {
                mColorAdvanceComp = (KKColorAdvanceComponent *)component;
            }
        }
        else if (component->getFamilyId() == KKShaderComponent::familyId)
        {
            if (!mShaderComp)
            {
                mShaderComp = (KKShaderComponent *)component;
            }
        }
    }
    else if (!str.compare(COMPONENT_WILL_REMOVE))
    {
        if (mTransComp == info)
        {
            mTransComp = nullptr;
        }
        else if (mColorComp == info)
        {
            mColorComp = nullptr;
        }
        else if (mColorAdvanceComp == info)
        {
            mColorAdvanceComp = nullptr;
        }
        else if (mShaderComp == info)
        {
            mShaderComp = nullptr;
        }
    }
}


NS_KK_END