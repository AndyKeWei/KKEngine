//
//  KKView.cpp
//  KKEngine
//
//  Created by kewei on 5/16/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#include "KKView.hpp"
#include "KKTouchDispatcher.hpp"
#if defined(TARGET_MACOS)
#include "KKMouseDispatcher.hpp"
#endif
#include "KKDrawingPrimitives.hpp"

NS_KK_BEGIN

NS_KK_BEGIN_GUI

KKView::KKView(KKView *parent)
{
    if (parent)
    {
        parent->addSubview(this);
    }
}

KKView::KKView(KKView *parent, KKRect rect)
{
    
}

KKView::KKView(KKView *parent, const KKView& copy)
{
    
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

void KKView::addSubview(KKView *view) {
    KKASSERT(view->mParent == nullptr, "view already has parent");
    KKASSERT(!(view == this), "cannot add self to subview");
    
    mSubviews.push_back(view);
    view->mParent = this;
    view->setNeedsUpdateTransform();
    view->onInternalEvent("subview_added");
#if defined(TARGET_MACOS)
    KKMouseDispatcher::shareMouseDispatcher()->needsHitTest();
#endif
    KKTouchDispatcher::sharedTouchDispatcher()->needsHitTest();
}

void KKView::visit()
{
    if (!mVisible)
    {
        return;
    }
    
    KKGLMatrixStack *matrixStack = KKGLMatrixStack::currentMatrixStack();
    matrixStack->pushMatrix();
    matrixStack->multMatrix(getTransform());//矩阵相乘
    kkglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  //GL_SRC_ALPHA：表示使用源颜色的alpha值来作为源因子 GL_ONE_MINUS_SRC_ALPHA：表示用1.0减去源颜色的alpha值来作为目标因子
    if (mBackgroundColor.a != 0) {
        drawColor4B(mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b, mBackgroundColor.a);
        drawSolidRect(KKPoint(-mEdgeInsets.left, -mEdgeInsets.bottom), KKPoint(mFrame.size.width-mEdgeInsets.left, mFrame.size.height-mEdgeInsets.bottom));
    }
    bool scissorEnabled = kkglIsEnabled(GL_SCISSOR_TEST); //是否裁剪
    GLint scissorBox[4];
    if (mClipToBounds)
    {
        if (scissorEnabled)
        {
            kkglGetScissorBox(scissorBox);
        }
        else
        {
            kkglEnable(GL_SCISSOR_TEST);
        }
        float scale = KKDirector::getSingleton()->getWinSizeScale();
        kmMat4 trans;
        KKGLMatrixStack::currentMatrixStack()->getMatrix(KM_GL_MODELVIEW, &trans);
        KKSize winSizeInPixel = KKDirector::getSingleton()->getWinSizeInPixels();
        KKPoint loc = KKPoint(trans.mat[12], trans.mat[13]);
        loc = KKDirector::getSingleton()->convertToScreenPoint(loc) * CONTENT_SCALE_FACTOR;
    }
}

void KKView::onDraw()
{
    
}


KKView *KKView::hitTest(KKPoint p)
{
    return nullptr;
}


KKView::~KKView()
{
    
}





NS_KK_END_GUI

NS_KK_END
