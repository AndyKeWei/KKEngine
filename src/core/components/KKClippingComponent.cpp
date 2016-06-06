//
//  KKClippingComponent.cpp
//  KKEngine
//
//  Created by kewei on 3/12/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#include "KKClippingComponent.hpp"
#include "KKTransform2DComponent.hpp"
#include "KKStringUtil.hpp"
#include "KKGLMatrixStack.h"
#include "KKDrawingPrimitives.hpp"
#include "KKDirector.h"

NS_KK_BEGIN

INIT_FAMILYID(KKClippingComponent)
PROPERTIES_INITIAL(KKClippingComponent)
REGISTER_PROPERTY(KKClippingComponent, KKEntity *, stencil)
REGISTER_PROPERTY(KKClippingComponent, bool, inverted)
//REGISTER_PROPERTY_READONLY(KKClippingComponent, bool, drawStencil)
//REGISTER_PROPERTY(KKClippingComponent, GLint, layer)


extern GLint g_sStencilBits;

static bool g_sStencilBitsInit = false;

////这里定义一个静态变量值，用于保存当前设备运行程序时模版缓冲的位数，这个位数由设备的深度缓冲区格式决定，一般深度缓冲用D24S8,即24位深度缓冲+8位模版缓冲，所以这个值一般为8。
//static GLint g_sStencilBits = -1;

//使用了模板遮罩的次数
static int g_clippingCompCount = 0;

KKClippingComponent::KKClippingComponent()
{
    if (!g_sStencilBitsInit)
    {
        //OPENGL相应的接口函数取得程序运行在当前设备的模版缓冲区位数。
        glGetIntegerv(GL_STENCIL_BITS, &g_sStencilBits);
        if (g_sStencilBits <= 0)
        {
            KKLOG("Stencil buffer is not enabled.");
        }
        g_sStencilBitsInit = true;
    }
    ++g_clippingCompCount;
}

KKClippingComponent::~KKClippingComponent()
{
    --g_clippingCompCount;
    if (g_clippingCompCount <= 0)
    {
        g_sStencilBitsInit = false;
        g_sStencilBits = -1;
    }
}

