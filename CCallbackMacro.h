//
// Created by 이연지 on 2017-06-12.
//

#ifndef AROUNDVIEW_CCALLBACKMACRO_H
#define AROUNDVIEW_CCALLBACKMACRO_H
#include <jni.h>
#include <vector>
#include <map>

#include "cmnDef.h"
#include "CLogger.h"
#include "cmnTypeDef.h"

/*
 *   jboolean Z;
 *   jbyte    B;
 *   jchar    C;
 *   jshort   S;
 *   jint     I;
 *   jlong    J;
 *   jfloat   F;
 *   jdouble  D;
 *   jobject  L;
 */

#define CB_METHOD_jobject      Object
#define CB_METHOD_bool         Boolean
#define CB_METHOD_jbyte        Byte
#define CB_METHOD_jchar        Char
#define CB_METHOD_short        Short
#define CB_METHOD_int          Int
#define CB_METHOD_jlong        Long
#define CB_METHOD_float        Float
#define CB_METHOD_double       Double
#define CB_METHOD_void         Void

/*
 * jlong형은 long long 형인 것에 주의. long형을 보내면 메모리 크기가 달라서 쓰레기값이 되어버린다.
*/

/*
 * g_ 변수들은 초기화 시의 Android스레드와 같은 스레드에서 동작하는 것을 전제로 한 글로벌 변수들이므로
 * native단에서 스레드를 새로 생성하였을 때에는 CCallbackMathod를 따로 선언해서 사용할 것.
 * 예시는 해당 파일 가장 아랫 부분에 주석처리 해 놓음.
*/

namespace nsJavaCB{

    typedef std::map< std::string, jmethodID, nsCmn::ci_less > TyMapMethodID;
    const char* const TAG = "JNICallBack";

    class CCallbackMathod;

    namespace detail{
        static JavaVM* g_jvm = NULL;
        static JNIEnv* g_env = NULL;
        static jobject g_obj = NULL;
        static jclass g_cls = NULL;
        static CCallbackMathod* g_cb = NULL;
    }

    JavaVM* getJvm();
    JNIEnv* getEnv();
    jobject* getObj();
    jclass* getCls();
    CCallbackMathod* getCb();

    //할당 받은 jbyteArray는 쓰고 반드시 deleteLocalRef로 지워주세요!
    jbyteArray convertTojByteArray( const char* str, int len, JNIEnv* env = nsJavaCB::getEnv() );
    //할당 받은 jstring은 쓰고 반드시 deleteLocalRef로 지워주세요!
    jstring convertTojString( const char* str, int len, JNIEnv* env = nsJavaCB::getEnv() );

    bool IsError( JNIEnv* env = nsJavaCB::getEnv() );

    bool attachCurrentThread( JNIEnv*& env, JavaVM* jvm = nsJavaCB::getJvm() );
    bool detachCurrentThread( JavaVM* jvm = nsJavaCB::getJvm() );

    bool setJavaInfo(JNIEnv *env, jobject* obj);
    void clear();
    void clearWithoutDeleteRef();

    template< class Ref >
    void deleteLocalRef( Ref ref, JNIEnv* env = nsJavaCB::getEnv() )
    {
        if( env == NULL || ref == NULL )
            return;

        env->DeleteLocalRef(ref);
        IsError(env);
        ref = NULL;
    };

