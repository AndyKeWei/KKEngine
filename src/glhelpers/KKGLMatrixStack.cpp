//
//  KKGLMatrixStack.cpp
//  
//
//  Created by kewei on 1/8/16.
//
//

#include "KKGLMatrixStack.h"
#include <map>
#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <pthread.h>
#include "KKUtil.h"

/* 终于明白为什么使用glPushMatrix()和glPopMatrix()的原因了。
 将本次需要执行的缩放、平移等操作放在glPushMatrix和glPopMatrix之间。
 glPushMatrix()和glPopMatrix()的配对使用可以消除上一次的变换对本次变换的影响。
 使本次变换是以世界坐标系的原点为参考点进行
 
 参考： http://anony3721.blog.163.com/blog/static/511974201133095555708/
 
 */

NS_KK_BEGIN

std::map<pthread_t, KKGLMatrixStack *> matrix_stacks;
KKGLMatrixStack* main_stack = nullptr;

OpenThreads::Mutex matrix_mutex;

KKGLMatrixStack::KKGLMatrixStack()
{
    kmMat4 identity; //Temporary identity matrix
    
    //Initialize all 3 stacks
    //modelview_matrix_stack = (km_mat4_stack*) malloc(sizeof(km_mat4_stack));
    km_mat4_stack_initialize(&modelview_matrix_stack);
    
    //projection_matrix_stack = (km_mat4_stack*) malloc(sizeof(km_mat4_stack));
    km_mat4_stack_initialize(&projection_matrix_stack);
    
    //texture_matrix_stack = (km_mat4_stack*) malloc(sizeof(km_mat4_stack));
    km_mat4_stack_initialize(&texture_matrix_stack);
    
    current_stack = &modelview_matrix_stack;
    
    kmMat4Identity(&identity);
    
    //Make sure that each stack has the identity matrix
    km_mat4_stack_push(&modelview_matrix_stack, &identity);
    km_mat4_stack_push(&projection_matrix_stack, &identity);
    km_mat4_stack_push(&texture_matrix_stack, &identity);
}

KKGLMatrixStack *KKGLMatrixStack::currentMatrixStack()
{
    if (currentIsMainThread())
    {
        if (!main_stack)
        {
            auto itr = matrix_stacks.find(pthread_self());
            if (itr != matrix_stacks.end())
            {
                main_stack = itr->second;
            }
            else
            {
                main_stack = new KKGLMatrixStack();
            }
        }
        return main_stack;
    }
    pthread_t thread = pthread_self();
    
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(matrix_mutex);
    
    auto itr = matrix_stacks.find(thread);
    if (itr != matrix_stacks.end())
    {
        KKGLMatrixStack *stack = itr->second;
        return stack;
    }
    else
    {
        KKGLMatrixStack *stack = new KKGLMatrixStack();
        matrix_stacks.insert(std::make_pair(thread, stack));
        return stack;
    }
}

void KKGLMatrixStack::destroyCurrentMatrix()
{
    pthread_t thread = pthread_self();
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(matrix_mutex);
    auto itr = matrix_stacks.find(thread);
    if (itr != matrix_stacks.end())
    {
        matrix_stacks.erase(itr);
    }
}

void KKGLMatrixStack::pushMatrix()
{
    kmMat4 top;
    
    //Duplicate the top of the stack (i.e the current matrix)
    kmMat4Assign(&top, current_stack->top); //将栈顶矩阵复制到top
    km_mat4_stack_push(current_stack, &top);//将top压栈
}

void KKGLMatrixStack::popMatrix()
{
    km_mat4_stack_pop(current_stack, nullptr); //当前栈 弹出一个矩阵
}

void KKGLMatrixStack::getMatrix(kmGLEnum mode, kmMat4 *pOut)
{
    switch (mode) {
        case KM_GL_MODELVIEW:
            kmMat4Assign(pOut, modelview_matrix_stack.top);
            break;
        case KM_GL_PROJECTION:
            kmMat4Assign(pOut, projection_matrix_stack.top);
            break;
        case KM_GL_TEXTURE:
            kmMat4Assign(pOut, texture_matrix_stack.top);
            break;
        default:
            assert("Invalid matrix mode specified");
            break;
    }
}

void KKGLMatrixStack::matrixMode(kmGLEnum mode)
{
    switch (mode) {
        case KM_GL_MODELVIEW:
            current_stack = &modelview_matrix_stack;
            break;
        case KM_GL_PROJECTION:
            current_stack = &projection_matrix_stack;
            break;
        case KM_GL_TEXTURE:
            current_stack = &texture_matrix_stack;
            break;
        default:
            assert("Invalid matrix mode specified");
            break;
    }
}

//创建单位矩阵
void KKGLMatrixStack::loadIdentity()
{
    //kmMat4Identify用于初始化“单位矩阵(Indentify Matrix)”，所谓"单位矩阵"，指的是对脚线上元素都为1的矩阵。从kmMat4Identify的实现，我们也可以看出这一点：
    kmMat4Identity(current_stack->top);
}

void KKGLMatrixStack::loadMatrix(const kmMat4 *pIn)
{
    //获取MV栈栈顶矩阵 (模型视图矩阵（MV）)
    kmMat4Assign(current_stack->top, pIn);
}

void KKGLMatrixStack::multMatrix(const kmMat4 *pIn)
{
    //将pIn与当前栈栈顶矩阵相乘，结果存入栈顶矩阵
    kmMat4Multiply(current_stack->top, current_stack->top, pIn);
}

void KKGLMatrixStack::translatef(float x, float y, float z)
{
    kmMat4 translation;
    
    //Create a translation matrix
    kmMat4Translation(&translation, x, y, z);
    
    //Multiply the translation matrix by the current matrix
    kmMat4Multiply(current_stack->top, current_stack->top, &translation);
}

void KKGLMatrixStack::rotatef(float angle, float x, float y, float z)
{
    kmVec3 axis;
    kmMat4 rotation;
    
    kmVec3Fill(&axis, x, y, z);
    
    //Create a rotation matrix using the axis and the angle
    kmMat4RotationAxisAngle(&rotation, &axis, kmDegreesToRadians(angle));
    
    //Multiply the rolation matrix by the current matrix
    kmMat4Multiply(current_stack->top, current_stack->top, &rotation);
}

void KKGLMatrixStack::scalef(float x, float y, float z)
{
    kmMat4 scaling;
    
    //Create a scale matrix
    kmMat4Scaling(&scaling, x, y, z);
    
    //Multiply the rolation matrix by the current matrix
    kmMat4Multiply(current_stack->top, current_stack->top, &scaling);
}

NS_KK_END
