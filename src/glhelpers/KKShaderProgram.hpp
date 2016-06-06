//
//  KKShaderProgram.hpp
//  KKEngine
//
//  Created by kewei on 3/14/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#ifndef KKShaderProgram_hpp
#define KKShaderProgram_hpp

#include "KKObject.h"
#include "KKGL.h"
#include <vector>
/*
shader就是专门用来渲染3D图形的一种技 术，通过shader，程序设计人员可以自己编写显卡渲染画面的算法，使画面更漂亮、更逼真。
几年前并没有shader这个东西，所以那时候的显卡，就不 支持shader，而只支持固定管线渲染，游戏画面也没有现在的酷。
　　shader又分两种
，一种是顶点shader（3D图形都是由一个一个三角形组 成的，顶点shader就是计算顶点位置，并为后期像素渲染做准备的），另一种是像素shader，像素shader顾名思义，就是以像素为单位，计算光 照、颜色的一系列算法。
 */

NS_KK_BEGIN

enum {
    kVertexAttrib_Position,
    kVertexAttrib_Color,
    kVertexAttrib_TexCoords,
    
    kVertexAttrib_MAX,
};


enum {
    kShaderPositionTextureColor = 0,
    kShaderPositionTexture,
    kShaderPositionTextureColorAdvance,
    kShaderPositionTextureColorGray,
    kShaderPositionTextureColorAdvanceGray,
    kShaderPositionColor,
    kShaderPosition_uColor,
    kShaderPositionTextureA8Color,
    kShaderPositionTextureA8Dist,
    kShaderPositionTextureA8DistOutline,
    kShaderPositionTextureColorAlphaTest,
    kShaderPositionTextureColorWithExtraAlpha,
    kShaderPositionTextureWithExtraAlpha,
    kShaderPositionTextureColorAdvanceWithExtraAlpha,
    kShaderPositionTextureColorGrayWithExtraAlpha,
    kShaderPositionTextureColorAdvanceGrayWithExtraAlpha,
    kShaderPositionTextureColorPreMultAlpha,
    kShaderPositionTexturePreMultAlpha,
    kShaderPositionTextureColorAdvancePreMultAlpha,
    kShaderPositionTextureColorGrayPreMultAlpha,
    kShaderPositionTextureColorAdvanceGrayPreMultAlpha,
    kShaderPositionTextureColorWithExtraAlphaPreMultAlpha,
    kShaderPositionTextureWithExtraAlphaPreMultAlpha,
    kShaderPositionTextureColorAdvanceWithExtraAlphaPreMultAlpha,
    kShaderPositionTextureColorGrayWithExtraAlphaPreMultAlpha,
    kShaderPositionTextureColorAdvanceGrayWithExtraAlphaPreMultAlpha
};

// uniform names
const char* const kUniformMVPMatrix_s = "u_MVPMatrix";
const char* const kUniformSampler_s = "u_texture";
const char* const kUniformSamplerAlpha_s = "u_texture_alpha";
const char* const kUniformBrightness = "u_brightness";
const char* const kUniformColor = "u_color";
const char* const kUniformColorPercent = "u_colorPercent";
const char* const kUniformColorAmount = "u_colorAmount";
const char* const kUniformPointSize = "u_pointSize";
const char* const kUniformBase = "u_base";
const char* const kUniformScale = "u_scale";
const char* const kUniformOutlineColor = "u_outlinecolor";
const char* const kUniformBorderOffset = "u_borderoffset";
const char* const kUniformAlphaThreshold = "u_alpha_theshold";
const char* const kUniformThreshold = "u_threshold";

// Attribute names
#define    kAttributeNameColor            "a_color"
#define    kAttributeNamePosition        "a_position"
#define    kAttributeNameTexCoord        "a_texCoord"

class KKProgramUniform
{
public:
    KKProgramUniform(){} //:data(NULL) {}
    
    char name[64];
    GLint location;
    
    void setUniform1i(GLint i1);
    void setUniform1f(GLfloat f1);
    void setUniform2i(GLint i1, GLint i2);
    void setUniform2f(GLfloat f1, GLfloat f2);
    void setUniform3i(GLint i1, GLint i2, GLint i3);
    void setUniform3f(GLfloat f1, GLfloat f2, GLfloat f3);
    void setUniform4i(GLint i1, GLint i2, GLint i3, GLint i4);
    void setUniform4f(GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4);
    void setUniform2iv(GLint* ints, unsigned int num);
    void setUniform2fv(GLfloat* floats, unsigned int num);
    void setUniform3iv(GLint* ints, unsigned int num);
    void setUniform3fv(GLfloat* floats, unsigned int num);
    void setUniform4iv(GLint* ints, unsigned int num);
    void setUniform4fv(GLfloat* floats, unsigned int num);
    void setUniformMatrix4fv(GLfloat* matrix, unsigned int num);
private:
    std::vector<uint8_t> data;
    bool updateUniform(GLvoid* data, unsigned int bytes);
};

class KKProgram:public KKObject
{
    friend class KKProgramManager;
private:
    GLuint            _program;
    GLuint            _vertShader; //顶点着色器
    GLuint            _fragShader;//片断着色器(像素着色器)
    std::vector<KKProgramUniform> mUniforms;
    GLint             _mvpUniformLocation;
    
public:
    //    KKProgram(const std::string vShaderFilename, const std::string fShaderFilename)
    //    {
    //        const GLchar* vertexSource = HLString::stringWithContentsOfFile(vShaderFilename.c_str())->_value.c_str();
    //        const GLchar* fragmentSource = HLString::stringWithContentsOfFile(fShaderFilename.c_str())->_value.c_str();
    //        KKProgram(vertexSource, fragmentSource);
    //    }
    static KKProgram* programFromFiles(const char* vShaderFilename, const char* fShaderFilename);
    
    KKProgram(const GLchar* vShaderByteArray, const GLchar* fShaderByteArray);
    
    bool compileShader(GLuint * shader, GLenum type, const GLchar* source);
    
public:
    virtual ~KKProgram();
    
    void addAttribute(const char* attributeName, GLuint index);
    
    bool link();
    
    void use()
    {
        kkglUseProgram(_program);
    }
    
    void updateUniforms();
    
    void setUniformForModelViewProjectionMatrix();
    
    void reset()
    {
        _vertShader = _fragShader = 0;
        mUniforms.clear();
        _program = 0;
    }
    
    std::vector<KKProgramUniform>& getUniforms() {return mUniforms;}
    
    KKProgramUniform& getUniform(const char* name);
    
    inline const GLuint getProgram() const { return _program; }
};

class KKProgramManager: public KKObject
{
private:
    std::vector<KKProgram*> mPrograms;
    unsigned int mProgramCount;
    
    KKProgramManager() {loadDefaultShaders();}
    
public:
    
    virtual ~KKProgramManager();
    
    void loadDefaultShaders();
    
    static KKProgramManager* currentManager();
    
    unsigned int addProgram(const char* vShader, const char* fShader);
    
    KKProgram* getProgram(unsigned int index)
    {
        return mPrograms[index];
    }
};

NS_KK_END

#endif /* KKShaderProgram_hpp */
