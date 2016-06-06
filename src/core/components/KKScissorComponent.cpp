//
//  KKScissorComponent.cpp
//  KKEngine
//
//  Created by kewei on 1/17/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#include "KKScissorComponent.hpp"
#include "KKDirector.h"

NS_KK_BEGIN

INIT_FAMILYID(KKScissorComponent)

PROPERTIES_INITIAL(KKScissorComponent)
REGISTER_PROPERTY(KKScissorComponent, KKRect, scissorRect)
REGISTER_PROPERTY_READONLY(KKScissorComponent, bool, scissorEnabled)
REGISTER_PROPERTY_READONLY(KKScissorComponent, bool, outside)

void KKScissorComponent::visit()
{
    //模板裁切渲染
    m_scissorEnabled = kkglIsEnabled(GL_SCISSOR_TEST); //是否启用裁剪测试
    //剪裁测试用于限制绘制区域。我们可以指定一个矩形的剪裁窗口，当启用剪裁测试后，只有在这个窗口之内的像素才能被绘制，其它像素则会被丢弃。换句话说，无论怎么绘制，剪裁窗口以外的像素将不会被修改
    if (m_scissorRect  != KKRectZero)
    {
        if (m_scissorEnabled) {
            KK_SAFE_DELETE(scissorBox);
            
            scissorBox = new GLint[4];
            
            kkglGetScissorBox(scissorBox);
        }
        else
        {
            kkglEnable(GL_SCISSOR_TEST);//开启裁剪测试
        }
        
        float scale = KKDirector::getSingleton()->getWinSizeScale();
        KKSize winSizeInPixel = KKDirector::getSingleton()->getWinSizeInPixels();
        KKPoint loc = KKDirector::getSingleton()->convertToScreenPoint(m_scissorRect.origin);
        loc *= CONTENT_SCALE_FACTOR;
        loc.y = winSizeInPixel.height - loc.y;
        
        if (m_scissorEnabled)
        {
            float left = MAX(loc.x, scissorBox[0]);
            float bottom = MAX(loc.y, scissorBox[1]);
            float right = MAX(loc.x+m_scissorRect.size.width/scale, scissorBox[0]+scissorBox[2]);
            float top = MAX(loc.y+m_scissorRect.size.height/scale, scissorBox[1]+scissorBox[3]);
            if (right > left && top > bottom)
            {
                //设置裁剪区域
                m_outside = false;
                kkglScissor(roundf(left), roundf(bottom), roundf(right-left), roundf(top-bottom));
            }
            else
            {
                m_outside = true;
            }
        }
        else
        {
            //设置指定区域为裁剪区域
            kkglScissor(roundf(loc.x), roundf(loc.y), m_scissorRect.size.width/scale, m_scissorRect.size.height/scale);
        }
    }
}


NS_KK_END