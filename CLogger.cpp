/*
 * CLogger.cpp
 *
 *  Created on: 2017. 5. 19.
 *      Author: 이연지
 */

#include "CLogger.h"

namespace nsCmn
{
    namespace nsCmnLogger
    {

        const std::string getLogLevelTag( char logLevel )
        {
            std::string tagLogLevel;

            switch(logLevel)
            {
                case LOG_LV_DEFAULT:
                tagLogLevel = "DEFAULT";
                break;
                case LOG_LV_VERBOSE:
                tagLogLevel = "VERBOSE";
                break;
                case LOG_LV_DEBUG:
                tagLogLevel = "DEBUG";
                break;
                case LOG_LV_INFO:
                tagLogLevel = "INFO";
                break;
                case LOG_LV_WARNING:
                tagLogLevel = "WARNING";
                break;
                case LOG_LV_ERROR:
                tagLogLevel = "ERROR";
                break;
                case LOG_LV_FATAL:
                tagLogLevel = "FATAL";
                break;
                case LOG_LV_SILENT:
                tagLogLevel = "SILENT";
                break;
                case LOG_LV_UNKNOWN:
                default:
                tagLogLevel = "UNKNOWN";
            }

            return tagLogLevel;
        }

        const std::string currentDateTime()
        {
            time_t     now = time(nullptr);
            struct tm  tstruct;
            char buf[20] = {0,};
            tstruct = *localtime(&now);
            // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
            // for more information about date/time format
            strftime(buf, sizeof(buf), "%F %T", &tstruct);

            return buf;
        }

#if defined(__JNI_DEBUG)
        const std::string addLogInfo( const char* function, int line, char logLevel, const char* format )
        {
            std::string strFormat = nsCmn::string_format( "%s(%d): %s, ", function, line, getLogLevelTag( logLevel ).c_str() );

            if( format != NULL && format[0] != '\0')
                strFormat.append( format );

            return strFormat;
        }
#endif
#if defined( __QT_DEBUG )
        void printQDebugLog(const std::string& dateTime, long int pid, long int tid, char logLevel, const char* caller, const char* function, int line, QString format )
        {
            qDebug("%s PID: %ld, TID: %ld, %s: %s(%d): %s, %s", dateTime.c_str(), pid, tid
                   , caller, function, line, getLogLevelTag( logLevel ).c_str(), format.toStdString().c_str() );
        }

        void printQInfoLog(const std::string& dateTime, long int pid, long int tid, char logLevel, const char* caller, const char* function, int line, QString format )
        {
            qInfo("%s PID: %ld, TID: %ld, %s: %s(%d): %s, %s", dateTime.c_str(), pid, tid
                   , caller, function, line, getLogLevelTag( logLevel ).c_str(), format.toStdString().c_str() );
        }

        void printQWarningLog(const std::string& dateTime, long int pid, long int tid, char logLevel, const char* caller, const char* function, int line, QString format )
        {
            qWarning("%s PID: %ld, TID: %ld, %s: %s(%d): %s, %s", dateTime.c_str(), pid, tid
                   , caller, function, line, getLogLevelTag( logLevel ).c_str(), format.toStdString().c_str() );
        }

        void printQCriticalLog(const std::string& dateTime, long int pid, long int tid, char logLevel, const char* caller, const char* function, int line, QString format )
        {
            qCritical("%s PID: %ld, TID: %ld, %s: %s(%d): %s, %s", dateTime.c_str(), pid, tid
                   , caller, function, line, getLogLevelTag( logLevel ).c_str(), format.toStdString().c_str() );
        }

        void printQFatalLog(const std::string& dateTime, long int pid, long int tid, char logLevel, const char* caller, const char* function, int line, QString format )
        {
            qFatal("%s PID: %ld, TID: %ld, %s: %s(%d): %s, %s", dateTime.c_str(), pid, tid
                   , caller, function, line, getLogLevelTag( logLevel ).c_str(), format.toStdString().c_str() );
        }
#endif
    }

}

