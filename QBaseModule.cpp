#include "QBaseModule.h"

#include "CLogger.h"
#include <ModuleEnum.h>

namespace nsQBaseModule{
    const char* TAG = "QBaseModule";
}

using namespace nsQBaseModule;

QBaseModule::QBaseModule( QObject *parent )
: QObject( parent ), m_isSet(false), m_isStop(false), m_isStopped(false), m_eInitMethod(INIT_MODULE_MANAGER)
, m_isIndependentModule(false)
{

}

QBaseModule::~QBaseModule()
{
    m_mapReqCodeToFuncName.clear();
    m_mapResCodeToFuncName.clear();
}

void QBaseModule::setIsSet(bool isSet)
{
    m_isSet = isSet;
}

bool QBaseModule::IsSet()
{
    return m_isSet;
}

QBaseModule::eInitMethod QBaseModule::InitMethod()
{
    return m_eInitMethod;
}

void QBaseModule::setStop(bool isStop)
{
    m_isStop = isStop;
}

void QBaseModule::moduleStopped()
{
    if( m_isStop == false )
        stopModule();
    m_isStopped = true;
}

bool QBaseModule::IsStop()
{
    return m_isStop;
}

bool QBaseModule::IsStopped()
{
    return m_isStopped;
}

void QBaseModule::setIndependentModule(bool isIndependentModule)
{
    m_isIndependentModule = isIndependentModule;
}

void QBaseModule::setInitMethod(eInitMethod eInitMethod)
{
    m_eInitMethod = eInitMethod;
}

void QBaseModule::connectThreadSignals(QThread *pThModule)
{
    connect( pThModule, SIGNAL( started() ), this, SLOT( doRun() ), Qt::DirectConnection );
    connect( pThModule, SIGNAL( finished() ), this, SLOT( moduleStopped() ), Qt::DirectConnection );
}

void QBaseModule::disconnectThreadSignals(QThread *pThModule)
{
    disconnect( pThModule, SIGNAL( started() ), this, SLOT( doRun() ) );
    disconnect( pThModule, SIGNAL( finished() ), this, SLOT( moduleStopped() ) );
}

bool QBaseModule::IsIndependentModule()
{
    return m_isIndependentModule;
}

void QBaseModule::insertFunctionToReqCode(int reqCode, const QString& functionName )
{
    m_mapReqCodeToFuncName.insert( reqCode, functionName );
}

QString QBaseModule::getFunctionNameFromReqCode(int reqCode)
{
    return m_mapReqCodeToFuncName.value( reqCode, "" );
}

void QBaseModule::insertFunctionToResCode( int resCode, const QString& functionName )
{
    m_mapResCodeToFuncName.insert( resCode, functionName );
}

QString QBaseModule::getFunctionNameFromResCode(int resCode)
{
    return m_mapResCodeToFuncName.value( resCode, "" );
}

void QBaseModule::init()
{
    setIsSet(true);
}

void QBaseModule::stopModule()
{
    setStop(true);
    setIsSet(false);
}

void QBaseModule::getRequest(int reqCode, int sender, bool response, QVariantMap reqPacket)
{
    if( IsSet() == false || IsStop() )
        return;

    QString functionName = getFunctionNameFromReqCode( reqCode );
    if( functionName.isEmpty() )
    {
        LOGE( TAG, "Function %s is not registed", functionName.toStdString().c_str() );
        return;
    }

    QMetaObject::invokeMethod( this, functionName.toStdString().c_str(), Qt::DirectConnection
                               , Q_ARG( int, sender ), Q_ARG( bool, response ), Q_ARG( QVariantMap, reqPacket ) );
}

void QBaseModule::getResponse(int resCode, int sender, QVariantMap resPacket )
{
    if( IsSet() == false || IsStop() )
        return;

    QString functionName = getFunctionNameFromResCode( resCode );
    if( functionName.isEmpty() )
    {
        LOGE( TAG, "Function %s is not registed", functionName.toStdString().c_str() );
        return;
    }

    QMetaObject::invokeMethod( this, functionName.toStdString().c_str(), Qt::DirectConnection
                               , Q_ARG( int, sender ), Q_ARG( QVariantMap, resPacket ) );
}
