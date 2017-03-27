//
//  KKView.cpp
//  KKEngine
//
//  Created by kewei on 5/16/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#include "KKView.hpp"

NS_KK_BEGIN

NS_KK_BEGIN_GUI

KKView::KKView(KKView *view)
{
    if (view)
    {
        mSubviews.push_back(view);
    }
}

KKView *KKView::findViewWithTag(int32_t tag)
{
    auto target = std::find_if(mSubviews.begin(), mSubviews.end(), [ = ](KKView *view){
        
        return  view && view->getTag() == tag;
        
    });
    
    
    return target != mSubviews.end() ? (*target) : nullptr;
}

void KKView::centerInParent()
{
    float x = (mParent->getSize().width - mFrame.size.width) / 2.0f;
    float y = (mParent->getSize().height - mFrame.size.height) / 2.0f;
    
    setPosition(x, y);
}

void KKView::centerHorizontallyInParent()
{
    float x = (mParent->getSize().width - mFrame.size.width) / 2.0f;
    setPosition(x, mFrame.origin.y);
}

void KKView::centerVerticallyInParent()
{
    float y = (mParent->getSize().height - mFrame.size.height) / 2.0f;
    setPosition(mFrame.origin.x, y);
}

void KKView::leftInParent()
{
    setPosition(0, mFrame.origin.y);
}

void KKView::rightInParent()
{
    setPosition(mParent->getSize().width-mFrame.size.width, mFrame.origin.y);
}

void KKView::topInParent()
{
    setPosition(mFrame.origin.x, 0);
}

void KKView::bottomInParent()
{
    setPosition(mFrame.origin.x, mParent->getSize().height-mFrame.size.height);
}


void KKView::visit()
{
    if (!mVisible)
    {
        return;
    }
    
    KKGLMatrixStack *matrixStack = KKGLMatrixStack::currentMatrixStack();
    matrixStack->pushMatrix();
    
}










NS_KK_END_GUI

NS_KK_END