//void KKClippingComponent::visit()
//{
//    if (g_sStencilBits < 1)
//    {
//        return;
//    }
//    if (!m_stencil || !m_stencil->isVisible()) {
//        return;
//    }
//    
//    //定义静态变量，用于记录当前程序一共用到的模版缓冲遮罩数量
//    //static GLint layer = -1;
//    
//    // 如果这个数量已经把模版缓冲的位数都占光了，那也洗洗上床睡吧。怎么才会占光呢？后面再讲。
//    if ((m_layer + 1) == g_sStencilBits)
//    {
//        static bool once = true;
//        if (once) {
//            
//            KKLOG("Nesting more than %d stencils is not supported. Everything will be drawn without stencil for this node and its childs.", g_sStencilBits);
//            
//            once = false;
//        }
//        return;
//    }
//    
//    //如果还可以继续使用新的模版缓冲位，那可以干正事了。
//    //对数量值加一，也就是占下相应的模版缓冲位。
//    m_layer++;
//    
//    //计算出当前模版缓冲位的参数值
//    // mask of the current layer (ie: for layer 3: 00000100)
//    GLint mask_layer = 0x1 << m_layer;
//    
//    // 计算出当前模版缓冲位的参数值减1.那结果值肯定是当前位数以下的值都取反，即掩码值。
//    // mask of all layers less than the current (ie: for layer 3: 00000011)
//    GLint mask_layer_1 = mask_layer - 1;
//    
//    // 上面两个值做或运算的结果值。
//    // mask of all layers less than or equal to the current (ie: for layer 3: 00000111)
//    GLint mask_layer_le = mask_layer | mask_layer_1;
//    
//    // 这里是模版运算的一大堆相关值。
//    //是否使用模版缓冲
//    GLboolean currentStencilEnabled = GL_FALSE;
//    //写入的模版参数
//    GLuint currentStencilWriteMask = ~0;
//    //模版运算的判断
//    GLenum currentStencilFunc = GL_ALWAYS;
//    GLint currentStencilRef = 0;
//    GLuint currentStencilValueMask = ~0;
//    GLenum currentStencilFail = GL_KEEP;
//    GLenum currentStencilPassDepthFail = GL_KEEP;
//    GLenum currentStencilPassDepthPass = GL_KEEP;
//    currentStencilEnabled = glIsEnabled(GL_STENCIL_TEST);
//    //取得上面的这些变量值。
//    glGetIntegerv(GL_STENCIL_WRITEMASK, (GLint *)&currentStencilWriteMask);
//    glGetIntegerv(GL_STENCIL_FUNC, (GLint *)&currentStencilFunc);
//    glGetIntegerv(GL_STENCIL_REF, &currentStencilRef);
//    glGetIntegerv(GL_STENCIL_VALUE_MASK, (GLint *)&currentStencilValueMask);
//    glGetIntegerv(GL_STENCIL_FAIL, (GLint *)&currentStencilFail);
//    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, (GLint *)&currentStencilPassDepthFail);
//    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, (GLint *)&currentStencilPassDepthPass);
//    
//    //开始模版测试。
//    kkglEnable(GL_STENCIL_TEST);
//    CHECK_GL_ERROR_DEBUG();
//    
//    //设置模版缓冲的掩码值。
//    glStencilMask(mask_layer);
//    
//    //取得是否可以定入模版掩码参数。
//    GLboolean currentDepthWriteMask = GL_TRUE;
//    glGetBooleanv(GL_DEPTH_WRITEMASK, &currentDepthWriteMask);
//    
//    //禁止写入深度缓冲。
//    glDepthMask(GL_FALSE);
//    
//    //下面这一句是指永远不能通过测试
//    glStencilFunc(GL_NEVER, mask_layer, mask_layer);
//    //根据是否反向运算来决定如果测试不能通过时是否将相应像素位置的模版缓冲位的值设为0。
//    glStencilOp(!m_inverted ? GL_ZERO : GL_REPLACE, GL_KEEP, GL_KEEP);
//    
//    KKGLMatrixStack* matrixStack = KKGLMatrixStack::currentMatrixStack();
//    matrixStack->pushMatrix();
//    matrixStack->loadIdentity();
//    
//    //画全屏的矩形来清除模板(stencil) 缓冲(buffer)
//    //用白色绘制一下屏幕矩形，因为都不能通过嘛，所以就全屏的模版缓冲位的值都被设为0。
//    drawColor4F(1, 1, 1, 1);
//    KKSize winSize = KKDirector::getSingleton()->getWinSize();
//    drawSolidRect(KKPointZero, KKPoint(winSize.width, winSize.height));
//    matrixStack->popMatrix();
//    
//    // DRAW CLIPPING STENCIL
//    //模板测试
//    //模板测试只有存在模板缓冲区的情况下进行，模板测试把像素存储在模板缓冲区的点与一个参考值进行比较（glStencilFunc），根据测试结果，对模板缓冲区的值进行响应的修改glStencilOp
//    //　func：GL_NEVER 从来不能通过
//    //  GL_ALWAYS 永远可以通过（默认值)
//    //　GL_LESS 小于参考值可以通过
//    //　GL_LEQUAL 小于或者等于可以通过
//    //　GL_EQUAL 等于通过
//    //　GL_GEQUAL 大于等于通过
//    //　GL_GREATER 大于通过
//    //　GL_NOTEQUAL 不等于通过
//    //永远不能通过测试
//    glStencilFunc(GL_NEVER, mask_layer, mask_layer);
//    //　　void glStencilOp (GLenum fail, GLenum zfail, GLenum zpass);
//    //　　fail模板测试未通过时该如何变化；zfail表示模板测试通过，但深度测试未通过时该如何变化；zpass表示模板测试和深度测试或者未执行深度测试均通过时该如何变化
//    //　　GL_KEEP（不改变，这也是默认值）
//    //　　GL_ZERO（回零）
//    //　　GL_REPLACE（使用测试条件中的设定值来代替当前模板值）
//    //　　GL_INCR（增加1，但如果已经是最大值，则保持不变），
//    //　　GL_INCR_WRAP（增加1，但如果已经是最大值，则从零重新开始）
//    //　　GL_DECR（减少1，但如果已经是零，则保持不变），
//    //　　GL_DECR_WRAP（减少1，但如果已经是零，则重新设置为最大值）
//    //　　GL_INVERT（按位取反）
//    //根据是否反向运算来决定如果测试不能通过时是否将相应像素位置的模版缓冲位的值设为当前参数值。
//    glStencilOp(!m_inverted ? GL_REPLACE : GL_ZERO, GL_KEEP, GL_KEEP);
//    
//    // draw the stencil node as if it was one of our child
//     //将当前环境所用矩阵压栈后应用相应的矩阵变化再调用模版遮罩精灵结点的渲染函数，完事了矩阵出栈恢复环境所用矩阵。
//    matrixStack->pushMatrix();
//    //获得节点矩阵信息
//    KKTransform2DComponent *transformComp = m_stencil->getComponent<KKTransform2DComponent>();
//    if (transformComp)
//    {
//        kmMat4 *transform = transformComp->get_transform();
//        matrixStack->multMatrix(transform); //矩阵相乘
//    }
//    //visit stencil
//    m_stencil->getEntityManager()->getSceneManager()->drawEntity(m_stencil);
//    
//    //恢复环境所用矩阵
//    matrixStack->popMatrix();
//    
//    // 恢复深度写入
//    glDepthMask(currentDepthWriteMask);
//    
//    //这里设置如果当前模版缓冲中的模版值与运算结果相等则保留相应像素。这里为什么要用mask_layer_le而不是mask_layer呢？下面再说。
//    glStencilFunc(GL_EQUAL, mask_layer_le, mask_layer_le);
//    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
//    
//    //调用基类渲染函数，做为窗外风景的子类结点都受到模板结果的裁剪。如果子结点也是ClippingNode，则可能会继续进行模板运算，那么模板的位数layer值就必须加1，使用新的模版缓冲位来进行测试，可能造成模版缓冲的位数都占光。而且位数的增加在模版运算时要考虑进去，所以上面的模版缓冲运算的参数是mask_layer_le而不是mask_layer。
//    
//    //CCNode::visit();  ie. for cocos2d-x
// 
//    m_drawStencil = true;
//    
//    onResoreStencilState = [ = ](){
//    
//        //KKLOG("callback now");
//        //恢复相应的模版缓冲运算设置
//        glStencilFunc(currentStencilFunc, currentStencilRef, currentStencilValueMask);
//        glStencilOp(currentStencilFail, currentStencilPassDepthFail, currentStencilPassDepthPass);
//        glStencilMask(currentStencilWriteMask);
//        if (!currentStencilEnabled)
//        {
//            //关闭模板测试
//            kkglDisable(GL_STENCIL_TEST);
//        }
//        
//        // we are done using this layer, decrement
//        m_layer--;
//    };
//
//}






















NS_KK_END