//
//  KKView.hpp
//  KKEngine
//
//  Created by kewei on 5/16/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#ifndef KKView_hpp
#define KKView_hpp

#include "KKMacros.h"
#include "KKGeometry.h"
#include "KKUtil.h"
#include "KKGL.h"
#include "KKGLMatrixStack.h"
#include <list>

NS_KK_BEGIN

NS_KK_BEGIN_GUI

typedef struct
{
    float top;
    float left;
    float bottom;
    float right;
} KKEdgeInsets;

class KKView {
    
public:
    KKView(KKView *view);
    KKView(KKView *view, KKRect rect);
    KKView(KKView *view, const KKView& copy);
    
    virtual ~KKView();
    
    KKView *getParentView() { return  mParent; }
    
    void setSize(KKSize size)
    {
        setSize(size.width, size.height);
    }
    
    void setSize(float width, float height)
    {
        mFrame.size.width = width;
        mFrame.size.height = height;
        mBounds.size.width = width - mEdgeInsets.left - mEdgeInsets.right;
        mBounds.size.height = height - mEdgeInsets.top - mEdgeInsets.bottom;
        setNeedsUpdateTransform();
        onInternalEvent("size_changed");
    }
    
    const KKRect& getFrame() const
    {
        return  mFrame;
    }
    
    void setFrame(KKRect frame) {
        mFrame = frame;
        
        mBounds.size.width = mFrame.size.width - mEdgeInsets.left - mEdgeInsets.right;
        mBounds.size.height = mFrame.size.height - mEdgeInsets.top - mEdgeInsets.bottom;
        setNeedsUpdateTransform();
        onInternalEvent("size_changed");
    }
    
    const KKSize& getSize() const
    {
        return mFrame.size;
    }
    
    const KKSize getSizePercent() const
    {
        KKSize size;
        size.width = mFrame.size.width / mParent->getSize().width;
        size.height = mFrame.size.height / mParent->getSize().height;
        
        return size;
    }
    
    void setSizePercent(KKSize size)
    {
        setSizePercent(size.width, size.height);
    }
    
    void setSizePercent(float width, float height)
    {
        mFrame.size.width = mParent->getSize().width * width;
        mFrame.size.height = mParent->getSize().height * height;
        mBounds.size.width = width - mEdgeInsets.left - mEdgeInsets.right;
        mBounds.size.height = height - mEdgeInsets.top - mEdgeInsets.bottom;
        setNeedsUpdateTransform();
        onInternalEvent("size_changed");
    }
    
    void setPosition(float x, float y)
    {
        mFrame.origin.x = x;
        mFrame.origin.y = y;
        setNeedsUpdateTransform();
        onInternalEvent("position_changed");
    }
    
    void setPosition(KKPoint pos)
    {
        setPosition(pos.x, pos.y);
    }
    
    const KKPoint& getPosition() const
    {
        return mFrame.origin;
    }
    
    void setCenter(float x, float y)
    {
        setPosition(x - mFrame.size.width/2, y - mFrame.size.height/2);
    }
    
    void setCenter(KKPoint pos)
    {
        setCenter(pos.x, pos.y);
    }
    
    void setNeedsUpdateTransform()
    {
        mNeedsUpdateTransform = true;
        
        for ( auto &subView : mSubviews)
        {
            if (subView)
                subView->setNeedsUpdateTransform();
        }
    }
    
    void setPositionPercent(KKPoint pos)
    {
        setPositionPercent(pos.x, pos.y);
    }
    
    void setPositionPercent(float x, float y)
    {
        mFrame.origin.x = mParent->getPosition().x * x;
        mFrame.origin.y = mParent->getPosition().y * y;
        setNeedsUpdateTransform();
        onInternalEvent("position_changed");
    }
    
    const KKPoint getPositionPercent() const
    {
        KKPoint pos;
        pos.x = mFrame.origin.x / mParent->getPosition().x;
        pos.y =  mFrame.origin.y / mParent->getPosition().y;
        return  pos;
    }
    
    void setEdgeInsets(KKEdgeInsets insets)
    {
        mEdgeInsets = insets;
        mBounds.origin.x = insets.left;
        mBounds.origin.y = insets.top;
        mBounds.size.width = mFrame.size.width - mEdgeInsets.left - mEdgeInsets.right;
        mBounds.size.height = mFrame.size.height - mEdgeInsets.top - mEdgeInsets.bottom;
        setNeedsUpdateTransform();
        onInternalEvent("edgeinsets_changed");
    }
    
    void setVisible(bool visible)
    {
        if (mVisible != visible)
        {
            mVisible = visible;
            onInternalEvent("visible_changed");
        }
    }
    
    bool isVisible() const
    {
        return mVisible;
    }
    
    void setUserInteractionEnabled(bool enable)
    {
        if (mUserInteractionEnabled != enable)
        {
            mUserInteractionEnabled = enable;
            onInternalEvent("userInteraction_changed");
        }
        
    }
    
    bool userInteractionEnabled() const
    {
        return mUserInteractionEnabled;
    }
    
    void setMultiTouchEnabled(bool enable)
    {
        mMultiTouchEnabled = enable;
    }
    
    bool multiTouchEnabled() const
    {
        return mMultiTouchEnabled;
    }
    
    void setClipToBounds(bool clip)
    {
        if (mClipToBounds != clip)
        {
            mClipToBounds = clip;
            onInternalEvent("clipToBounds_changed");
        }
    }
    
    bool clipToBounds() const
    {
        return mClipToBounds;
    }
    
    void setBackgroundColor(color4B color)
    {
        mBackgroundColor = color;
        onInternalEvent("backgroundcolor_changed");
    }
    
    const color4B& getBackgroundColor() const
    {
        return mBackgroundColor;
    }
    
    void setTag(int32_t tag)
    {
        mTag = tag;
        onInternalEvent("tag_changed");
    }
    
    int32_t getTag() const
    {
        return mTag;
    }
    
    KKView *findViewWithTag(int32_t tag);
    
    void centerInParent();
    
    void centerHorizontallyInParent();
    void centerVerticallyInParent();
    void leftInParent();
    void rightInParent();
    void topInParent();
    void bottomInParent();
    
    virtual void visit() {}
    
protected:
    virtual void onDraw();
    virtual void onInternalEvent(const char *event) {}
    
    KKView() { mBackgroundColor.a = 0; }
    
protected:
    KKRect mFrame { KKRectZero };
    KKRect mBounds { KKRectZero };
    KKEdgeInsets mEdgeInsets {0.f, 0.f, 0.f, 0.f};
    KKView *mParent {nullptr};
    
    std::list<KKView *> mSubviews;
    
    bool mVisible {true};
    bool mUserInteractionEnabled {false};
    bool mMultiTouchEnabled {false};
    bool mClipToBounds {false};
    color4B mBackgroundColor;
    
    int32_t mTag {0};
    
    //kmMat4
    kmMat4 mNodeToWorldTransform; // 节点转世界坐标矩阵
    kmMat4 mWorldToNodeTransform; // 世界转当前节点坐标矩阵
    
    bool mNeedsUpdateTransform {false};
    kmMat4 mTransform; //当前矩阵
};








NS_KK_END_GUI

NS_KK_END

#endif /* KKView_hpp */
