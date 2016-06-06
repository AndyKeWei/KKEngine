//
//  KKClippingComponent.hpp
//  KKEngine
//
//  Created by kewei on 3/12/16.  模版遮罩
//  Copyright © 2016 kewei. All rights reserved.
//

/*
 
 什么是“ClippingNode”？在我看来，“ClippingNode”可以定义为利用模版遮罩来完成对Node进行区域剪切的技术。我们来打个比方，在寒冷的冬季，我们家里的窗户玻璃上常常凝结一层雾以致于我们无法看到玻璃外的景物，这时候我们常常会抹掉这层雾，当然，很多小朋友也会像红孩儿一样兴致勃勃的在抹这层雾时顺手写几笔“数风流人物，还看今朝!”，当雾被抹掉后，玻璃外的景色也就显露出来了。那么，如果我们把窗户玻璃当做显示景物的层，则玻璃上的这层雾就可称为模版遮罩了，实际上，本节的技术无非就是如何在这个模版遮罩上写出那几笔风流大字。
 */

#ifndef KKClippingComponent_hpp
#define KKClippingComponent_hpp

#include "KKComponent.h"
#include "KKEntityManager.hpp"
#include "KKGL.h"

NS_KK_BEGIN

class KKClippingComponent : public KKComponent {
    
    PROPERTIES_DECLARE
    
protected:
    
    //所用的模版缓冲遮罩结点，也就是那层玻璃上的雾像
    PROPERTY_DECLARE_IMPLEMENT_INIT(KKClippingComponent, KKEntity *, stencil, nullptr)
    //这个值其实是指的遮罩运算是否按取反设置
    PROPERTY_DECLARE_IMPLEMENT_INIT(KKClippingComponent, bool, inverted, false)
    //是否需要drawStencil
    //PROPERTY_READONLY_DECLARE_IMPLEMENT_INIT(KKClippingComponent, bool, drawStencil, false)
    
    //定义静态变量，用于记录当前程序一共用到的模版缓冲遮罩数量
    //PROPERTY_DECLARE_IMPLEMENT_INIT(KKClippingComponent, GLint, layer, -1)
    
public:
    KKClippingComponent();
    ~KKClippingComponent();
    
    //virtual void visit();
    
    std::function<void()> onResoreStencilState;
    
public:
    FAMILYID
};


NS_KK_END


#endif /* KKClippingComponent_hpp */
