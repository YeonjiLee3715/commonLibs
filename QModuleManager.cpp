#include "QModuleManager.h"

#include <QModuleFactory.h>
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

        (*it)->start();
    }

    return true;
}

bool QModuleManager::runIndependentModule(int nModuleId)
{
    QThread* thModule = m_mapIdToThread.value( nModuleId, nullptr );

    if( thModule != nullptr && thModule->isRunning() == false )
        thModule->start();

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

bool QModuleManager::registModule(QBaseModule* pModule, int nModuleId)
{
    if( pModule == nullptr )
    {
        printError(nModuleId, "is nullptr" );
        return false;
    }

    {
        QReadLocker lck( &m_lck );
        if( m_mapIdToPtr.contains( nModuleId ) )
        {
            printError(nModuleId, "is already exist" );
            return false;
        }
    }

    if( pModule->InitMethod() == QBaseModule::eInitMethod::INIT_MODULE_MANAGER )
    {
        pModule->init();
        if( pModule->IsSet() == false )
        {
            printError(nModuleId, "failed to init" );
            return false;
        }
    }

    if( pModule->IsIndependentModule() )
    {
        QThread* thModule = new QThread;

        pModule->moveToThread( thModule );
        pModule->connectThreadSignals( thModule );
        pModule->setIndependentModule( true );

        QWriteLocker lck( &m_lck );
        m_mapIdToPtr.insert( nModuleId, reinterpret_cast<QBaseModule*>( pModule ) );
        m_mapIdToThread.insert( nModuleId, thModule );

        if( pModule->IsSet() )
            thModule->start();
    }
    else
    {
        QWriteLocker lck( &m_lck );
        m_mapIdToPtr.insert( nModuleId, reinterpret_cast<QBaseModule*>( pModule ) );
    }

    return true;
}

void QModuleManager::deregistModule(int nModuleId)
{
    QBaseModule* pModule = getModuleById( nModuleId );
    QThread* thModule = m_mapIdToThread.value( nModuleId, nullptr );

    {
        QWriteLocker lck( &m_lck );
        m_mapIdToPtr.remove( nModuleId );
        m_mapIdToThread.remove( nModuleId );
    }

    if( thModule != nullptr )
    {
        if( thModule->isRunning() )
        {
            thModule->quit();
            thModule->wait();
        }

        pModule->disconnectThreadSignals( thModule );

        thModule->deleteLater();
        thModule = nullptr;
    }


    if( pModule != nullptr )
    {
        if( pModule->IsStop() == false )
            pModule->stopModule();

        pModule->deleteLater();
        pModule = nullptr;
    }

    return;
}

QBaseModule *QModuleManager::getModuleById(int nModuleId, bool bCreate)
{
     QBaseModule* pModule = nullptr;

    {
        QReadLocker lck( &m_lck );
        pModule = m_mapIdToPtr.value( nModuleId, nullptr );
        if( pModule != nullptr )
            return pModule;
    }

    if( bCreate == false )
        return pModule;

    pModule =  createModule( nModuleId );
    if( pModule == nullptr )
    {
        printError( nModuleId, "Failed to create module" );
        return pModule;
    }

    if( registModule( pModule, nModuleId ) == false )
    {
        printError( nModuleId, "Failed to register module" );
        pModule->deleteLater();
        pModule = nullptr;
        return pModule;
    }

    return pModule;
}

QBaseModule *QModuleManager::getModuleByName(const QString &strName, bool bCreate)
{
    int nModuleId = (int)QModuleEnum::getIndexFromString( strName );
    return getModuleById(nModuleId, bCreate);
}

QBaseModule* QModuleManager::createModule(int nModuleId)
{
    return QModuleFactory::CreateModule((QModuleEnum::eMODULE)nModuleId );
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
