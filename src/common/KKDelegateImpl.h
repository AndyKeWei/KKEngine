//
//  KKDelegateImpl.h
//  KKEngine
//
//  Created by KK on 13-1-18.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#define COMBINE(a, b)           COMBINE1(a, b)
#define COMBINE1(a, b)          a##b

#define I_DELEGATE              COMBINE(IDelegate, HL_SUFFIX)

#define C_STATIC_DELEGATE       COMBINE(CStaticDelegate, HL_SUFFIX)
#define C_METHOD_DELEGATE       COMBINE(CMethodDelegate, HL_SUFFIX)
#if USE_JS_SCRIPTING
#define C_JS_DELEGATE           COMBINE(CJSDelegate, HL_SUFFIX)
#endif

#define C_DELEGATE              COMBINE(CDelegate, HL_SUFFIX)
#define C_MULTI_DELEGATE        COMBINE(CMultiDelegate, HL_SUFFIX)


HL_TEMPLATE HL_TEMPLATE_PARAMS
class I_DELEGATE
{
public:
    virtual ~I_DELEGATE() { }
    virtual bool isType(const std::type_info& _type) const = 0;
    virtual void invoke(HL_PARAMS) = 0;
    virtual bool compare(I_DELEGATE HL_TEMPLATE_ARGS* _delegate) const = 0;
    virtual bool compare(IDelegateUnlink* _unlink) const
    {
        return false;
    }
};

#if USE_JS_SCRIPTING
HL_TEMPLATE HL_TEMPLATE_PARAMS
class C_JS_DELEGATE : public I_DELEGATE HL_TEMPLATE_ARGS
{
public:
    C_JS_DELEGATE(JSObject* _obj, jsval _fval):jsobj(_obj),fval(_fval)
    {
        JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
        m_jsobject = JS_NewObject(cx, NULL, NULL, NULL);
        JS_AddObjectRoot(cx, &m_jsobject);
        JS::RootedValue val(cx, OBJECT_TO_JSVAL(_obj));
        JS_SetProperty(cx, m_jsobject, "target", val);
        JS::RootedValue val1(cx, fval);
        JS_SetProperty(cx, m_jsobject, "function", val1);
    }
    
    virtual ~C_JS_DELEGATE()
    {
        JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
        if (m_jsobject)
        {
            JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject());
            JS_RemoveObjectRoot(cx, &m_jsobject);
            JS_MaybeGC(cx);
        }
    }
    
    virtual bool isType(const std::type_info& _type) const
    { return typeid(C_JS_DELEGATE HL_TEMPLATE_ARGS) == _type; }
    
    virtual void invoke(HL_PARAMS)
    {
        HL_CALL_JS_FUNC
    }
    
    virtual bool compare(I_DELEGATE HL_TEMPLATE_ARGS* _delegate) const
    {
        if (NULL == _delegate || !_delegate->isType(typeid(C_JS_DELEGATE HL_TEMPLATE_ARGS)))
        {
            return false;
        }
        C_JS_DELEGATE HL_TEMPLATE_ARGS* cast = static_cast<C_JS_DELEGATE HL_TEMPLATE_ARGS* >(_delegate);
        return cast->fval == fval;
    }
private:
    JSObject* jsobj;
    jsval fval;
    JSObject* m_jsobject;
};
#endif

HL_TEMPLATE HL_TEMPLATE_PARAMS
class C_STATIC_DELEGATE : public I_DELEGATE HL_TEMPLATE_ARGS
{
public:
    typedef void (*Func)(HL_PARAMS);
    
    C_STATIC_DELEGATE (Func _func): mFunc(_func) { }
    
    virtual bool isType(const std::type_info& _type) const
    { return typeid(C_STATIC_DELEGATE HL_TEMPLATE_ARGS) == _type; }
    
    virtual void invoke(HL_PARAMS)
    {
        mFunc(HL_ARGS);
    }
    
    virtual bool compare(I_DELEGATE HL_TEMPLATE_ARGS* _delegate) const
    {
        if (NULL == _delegate || !_delegate->isType(typeid(C_STATIC_DELEGATE HL_TEMPLATE_ARGS)))
        {
            return false;
        }
        C_STATIC_DELEGATE HL_TEMPLATE_ARGS* cast = static_cast<C_STATIC_DELEGATE HL_TEMPLATE_ARGS*>(_delegate);
        return cast->mFunc == mFunc;
    }
    
    virtual bool compare(IDelegateUnlink* _unlink) const
    {
        return false;
    }
    
private:
    Func mFunc;
};

template HL_T_TEMPLATE_PARAMS
class C_METHOD_DELEGATE : public I_DELEGATE HL_TEMPLATE_ARGS
{
public:
    typedef void (T::*Method)(HL_PARAMS);
    
