//
//  KKGLMatrixStack.h
//  
//
//  Created by kewei on 1/8/14.
//
//

#ifndef ____KKGLMatrixStack__
#define ____KKGLMatrixStack__

#include "KKMacros.h"
#include "kazmath/GL/matrix.h"
#include "kazmath/GL/mat4stack.h"

NS_KK_BEGIN

class KKGLMatrixStack {
    
public:
    static KKGLMatrixStack *currentMatrixStack();
    static void destroyCurrentMatrix();
    void pushMatrix();
    void popMatrix();
    void getMatrix(kmGLEnum mode, kmMat4 *pOut);
    void matrixMode(kmGLEnum mode);
    void loadIdentity();
    void loadMatrix(const kmMat4 *pIn);
    void multMatrix(const kmMat4 *pIn);
    void translatef(float x, float y, float z);
    void rotatef(float angle, float x, float y, float z);
    void scalef(float x, float y, float z);
private:
    KKGLMatrixStack();
    
    km_mat4_stack modelview_matrix_stack;//模型视图矩阵栈
    km_mat4_stack projection_matrix_stack;//投影矩阵栈
    km_mat4_stack texture_matrix_stack;//纹理矩阵栈
    km_mat4_stack* current_stack {nullptr};//指向当前所使用的那个变换矩阵栈
    
    //不过Cocos2d-x引 擎只用到了前两个变化矩阵栈
};

NS_KK_END

//Cocos2d-x采用“渲染树”的方式进行绘制，即先从场景(Scene)的顶层根节点开始，深度优先的递归绘制Child Node。而整个绘制的顶层节点是CCScene。绘制从m_pRunningScene->visit()真正开始。visit是Scene、 Layer、Sprite的共同父类CCNode实现的方法：
//void CCNode::visit()
//{
//    if (!m_bVisible)
//    {
//        return;
//    }
//    kmGLPushMatrix();
//    … …
//    this->transform();
//    … …
//    
//    if(m_pChildren &&
//       m_pChildren->count() > 0)
//    {
//        sortAllChildren();
//        // draw children zOrder < 0
//        … ..
//        // self draw
//        this->draw();
//        
//        // draw other children nodes
//        … …
//    } else {
//        this->draw();
//    }
//    … …
//    kmGLPopMatrix();
//}
//
//Visit大致做了这么几件事：
//– 向当前OpenGL变换矩阵栈Push元素
//– 用当前OpenGL变换矩阵栈栈顶元素的变换参数做节点变换
//– 递归绘制zOrder < 0 的子节点
//– 绘制自己
//– 递归绘制其他子节点
//– 从当前OpenGL变换矩阵栈Pop元素

//如果你想知道为什么父节点缩放(Scale)、旋转(Rotate)、扭曲(Skew)后，子节点也会跟着父节点同样缩放(Scale)、旋 转(Rotate)、扭曲？其原理就在这里的transform方法中：
//真正绘制Node的方法是CCNode::draw的override方法。CCNode::draw是一个空函数，各个子类 override该方法进行各自的绘制。以CCSprite::draw为例：
/** @def CC_NODE_DRAW_SETUP
 Helpful macro that setups the GL server state,
 the correct GL program and sets the Model View
 Projection matrix
 @since v2.0
 */
/*
#define CC_NODE_DRAW_SETUP() \
do { \
ccGLEnable(m_eGLServerState); \
CCAssert(getShaderProgram(), "No shader program set for this node"); \
{ \
getShaderProgram()->use(); \
getShaderProgram()->setUniformsForBuiltins(); \
} \
} while(0)

void CCGLProgram::setUniformsForBuiltins()
{
    kmMat4 matrixP;
    kmMat4 matrixMV;
    kmMat4 matrixMVP;
    
    kmGLGetMatrix(KM_GL_PROJECTION, &matrixP);
    kmGLGetMatrix(KM_GL_MODELVIEW, &matrixMV);
    
    kmMat4Multiply(&matrixMVP, &matrixP, &matrixMV);
    
    setUniformLocationWithMatrix4fv(m_uUniforms[kCCUniformPMatrix],
                                    matrixP.mat, 1);
    setUniformLocationWithMatrix4fv(m_uUniforms[kCCUniformMVMatrix],
                                    matrixMV.mat, 1);
    setUniformLocationWithMatrix4fv(m_uUniforms[kCCUniformMVPMatrix],
                                    matrixMVP.mat, 1);
    … …
}

经过计算顶点、绑定纹理等步骤后，最终由glDrawArrays完成Node绘制
*/

#endif /* defined(____KKGLMatrixStack__) */