    template< class Ref >
    void deleteGlobalRef( Ref ref, JNIEnv* env = nsJavaCB::getEnv() )
    {
        if( env == NULL || ref == NULL )
            return;

        env->DeleteGlobalRef(ref);
        IsError(env);
        ref = NULL;
    };

extern "C" {


#define REGSTER_GLOBAL_CB_FUNCTION_NONE_ARGS( RT, funcName  )                           \
    RT Call_Global_##funcName(bool* isSuccess)                                                      \
    {                                                                                           \
        RT result = CALL_GLOBAL_METHOD( RT, isSuccess, #funcName );                                    \
        return result;                                                                           \
    };
#define REGSTER_GLOBAL_CB_FUNCTION_WITH_ARGS( RT, funcName  )                           \
    RT Call_Global_##funcName(bool* isSuccess, ...)                                                    \
    {                                                                                             \
        RT result;                                                                              \
        va_list args;                                                                           \
        va_start(args, isSuccess);                                                              \
        result = CALL_GLOBAL_METHOD_ARGS( RT, isSuccess, #funcName, &args );                           \
        va_end(args);                                                                         \
        return result;                                                                           \
    };
#define REGSTER_GLOBAL_CB_FUNCTION_NONE_ARGS_NONE_RT( funcName )                              \
    void Call_Global_##funcName(bool* isSuccess)                                                     \
    {                                                                                           \
        CALL_GLOBAL_METHOD( void, isSuccess, #funcName );                                            \
    };
#define REGSTER_GLOBAL_CB_FUNCTION_WITH_ARGS_NONE_RT( funcName )                    \
    void Call_Global_##funcName(bool* isSuccess, ...)                                                  \
    {                                                                                           \
        va_list args;                                                                         \
        va_start(args, isSuccess);                                                            \
        CALL_GLOBAL_METHOD_ARGS( void, isSuccess, #funcName, &args );                    \
        va_end(args);                                                                         \
    };

#define REGSTER_GLOBAL_CB_STATIC_FUNCTION_NONE_ARGS( RT, funcName  )                \
    RT Call_Global_##funcName(bool* isSuccess)                                                     \
    {                                                                                           \
        RT result = CALL_GLOBAL_STATIC_METHOD( RT, isSuccess, #funcName );                      \
        return result;                                                                           \
    };
#define REGSTER_GLOBAL_CB_STATIC_FUNCTION_WITH_ARGS( RT, funcName  )                 \
    RT Call_Global_##funcName(bool* isSuccess, ...)                                                    \
    {                                                                                           \
        RT result;                                                              \
        va_list args;                                                           \
        va_start(args, isSuccess);                                              \
        result = CALL_GLOBAL_STATIC_METHOD_ARGS( RT, isSuccess, #funcName, &args );            \
        va_end(args);                                                                         \
        return result;                                                                           \
    };
#define REGSTER_GLOBAL_CB_STATIC_FUNCTION_NONE_ARGS_NONE_RT( funcName )                              \
    void Call_Global_##funcName(bool* isSuccess)                                                     \
    {                                                                                           \
        CALL_GLOBAL_STATIC_METHOD( void, isSuccess, #funcName );                                            \
    };
#define REGSTER_GLOBAL_CB_STATIC_FUNCTION_WITH_ARGS_NONE_RT( funcName )               \
    void Call_Global_##funcName(bool* isSuccess, ...)                                                  \
    {                                                                                           \
        va_list args;                                                                         \
        va_start(args, isSuccess);                                                            \
        CALL_GLOBAL_STATIC_METHOD_ARGS( void, isSuccess, #funcName, &args );                    \
        va_end(args);                                                                         \
    };

#define ADD_GLOBAL_CALLBACK( funcName, argString )                                                 \
    nsJavaCB::getCb()->addCallBack( false, #funcName, argString )

#define ADD_GLOBAL_STATIC_CALLBACK( funcName, argString )                                          \
    nsJavaCB::getCb()->addCallBack( true, #funcName, argString )

#define REGSTER_CB_FUNCTION_NONE_ARGS( RT, funcName  )                           \
    RT Call_##funcName( nsJavaCB::CCallbackMathod* cb, bool* isSuccess)                      \
    {                                                                                           \
        RT result = CALL_METHOD( cb, RT, isSuccess, #funcName );                                    \
        return result;                                                                           \
    };
#define REGSTER_CB_FUNCTION_WITH_ARGS( RT, funcName  )                           \
    RT Call_##funcName(nsJavaCB::CCallbackMathod* cb, bool* isSuccess, ...)                                                    \
    {                                                                                             \
        RT result;                                                                              \
        va_list args;                                                                           \
        va_start(args, isSuccess);                                                              \
        result = CALL_METHOD_ARGS( cb, RT, isSuccess, #funcName, &args );                           \
        va_end(args);                                                                         \
        return result;                                                                           \
    };
#define REGSTER_CB_FUNCTION_NONE_ARGS_NONE_RT( funcName )                              \
    void Call_##funcName(nsJavaCB::CCallbackMathod* cb, bool* isSuccess)                      \
    {                                                                                           \
        CALL_METHOD( cb, void, isSuccess, #funcName );                                            \
    };
#define REGSTER_CB_FUNCTION_WITH_ARGS_NONE_RT( funcName )                    \
    void Call_##funcName(nsJavaCB::CCallbackMathod* cb, bool* isSuccess, ...)                  \
    {                                                                                           \
        va_list args;                                                                         \
        va_start(args, isSuccess);                                                            \
        CALL_METHOD_ARGS( cb, void, isSuccess, #funcName, &args );                    \
        va_end(args);                                                                         \
    };

#define REGSTER_CB_STATIC_FUNCTION_NONE_ARGS( RT, funcName  )                \
    RT Call_##funcName(nsJavaCB::CCallbackMathod* cb, bool* isSuccess)                       \
    {                                                                                           \
        RT result = CALL_STATIC_METHOD( cb, RT, isSuccess, #funcName );                      \
        return result;                                                                           \
    };
#define REGSTER_CB_STATIC_FUNCTION_WITH_ARGS( RT, funcName  )                 \
    RT Call_##funcName(nsJavaCB::CCallbackMathod* cb, bool* isSuccess, ...)                   \
    {                                                                                           \
        RT result;                                                              \
        va_list args;                                                           \
        va_start(args, isSuccess);                                              \
        result = CALL_STATIC_METHOD_ARGS( cb, RT, isSuccess, #funcName, &args );            \
        va_end(args);                                                                         \
        return result;                                                                           \
    };
#define REGSTER_CB_STATIC_FUNCTION_NONE_ARGS_NONE_RT( funcName )                              \
    void Call_##funcName(nsJavaCB::CCallbackMathod* cb, bool* isSuccess)                       \
    {                                                                                           \
        CALL_STATIC_METHOD( cb, void, isSuccess, #funcName );                            \
    };
#define REGSTER_CB_STATIC_FUNCTION_WITH_ARGS_NONE_RT( funcName )               \
    void Call_##funcName(nsJavaCB::CCallbackMathod* cb, bool* isSuccess, ...)                 \
    {                                                                                           \
        va_list args;                                                                         \
        va_start(args, isSuccess);                                                            \
        CALL_STATIC_METHOD_ARGS( cb, void, isSuccess, #funcName, &args );                    \
        va_end(args);                                                                         \
    };

#define ADD_CALLBACK( cb, funcName, argString )                                                 \
    cb->addCallBack( false, #funcName, argString )

#define ADD_STATIC_CALLBACK( cb, funcName, argString )                                          \
    cb->addCallBack( true, #funcName, argString )

class CCallbackMathod {
public:
    CCallbackMathod(JNIEnv *env, jobject* obj);
    CCallbackMathod(JNIEnv *env, jobject* obj, jclass* cls);
    CCallbackMathod(JNIEnv *env, jobject* obj, bool isStatic, const char *funcName, const char *argString );
    CCallbackMathod(JNIEnv *env, jobject* obj, jclass* cls, bool isStatic, const char *funcName, const char *argString );
    ~CCallbackMathod();

    jmethodID getMethodID( const char* methodName );
    jmethodID getStaticMethodID( const char* methodName );

    bool addCallBack( bool isStatic, const char *funcName, const char *argString );
    bool isValid();

    JNIEnv* getEnv(){ return m_env; };
    jobject* getObj(){ return m_obj; };

    void clear();

#define DEF_CALLBACK_NONE_ARGS(RT, method)                                              \
    RT CallMethod##RT(bool* isSuccess, const char* funcName)                          \
    {                                                                                           \
        RT result;                                                                              \
        bool bAttached = false;                                                                 \
        JavaVM* jvm = getJvm();                                                                 \
        JNIEnv* env = nullptr;                                                                  \
        jint res = jvm->GetEnv((void**)&env, JNI_VERSION_1_6);                                  \
        if( res == JNI_EDETACHED )                                                              \
        {                                                                                       \
            if( jvm->AttachCurrentThread( reinterpret_cast<JNIEnv**>(&env), nullptr ) == JNI_OK ) \
                bAttached = true;                                                               \
        }                                                                                       \
        jmethodID methodID = getMethodID( funcName );                                          \
        if( isValid() == true && methodID != NULL )                                           \
        {                                                                                       \
            result = env->Call##method##Method( *m_obj, methodID );                          \
            *isSuccess = !IsError(env);                                                       \
        }                                                                                       \
        else                                                                                       \
        {                                                                                       \
          LOGE(TAG, "jni info invalid or call wroing function. funcName: %s, methodID is null? %s"    \
          , funcName, (methodID == NULL ? "true":"false"));                                       \
        }                                                                                         \
        if( bAttached )                                                                           \
            jvm->DetachCurrentThread();                                                           \
        return result;                                                                           \
    };

#define DEF_CALLBACK_WITH_ARGS(RT, method)                                                 \
    RT CallMethod##RT##WithArgs(bool* isSuccess, const char* funcName, va_list* args)  \
    {                                                                                           \
        RT result;                                                                              \
        bool bAttached = false;                                                                 \
        JavaVM* jvm = getJvm();                                                                 \
        JNIEnv* env = nullptr;                                                                  \
        jint res = jvm->GetEnv((void**)&env, JNI_VERSION_1_6);                                  \
        if( res == JNI_EDETACHED )                                                              \
        {                                                                                       \
            if( jvm->AttachCurrentThread( reinterpret_cast<JNIEnv**>(&env), nullptr ) == JNI_OK ) \
                bAttached = true;                                                               \
        }                                                                                       \
        jmethodID methodID = getMethodID( funcName );                                          \
        if( isValid() == true && methodID != NULL )                                           \
        {                                                                                       \
            result = env->Call##method##MethodV( *m_obj, methodID, *args );               \
            *isSuccess = !IsError(env);                                                        \
        }                                                                                       \
        else                                                                                       \
        {                                                                                       \
          LOGE(TAG, "jni info invalid or call wroing function. funcName: %s, methodID is null? %s"    \
          , funcName, (methodID == NULL ? "true":"false"));                                       \
        }                                                                                         \
        if( bAttached )                                                                           \
            jvm->DetachCurrentThread();                                                           \
        return result;                                                                           \
    };

#define DEF_CALLBACK(RT, method)                                                     \
    DEF_CALLBACK_NONE_ARGS(RT, method)                                              \
    DEF_CALLBACK_WITH_ARGS(RT, method)

    DEF_CALLBACK(jobject, CB_METHOD_jobject)
    DEF_CALLBACK(bool, CB_METHOD_bool)
    DEF_CALLBACK(jbyte, CB_METHOD_jbyte)
    DEF_CALLBACK(jchar, CB_METHOD_jchar)
    DEF_CALLBACK(short, CB_METHOD_short)
    DEF_CALLBACK(int, CB_METHOD_int)
    DEF_CALLBACK(jlong, CB_METHOD_jlong)
    DEF_CALLBACK(float, CB_METHOD_float)
    DEF_CALLBACK(double, CB_METHOD_double)

    void CallMethodvoid(bool* isSuccess, const char* funcName)
    {
        bool bAttached = false;
        JavaVM* jvm = getJvm();
        JNIEnv* env = nullptr;
        jint res = jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
        if( res == JNI_EDETACHED )
        {
            if( jvm->AttachCurrentThread( reinterpret_cast<JNIEnv**>(&env), nullptr ) == JNI_OK )
                bAttached = true;
        }
        jmethodID methodID = getMethodID( funcName );
        if( isValid() == true && methodID != NULL )
        {
            env->CallVoidMethod(*m_obj, methodID);
            *isSuccess = !IsError(env);
        }
        else
        {
          LOGE(TAG, "jni info invalid or call wroing function. funcName: %s, methodID is null? %s"
          , funcName, (methodID == NULL ? "true":"false"));
        }
        if( bAttached )
            jvm->DetachCurrentThread();
    }

    void CallMethodvoidWithArgs(bool* isSuccess, const char* funcName, va_list* args)
    {
        bool bAttached = false;
        JavaVM* jvm = getJvm();
        JNIEnv* env = nullptr;
        jint res = jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
        if( res == JNI_EDETACHED )
        {
            if( jvm->AttachCurrentThread( reinterpret_cast<JNIEnv**>(&env), nullptr ) == JNI_OK )
                bAttached = true;
        }
        jmethodID methodID = getMethodID( funcName );
        if (isValid() == true && methodID != NULL )
        {
            env->CallVoidMethodV(*m_obj, methodID, *args);
            *isSuccess = !IsError(env);
        }
        else
        {
            LOGE(TAG, "jni info invalid or call wroing function. funcName: %s, methodID is null? %s"
            , funcName, (methodID == NULL ? "true":"false"));
        }
        if( bAttached )
            jvm->DetachCurrentThread();
    }

#define DEF_STATIC_CALLBACK_NONE_ARGS(RT, method)                                              \
    RT CallStaticMethod##RT(bool* isSuccess, const char* funcName)                          \
    {                                                                                           \
        RT result;                                                                              \
        bool bAttached = false;                                                                 \
        JavaVM* jvm = getJvm();                                                                 \
        JNIEnv* env = nullptr;                                                                  \
        jint res = jvm->GetEnv((void**)&env, JNI_VERSION_1_6);                                  \
        if( res == JNI_EDETACHED )                                                              \
        {                                                                                       \
            if( jvm->AttachCurrentThread( reinterpret_cast<JNIEnv**>(&env), nullptr ) == JNI_OK ) \
                bAttached = true;                                                               \
        }                                                                                       \
        jmethodID methodID = getStaticMethodID( funcName );                                     \
        if( isValid() == true && methodID != NULL )                                           \
        {                                                                                       \
            result = env->CallStatic##method##Method( m_cls, methodID );                      \
            *isSuccess = !IsError(env);                                                            \
        }                                                                                       \
        else                                                                                       \
        {                                                                                       \
          LOGE(TAG, "jni info invalid or call wroing function. funcName: %s, methodID is null? %s"    \
          , funcName, (methodID == NULL ? "true":"false"));                                       \
        }                                                                                         \
        if( bAttached )                                                                           \
            jvm->DetachCurrentThread();                                                           \
        return result;                                                                           \
    };

#define DEF_STATIC_CALLBACK_WITH_ARGS(RT, method)                                                 \
    RT CallStaticMethod##RT##WithArgs(bool* isSuccess, const char* funcName, va_list* args)  \
    {                                                                                           \
        RT result;                                                                              \
        bool bAttached = false;                                                                 \
        JavaVM* jvm = getJvm();                                                                 \
        JNIEnv* env = nullptr;                                                                  \
        jint res = jvm->GetEnv((void**)&env, JNI_VERSION_1_6);                                  \
        if( res == JNI_EDETACHED )                                                              \
        {                                                                                       \
            if( jvm->AttachCurrentThread( reinterpret_cast<JNIEnv**>(&env), nullptr ) == JNI_OK ) \
                bAttached = true;                                                               \
        }                                                                                       \
        jmethodID methodID = getStaticMethodID( funcName );                                          \
        if( isValid() == true && methodID != NULL )                                           \
        {                                                                                       \
            result = env->CallStatic##method##MethodV( m_cls, methodID, *args );               \
            *isSuccess = !IsError(env);                                                            \
        }                                                                                       \
        else                                                                                       \
        {                                                                                       \
          LOGE(TAG, "jni info invalid or call wroing function. funcName: %s, methodID is null? %s"    \
          , funcName, (methodID == NULL ? "true":"false"));                                       \
        }                                                                                         \
        if( bAttached )                                                                           \
            jvm->DetachCurrentThread();                                                           \
        return result;                                                                           \
    };

#define DEF_STATIC_CALLBACK(RT, method)                                                     \
    DEF_STATIC_CALLBACK_NONE_ARGS(RT, method)                                              \
    DEF_STATIC_CALLBACK_WITH_ARGS(RT, method)

    DEF_STATIC_CALLBACK(jobject, CB_METHOD_jobject)
    DEF_STATIC_CALLBACK(bool, CB_METHOD_bool)
    DEF_STATIC_CALLBACK(jbyte, CB_METHOD_jbyte)
    DEF_STATIC_CALLBACK(jchar, CB_METHOD_jchar)
    DEF_STATIC_CALLBACK(short, CB_METHOD_short)
    DEF_STATIC_CALLBACK(int, CB_METHOD_int)
    DEF_STATIC_CALLBACK(jlong, CB_METHOD_jlong)
    DEF_STATIC_CALLBACK(float, CB_METHOD_float)
    DEF_STATIC_CALLBACK(double, CB_METHOD_double)

    void CallStaticMethodvoid(bool* isSuccess, const char* funcName)
    {
        bool bAttached = false;
        JavaVM* jvm = getJvm();
        JNIEnv* env = nullptr;
        jint res = jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
        if( res == JNI_EDETACHED )
        {
            if( jvm->AttachCurrentThread( reinterpret_cast<JNIEnv**>(&env), nullptr ) == JNI_OK )
                bAttached = true;
        }
        jmethodID methodID = getStaticMethodID( funcName );
        if( isValid() == true && methodID != NULL )
        {
            env->CallStaticVoidMethod(m_cls, methodID);
            *isSuccess = !IsError(env);
        }
        else
        {
            LOGE(TAG, "jni info invalid or call wroing function. funcName: %s, methodID is null? %s"
            , funcName, (methodID == NULL ? "true":"false"));
        }
        if( bAttached )
            jvm->DetachCurrentThread();
    }

    void CallStaticMethodvoidWithArgs(bool* isSuccess, const char* funcName, va_list* args)
    {
        bool bAttached = false;
        JavaVM* jvm = getJvm();
        JNIEnv* env = nullptr;
        jint res = jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
        if( res == JNI_EDETACHED )
        {
            if( jvm->AttachCurrentThread( reinterpret_cast<JNIEnv**>(&env), nullptr ) == JNI_OK )
                bAttached = true;
        }
        jmethodID methodID = getStaticMethodID( funcName );
        if (isValid() == true && methodID != NULL )
        {
            env->CallStaticVoidMethodV(m_cls, methodID, *args);
            *isSuccess = !IsError(env);
        }
        else
        {
            LOGE(TAG, "jni info invalid or call wroing function. funcName: %s, methodID is null? %s"
            , funcName, (methodID == NULL ? "true":"false"));
        }
        if( bAttached )
            jvm->DetachCurrentThread();
    }
private:

    JNIEnv* m_env;
    jobject* m_obj;
    jclass m_cls;

    bool isLocalCls;

    TyMapMethodID m_mapMethodID;
    TyMapMethodID m_mapStaticMethodID;
};

}

class InstantCB       //call back
{
public:

#define DEF_INSTANT_CALL_BACK_NONE_ARGS( RT )                            \
    static RT InstantCB_##RT( bool* isSuccess, bool isStatic, const char* funcName, const char* argString ) \
    {                                                                           \
        RT result;                                                              \
        CCallbackMathod cb( nsJavaCB::getEnv(), nsJavaCB::getObj(), nsJavaCB::getCls(), isStatic, funcName, argString ); \
        if( isStatic )                                                                      \
            result = cb.CallStaticMethod##RT( isSuccess, funcName );                              \
        else                                                                                \
            result = cb.CallMethod##RT( isSuccess, funcName );                       \
        return result;                                                                      \
    };

#define DEF_INSTANT_CALL_BACK_WITH_ARGS( RT )                               \
    static RT InstantCB_##RT##WithArgs( bool* isSuccess, bool isStatic, const char *funcName, const char *argString, ... )  \
    {                                                                           \
        RT result;                                                              \
        va_list args;                                                           \
        va_start(args, argString);                                              \
        CCallbackMathod cb( nsJavaCB::getEnv(), nsJavaCB::getObj(), nsJavaCB::getCls(), isStatic, funcName, argString );  \
        if( isStatic )                                                                      \
            result = cb.CallStaticMethod##RT##WithArgs( isSuccess, funcName, &args );           \
        else                                                                                \
            result = cb.CallMethod##RT##WithArgs( isSuccess, funcName, &args );           \
        va_end(args);                                                                           \
        return result;                                                                          \
    };

#define DEF_INSTANT_CALL_BACK(RT)                                         \
    DEF_INSTANT_CALL_BACK_NONE_ARGS(RT)                                   \
    DEF_INSTANT_CALL_BACK_WITH_ARGS(RT)

    DEF_INSTANT_CALL_BACK(jobject)
    DEF_INSTANT_CALL_BACK(bool)
    DEF_INSTANT_CALL_BACK(jbyte)
    DEF_INSTANT_CALL_BACK(jchar)
    DEF_INSTANT_CALL_BACK(short)
    DEF_INSTANT_CALL_BACK(int)
    DEF_INSTANT_CALL_BACK(jlong)
    DEF_INSTANT_CALL_BACK(float)
    DEF_INSTANT_CALL_BACK(double)

    static void InstantCB_void( bool* isSuccess, bool isStatic, const char* funcName, const char* argString );
    static void InstantCB_voidWithArgs( bool* isSuccess, bool isStatic, const char *funcName, const char *argString,  ... );
};

// 한 번만 쓰는 경우. 전역 g_cb에 등록되지 않음!
#define INST_CALL_METHOD( RT, isSuccess, funcName, argString )                      \
    nsJavaCB::InstantCB::InstantCB_##RT( isSuccess, false, funcName, argString )

#define INST_CALL_METHOD_ARGS( RT, isSuccess, funcName, argString, ... )           \
    nsJavaCB::InstantCB::InstantCB_##RT##WithArgs( isSuccess, false, funcName, argString, __VA_ARGS__ )

#define INST_CALL_STATIC_METHOD( RT, isSuccess, funcName, argString )                      \
    nsJavaCB::InstantCB::InstantCB##RT( isSuccess, true, funcName, argString )

#define INST_CALL_STATIC_METHOD_ARGS( RT, isSuccess, funcName, argString, ... )           \
    nsJavaCB::InstantCB::InstantCB##RT##WithArgs( isSuccess, true, funcName, argString, __VA_ARGS__ )
/*
 * 전역 g_cb에 등록 된 것을 사용한다. ADD_CALLBACK으로 등록 한 것만 사용 가능.
 * REGSTER_CB_FUNCTION_NONE_ARGS / REGSTER_CB_FUNCTION_WITH_ARGS
 * / REGSTER_CB_FUNCTION_NONE_ARGS_NONE_RT / REGSTER_CB_FUNCTION_WITH_ARGS_NONE_RT
 * 중 알맞는 것으로 함수 등록을 해주면 Call_FunctionName( &isSuccess, args ) 의 형태로 간단하게 사용 가능.
*/
#define CALL_GLOBAL_METHOD( RT, isSuccess, funcName )                      \
    nsJavaCB::getCb()->CallMethod##RT( isSuccess, funcName )

#define CALL_GLOBAL_METHOD_ARGS( RT, isSuccess, funcName, args )           \
    nsJavaCB::getCb()->CallMethod##RT##WithArgs( isSuccess, funcName, args )

#define CALL_GLOBAL_STATIC_METHOD( RT, isSuccess, funcName )                      \
    nsJavaCB::getCb()->CallStaticMethod##RT( isSuccess, funcName )

#define CALL_GLOBAL_STATIC_METHOD_ARGS( RT, isSuccess, funcName, args )           \
    nsJavaCB::getCb()->CallStaticMethod##RT##WithArgs( isSuccess, funcName, args )

#define CALL_METHOD( cb, RT, isSuccess, funcName )                      \
    cb->CallMethod##RT( isSuccess, funcName )

#define CALL_METHOD_ARGS( cb, RT, isSuccess, funcName, args )           \
    cb->CallMethod##RT##WithArgs( isSuccess, funcName, args )

#define CALL_STATIC_METHOD( cb, RT, isSuccess, funcName )                      \
    cb->CallStaticMethod##RT( isSuccess, funcName )

#define CALL_STATIC_METHOD_ARGS( cb, RT, isSuccess, funcName, args )           \
    cb->CallStaticMethod##RT##WithArgs( isSuccess, funcName, args )

#define GET_NEW_CALLBACKMATHOD                                                  \
    nsJavaCB::newCallbackMethodOfCurrentThread()
#define DELETE_CALLBACKMATHOD( cb )                                                  \
    nsJavaCB::deleteCallbackMethod( cb )

#define ATTACH_CURRENT_THREAD(env)                                                   \
    nsJavaCB::attachCurrentThread( env, nsJavaCB::getJvm() )

#define DETACH_CURRENT_THREAD                                                          \
    nsJavaCB::detachCurrentThread( nsJavaCB::getJvm() )

nsJavaCB::CCallbackMathod* newCallbackMethodOfCurrentThread();
void deleteCallbackMethod( nsJavaCB::CCallbackMathod* cb );

#define FIND_CLASS_FAILED_BREAK( env, class, classPath, errMsg, ... )   \
env->FindClass( classPath ); \
if( class == nullptr ) \
{ \
    LOGE( TAG, errMsg, __VA_ARGS__ ); \
    break; \
}

#define GET_OBJ_METHOD_FAILED_BREAK( env, method, class, funcName, argString, errMsg, ... )   \
env->GetMethodID( class, #funcName, argString ); \
if( method == nullptr ) \
{ \
    LOGE( TAG, errMsg, __VA_ARGS__ ); \
    break; \
}
}

#endif //AROUNDVIEW_CCALLBACKMACRO_H





//void AroundView::startAroundViewThread( AroundView* pAroundView )
//{
//    JNIEnv* env = NULL;
//
//    //native단에서 생성한 스레드이므로 java vm에 등록이 필요함.
//    if( ATTACH_CURRENT_THREAD(env) == false )
//    {
//        LOGE(TAG, "Failed to attach run thread", "" );
//        m_isStop = true;
//        return;
//    }
//
//    jobject obj = env->NewGlobalRef(*nsJavaCB::getObj());
//    if ( nsJavaCB::IsError(env) )
//    {
//        LOGE(TAG, "Failed to get object", "" );
//        m_isStop = true;
//        return;
//    }
//
//    nsJavaCB::CCallbackMathod cb ( env, &obj );
//    cb.addCallBack(false, JAVA_FUNC_DISPLAYIMAGE, "()V");
//
//    pAroundView->run( &cb );
//
//    cb.clear();
//
//    env->DeleteGlobalRef( obj );
//    nsJavaCB::IsError(env);
//
//    if( DETACH_CURRENT_THREAD == false )
//        LOGE(TAG, "Failed to detach run thread", "" );
//
//    LOGI( TAG, "Around view thread finished", "" );
//}