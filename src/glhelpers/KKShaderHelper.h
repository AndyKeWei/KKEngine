//
//  KKShaderHelper.h
//  KKEngine
//
//  Created by kewei on 3/16/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#ifndef KKShaderHelper_h
#define KKShaderHelper_h

#define STRINGIZE(x) #x
#define STRINGIZE2(x) STRINGIZE(x)
#define SHADER_STRING(text) STRINGIZE2(text)

NS_KK_BEGIN

const GLchar* shaderPositionTextureColorAdvanceFrag = SHADER_STRING
(
 \n#ifdef GL_ES\n
 precision lowp float;
 \n#endif\n
 varying vec2 v_texCoord;
 uniform sampler2D u_texture;
 \n#ifdef EXTRA_ALPHA\n
 uniform sampler2D u_texture_alpha;
 \n#endif\n
 uniform float u_brightness;
 uniform vec4 u_colorPercent;
 uniform vec4 u_colorAmount;
 
 void main()
{
    vec4 texColor = texture2D(u_texture, v_texCoord);
    \n#ifdef EXTRA_ALPHA\n
    texColor.a = texture2D(u_texture_alpha, v_texCoord).r;
    \n#endif\n
    \n#ifdef PREMULT_ALPHA\n
    texColor = vec4(texColor.rgb * u_brightness, texColor.a) * u_colorPercent + u_colorAmount;
    gl_FragColor = vec4(texColor.rgb * texColor.a, texColor.a);
    \n#else\n
        gl_FragColor = vec4(texColor.rgb * u_brightness, texColor.a) * u_colorPercent + u_colorAmount;
    \n#endif\n
}
 );

const GLchar* shaderPositionTextureColorAdvanceVert = SHADER_STRING
(
 attribute vec4 a_position;
 attribute vec2 a_texCoord;
 
 uniform        mat4 u_MVPMatrix; //model(模型)、view(视图)、projection(投影)矩阵集合  uniform相当于常量(const)
 
 \n#ifdef GL_ES\n
 varying mediump vec2 v_texCoord;
 \n#else\n
 varying vec2 v_texCoord;
 \n#endif\n
 
 void main()
{
    gl_Position = u_MVPMatrix * a_position;
    v_texCoord = a_texCoord;
}
 );

const GLchar* shaderPositionTextureColorFrag = SHADER_STRING
(
 \n#ifdef GL_ES\n
 precision lowp float;
 \n#endif\n
 
 varying vec4 v_fragmentColor;
 varying vec2 v_texCoord;
 uniform sampler2D u_texture;
 \n#ifdef EXTRA_ALPHA\n
 uniform sampler2D u_texture_alpha;
 \n#endif\n
 uniform float u_brightness;
 
 void main()
{
    \n#ifdef EXTRA_ALPHA\n
    vec4 texColor = vec4(texture2D(u_texture, v_texCoord).rgb, texture2D(u_texture_alpha, v_texCoord).r);
    \n#else\n
        vec4 texColor = texture2D(u_texture, v_texCoord);
    \n#endif\n
    \n#ifdef PREMULT_ALPHA\n
    texColor = vec4(texColor.rgb * u_brightness, texColor.a) * v_fragmentColor;
    gl_FragColor = vec4(texColor.rgb * texColor.a, texColor.a);
    \n#else\n
        gl_FragColor = vec4(texColor.rgb * u_brightness, texColor.a) * v_fragmentColor;
    \n#endif\n
}
 );

const GLchar* shaderPositionTextureColorGrayFrag = SHADER_STRING
(
 \n#ifdef GL_ES\n
 precision lowp float;
 \n#endif\n
 
 varying vec4 v_fragmentColor;
 varying vec2 v_texCoord;
 uniform sampler2D u_texture;
 \n#ifdef EXTRA_ALPHA\n
 uniform sampler2D u_texture_alpha;
 \n#endif\n
 uniform float u_brightness;
 
 void main()
{
    \n#ifdef EXTRA_ALPHA\n
    vec4 texColor = v_fragmentColor * vec4(texture2D(u_texture, v_texCoord).rgb, texture2D(u_texture_alpha, v_texCoord).r);
    \n#else\n
        vec4 texColor = v_fragmentColor * texture2D(u_texture, v_texCoord);
    \n#endif\n
    \n#ifdef PREMULT_ALPHA\n
    float color = (0.299*texColor.r+0.587*texColor.g+0.114*texColor.b)*u_brightness*texColor.a;
    \n#else\n
        float color = (0.299*texColor.r+0.587*texColor.g+0.114*texColor.b)*u_brightness;
    \n#endif\n
    gl_FragColor = vec4( color, color, color, texColor.a);
}
 );