    C_METHOD_DELEGATE(IDelegateUnlink* _unlink, T* _object, Method _method):mUnlink(_unlink), mObject(_object), mMethod(_method) { }
    
    virtual bool isType(const std::type_info& _type) const
    { return typeid(C_METHOD_DELEGATE HL_T_TEMPLATE_ARGS) == _type; }
    
    virtual void invoke(HL_PARAMS)
    {
        (mObject->*mMethod)(HL_ARGS);
    }
    
    virtual bool compare(I_DELEGATE HL_TEMPLATE_ARGS* _delegate) const
    {
        if (NULL == _delegate || !_delegate->isType(typeid(C_METHOD_DELEGATE HL_T_TEMPLATE_ARGS)))
        {
            return false;
        }
        C_METHOD_DELEGATE HL_T_TEMPLATE_ARGS* cast = static_cast<C_METHOD_DELEGATE HL_T_TEMPLATE_ARGS* >(_delegate);
//        if (!cast)
//            return false;
        return cast->mObject == mObject && cast->mMethod == mMethod;
    }
    
    virtual bool compare(IDelegateUnlink* _unlink) const
    {
        return mUnlink == _unlink;
    }
    
private:
    IDelegateUnlink* mUnlink;
    T* mObject;
    Method mMethod;
};

HL_TEMPLATE HL_TEMPLATE_PARAMS
inline I_DELEGATE HL_TEMPLATE_ARGS* newDelegate(void (*_func)(HL_PARAMS))
{
    return new C_STATIC_DELEGATE HL_TEMPLATE_ARGS (_func);
}

#if USE_JS_SCRIPTING
HL_TEMPLATE HL_TEMPLATE_PARAMS
inline I_DELEGATE HL_TEMPLATE_ARGS* newDelegate(JSObject* obj, jsval val)
{
    return new C_JS_DELEGATE HL_TEMPLATE_ARGS (obj, val);
}
#endif

template HL_T_TEMPLATE_PARAMS
inline I_DELEGATE HL_TEMPLATE_ARGS* newDelegate(T* _object, void (T::*_method)(HL_PARAMS))
{
    return new C_METHOD_DELEGATE HL_T_TEMPLATE_ARGS (getDelegateUnlink(_object), _object, _method);
}

HL_TEMPLATE HL_TEMPLATE_PARAMS
class COMBINE(CDelegate, HL_SUFFIX)
{
public:
    typedef I_DELEGATE HL_TEMPLATE_ARGS IDelegate;
    
    C_DELEGATE () : mDelegate(NULL) { }
    C_DELEGATE (const C_DELEGATE HL_TEMPLATE_ARGS& _event) : mDelegate(NULL)
    {
        IDelegate* del = _event.mDelegate;
        const_cast<C_DELEGATE HL_TEMPLATE_ARGS& >(_event).mDelegate = NULL;
        
        if (mDelegate != NULL && !mDelegate->compare(del))
        {
            delete mDelegate;
        }
        
        mDelegate = del;
    }
    ~C_DELEGATE ()
    {
        clear();
    }
    
    bool empty() const
    {
        return mDelegate == NULL;
    }
    
    void clear()
    {
        if (mDelegate)
        {
            delete mDelegate;
            mDelegate = NULL;
        }
    }
    
    C_DELEGATE HL_TEMPLATE_ARGS& operator=(IDelegate* _delegate)
    {
        if (mDelegate)
            delete mDelegate;
        mDelegate = _delegate;
        return *this;
    }
    
    C_DELEGATE HL_TEMPLATE_ARGS& operator=(const C_DELEGATE HL_TEMPLATE_ARGS& _event)
    {
        IDelegate* del = _event.mDelegate;
        const_cast< C_DELEGATE HL_TEMPLATE_ARGS& >(_event).mDelegate = NULL;
        
        if (mDelegate != NULL && !mDelegate->compare(del))
        {
            delete mDelegate;
        }
        
        mDelegate = del;
        
        return *this;
    }
    
    bool operator==(IDelegate* _delegate)
    {
        if (mDelegate == NULL)
            return false;
        bool res = mDelegate->compare(_delegate);
        delete _delegate;
        return res;
    }
    
    void operator()( HL_PARAMS )
    {
        if (mDelegate == NULL) return;
        mDelegate->invoke( HL_ARGS );
    }
    
private:
    IDelegate* mDelegate;
};

HL_TEMPLATE   HL_TEMPLATE_PARAMS
class C_MULTI_DELEGATE
{
public:
    typedef  I_DELEGATE HL_TEMPLATE_ARGS  IDelegate;
    typedef HL_TYPENAME std::list<IDelegate* > ListDelegate;
    typedef HL_TYPENAME ListDelegate::iterator ListDelegateIterator;
    typedef HL_TYPENAME ListDelegate::const_iterator ConstListDelegateIterator;
    
