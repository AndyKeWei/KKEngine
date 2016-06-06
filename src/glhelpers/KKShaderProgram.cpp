//
//  KKShaderProgram.cpp
//  KKEngine
//
//  Created by kewei on 3/14/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#include "KKShaderProgram.hpp"
#include <stdio.h>
#include <pthread.h>
#include "KKObject.h"
#include "KKGLMatrixStack.h"
#include "KKShaderHelper.h"

#ifdef TARGET_WIN32
extern bool operator<(const pthread_t& first, const pthread_t& second);
#endif

extern pthread_t mainthread;

NS_KK_BEGIN

std::map<pthread_t, KKProgramManager * > programManager;

void KKProgramUniform::setUniform1i(GLint i1)
{
    if (updateUniform(&i1, sizeof(GLint)))
    {
        glUniform1i(location, i1);
    }
}

void KKProgramUniform::setUniform1f(GLfloat f1)
{
    if (updateUniform(&f1, sizeof(GLfloat)))
    {
        glUniform1f(location, f1);
    }
}

void KKProgramUniform::setUniform2i(GLint i1, GLint i2)
{
    GLint ints[2] = {i1, i1};
    if (updateUniform(ints, sizeof(ints)))
    {
        glUniform2i(location, i1, i1);
    }
}

void KKProgramUniform::setUniform2f(GLfloat f1, GLfloat f2)
{
    GLfloat floats[2] = {f1, f2};
    if (updateUniform(floats, sizeof(floats)))
    {
        glUniform2f(location, f1, f2);
    }
}

void KKProgramUniform::setUniform3i(GLint i1, GLint i2, GLint i3)
{
    GLint ints[3] = {i1, i2, i3};
    if (updateUniform(ints, sizeof(ints)))
    {
        glUniform3i(location, i1, i2, i3);
    }
}

void KKProgramUniform::setUniform3f(GLfloat f1, GLfloat f2, GLfloat f3)
{
    GLfloat floats[3] = {f1, f2, f3};
    if (updateUniform(floats, sizeof(floats)))
    {
        glUniform3f(location, f1, f2, f3);
    }
}

void KKProgramUniform::setUniform4i(GLint i1, GLint i2, GLint i3, GLint i4)
{
    GLint ints[4] = {i1, i2, i3, i4};
    if (updateUniform(ints, sizeof(ints)))
    {
        glUniform4i(location, i1, i2, i3, i4);
    }
}

void KKProgramUniform::setUniform4f(GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4)
{
    GLfloat floats[4] = {f1, f2, f3, f4};
    if (updateUniform(floats, sizeof(floats)))
    {
        glUniform4f(location, f1, f2, f3, f4);
    }
}

void KKProgramUniform::setUniform2iv(GLint* ints, unsigned int num)
{
    if (updateUniform(ints, sizeof(GLint)*2*num))
    {
        glUniform2iv(location, num, ints);
    }
}

void KKProgramUniform::setUniform2fv(GLfloat* floats, unsigned int num)
{
    if (updateUniform(floats, sizeof(GLfloat)*2*num))
    {
        glUniform2fv(location, num, floats);
    }
}

void KKProgramUniform::setUniform3iv(GLint* ints, unsigned int num)
{
    if (updateUniform(ints, sizeof(GLint)*3*num))
    {
        glUniform3iv(location, num, ints);
    }
}

void KKProgramUniform::setUniform3fv(GLfloat* floats, unsigned int num)
{
    if (updateUniform(floats, sizeof(GLfloat)*3*num))
    {
        glUniform3fv(location, num, floats);
    }
}

void KKProgramUniform::setUniform4iv(GLint* ints, unsigned int num)
{
    if (updateUniform(ints, sizeof(GLint)*4*num))
    {
        glUniform4iv(location, num, ints);
    }
}

void KKProgramUniform::setUniform4fv(GLfloat* floats, unsigned int num)
{
    if (updateUniform(floats, sizeof(GLfloat)*4*num))
    {
        glUniform4fv(location, num, floats);
    }
}

void KKProgramUniform::setUniformMatrix4fv(GLfloat* matrix, unsigned int num)
{
    if (updateUniform(matrix, sizeof(GLfloat)*16*num))
    {
        glUniformMatrix4fv(location, num, GL_FALSE, matrix);
    }
}