const GLchar* shaderPositionTextureColorAdvanceGrayFrag = SHADER_STRING
(
 \n#ifdef GL_ES\n
 precision lowp float;
 \n#endif\n
 
 varying vec2 v_texCoord;
 uniform sampler2D u_texture;
 \n#ifdef EXTRA_ALPHA\n
 uniform sampler2D u_texture_alpha;
 \n#endif\n
 uniform float u_brightness;
 uniform vec4 u_colorPercent;
 uniform vec4 u_colorAmount;
 
 void main()
{
    \n#ifdef EXTRA_ALPHA\n
    vec4 texColor = u_colorPercent * vec4(texture2D(u_texture, v_texCoord).rgb, texture2D(u_texture_alpha, v_texCoord).r) + u_colorAmount;
    \n#else\n
        vec4 texColor = u_colorPercent * texture2D(u_texture, v_texCoord) + u_colorAmount;
    \n#endif\n
    \n#ifdef PREMULT_ALPHA\n
    float color = (0.299*texColor.r+0.587*texColor.g+0.114*texColor.b)*u_brightness*texColor.a;
    \n#else\n
        float color = (0.299*texColor.r+0.587*texColor.g+0.114*texColor.b)*u_brightness;
    \n#endif\n
    gl_FragColor = vec4( color, color, color, texColor.a);
}
 );

const GLchar* shaderPositionTextureColorVert = SHADER_STRING
(
 attribute vec4 a_position;
 attribute vec2 a_texCoord;
 attribute vec4 a_color;
 
 uniform        mat4 u_MVPMatrix;
 
 \n#ifdef GL_ES\n
 varying lowp vec4 v_fragmentColor;
 varying mediump vec2 v_texCoord;
 \n#else\n
 varying vec4 v_fragmentColor;
 varying vec2 v_texCoord;
 \n#endif\n
 
 void main()
{
    gl_Position = u_MVPMatrix * a_position;
    v_fragmentColor = a_color;
    v_texCoord = a_texCoord;
}
 );

const GLchar * shaderPositionColorFrag = SHADER_STRING
(
 \n#ifdef GL_ES\n
 precision lowp float;
 \n#endif\n
 
 varying vec4 v_fragmentColor;
 
 void main()
{
    gl_FragColor = v_fragmentColor;
}
 );


const GLchar * shaderPositionColorVert = SHADER_STRING
(
 attribute vec4 a_position;
 attribute vec4 a_color;
 uniform    mat4 u_MVPMatrix;
 
 \n#ifdef GL_ES\n
 varying lowp vec4 v_fragmentColor;
 \n#else\n
 varying vec4 v_fragmentColor;
 \n#endif\n
 
 void main()
{
    gl_Position = u_MVPMatrix * a_position;
    v_fragmentColor = a_color;
}
 );

const GLchar * shaderPosition_uColorFrag = SHADER_STRING
(
 \n#ifdef GL_ES\n
 precision lowp float;
 \n#endif\n
 
 varying vec4 v_fragmentColor;
 
 void main()
{
    gl_FragColor = v_fragmentColor;
}
 );

const GLchar * shaderPosition_uColorVert = SHADER_STRING
(
 attribute vec4 a_position;
 uniform    mat4 u_MVPMatrix;
 uniform    vec4 u_color;
 uniform float u_pointSize;
 
 \n#ifdef GL_ES\n
 varying lowp vec4 v_fragmentColor;
 \n#else\n
 varying vec4 v_fragmentColor;
 \n#endif\n
 
 void main()
{
    gl_Position = u_MVPMatrix * a_position;
    gl_PointSize = u_pointSize;
    v_fragmentColor = u_color;
}
 );

const GLchar * shaderPositionTextureA8ColorFrag = SHADER_STRING
(
 \n#ifdef GL_ES\n
 precision lowp float;
 \n#endif\n
 
 varying vec4 v_fragmentColor;
 varying vec2 v_texCoord;
 uniform sampler2D u_texture;
 uniform float u_brightness;
 
 void main()
{
    gl_FragColor = vec4(v_fragmentColor.rgb * u_brightness,
                        v_fragmentColor.a * texture2D(u_texture, v_texCoord).a
                        );
}
 );

const GLchar * shaderPositionTextureColorAlphaTestFrag = SHADER_STRING
(
 \n#ifdef GL_ES\n
 precision lowp float;
 \n#endif\n
 
 varying vec4 v_fragmentColor;
 varying vec2 v_texCoord;
 uniform sampler2D u_texture;
 uniform float u_alpha_theshold;
 
 void main()
{
    vec4 texColor = texture2D(u_texture, v_texCoord);
    if (texColor.a <= u_alpha_theshold)
        discard;
    gl_FragColor = texColor * v_fragmentColor;
}
 );