    C_MULTI_DELEGATE () { }
    ~C_MULTI_DELEGATE ()
    {
        clear();
    }
    
    bool empty() const
    {
        for (ConstListDelegateIterator iter = mListDelegates.begin(); iter != mListDelegates.end(); ++iter)
        {
            if (*iter) return false;
        }
        return true;
    }
    
    void clear()
    {
        for (ListDelegateIterator iter = mListDelegates.begin(); iter != mListDelegates.end(); ++iter)
        {
            if (*iter)
            {
                delete (*iter);
                (*iter) = NULL;
            }
        }
    }
    
    void clear(IDelegateUnlink* _unlink)
    {
        for (ListDelegateIterator iter = mListDelegates.begin(); iter != mListDelegates.end(); ++iter)
        {
            if ((*iter) && (*iter)->compare(_unlink))
            {
                delete (*iter);
                (*iter) = NULL;
            }
        }
    }
    
    C_MULTI_DELEGATE  HL_TEMPLATE_ARGS& operator+=(IDelegate* _delegate)
    {
        for (ListDelegateIterator iter = mListDelegates.begin(); iter != mListDelegates.end(); ++iter)
        {
            if ((*iter) && (*iter)->compare(_delegate))
            {
                delete _delegate;
                return *this;
            }
        }
        mListDelegates.push_back(_delegate);
        return *this;
    }
    
    C_MULTI_DELEGATE  HL_TEMPLATE_ARGS& operator-=(IDelegate* _delegate)
    {
        for (ListDelegateIterator iter = mListDelegates.begin(); iter != mListDelegates.end(); ++iter)
        {
            if ((*iter) && (*iter)->compare(_delegate))
            {
                if ((*iter) != _delegate) delete (*iter);
                (*iter) = NULL;
                break;
            }
        }
        delete _delegate;
        return *this;
    }
    
    void operator()( HL_PARAMS )
    {
        ListDelegateIterator iter = mListDelegates.begin();
        while (iter != mListDelegates.end())
        {
            if (NULL == (*iter))
            {
                iter = mListDelegates.erase(iter);
            }
            else
            {
                (*iter)->invoke( HL_ARGS );
                ++iter;
            }
        }
    }
    
    ListDelegateIterator begin()
    {
        return mListDelegates.begin();
    }
    
    ListDelegateIterator end()
    {
        return mListDelegates.end();
    }
    
    C_MULTI_DELEGATE (const C_MULTI_DELEGATE  HL_TEMPLATE_ARGS& _event)
    {
        ListDelegate del = _event.mListDelegates;
        const_cast< C_MULTI_DELEGATE  HL_TEMPLATE_ARGS& >(_event).mListDelegates.clear();
        
        safe_clear(del);
        
        mListDelegates = del;
    }
    
    C_MULTI_DELEGATE  HL_TEMPLATE_ARGS& operator=(const C_MULTI_DELEGATE  HL_TEMPLATE_ARGS& _event)
    {
        ListDelegate del = _event.mListDelegates;
        const_cast< C_MULTI_DELEGATE  HL_TEMPLATE_ARGS& >(_event).mListDelegates.clear();
        
        safe_clear(del);
        
        mListDelegates = del;
        
        return *this;
    }
    
    //use : operator += 
//    C_MULTI_DELEGATE  HL_TEMPLATE_ARGS& operator=(IDelegate* _delegate)
//    {
//        clear();
//        *this += _delegate;
//        return *this;
//    }
    
private:
    void safe_clear(ListDelegate& _delegates)
    {
        for (ListDelegateIterator iter = mListDelegates.begin(); iter != mListDelegates.end(); ++iter)
        {
            if (*iter)
            {
                IDelegate* del = (*iter);
                (*iter) = NULL;
                delete_is_not_found(del, _delegates);
            }
        }
    }
    
    void delete_is_not_found(IDelegate* _del, ListDelegate& _delegates)
    {
        for (ListDelegateIterator iter = _delegates.begin(); iter != _delegates.end(); ++iter)
        {
            if ((*iter) && (*iter)->compare(_del))
            {
                return;
            }
        }
        
        delete _del;
    }
    
private:
    ListDelegate mListDelegates;
};

#undef COMBINE 

#undef I_DELEGATE 

#undef C_STATIC_DELEGATE 
#undef C_METHOD_DELEGATE 

#undef C_DELEGATE 
#undef C_MULTI_DELEGATE 

#undef HL_SUFFIX
#undef HL_TEMPLATE
#undef HL_TEMPLATE_PARAMS
#undef HL_TEMPLATE_ARGS
#undef HL_T_TEMPLATE_PARAMS
#undef HL_T_TEMPLATE_ARGS
#undef HL_PARAMS
#undef HL_ARGS
#undef HL_TYPENAME
#undef HL_CALL_JS_FUNC