bool KKProgramUniform::updateUniform(GLvoid* data1, unsigned int bytes)
{
    if (data.size() != bytes)
    {
        data.resize(bytes);
        memcpy(&data[0], data1, bytes);
    }
    else
    {
        if (memcmp(&data[0], data1, bytes))
        {
            memcpy(&data[0], data1, bytes);
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

KKProgram* KKProgram::programFromFiles(const char* vShaderFilename, const char* fShaderFilename)
{
    const GLchar* vertexSource = KKString::stringWithContentsOfFile(vShaderFilename)->_value.c_str();
    const GLchar* fragmentSource = KKString::stringWithContentsOfFile(fShaderFilename)->_value.c_str();
    KKProgram *p = new KKProgram(vertexSource, fragmentSource);
    p->autorelease();
    return p;
}

KKProgram::KKProgram(const GLchar* vShaderByteArray, const GLchar* fShaderByteArray)
{
    _program = glCreateProgram();
    CHECK_GL_ERROR_DEBUG();
    _vertShader = _fragShader = 0;
    
    if (vShaderByteArray)
    {
        if (!compileShader(&_vertShader, GL_VERTEX_SHADER, vShaderByteArray))
        {
            printf("%s", vShaderByteArray);
            KKLOG("ERROR: Failed to compile vertex shader");
        }
    }
    
    if (fShaderByteArray)
    {
        if (!compileShader(&_fragShader, GL_FRAGMENT_SHADER, fShaderByteArray))
        {
            printf("%s", fShaderByteArray);
            KKLOG("ERROR: Failed to compile fragment shader");
        }
    }
    
    if (_vertShader)
    {
        glAttachShader(_program, _vertShader);
    }
    CHECK_GL_ERROR_DEBUG();
    
    if (_fragShader)
    {
        glAttachShader(_program, _fragShader);
    }
    CHECK_GL_ERROR_DEBUG();
}

KKProgram::~KKProgram()
{
    if (_program)
    {
        glDeleteProgram(_program);
    }
}

bool KKProgram::compileShader(GLuint * shader, GLenum type, const GLchar* source)
{
    GLint status;
    
    if (!source)
    {
        return false;
    }
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    CHECK_GL_ERROR_DEBUG();
    glCompileShader(*shader);
    CHECK_GL_ERROR_DEBUG();
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    CHECK_GL_ERROR_DEBUG();
    
    return status == GL_TRUE;
}

bool KKProgram::link()
{
    glLinkProgram(_program);
    
    if (_vertShader)
    {
        glDeleteShader(_vertShader);
    }
    if (_fragShader) {
        glDeleteShader(_fragShader);
    }
    _vertShader = _fragShader = 0;
    
    return true;
}

void KKProgram::addAttribute(const char* attributeName, GLuint index)
{
    glBindAttribLocation(_program,
                         index,
                         attributeName);
}

void KKProgram::updateUniforms()
{
    int len, total, size, type;
    glGetProgramiv( _program, GL_ACTIVE_UNIFORMS, &total );
    GLint sampler = -1;
    GLint sampler1 = -1;
    for (int i = 0; i < total; ++i)
    {
        KKProgramUniform uniform;
        glGetActiveUniform(_program, (GLuint)i, (GLsizei)64, (GLsizei *)&len, (GLint*)&size, (GLenum*)&type, uniform.name);
        if (!strcmp(uniform.name, kUniformMVPMatrix_s))
        {
            _mvpUniformLocation = glGetUniformLocation(_program, uniform.name);
        }
        else if (!strcmp(uniform.name, kUniformSampler_s))
        {
            sampler = glGetUniformLocation(_program, uniform.name);
        }
        else if (!strcmp(uniform.name, kUniformSamplerAlpha_s))
        {
            sampler1 = glGetUniformLocation(_program, uniform.name);
        }
        else
        {
            uniform.location = glGetUniformLocation(_program, uniform.name);
            mUniforms.push_back(uniform);
        }
    }
    this->use();
    if (sampler >= 0)
    {
        glUniform1i(sampler, 0);
    }
    if (sampler1 >= 0)
    {
        glUniform1i(sampler1, 1);
    }
}

KKProgramUniform& KKProgram::getUniform(const char* name)
{
    static KKProgramUniform uniform;
    uniform.location = -1;
    std::vector<KKProgramUniform>::iterator itr = mUniforms.begin();
    for (; itr != mUniforms.end(); ++itr)
    {
        if (!strcmp(name, (*itr).name))
        {
            return *itr;
        }
    }
    return uniform;
}

void KKProgram::setUniformForModelViewProjectionMatrix()
{
    kmMat4 matrixP;
    kmMat4 matrixMV;
    kmMat4 matrixMVP;
    
    KKGLMatrixStack* matrixStack = KKGLMatrixStack::currentMatrixStack();
    matrixStack->getMatrix(KM_GL_PROJECTION, &matrixP );
    matrixStack->getMatrix(KM_GL_MODELVIEW, &matrixMV );
    
    kmMat4Multiply(&matrixMVP, &matrixP, &matrixMV);
    
    glUniformMatrix4fv(_mvpUniformLocation, (GLsizei)1, GL_FALSE, matrixMVP.mat);
}

unsigned int KKProgramManager::addProgram(const char* vShader, const char* fShader)
{
    KKProgram *program = new KKProgram(vShader, fShader);
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    return (unsigned int)mPrograms.size()-1;
}

KKProgramManager* KKProgramManager::currentManager()
{
    pthread_t thread = pthread_self();
    std::map<pthread_t, KKProgramManager * >::iterator itr = programManager.find(thread);
    if (itr == programManager.end())
    {
        KKProgramManager* mgr = new KKProgramManager();
        programManager.insert(std::make_pair(thread, mgr));
        if (!pthread_equal(mainthread, thread))
        {
            mgr->autorelease();
        }
        return mgr;
    }
    return itr->second;
}

KKProgramManager::~KKProgramManager()
{
    pthread_t thread = pthread_self();
    programManager.erase(thread);
    std::vector<KKProgram*>::iterator itr;
    for (itr = mPrograms.begin(); itr != mPrograms.end(); itr++)
    {
        (*itr)->release();
    }
}

void KKProgramManager::loadDefaultShaders()
{
    //取得所有shader
    KKProgram *program = new KKProgram(shaderPositionTextureColorVert, shaderPositionTextureColorFrag);
    //先用glBindAttribLocation绑定这些,顶点坐标、每个顶点的颜色、纹理坐标值等的位置
    //有了这些缺省的值，后续就可以直接通过 kVertexAttrib_Position、kVertexAttrib_Color、kVertexAttrib_TexCoords等值通过glVertexAttribPointer为绑定的变量赋值
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameColor, kVertexAttrib_Color);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureVert, shaderPositionTextureFrag);
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureColorAdvanceVert, shaderPositionTextureColorAdvanceFrag);
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureA8ColorVert, shaderPositionTextureColorGrayFrag);
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameColor, kVertexAttrib_Color);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureColorAdvanceVert, shaderPositionTextureColorAdvanceGrayFrag);
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionColorVert, shaderPositionColorFrag);
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameColor, kVertexAttrib_Color);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPosition_uColorVert, shaderPosition_uColorFrag);
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureA8ColorVert, shaderPositionTextureA8ColorFrag);
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameColor, kVertexAttrib_Color);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureA8ColorVert, shaderPositionTextureA8DistFrag);
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameColor, kVertexAttrib_Color);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureA8ColorVert, shaderPositionTextureA8DistOutlineFrag);
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameColor, kVertexAttrib_Color);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureColorVert, shaderPositionTextureColorAlphaTestFrag);
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameColor, kVertexAttrib_Color);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    std::string tmp = "#define EXTRA_ALPHA\n";
    program = new KKProgram(shaderPositionTextureColorVert, (tmp+shaderPositionTextureColorFrag).c_str());
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameColor, kVertexAttrib_Color);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureVert, shaderPositionTextureFrag);
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureColorAdvanceVert, (tmp+shaderPositionTextureColorAdvanceFrag).c_str());
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureA8ColorVert, (tmp+shaderPositionTextureColorGrayFrag).c_str());
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameColor, kVertexAttrib_Color);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureColorAdvanceVert, (tmp+shaderPositionTextureColorAdvanceGrayFrag).c_str());
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    tmp = "#define PREMULT_ALPHA\n";
    program = new KKProgram(shaderPositionTextureColorVert, (tmp+shaderPositionTextureColorFrag).c_str());
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameColor, kVertexAttrib_Color);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureVert, shaderPositionTextureFrag);
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureColorAdvanceVert, (tmp+shaderPositionTextureColorAdvanceFrag).c_str());
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureA8ColorVert, (tmp+shaderPositionTextureColorGrayFrag).c_str());
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameColor, kVertexAttrib_Color);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureColorAdvanceVert, (tmp+shaderPositionTextureColorAdvanceGrayFrag).c_str());
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    tmp += "#define EXTRA_ALPHA\n";
    program = new KKProgram(shaderPositionTextureColorVert, (tmp+shaderPositionTextureColorFrag).c_str());
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameColor, kVertexAttrib_Color);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureVert, shaderPositionTextureFrag);
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureColorAdvanceVert, (tmp+shaderPositionTextureColorAdvanceFrag).c_str());
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureA8ColorVert, (tmp+shaderPositionTextureColorGrayFrag).c_str());
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameColor, kVertexAttrib_Color);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
    
    program = new KKProgram(shaderPositionTextureColorAdvanceVert, (tmp+shaderPositionTextureColorAdvanceGrayFrag).c_str());
    program->addAttribute(kAttributeNamePosition, kVertexAttrib_Position);
    program->addAttribute(kAttributeNameTexCoord, kVertexAttrib_TexCoords);
    program->link();
    program->updateUniforms();
    mPrograms.push_back(program);
    CHECK_GL_ERROR_DEBUG();
}


NS_KK_END