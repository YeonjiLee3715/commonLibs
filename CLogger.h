/*
 * CLogger.h
 *
 *  Created on: 2017. 5. 19.
 *      Author: 이연지
 */

#ifndef CLOGGER_H_
#define CLOGGER_H_

#include <iostream>
#include <string>
#include <sys/types.h>

#if defined( __linux__ ) || defined( __APPLE__ )
#include <unistd.h>
#include <sys/syscall.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <Windows.h>       // Or something like it.
#include <process.h>
#endif

#include <time.h>

#include <cmnDef.h>

#if defined(__JNI_DEBUG) || defined(__DEBUG)
#include "CStrManager.h"
#endif

#if defined( __QT_DEBUG )
#include<QtDebug>
#include<QString>
#else
#endif

#define LOG_LV_UNKNOWN    0
#define LOG_LV_DEFAULT    1
#define LOG_LV_VERBOSE    2
#define LOG_LV_DEBUG      3
#define LOG_LV_INFO       4
#define LOG_LV_WARNING    5
#define LOG_LV_ERROR      6
#define LOG_LV_FATAL      7
#define LOG_LV_SILENT     8

namespace nsCmn{

namespace nsCmnLogger {
        const std::string getLogLevelTag( char logLevel );
        const std::string currentDateTime();
#if defined(__JNI_DEBUG)
        const std::string addLogInfo( const char* function, int line, char logLevel, const char* format );
#elif defined(__DEBUG)
        template<typename ... Args>
        void printLog( const std::string& dateTime, long int pid, long int tid, char logLevel, const char* function, int line, const char* caller, const std::string& format )
        {
            std::cout << dateTime << " PID: " << pid << ", TID: " << tid << ", " << caller << ": " << function << "(" << line << "): " << getLogLevelTag( logLevel ).c_str() << ", " << format.c_str() << std::endl;
        }
#elif defined( __QT_DEBUG )
        void printQDebugLog( const std::string& dateTime, long int pid, long int tid, char logLevel, const char* caller, const char* function, int liner, QString format );

        void printQInfoLog( const std::string& dateTime, long int pid, long int tid, char logLevel, const char* caller, const char* function, int line, QString format );
        void printQWarningLog( const std::string& dateTime, long int pid, long int tid, char logLevel, const char* caller, const char* function, int line, QString format );

        void printQCriticalLog( const std::string& dateTime, long int pid, long int tid, char logLevel, const char* caller, const char* function, int line, QString format );
        void printQFatalLog( const std::string& dateTime, long int pid, long int tid, char logLevel, const char* caller, const char* function, int line, QString format );

#endif
    }
}

#if defined( __JNI_DEBUG )
#include <android/log.h>
#if LOG_LV <= LOG_LV_UNKNOWN
#define LOGUNK(caller, format, ...) \
    __android_log_print(LOG_LV_UNKNOWN, caller, nsCmn::nsCmnLogger::addLogInfo( __FUNCTION__, __LINE__, LOG_LV_UNKNOWN, format ).c_str(), __VA_ARGS__)
#else
#define LOGUNK(caller, format, ...)
#endif // end LOG_LV <= LOG_LV_UNKNOWN
#if LOG_LV <= LOG_LV_DEFAULT
#define LOGDEF(caller, format, ...) \
    __android_log_print(LOG_LV_DEFAULT, caller, nsCmn::nsCmnLogger::addLogInfo( __FUNCTION__, __LINE__, LOG_LV_DEFAULT, format ).c_str(), __VA_ARGS__)
#else
#define LOGDEF(caller, format, ...)
#endif // end LOG_LV <= LOG_LV_DEFAULT
#if LOG_LV <= LOG_LV_VERBOSE
#define LOGV(caller, format, ...) \
    __android_log_print(LOG_LV_VERBOSE, caller, nsCmn::nsCmnLogger::addLogInfo( __FUNCTION__, __LINE__, LOG_LV_VERBOSE, format ).c_str(), __VA_ARGS__ )
#else
#define LOGV(caller, format, ...)
#endif // end LOG_LV <= LOG_LV_VERBOSE
#if LOG_LV <= LOG_LV_DEBUG
#define LOGD(caller, format, ...) \
    __android_log_print(LOG_LV_DEBUG, caller, nsCmn::nsCmnLogger::addLogInfo( __FUNCTION__, __LINE__, LOG_LV_DEBUG, format ).c_str(), __VA_ARGS__ )