const GLchar * shaderPositionTextureA8DistFrag = SHADER_STRING
(
 \n#ifdef GL_ES\n
 precision highp float;
 \n#endif\n
 
 varying vec4 v_fragmentColor;
 varying vec2 v_texCoord;
 uniform sampler2D u_texture;
 uniform float u_brightness;
 uniform float u_base;
 uniform float u_scale;
 uniform float u_borderoffset;
 uniform float u_threshold;
 
 void main()
{
    float distMask = texture2D(u_texture, v_texCoord).a * u_scale + u_base + u_borderoffset;
    float alpha = smoothstep(u_threshold, 0.0, distMask);
    gl_FragColor = vec4(v_fragmentColor.rgb * u_brightness, v_fragmentColor.a * alpha);
}                                                    \n\
 );

const GLchar * shaderPositionTextureA8DistOutlineFrag = SHADER_STRING
(
 \n#ifdef GL_ES\n
 precision highp float;
 \n#endif\n
 
 varying vec4 v_fragmentColor;
 varying vec2 v_texCoord;
 uniform sampler2D u_texture;
 uniform float u_brightness;
 uniform float u_base;
 uniform float u_scale;
 uniform vec4 u_outlinecolor;
 uniform float u_borderoffset;
 uniform float u_threshold;
 
 void main()
{
    float distMask = texture2D(u_texture, v_texCoord).a * u_scale + u_base + u_borderoffset;
    float threshold1 = u_threshold - 0.01;
    float threshold2 = u_threshold - 0.03;
    if (distMask>=0.0) {
        gl_FragColor = vec4(v_fragmentColor.rgb * u_brightness, v_fragmentColor.a);
    } else if ((distMask>=threshold2)&&(distMask<threshold1)) {
        gl_FragColor = vec4(u_outlinecolor.rgb * u_brightness, u_outlinecolor.a*smoothstep(threshold2, threshold1, distMask));
    } else if((distMask>=threshold1)&&(distMask<u_threshold)) {
        gl_FragColor = vec4(u_outlinecolor.rgb * u_brightness, u_outlinecolor.a);
    } else if ((distMask>=u_threshold)&&(distMask<0.0)) {
        vec4 color = mix(u_outlinecolor, v_fragmentColor, smoothstep(u_threshold, 0.0, distMask));
        gl_FragColor = vec4(color.rgb * u_brightness, color.a);
    } else {
        gl_FragColor = vec4(0.0);
    }
}
 );

const GLchar * shaderPositionTextureA8ColorVert = SHADER_STRING
(
 attribute vec4 a_position;
 attribute vec2 a_texCoord;
 attribute vec4 a_color;
 uniform        mat4 u_MVPMatrix;
 
 \n#ifdef GL_ES\n
 varying lowp vec4 v_fragmentColor;
 varying mediump vec2 v_texCoord;
 \n#else\n
 varying vec4 v_fragmentColor;
 varying vec2 v_texCoord;
 \n#endif\n
 
 void main()
{
    gl_Position = u_MVPMatrix * a_position;
    v_fragmentColor = a_color;
    v_texCoord = a_texCoord;
}
 );

const GLchar * shaderPositionTextureVert = SHADER_STRING
(
 attribute vec4 a_position;
 attribute vec2 a_texCoord;
 uniform    mat4 u_MVPMatrix;
 
 \n#ifdef GL_ES\n
 varying mediump vec2 v_texCoord;
 \n#else\n
 varying vec2 v_texCoord;
 \n#endif\n
 
 void main()
{
    gl_Position = u_MVPMatrix * a_position;
    v_texCoord = a_texCoord;
}
 );

const GLchar * shaderPositionTextureFrag = SHADER_STRING
(
 \n#ifdef GL_ES\n
 precision lowp float;
 \n#endif\n
 
 varying vec2 v_texCoord;
 uniform sampler2D u_texture;
 \n#ifdef EXTRA_ALPHA\n
 uniform sampler2D u_texture_alpha;
 \n#endif\n
 
 void main()
{
    vec4 texColor = texture2D(u_texture, v_texCoord);
    \n#ifdef EXTRA_ALPHA\n
    texColor.a = texture2D(u_texture_alpha, v_texCoord).r;
    \n#endif\n
    \n#ifdef PREMULT_ALPHA\n
    gl_FragColor = vec4(texColor.rgb*texColor.a, texColor.a);
    \n#else\n
        gl_FragColor = texColor;
    \n#endif\n
}
 );
NS_KK_END


#endif /* KKShaderHelper_h */
