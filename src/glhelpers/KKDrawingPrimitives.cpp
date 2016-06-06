//
//  KKDrawingPrimitives.cpp
//  KKEngine
//
//  Created by kewei on 3/14/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#include "KKDrawingPrimitives.hpp"
#include "KKShaderProgram.hpp"

NS_KK_BEGIN

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

static bool initialized = false;
static KKProgram *shader_ = nullptr;
static KKProgramUniform *colorUniform = nullptr;
static color4F color_ = {1,1,1,1};
static KKProgramUniform *pointSizeUniform = nullptr;
static GLfloat pointSize_ = 1.0f;

static void lazy_init()
{
    if (!initialized) {
        shader_ = KKProgramManager::currentManager()->getProgram(kShaderPosition_uColor);
        
        std::vector<KKProgramUniform>&v = shader_->getUniforms();
        for (std::vector<KKProgramUniform>::iterator it = v.begin(); it != v.end(); ++it)
        {
            KKProgramUniform* u = &(*it);
            if (!strcmp(u->name, kUniformColor))
            {
                colorUniform = u;
            }
            else if (!strcmp(u->name, kUniformPointSize))
            {
                pointSizeUniform = u;
            }
        }
        shader_->use();
        pointSizeUniform->setUniform1f(pointSize_);
        
        initialized = true;
    }
}

void drawLine(const KKPoint& origin, const KKPoint& dest)
{
    lazy_init();
    
    GLfloat vertices[4] = {origin.x, origin.y, dest.x, dest.y};
    shader_->use();
    CHECK_GL_ERROR_DEBUG();
    shader_->setUniformForModelViewProjectionMatrix();
    CHECK_GL_ERROR_DEBUG();
    colorUniform->setUniform4fv((GLfloat *)&color_.r, 1);
    CHECK_GL_ERROR_DEBUG();
    //开启顶点属性数组
    glEnableVertexAttribArray(kVertexAttrib_Position);
    //为顶点着色器位置信息赋值，positionSlot表示顶点着色器位置属性（即，Position）；2表示每一个顶点信息由几个值组成，这个值必须位1，2，3或4；GL_FLOAT表示顶点信息的数据类型；GL_FALSE表示不要将数据类型标准化（即fixed-point）；stride表示数组中每个元素的长度；pCoords表示数组的首地址
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    CHECK_GL_ERROR_DEBUG();
    glDrawArrays(GL_LINE, 0, 2);//将顶点数组使用三角形渲染，GL_LINE表示直线， 0表示数组第一个值的位置，2表示数组长度
}

void drawRect(const KKPoint&  origin, const KKPoint&  dest)
{
    drawLine(KKPoint(origin.x, origin.y), KKPoint(dest.x, origin.y));
    drawLine(KKPoint(dest.x, origin.y), KKPoint(dest.x, dest.y));
    drawLine(KKPoint(dest.x,dest.y), KKPoint(origin.x, dest.y));
    drawLine(KKPoint(origin.x, dest.y), KKPoint(origin.x, origin.y));
}

void drawSolidRect(const KKPoint& origin, const KKPoint& dest)
{
    KKPoint vertices[] = {origin, KKPoint(dest.x, origin.y), dest, KKPoint(origin.x, dest.y)};
    drawSolidPoly(vertices, 4);
}

void drawSolidPoly(const KKPoint* poli, unsigned int numberOfPoints)
{
    lazy_init();
    
    shader_->use();
    shader_->setUniformForModelViewProjectionMatrix();
    colorUniform->setUniform4fv((GLfloat *)&color_.r, 1);
    CHECK_GL_ERROR_DEBUG();
    
    glEnableVertexAttribArray(kVertexAttrib_Position);
    //开启顶点属性数组
    GLfloat *newPoli = new GLfloat(numberOfPoints*2);
    if (sizeof(KKPoint) == sizeof(GLfloat)*2) {
        glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, poli);
    }
    else
    {
        for (unsigned int i = 0; i < numberOfPoints; i++) {
            newPoli[i*2] = poli[i].x;
            newPoli[i*2+1] = poli[i].y;
        }
        glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, newPoli);
    }
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)numberOfPoints);//将顶点数组使用三角形渲染，GL_TRIANGLE_FAN表示矩形， 0表示数组第一个值的位置，2表示数组长度
    
    delete newPoli;
}