#else
#define LOGD(caller, format, ...)
#endif // end LOG_LV <= LOG_LV_DEBUG
#if LOG_LV <= LOG_LV_INFO
#define LOGI(caller, format, ...) \
    __android_log_print(LOG_LV_INFO, caller, nsCmn::nsCmnLogger::addLogInfo( __FUNCTION__, __LINE__, LOG_LV_INFO, format ).c_str(), __VA_ARGS__ )
#else
#define LOGI(caller, format, ...)
#endif // end LOG_LV <= LOG_LV_INFO
#if LOG_LV <= LOG_LV_WARNING
#define LOGW(caller, format, ...) \
    __android_log_print(LOG_LV_WARNING, caller, nsCmn::nsCmnLogger::addLogInfo( __FUNCTION__, __LINE__, LOG_LV_WARNING, format ).c_str(), __VA_ARGS__ )
#else
#define LOGW(caller, format, ...)
#endif // end LOG_LV <= LOG_LV_WARNING
#if LOG_LV <= LOG_LV_ERROR
#define LOGE(caller, format, ...) \
    __android_log_print(LOG_LV_ERROR, caller, nsCmn::nsCmnLogger::addLogInfo( __FUNCTION__, __LINE__, LOG_LV_ERROR, format ).c_str(), __VA_ARGS__ )
#else
#define LOGE(caller, format, ...)
#endif // end LOG_LV <= LOG_LV_ERROR
#if LOG_LV <= LOG_LV_FATAL
#define LOGF(caller, format, ...) \
    __android_log_print(LOG_LV_FATAL, caller, nsCmn::nsCmnLogger::addLogInfo( __FUNCTION__, __LINE__, LOG_LV_FATAL, format ).c_str(), __VA_ARGS__ )
