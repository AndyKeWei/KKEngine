//
//  KKSceneManager.cpp
//  KKEngine
//
//  Created by kewei on 1/13/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#include "KKSceneManager.hpp"
#include "KKGL.h"
#include "KKTransform2DComponent.hpp"
#include "KKScissorComponent.hpp"
#include "KKClippingComponent.hpp"
#include "KKDirector.h"
#include "KKDrawingPrimitives.hpp"

NS_KK_BEGIN

KKSceneManager::~KKSceneManager()
{
    
}


void KKSceneManager::draw()
{
    KKGLMatrixStack::currentMatrixStack()->multMatrix(&mCameraMatrix);
    
    auto itr = entities.begin();
    
    while (itr != entities.end()) {
        if (*itr == nullptr || *itr == NULL)
        {
            itr = entities.erase(itr);
        }
        else
        {
            if ((*itr)->isEnabled() && (*itr)->isVisible())
            {
                drawEntity(*itr);
            }
            ++itr;
        }
    }
}

KKEntity *KKSceneManager::hitTest(const KKPoint& p)
{
#if defined(TARGET_MACOS)
    OpenThreads::ScopedReadLock lock(mMutex);
#endif
    for (auto itr = entities.rbegin();itr != entities.rend(); itr++)
    {
        kmMat4 t;
        kmMat4Identity(&t);//创建单位矩阵
        kmMat4Multiply(&t, &t, &mCameraMatrix);
        if (*itr != nullptr)
        {
            KKEntity *res = (*itr)->hitTest(&t, p);
            if (res)
            {
                return res;
            }
        }
    }

    return nullptr;
}

//记录当前设备运行程序时模版缓冲的位数
GLint g_sStencilBits = -1;