void drawPoly(const KKPoint *poli, unsigned int numberOfPoints, bool closePolygon)
{
    lazy_init();
    
    shader_->setUniformForModelViewProjectionMatrix();
    colorUniform->setUniform4fv((GLfloat *)&color_.r, 1);
    CHECK_GL_ERROR_DEBUG();
    
    glEnableVertexAttribArray(kVertexAttrib_Position);
    //开启顶点属性数组
    GLfloat *newPoli = new GLfloat(numberOfPoints*2);
    
    if (sizeof(KKPoint) == sizeof(GLfloat)*2) {
        glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, poli);
    }
    else
    {
        for (unsigned int i = 0; i < numberOfPoints; i++) {
            newPoli[i*2] = poli[i].x;
            newPoli[i*2+1] = poli[i].y;
        }
        glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, newPoli);
    }
    
    if (closePolygon) {
        glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) numberOfPoints);
    }
    else
    {
        glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) numberOfPoints);
    }
    
    delete newPoli;
}

void drawCircle( const KKPoint& center, float radius, float angle, unsigned int segments, bool drawLineToCenter)
{
    lazy_init();
    
    int additionalSegment = 1;
    if (drawLineToCenter)
        additionalSegment++;
    
    const float coef = 2.0f * (float)M_PI/segments;
    
    GLfloat *vertices = (GLfloat*)calloc( sizeof(GLfloat)*2*(segments+2), 1);
    if( ! vertices )
        return;
    
    for(unsigned int i = 0;i <= segments; i++) {
        float rads = i*coef;
        GLfloat j = radius * cosf(rads + angle) + center.x;
        GLfloat k = radius * sinf(rads + angle) + center.y;
        
        vertices[i*2] = j;
        vertices[i*2+1] = k;
    }
    vertices[(segments+1)*2] = center.x;
    vertices[(segments+1)*2+1] = center.y;
    
    shader_->use();
    shader_->setUniformForModelViewProjectionMatrix();
    colorUniform->setUniform4fv((GLfloat*) &color_.r, 1);
    
    glEnableVertexAttribArray(kVertexAttrib_Position);
    
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) segments+additionalSegment);
    
    free( vertices );
}

void drawSolidCircle( const KKPoint& center, float radius, float angle, unsigned int segments)
{
    const float coef = 2.0f * (float)M_PI/segments;
    
    GLfloat *vertices = (GLfloat*)calloc( sizeof(GLfloat)*2*(segments+2), 1);
    if( ! vertices )
        return;
    
    for(unsigned int i = 0;i <= segments; i++) {
        float rads = i*coef;
        GLfloat j = radius * cosf(rads + angle) + center.x;
        GLfloat k = radius * sinf(rads + angle) + center.y;
        
        vertices[i*2] = j;
        vertices[i*2+1] = k;
    }
    vertices[(segments+1)*2] = center.x;
    vertices[(segments+1)*2+1] = center.y;
    
    drawSolidPoly((KKPoint *)vertices, segments);
    
    free( vertices );
}

void drawColor4F(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    color_.r = r;
    color_.g = g;
    color_.b = b;
    color_.a = a;
}

void drawColor4F(color4F color)
{
    color_ = color;
}

void drawColor4B(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
    color_.r = r / 255.0f;
    color_.g = g / 255.0f;
    color_.b = b / 255.0f;
    color_.a = a / 255.0f;
}

void drawColor4B(color4B color)
{
    color_.r = color.r / 255.0f;
    color_.g = color.g / 255.0f;
    color_.b = color.b / 255.0f;
    color_.a = color.a / 255.0f;
}



NS_KK_END