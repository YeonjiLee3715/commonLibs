#include "QModuleManager.h"

#include "CLogger.h"

bool QModuleManager::m_isDestroyed = false;
QModuleManager* QModuleManager::m_pModuleManager = nullptr;

namespace nsQModuleManager{
    const char* TAG = "QModuleManager";
}

using namespace nsQModuleManager;

QModuleManager::QModuleManager(QObject *parent) : QObject(parent)
{

}

QModuleManager::~QModuleManager()
{
    m_isDestroyed = true;
}

bool QModuleManager::init()
{
    return true;
}

bool QModuleManager::runIndependentModules()
{
    for( QHash< int, QThread* >::iterator it = m_mapIdToThread.begin()
         ; it != m_mapIdToThread.end(); ++it )
    {
        LOGD( TAG, "module: %s ( ID: %d )"
              , QModuleEnum::getStringFromEnum((QModuleEnum::eMODULE)it.key() ).toStdString().c_str()
              , it.key() );
        QBaseModule* pModule = getBaseModuleById(it.key());

        if( pModule == nullptr )
        {
            printError( it.key(), "does not registed yet" );
            continue;
        }

        (*it)->start();
    }

    return true;
}

bool QModuleManager::stopIndependentModules()
{
    for( QHash< int, QThread* >::iterator it = m_mapIdToThread.begin()
         ; it != m_mapIdToThread.end(); ++it )
    {
        if( (*it) != nullptr && (*it)->isRunning() )
        {
            (*it)->quit();
            (*it)->wait();
        }
    }

    return true;
}

bool QModuleManager::stopIndependentModule( int nModuleId )
{
    QThread* thModule = m_mapIdToThread.value( nModuleId, nullptr );

    if( thModule != nullptr && thModule->isRunning() )
    {
        thModule->quit();
        thModule->wait();
    }

    return true;
}

QBaseModule *QModuleManager::getBaseModuleById(int nModuleId)
{
    QReadLocker lck( &m_lck );
    return m_mapIdToPtr.value( nModuleId, nullptr );
}

QBaseModule *QModuleManager::getBaseModuleByName(const QString &strName)
{
    QReadLocker lck( &m_lck );
    return m_mapIdToPtr.value( (int)QModuleEnum::getIndexFromString( strName ), nullptr );
}

void QModuleManager::printError(int nModuleId, std::string errorMessage)
{
    LOGE( TAG, "Module: %s ( ID: %d ) %s."
          , QModuleEnum::getStringFromEnum((QModuleEnum::eMODULE)nModuleId ).toStdString().c_str()
          , nModuleId
          , errorMessage.c_str() );
}

void QModuleManager::sendRequest(int reqCode, int sender, int reciver, bool response, QVariantMap reqPacket)
{
    QReadLocker lck( &m_lck );
    QBaseModule* pModule = m_mapIdToPtr.value( reciver, nullptr );

    if( pModule == nullptr || pModule->IsSet() == false || pModule->IsStop() )
        return;

    QMetaObject::invokeMethod( pModule, "getRequest", Qt::QueuedConnection
                               , Q_ARG( int, reqCode ), Q_ARG( int, sender ), Q_ARG( bool, response ), Q_ARG( QVariantMap, reqPacket )  );

}

void QModuleManager::sendResponse(int resCode, int sender, int reciver, QVariantMap resPacket )
{
    QReadLocker lck( &m_lck );
    QBaseModule* pModule = m_mapIdToPtr.value( reciver, nullptr );

    if( pModule == nullptr || pModule->IsSet() == false || pModule->IsStop() )
    {
        if( QModuleEnum::isMainManager( (QModuleEnum::eMODULE)reciver ) )
        {
            QMetaObject::invokeMethod( mManager, "getResponse", Qt::QueuedConnection
                                       , Q_ARG( int, resCode ), Q_ARG( int, sender ), Q_ARG( QVariantMap, resPacket )  );
        }
        return;
    }

    QMetaObject::invokeMethod( pModule, "getResponse", Qt::QueuedConnection
                               , Q_ARG( int, resCode ), Q_ARG( int, sender ), Q_ARG( QVariantMap, resPacket )  );
}