void KKSceneManager::drawEntity(KKEntity *entity)
{
    if (!entity->isVisible()) {
        
        return;
    }
    
    //获得矩阵堆栈
    KKGLMatrixStack *matrixStack = KKGLMatrixStack::currentMatrixStack();
    
    //模板裁切组件
    KKScissorComponent *scissorComp = entity->getComponent<KKScissorComponent>();
    bool scissorEnabled = false;//是否启用裁剪测试
    if (scissorComp)
    {
        scissorComp->visit();
        scissorEnabled = scissorComp->get_scissorEnabled();
        bool scissorOutside = scissorComp->get_outside();
        if (scissorOutside)
        {
            //outside area
            matrixStack->popMatrix();
            return;
        }
    }
    
    //模板遮罩组件
    bool drawstencil = false;
    
    //定义静态变量，用于记录当前程序一共用到的模版缓冲遮罩数量
    static GLint layer = -1;
    // 这里是模版运算的一大堆相关值。
    //是否使用模版缓冲
    GLboolean currentStencilEnabled = GL_FALSE;
    //写入的模版参数
    GLuint currentStencilWriteMask = ~0;
    //模版运算的判断
    GLenum currentStencilFunc = GL_ALWAYS;
    GLint currentStencilRef = 0;
    GLuint currentStencilValueMask = ~0;
    GLenum currentStencilFail = GL_KEEP;
    GLenum currentStencilPassDepthFail = GL_KEEP;
    GLenum currentStencilPassDepthPass = GL_KEEP;
    
    do {
        
        if (g_sStencilBits < 1)
        {
            break;
        }
        
        KKClippingComponent *comp = entity->getComponent<KKClippingComponent>();
        if (!comp)
        {
            break;
        }
        
        // 如果这个数量已经把模版缓冲的位数都占光了，那也洗洗上床睡吧。怎么才会占光呢？后面再讲。
        if ((layer + 1) == g_sStencilBits)
        {
            static bool once = true;
            if (once) {
                
                KKLOG("Nesting more than %d stencils is not supported. Everything will be drawn without stencil for this node and its childs.", g_sStencilBits);
                
                once = false;
            }
            return;
        }
        
        //如果还可以继续使用新的模版缓冲位，那可以干正事了。
        //对数量值加一，也就是占下相应的模版缓冲位。
        layer++;
        
        //计算出当前模版缓冲位的参数值
        // mask of the current layer (ie: for layer 3: 00000100)
        GLint mask_layer = 0x1 << layer;
        
        // 计算出当前模版缓冲位的参数值减1.那结果值肯定是当前位数以下的值都取反，即掩码值。
        // mask of all layers less than the current (ie: for layer 3: 00000011)
        GLint mask_layer_1 = mask_layer - 1;
        
        // 上面两个值做或运算的结果值。
        // mask of all layers less than or equal to the current (ie: for layer 3: 00000111)
        GLint mask_layer_le = mask_layer | mask_layer_1;
        
        currentStencilEnabled = glIsEnabled(GL_STENCIL_TEST);
        //取得上面的这些变量值。
        glGetIntegerv(GL_STENCIL_WRITEMASK, (GLint *)&currentStencilWriteMask);
        glGetIntegerv(GL_STENCIL_FUNC, (GLint *)&currentStencilFunc);
        glGetIntegerv(GL_STENCIL_REF, &currentStencilRef);
        glGetIntegerv(GL_STENCIL_VALUE_MASK, (GLint *)&currentStencilValueMask);
        glGetIntegerv(GL_STENCIL_FAIL, (GLint *)&currentStencilFail);
        glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, (GLint *)&currentStencilPassDepthFail);
        glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, (GLint *)&currentStencilPassDepthPass);
        
        //开始模版测试。
        glEnable(GL_STENCIL_TEST);
        CHECK_GL_ERROR_DEBUG();
        
        //设置模版缓冲的掩码值。
        glStencilMask(mask_layer);
        
        //取得是否可以定入模版掩码参数。
        GLboolean currentDepthWriteMask = GL_TRUE;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &currentDepthWriteMask);
        
        //禁止写入深度缓冲。
        glDepthMask(GL_FALSE);
        
        //下面这一句是指永远不能通过测试
        glStencilFunc(GL_NEVER, mask_layer, mask_layer);
        //根据是否反向运算来决定如果测试不能通过时是否将相应像素位置的模版缓冲位的值设为0。
        glStencilOp(!comp->get_inverted() ? GL_ZERO : GL_REPLACE, GL_KEEP, GL_KEEP);
    
        matrixStack->pushMatrix();
        matrixStack->loadIdentity();
        
        //画全屏的矩形来清除模板(stencil) 缓冲(buffer)
        //用白色绘制一下屏幕矩形，因为都不能通过嘛，所以就全屏的模版缓冲位的值都被设为0。
        drawColor4F(1, 1, 1, 1);
        KKSize winSize = KKDirector::getSingleton()->getWinSize();
        drawSolidRect(KKPointZero, KKPoint(winSize.width, winSize.height));
        matrixStack->popMatrix();
        
        // DRAW CLIPPING STENCIL
        //模板测试
        //模板测试只有存在模板缓冲区的情况下进行，模板测试把像素存储在模板缓冲区的点与一个参考值进行比较（glStencilFunc），根据测试结果，对模板缓冲区的值进行响应的修改glStencilOp
        //　func：GL_NEVER 从来不能通过
        //  GL_ALWAYS 永远可以通过（默认值)
        //　GL_LESS 小于参考值可以通过
        //　GL_LEQUAL 小于或者等于可以通过
        //　GL_EQUAL 等于通过
        //　GL_GEQUAL 大于等于通过
        //　GL_GREATER 大于通过
        //　GL_NOTEQUAL 不等于通过
        //永远不能通过测试
        glStencilFunc(GL_NEVER, mask_layer, mask_layer);
        //　　void glStencilOp (GLenum fail, GLenum zfail, GLenum zpass);
        //　　fail模板测试未通过时该如何变化；zfail表示模板测试通过，但深度测试未通过时该如何变化；zpass表示模板测试和深度测试或者未执行深度测试均通过时该如何变化
        //　　GL_KEEP（不改变，这也是默认值）
        //　　GL_ZERO（回零）
        //　　GL_REPLACE（使用测试条件中的设定值来代替当前模板值）
        //　　GL_INCR（增加1，但如果已经是最大值，则保持不变），
        //　　GL_INCR_WRAP（增加1，但如果已经是最大值，则从零重新开始）
        //　　GL_DECR（减少1，但如果已经是零，则保持不变），
        //　　GL_DECR_WRAP（减少1，但如果已经是零，则重新设置为最大值）
        //　　GL_INVERT（按位取反）
        //根据是否反向运算来决定如果测试不能通过时是否将相应像素位置的模版缓冲位的值设为当前参数值。
        glStencilOp(!comp->get_inverted() ? GL_REPLACE : GL_ZERO, GL_KEEP, GL_KEEP);
        
        // draw the stencil node as if it was one of our child
        //将当前环境所用矩阵压栈后应用相应的矩阵变化再调用模版遮罩精灵结点的渲染函数，完事了矩阵出栈恢复环境所用矩阵。
        matrixStack->pushMatrix();
        
        KKTransform2DComponent *transComp = entity->getComponent<KKTransform2DComponent>();
        if (transComp)
        {
            kmMat4* transform = transComp->get_transform();
            matrixStack->multMatrix(transform);
        }
        
        KKEntity *e = comp->get_stencil();
        
        drawEntity(e);
        //恢复环境所用矩阵
        matrixStack->popMatrix();
        
        // 恢复深度写入
        glDepthMask(currentDepthWriteMask);
        
        //这里设置如果当前模版缓冲中的模版值与运算结果相等则保留相应像素。这里为什么要用mask_layer_le而不是mask_layer呢？下面再说。
        glStencilFunc(GL_EQUAL, mask_layer_le, mask_layer_le);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        
        //调用基类渲染函数，做为窗外风景的子类结点都受到模板结果的裁剪。如果子结点也是ClippingNode，则可能会继续进行模板运算，那么模板的位数layer值就必须加1，使用新的模版缓冲位来进行测试，可能造成模版缓冲的位数都占光。而且位数的增加在模版运算时要考虑进去，所以上面的模版缓冲运算的参数是mask_layer_le而不是mask_layer。
        
        //CCNode::visit();  ie. for cocos2d-x

        drawstencil = true;
    } while (0);
    

    KKTransform2DComponent *transformComp = entity->getComponent<KKTransform2DComponent>();
    matrixStack->pushMatrix();
    if (transformComp)
    {
        kmMat4 *transform = transformComp->get_transform();
        matrixStack->multMatrix(transform);
        
        //draw list for entity
        std::list<KKEntity *>&& children = transformComp->get_children();
        if (children.size() > 0)
        {
            auto itr = children.begin();
            while (itr != children.end())
            {
                if (*itr == nullptr)
                {
                    itr = children.erase(itr);
                    continue;
                }
                if ((*itr)->getComponent<KKTransform2DComponent>()->get_zOrder() < 0)
                {
                    drawEntity(*itr);
                }
                else
                {
                    break;
                }
                ++itr;
            }
            entity->onDrawHandler();
            while (itr != children.end())
            {
                if (*itr == nullptr)
                {
                    itr = children.erase(itr);
                }
                else
                {
                    drawEntity(*itr);
                    ++itr;
                }
            }
        }
        else
        {
            entity->onDrawHandler();
        }
    }
    else
    {
        entity->onDrawHandler();
    }
    
    matrixStack->popMatrix();
    
    if (drawstencil)
    {
        //恢复相应的模版缓冲运算设置
        glStencilFunc(currentStencilFunc, currentStencilRef, currentStencilValueMask);
        glStencilOp(currentStencilFail, currentStencilPassDepthFail, currentStencilPassDepthPass);
        glStencilMask(currentStencilWriteMask);
        if (!currentStencilEnabled)
        {
            //关闭模板测试
            kkglDisable(GL_STENCIL_TEST);
        }
        
        // we are done using this layer, decrement
        layer--;
    }
    
    if (scissorComp)
    {
        if (scissorEnabled)
        {
            const GLint *scissorBox = scissorComp->getScissorBox();
            kkglScissor(scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3]);
        }
        else
        {
            kkglDisable(GL_SCISSOR_TEST);
        }
    }
}











NS_KK_END
