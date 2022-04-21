//
// Created by 이연지 on 2017-06-12.
//

#include "CCallbackMacro.h"

namespace nsJavaCB{
    namespace detail {
    }

JavaVM* getJvm()
{
    return detail::g_jvm;
}

JNIEnv* getEnv()
{
    return detail::g_env;
}

jobject* getObj()
{
    return &(detail::g_obj);
}
jclass* getCls()
{
    return &(detail::g_cls);
}

CCallbackMathod* getCb()
{
    return detail::g_cb;
}

//할당 받은 jbyteArray는 쓰고 반드시 deleteLocalRef로 지워주세요!
jbyteArray convertTojByteArray( const char* str, int len, JNIEnv* env )
{
    jbyteArray arr = NULL;

    if(env == NULL || str == NULL || len <= 0 )
        return arr;

    arr = env->NewByteArray( len );
    env->SetByteArrayRegion(arr, 0, len, (jbyte *)str );

    return arr;
}

//할당 받은 jstring은 쓰고 반드시 deleteLocalRef로 지워주세요!
jstring convertTojString( const char* str, int len, JNIEnv* env )
{
    if(env == NULL || str == NULL || len <= 0 )
        return jstring();

    return env->NewStringUTF( str );
}

bool IsError(JNIEnv *env )
{
    bool isError = false;

    if( env == NULL  )
        return isError;

    isError = env->ExceptionCheck();
    env->ExceptionClear();

    return isError;
}

bool attachCurrentThread( JNIEnv*& env, JavaVM* jvm )
{
    if (jvm->AttachCurrentThread(&env, NULL) != JNI_OK)
    {
        LOGE( TAG,"Failed to attach current thread", "");
        return false;
    }

    return true;
}

bool detachCurrentThread( JavaVM* jvm )
{
    if (jvm->DetachCurrentThread() != JNI_OK)
    {
        LOGE( TAG, "Failed to detach current thread", "");
        return false;
    }

    return true;
}

bool setJavaInfo(JNIEnv *env, jobject* obj)
{
    if (env == NULL || obj == NULL)
    {
        LOGE(TAG, "Failed to set JAVA info", "" );
        return false;
    }

    if( detail::g_env != NULL || detail::g_obj != NULL || detail::g_cls != NULL || detail::g_jvm
        || detail::g_cb != NULL )
    {
        LOGE(TAG, "You have to clear global callback before reset", "");
        return false;
    }

    detail::g_env = env;

    detail::g_obj = detail::g_env->NewGlobalRef(*obj);
    if ( IsError() )
    {
        LOGE(TAG, "Failed to get object", "" );
        return false;
    }

    jclass cls = detail::g_env->GetObjectClass(detail::g_obj);
    if (IsError())
    {
        LOGE(TAG, "Failed to get JAVA class", "" );
        return false;
    }

    detail::g_cls = (jclass)env->NewGlobalRef(cls);
    if (IsError())
    {
        LOGE(TAG, "Failed to get JAVA class", "" );
        return false;
    }

    detail::g_env->DeleteLocalRef(cls);
    IsError();

    detail::g_env->GetJavaVM(&detail::g_jvm);
    if ( IsError())
    {
        LOGE(TAG, "Failed to get JavaVM", "" );
        return false;
    }

    detail::g_cb = new CCallbackMathod( detail::g_env, &detail::g_obj, &detail::g_cls );

    return true;
}

void clearWithoutDeleteRef()
{
    if( detail::g_cb != NULL )
    {
        delete detail::g_cb;
        detail::g_cb = NULL;
    }

    detail::g_jvm = NULL;

    if( detail::g_cls != NULL )
        detail::g_cls = NULL;

    if( detail::g_obj != NULL )
        detail::g_obj = NULL;

    if( detail::g_env != NULL )
        detail::g_env = NULL;
}

void clear()
{
    if( detail::g_cb != NULL )
    {
        delete detail::g_cb;
        detail::g_cb = NULL;
    }

    detail::g_jvm = NULL;

    if( detail::g_cls != NULL )
    {
        detail::g_env->DeleteGlobalRef(detail::g_cls);
        IsError();

        detail::g_cls = NULL;
    }

    if( detail::g_obj != NULL )
    {
        detail::g_env->DeleteGlobalRef(detail::g_obj);
        IsError();

        detail::g_obj = NULL;
    }
    detail::g_env = NULL;
}

CCallbackMathod::CCallbackMathod(JNIEnv *env, jobject* obj)
        : m_env(NULL), m_obj(NULL), m_cls(NULL), isLocalCls(false)
{
    if ( env == NULL || obj == NULL )
    {
        LOGE(TAG, "Failed to set JAVA info", "" );
        return;
    }
    
    m_env = env;
    m_obj = obj;

    isLocalCls = true;
    m_cls = m_env->GetObjectClass(*m_obj);
    if (IsError(m_env))
    {
        LOGE(TAG, "Failed to get JAVA class", "" );
        return;
    }
}

CCallbackMathod::CCallbackMathod(JNIEnv *env, jobject* obj, jclass* cls)
 : m_env(NULL), m_obj(NULL), m_cls(NULL), isLocalCls(false)
{
    if ( env == NULL || obj == NULL )
    {
        LOGE(TAG, "Failed to set JAVA info", "" );
        return;
    }

    m_env = env;
    m_obj = obj;

    m_cls = *cls;
    if (IsError(m_env))
    {
        LOGE(TAG, "Failed to get JAVA class", "" );
        return;
    }
}

CCallbackMathod::CCallbackMathod(JNIEnv *env, jobject* obj, bool isStatic, const char *funcName, const char *argString )
: m_env(NULL), m_obj(NULL), m_cls(NULL), isLocalCls(false)
{
    if ( env == NULL || obj == NULL || funcName == NULL || argString == NULL )
    {
        LOGE(TAG, "Failed to set JAVA info", "" );
        return;
    }

    m_env = env;
    m_obj = obj;

    isLocalCls = true;
    m_cls = m_env->GetObjectClass(*m_obj);
    if (IsError(m_env))
    {
        LOGE(TAG, "Failed to get JAVA class", "" );
        return;
    }

    addCallBack(isStatic, funcName, argString);
}

CCallbackMathod::CCallbackMathod(JNIEnv *env, jobject* obj, jclass* cls, bool isStatic, const char *funcName, const char *argString )
 : m_env(NULL), m_obj(NULL), m_cls(NULL), isLocalCls(false)
{
    if ( env == NULL || obj == NULL || funcName == NULL || argString == NULL )
    {
        LOGE(TAG, "Failed to set JAVA info", "" );
        return;
    }

    m_env = env;
    m_obj = obj;

    m_cls = *cls;
    if (IsError(m_env))
    {
        LOGE(TAG, "Failed to get JAVA class", "" );
        return;
    }

    addCallBack(isStatic, funcName, argString);
}

bool CCallbackMathod::addCallBack( bool isStatic, const char *funcName, const char *argString )
{
    if ( m_env == NULL || m_obj == NULL || m_cls == NULL || funcName == NULL || argString == NULL )
    {
        LOGE(TAG, "Failed to set JAVA info", "" );
        return false;
    }

    jmethodID methodID = NULL;

    if( isStatic )
        methodID = m_env->GetStaticMethodID(m_cls, funcName, argString);
    else
        methodID = m_env->GetMethodID(m_cls, funcName, argString);

    if (IsError(m_env))
    {
        LOGE(TAG, "Failed to get JAVA method ID funcName: %s, argString: %s, isStaticFunction: %s"
        , funcName, argString, nsCmn::boolToText(isStatic) );
        return false;
    }

    if( isStatic )
        m_mapStaticMethodID[funcName] = methodID;
    else
        m_mapMethodID[funcName] = methodID;

    LOGI(TAG, "New method funcName: %s, argString: %s, isStaticFunction: %s"
    , funcName, argString, nsCmn::boolToText(isStatic));

    return true;
}

jmethodID CCallbackMathod::getMethodID( const char* methodName )
{
    TyMapMethodID::iterator it = m_mapMethodID.find( methodName );
    if( it != m_mapMethodID.end() )
        return it->second;

    return NULL;
}

jmethodID CCallbackMathod::getStaticMethodID( const char* methodName )
{
    TyMapMethodID::iterator it = m_mapStaticMethodID.find( methodName );
    if( it != m_mapStaticMethodID.end() )
        return it->second;

    return NULL;
}

CCallbackMathod::~CCallbackMathod()
{
    clear();
}

bool CCallbackMathod::isValid()
{
    if(  m_env != NULL && m_obj != NULL && m_cls != NULL )
        return true;

    LOGE(TAG, "jni info invalid. m_env is null? %s, m_obj is null? %s, m_cls is null? %s"
    , (m_env == NULL ? "true" : "false")
    , (m_obj == NULL ? "true" : "false")
    , (m_cls == NULL ? "true" : "false"));

    return false;
}

void CCallbackMathod::clear()
{
    m_mapMethodID.clear();
    if( m_env != NULL && m_cls != NULL && isLocalCls == true )
    {
        m_env->DeleteLocalRef(m_cls);
        IsError(m_env);
        m_cls = NULL;
    }
}

void InstantCB::InstantCB_void( bool* isSuccess, bool isStatic, const char* funcName, const char* argString )
{
    CCallbackMathod cb( nsJavaCB::getEnv(), nsJavaCB::getObj(), nsJavaCB::getCls(), isStatic, funcName, argString );
    if( isStatic )
        cb.CallStaticMethodvoid( isSuccess, funcName );
    else
        cb.CallMethodvoid( isSuccess, funcName );
}

void InstantCB::InstantCB_voidWithArgs( bool* isSuccess, bool isStatic, const char *funcName, const char *argString, ... )
{
    va_list args;
    va_start(args, argString);
    CCallbackMathod cb( nsJavaCB::getEnv(), nsJavaCB::getObj(), nsJavaCB::getCls(), isStatic, funcName, argString );
    if( isStatic )
        cb.CallStaticMethodvoidWithArgs( isSuccess, funcName, &args );
    else
        cb.CallMethodvoidWithArgs( isSuccess, funcName, &args );
    va_end(args);
}

nsJavaCB::CCallbackMathod* newCallbackMethodOfCurrentThread()
{
    nsJavaCB::CCallbackMathod* cb = NULL;
    JNIEnv* env = NULL;

    if( ATTACH_CURRENT_THREAD( env ) == false )
    {
        LOGE(TAG, "Failed to attach run thread", "" );
        return NULL;
    }

    jobject* obj = new jobject( env->NewGlobalRef(*nsJavaCB::getObj()) );
    if ( nsJavaCB::IsError(env) )
    {
        LOGE(TAG, "Failed to get object", "" );
        return NULL;
    }

    cb = new nsJavaCB::CCallbackMathod( env, obj );
    if ( cb == NULL )
    {
        LOGE(TAG, "Failed to make call back class", "" );
        return NULL;
    }

    return cb;
}

void deleteCallbackMethod( nsJavaCB::CCallbackMathod* cb )
{
    if( cb == NULL )
        return;

    JNIEnv* env = cb->getEnv();
    jobject* obj = cb->getObj();

    delete cb;
    cb = NULL;

    if( env == NULL )
        return;

    if( obj != NULL ) {
        env->DeleteGlobalRef(*obj);
        nsJavaCB::IsError(env);

        delete obj;
        obj = NULL;
    }

    if( DETACH_CURRENT_THREAD == false )
        LOGE(TAG, "Failed to detach run thread", "" );

    return;
}
}