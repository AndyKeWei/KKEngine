//
//  KKDelegate.h
//  KKEngine
//
//  Created by KW on 13-1-18.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __KKDELEGATE_H__
#define __KKDELEGATE_H__

#include "KKMacros.h"
#include <iostream>
#include <list>
#include <stdint.h>

#include <typeinfo>
NS_KK_BEGIN

class IDelegateUnlink
{
public:
    virtual ~IDelegateUnlink() { }
    
    IDelegateUnlink()
    {
        m_baseDelegateUnlink = this;
    }
    
    bool compare(IDelegateUnlink* _unlink) const
    {
        return m_baseDelegateUnlink == _unlink->m_baseDelegateUnlink;
    }
    
private:
    IDelegateUnlink* m_baseDelegateUnlink;
};

inline IDelegateUnlink* getDelegateUnlink(void* _base)
{
    return 0;
}

inline IDelegateUnlink* getDelegateUnlink(IDelegateUnlink* _base)
{
    return _base;
}

#define CHECK_JS_ERROR \
if (ok == JS_FALSE)\
{   \
    if (!JS_ReportPendingException(cx)) \
    {   \
        JS_ClearPendingException(cx);   \
    }   \
}

#define HL_SUFFIX 0
#define HL_TEMPLATE
#define HL_TEMPLATE_PARAMS
#define HL_TEMPLATE_ARGS
#define HL_T_TEMPLATE_PARAMS <typename T>
#define HL_T_TEMPLATE_ARGS <T>
#define HL_PARAMS
#define HL_ARGS
#define HL_TYPENAME
#define HL_CALL_JS_FUNC \
JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext(); \
JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject()); \
jsval ret; \
JSBool ok = JS_CallFunctionValue(cx, jsobj, fval, 0, 0, &ret); \
CHECK_JS_ERROR

#include "KKDelegateImpl.h"

#define HL_SUFFIX 1
#define HL_TEMPLATE template
#define HL_TEMPLATE_PARAMS <typename TP1>
#define HL_TEMPLATE_ARGS <TP1>
#define HL_T_TEMPLATE_PARAMS <typename T, typename TP1>
#define HL_T_TEMPLATE_ARGS <T, TP1>
#define HL_PARAMS TP1 p1
#define HL_ARGS p1
#define HL_TYPENAME typename
#define HL_CALL_JS_FUNC \
JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext(); \
JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject()); \
jsval arg[1]; \
arg[0] = value_to_jsval(p1); \
jsval ret; \
JSBool ok = JS_CallFunctionValue(cx, jsobj, fval, 1, arg, &ret); \
CHECK_JS_ERROR

#include "KKDelegateImpl.h"

#define HL_SUFFIX 2
#define HL_TEMPLATE template
#define HL_TEMPLATE_PARAMS <typename TP1, typename TP2>
#define HL_TEMPLATE_ARGS <TP1, TP2>
#define HL_T_TEMPLATE_PARAMS <typename T, typename TP1, typename TP2>
#define HL_T_TEMPLATE_ARGS <T, TP1, TP2>
#define HL_PARAMS TP1 p1, TP2 p2
#define HL_ARGS p1, p2
#define HL_TYPENAME typename
#define HL_CALL_JS_FUNC \
JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext(); \
JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject()); \
jsval arg[2]; \
arg[0] = value_to_jsval(p1); \
arg[1] = value_to_jsval(p2); \
jsval ret; \
JSBool ok = JS_CallFunctionValue(cx, jsobj, fval, 2, arg, &ret); \
CHECK_JS_ERROR

#include "KKDelegateImpl.h"

#define HL_SUFFIX 3
#define HL_TEMPLATE template
#define HL_TEMPLATE_PARAMS <typename TP1, typename TP2, typename TP3>
#define HL_TEMPLATE_ARGS <TP1, TP2, TP3>
#define HL_T_TEMPLATE_PARAMS <typename T, typename TP1, typename TP2, typename TP3>
#define HL_T_TEMPLATE_ARGS <T, TP1, TP2, TP3>
#define HL_PARAMS TP1 p1, TP2 p2, TP3 p3
#define HL_ARGS p1, p2, p3
#define HL_TYPENAME typename
#define HL_CALL_JS_FUNC \
JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext(); \
JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject()); \
jsval arg[3]; \
arg[0] = value_to_jsval(p1); \
arg[1] = value_to_jsval(p2); \
arg[2] = value_to_jsval(p3); \
jsval ret; \
JSBool ok = JS_CallFunctionValue(cx, jsobj, fval, 3, arg, &ret); \
CHECK_JS_ERROR

#include "KKDelegateImpl.h"

#define HL_SUFFIX 4
#define HL_TEMPLATE template
#define HL_TEMPLATE_PARAMS <typename TP1, typename TP2, typename TP3, typename TP4>
#define HL_TEMPLATE_ARGS <TP1, TP2, TP3, TP4>
#define HL_T_TEMPLATE_PARAMS <typename T, typename TP1, typename TP2, typename TP3, typename TP4>
#define HL_T_TEMPLATE_ARGS <T, TP1, TP2, TP3, TP4>
#define HL_PARAMS TP1 p1, TP2 p2, TP3 p3, TP4 p4
#define HL_ARGS p1, p2, p3, p4
#define HL_TYPENAME typename
#define HL_CALL_JS_FUNC \
JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext(); \
JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject()); \
jsval arg[4]; \
arg[0] = value_to_jsval(p1); \
arg[1] = value_to_jsval(p2); \
arg[2] = value_to_jsval(p3); \
arg[3] = value_to_jsval(p4); \
jsval ret; \
JSBool ok = JS_CallFunctionValue(cx, jsobj, fval, 4, arg, &ret); \
CHECK_JS_ERROR

#include "KKDelegateImpl.h"

NS_KK_END

#endif