#else
#define LOGF(caller, format, ...)
#endif // LOG_LV <= LOG_LV_FATAL
#define LOGS(caller, format, ...)
#elif defined(__DEBUG)
#if defined( __linux__ ) || defined( __APPLE__ )
#if LOG_LV <= LOG_LV_UNKNOWN
#define LOGUNK( caller, format, ...) \
    nsCmn::nsCmnLogger::printLog( nsCmn::nsCmnLogger::currentDateTime(), getpid(), syscall(SYS_gettid), LOG_LV_UNKNOWN, __FUNCTION__, __LINE__, caller, nsCmn::string_format( format, ##__VA_ARGS__ ).c_str() )
#else
#define LOGUNK( caller, format, ...)
#endif // end LOG_LV <= LOG_LV_UNKNOWN
#if LOG_LV <= LOG_LV_DEFAULT
#define LOGDEF( caller, format, ...) \
    nsCmn::nsCmnLogger::printLog( nsCmn::nsCmnLogger::currentDateTime(), getpid(), syscall(SYS_gettid), LOG_LV_DEFAULT, __FUNCTION__, __LINE__, caller, nsCmn::string_format( format, ##__VA_ARGS__ ).c_str() )
#else
#define LOGDEF( caller, format, ...)
#endif // end LOG_LV <= LOG_LV_DEFAULT
#if LOG_LV <= LOG_LV_VERBOSE
#define LOGV( caller, format, ...) \
    nsCmn::nsCmnLogger::printLog( nsCmn::nsCmnLogger::currentDateTime(), getpid(), syscall(SYS_gettid), LOG_LV_VERBOSE, __FUNCTION__, __LINE__, caller, nsCmn::string_format( format, ##__VA_ARGS__ ).c_str() )
#else
#define LOGV( caller, format, ...)
#endif // end LOG_LV <= LOG_LV_VERBOSE
#if LOG_LV <= LOG_LV_DEBUG
#define LOGD( caller, format, ...) \
    nsCmn::nsCmnLogger::printLog( nsCmn::nsCmnLogger::currentDateTime(), getpid(), syscall(SYS_gettid), LOG_LV_DEBUG, __FUNCTION__, __LINE__, caller, nsCmn::string_format( format, ##__VA_ARGS__ ).c_str() )
#else
#define LOGD( caller, format, ...)
#endif // end LOG_LV <= LOG_LV_DEBUG
#if LOG_LV <= LOG_LV_INFO
#define LOGI( caller, format, ...) \
    nsCmn::nsCmnLogger::printLog( nsCmn::nsCmnLogger::currentDateTime(), getpid(), syscall(SYS_gettid), LOG_LV_INFO, __FUNCTION__, __LINE__, caller, nsCmn::string_format( format, ##__VA_ARGS__ ).c_str() )
#else
#define LOGI( caller, format, ...)
#endif // end LOG_LV <= LOG_LV_INFO
#if LOG_LV <= LOG_LV_WARNING
#define LOGW( caller, format, ...) \
    nsCmn::nsCmnLogger::printLog( nsCmn::nsCmnLogger::currentDateTime(), getpid(), syscall(SYS_gettid), LOG_LV_WARNING, __FUNCTION__, __LINE__, caller, nsCmn::string_format( format, ##__VA_ARGS__ ).c_str() )
#else
#define LOGW( caller, format, ...)
#endif // end LOG_LV <= LOG_LV_WARNING
#if LOG_LV <= LOG_LV_ERROR
#define LOGE( caller, format, ...) \
    nsCmn::nsCmnLogger::printLog( nsCmn::nsCmnLogger::currentDateTime(), getpid(), syscall(SYS_gettid), LOG_LV_ERROR, __FUNCTION__, __LINE__, caller, nsCmn::string_format( format, ##__VA_ARGS__ ).c_str() )
#else
#define LOGE( caller, format, ...)
#endif // end LOG_LV <= LOG_LV_ERROR
#if LOG_LV <= LOG_LV_FATAL
#define LOGF( caller, format, ...) \
    nsCmn::nsCmnLogger::printLog( nsCmn::nsCmnLogger::currentDateTime(), getpid(), syscall(SYS_gettid), LOG_LV_FATAL, __FUNCTION__, __LINE__, caller, nsCmn::string_format( format, ##__VA_ARGS__ ).c_str() )
#else
#define LOGF( caller, format, ...)
#endif // LOG_LV <= LOG_LV_FATAL
#define LOGS( caller, format, ...)
#elif defined(_WIN32) || defined(_WIN64)
#if LOG_LV <= LOG_LV_UNKNOWN
#define LOGUNK( caller, format, ...) \
    nsCmn::nsCmnLogger::printLog( nsCmn::nsCmnLogger::currentDateTime(), static_cast<long int>(_getpid()), static_cast<long int>(GetCurrentThreadId()), LOG_LV_UNKNOWN, __FUNCTION__, __LINE__, caller, nsCmn::string_format( format, ##__VA_ARGS__ ).c_str() )
#else
#define LOGUNK( caller, format, ...)
#endif // end LOG_LV <= LOG_LV_UNKNOWN
#if LOG_LV <= LOG_LV_DEFAULT
#define LOGDEF( caller, format, ...) \
    nsCmn::nsCmnLogger::printLog( nsCmn::nsCmnLogger::currentDateTime(), static_cast<long int>(_getpid()), static_cast<long int>(GetCurrentThreadId()), LOG_LV_DEFAULT, __FUNCTION__, __LINE__, caller, nsCmn::string_format( format, ##__VA_ARGS__ ).c_str() )
#else
#define LOGDEF( caller, format, ...)
#endif // end LOG_LV <= LOG_LV_DEFAULT
#if LOG_LV <= LOG_LV_VERBOSE
#define LOGV( caller, format, ...) \
    nsCmn::nsCmnLogger::printLog( nsCmn::nsCmnLogger::currentDateTime(), static_cast<long int>(_getpid()), static_cast<long int>(GetCurrentThreadId()), LOG_LV_VERBOSE, __FUNCTION__, __LINE__, caller, nsCmn::string_format( format, ##__VA_ARGS__ ).c_str() )
#else
#define LOGV( caller, format, ...)
#endif // end LOG_LV <= LOG_LV_VERBOSE
#if LOG_LV <= LOG_LV_DEBUG
#define LOGD( caller, format, ...) \
    nsCmn::nsCmnLogger::printLog( nsCmn::nsCmnLogger::currentDateTime(), static_cast<long int>(_getpid()), static_cast<long int>(GetCurrentThreadId()), LOG_LV_DEBUG, __FUNCTION__, __LINE__, caller, nsCmn::string_format( format, ##__VA_ARGS__ ).c_str() )
#else
#define LOGD( caller, format, ...)
#endif // end LOG_LV <= LOG_LV_DEBUG
#if LOG_LV <= LOG_LV_INFO
#define LOGI( caller, format, ...) \
    nsCmn::nsCmnLogger::printLog( nsCmn::nsCmnLogger::currentDateTime(), static_cast<long int>(_getpid()), static_cast<long int>(GetCurrentThreadId()), LOG_LV_INFO, __FUNCTION__, __LINE__, caller, nsCmn::string_format( format, ##__VA_ARGS__ ).c_str() )
#else
#define LOGI( caller, format, ...)
#endif  // end LOG_LV <= LOG_LV_INFO
#if LOG_LV <= LOG_LV_WARNING
#define LOGW( caller, format, ...) \
    nsCmn::nsCmnLogger::printLog( nsCmn::nsCmnLogger::currentDateTime(), static_cast<long int>(_getpid()), static_cast<long int>(GetCurrentThreadId()), LOG_LV_WARNING, __FUNCTION__, __LINE__, caller, nsCmn::string_format( format, ##__VA_ARGS__ ).c_str() )
#else
#define LOGW( caller, format, ...)
#endif // end LOG_LV <= LOG_LV_WARNING
#if LOG_LV <= LOG_LV_ERROR
#define LOGE( caller, format, ...) \
    nsCmn::nsCmnLogger::printLog( nsCmn::nsCmnLogger::currentDateTime(), static_cast<long int>(_getpid()), static_cast<long int>(GetCurrentThreadId()), LOG_LV_ERROR, __FUNCTION__, __LINE__, caller, nsCmn::string_format( format, ##__VA_ARGS__ ).c_str() )
#else
#define LOGE( caller, format, ...)
#endif // end LOG_LV <= LOG_LV_ERROR
#if LOG_LV <= LOG_LV_FATAL
#define LOGF( caller, format, ...) \
    nsCmn::nsCmnLogger::printLog( nsCmn::nsCmnLogger::currentDateTime(), static_cast<long int>(_getpid()), static_cast<long int>(GetCurrentThreadId()), LOG_LV_FATAL, __FUNCTION__, __LINE__, caller, nsCmn::string_format( format, ##__VA_ARGS__ ).c_str() )
#else
#define LOGF( caller, format, ...)
#endif // LOG_LV <= LOG_LV_FATAL
#define LOGS( caller, format, ...)
#endif // end defined( __linux__ ) || defined( __APPLE__ )
#elif defined( __QT_DEBUG )
#if defined( __linux__ ) || defined( __APPLE__ )
#if LOG_LV <= LOG_LV_VERBOSE
#define LOGV( caller, format, ... ) \
    nsCmn::nsCmnLogger::printQDebugLog( nsCmn::nsCmnLogger::currentDateTime(), getpid(), syscall(SYS_gettid), LOG_LV_VERBOSE, caller, __FUNCTION__, __LINE__, QString::asprintf( format, ##__VA_ARGS__ ) )
#else
#define LOGV( caller, format, ... )
#endif // end LOG_LV <= LOG_LV_VERBOSE
#if LOG_LV <= LOG_LV_DEBUG
#define LOGD( caller, format, ... ) \
    nsCmn::nsCmnLogger::printQDebugLog( nsCmn::nsCmnLogger::currentDateTime(), getpid(), syscall(SYS_gettid), LOG_LV_DEBUG, caller, __FUNCTION__, __LINE__, QString::asprintf( format, ##__VA_ARGS__ ) )
#else
#define LOGD( caller, format, ... )
#endif // end LOG_LV <= LOG_LV_DEBUG
#if LOG_LV <= LOG_LV_INFO
#define LOGI( caller, format, ... ) \
    nsCmn::nsCmnLogger::printQInfoLog( nsCmn::nsCmnLogger::currentDateTime(), getpid(), syscall(SYS_gettid), LOG_LV_INFO, caller, __FUNCTION__, __LINE__, QString::asprintf( format, ##__VA_ARGS__ ) )
#else
#define LOGI( caller, format, ... )
#endif // end LOG_LV <= LOG_LV_INFO
#if LOG_LV <= LOG_LV_WARNING
#define LOGW( caller, format, ... ) \
    nsCmn::nsCmnLogger::printQWarningLog( nsCmn::nsCmnLogger::currentDateTime(), getpid(), syscall(SYS_gettid), LOG_LV_WARNING, caller, __FUNCTION__, __LINE__, QString::asprintf( format, ##__VA_ARGS__ ) )
#else
#define LOGW( caller, format, ... )
#endif // end LOG_LV <= LOG_LV_WARNING
#if LOG_LV <= LOG_LV_ERROR
#define LOGE( caller, format, ... ) \
    nsCmn::nsCmnLogger::printQCriticalLog( nsCmn::nsCmnLogger::currentDateTime(), getpid(), syscall(SYS_gettid), LOG_LV_ERROR, caller, __FUNCTION__, __LINE__, QString::asprintf( format, ##__VA_ARGS__ ) )
#else
#define LOGE( caller, format, ... )
#endif // end LOG_LV <= LOG_LV_ERROR
#elif defined(_WIN32) || defined(_WIN64)
#if LOG_LV <= LOG_LV_VERBOSE
#define LOGV( caller, format, ... ) \
    nsCmn::nsCmnLogger::printQDebugLog( nsCmn::nsCmnLogger::currentDateTime(), static_cast<long int>(_getpid()), static_cast<long int>(GetCurrentThreadId()), LOG_LV_VERBOSE, caller, __FUNCTION__, __LINE__, QString::asprintf( format, ##__VA_ARGS__ ) )
#else
#define LOGV( caller, format, ... )
#endif // end LOG_LV <= LOG_LV_VERBOSE
#if LOG_LV <= LOG_LV_DEBUG
#define LOGD( caller, format, ... ) \
    nsCmn::nsCmnLogger::printQDebugLog( nsCmn::nsCmnLogger::currentDateTime(), static_cast<long int>(_getpid()), static_cast<long int>(GetCurrentThreadId()), LOG_LV_DEBUG, caller, __FUNCTION__, __LINE__, QString::asprintf( format, ##__VA_ARGS__ ) )
#else
#define LOGD( caller, format, ... )
#endif // end LOG_LV <= LOG_LV_DEBUG
#if LOG_LV <= LOG_LV_INFO
#define LOGI( caller, format, ... ) \
    nsCmn::nsCmnLogger::printQInfoLog( nsCmn::nsCmnLogger::currentDateTime(), static_cast<long int>(_getpid()), static_cast<long int>(GetCurrentThreadId()), LOG_LV_INFO, caller, __FUNCTION__, __LINE__, QString::asprintf( format, ##__VA_ARGS__ ) )
#else
#define LOGI( caller, format, ... )
#endif // end LOG_LV <= LOG_LV_INFO
#if LOG_LV <= LOG_LV_WARNING
#define LOGW( caller, format, ... ) \
    nsCmn::nsCmnLogger::printQWarningLog( nsCmn::nsCmnLogger::currentDateTime(), static_cast<long int>(_getpid()), static_cast<long int>(GetCurrentThreadId()), LOG_LV_WARNING, caller, __FUNCTION__, __LINE__, QString::asprintf( format, ##__VA_ARGS__ ) )
#else
#define LOGW( caller, format, ... )
#endif // end LOG_LV <= LOG_LV_WARNING
#if LOG_LV <= LOG_LV_ERROR
#define LOGE( caller, format, ... ) \
    nsCmn::nsCmnLogger::printQCriticalLog( nsCmn::nsCmnLogger::currentDateTime(), static_cast<long int>(_getpid()), static_cast<long int>(GetCurrentThreadId()), LOG_LV_ERROR, caller, __FUNCTION__, __LINE__, QString::asprintf( format, ##__VA_ARGS__ ) )
#else
#define LOGE( caller, format, ... )
#endif // end LOG_LV <= LOG_LV_ERROR
#endif // end defined( __linux__ ) || defined( __APPLE__ )
#else
#define LOGV( caller, format, ... )
#define LOGD( caller, format, ... )
#define LOGI( caller, format, ... )
#define LOGW( caller, format, ... )
#define LOGE( caller, format, ... )
#endif

#endif /* CLOGGER_H_ */
