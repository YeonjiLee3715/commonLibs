#ifndef QMODULEMANAGER_H
#define QMODULEMANAGER_H

#include <QThread>
#include <QHash>
#include <QObject>
#include <QReadWriteLock>

#include "QBaseModule.h"

#include <ModuleEnum.h>

#define REG_INDEPENDENT_MODULE( CLASS, pMODULE ) \
    QModuleManager::GetInstance()->registIndependentModule< CLASS >( pMODULE, MDL_ID( CLASS ) );
#define REG_DEPENDENT_MODULE( CLASS, pMODULE ) \
    QModuleManager::GetInstance()->registDependentModule< CLASS >( pMODULE, MDL_ID( CLASS ) );
#define DEREG_INDEPENDENT_MODULE( CLASS ) \
    QModuleManager::GetInstance()->deregistIndependentModule< CLASS >( MDL_ID( CLASS ) );
#define DEREG_DEPENDENT_MODULE( CLASS ) \
    QModuleManager::GetInstance()->deregistDependentModule< CLASS >( MDL_ID( CLASS ) );

#define GET_MODULE_INSTANCE( CLASS ) \
    QModuleManager::GetInstance()->getModuleById< CLASS >( MDL_ID( CLASS ) )

#define SEND_MODULE_REQ( REQ_CODE, SENDER_MODULE, RECIVER_MODULE, B_NEED_RES, MAP ) \
    QMetaObject::invokeMethod( QModuleManager::GetInstance(), "sendRequest", Qt::DirectConnection   \
                                            , Q_ARG( int, static_cast<int>( REQ_CODE ) )                           \
                                            , Q_ARG( int, static_cast<int>( MDL_ID( SENDER_MODULE ) ) )            \
                                            , Q_ARG( int, static_cast<int>( MDL_ID( RECIVER_MODULE ) ) )           \
                                            , Q_ARG( bool, B_NEED_RES )                             \
                                            , Q_ARG( QVariantMap, MAP ) )

#define SEND_MODULE_REQ_BY_ID( REQ_CODE, SENDER_MODULE, RECIVER_MODULE_ID, B_NEED_RES, MAP ) \
    QMetaObject::invokeMethod( QModuleManager::GetInstance(), "sendRequest", Qt::DirectConnection   \
                                            , Q_ARG( int, static_cast<int>( REQ_CODE ) )                           \
                                            , Q_ARG( int, static_cast<int>( MDL_ID( SENDER_MODULE ) ) )            \
                                            , Q_ARG( int, static_cast<int>( RECIVER_MODULE_ID ) )                      \
                                            , Q_ARG( bool, B_NEED_RES )                                             \
                                            , Q_ARG( QVariantMap, MAP ) )

#define SEND_MODULE_RES( RES_CODE, SENDER_MODULE, RECIVER_MODULE, MAP ) \
    QMetaObject::invokeMethod( QModuleManager::GetInstance(), "sendResponse", Qt::DirectConnection  \
                                            , Q_ARG( int, static_cast<int>( RES_CODE ) )                           \
                                            , Q_ARG( int, static_cast<int>( MDL_ID( SENDER_MODULE ) ) )            \
                                            , Q_ARG( int, static_cast<int>( MDL_ID( RECIVER_MODULE ) ) )           \
                                            , Q_ARG( QVariantMap, MAP ) )

#define SEND_MODULE_RES_BY_ID( RES_CODE, SENDER_MODULE, RECIVER_MODULE_ID, MAP ) \
    QMetaObject::invokeMethod( QModuleManager::GetInstance(), "sendResponse", Qt::DirectConnection                  \
                                            , Q_ARG( int, static_cast<int>( RES_CODE ) )                            \
                                            , Q_ARG( int, static_cast<int>( MDL_ID( SENDER_MODULE ) ) )             \
                                            , Q_ARG( int, static_cast<int>( RECIVER_MODULE_ID ) )                   \
                                            , Q_ARG( QVariantMap, MAP ) )

class QModuleManager : public QObject
{
    Q_OBJECT
public:
    explicit QModuleManager(QObject *parent = nullptr);
    ~QModuleManager();

private:
    static bool m_isDestroyed;
    static QModuleManager* m_pModuleManager;

    QHash< int, QBaseModule* > m_mapIdToPtr;
    QHash< int, QThread* > m_mapIdToThread;

    QReadWriteLock m_lck;

public:
    bool init();
    bool runIndependentModules();
    bool stopIndependentModules();
    bool stopIndependentModule( int nModuleId );

    template < typename T_MODULE >
    bool registDependentModule( T_MODULE* pModule, int nModuleId )
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

        {
            QWriteLocker lck( &m_lck );
            m_mapIdToPtr.insert( nModuleId, reinterpret_cast< QBaseModule* >( pModule ) );
        }

        return true;
    }

    template < typename T_MODULE >
    bool registIndependentModule( T_MODULE* pModule, int nModuleId )
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

        QThread* thModule = new QThread;

        pModule->moveToThread( thModule );
        connect( thModule, SIGNAL( started() ), pModule, SLOT( doRun() ), Qt::QueuedConnection );
        connect( thModule, SIGNAL( finished() ), pModule, SLOT( stopModule() ), Qt::QueuedConnection );

        pModule->setIndependentModule( true );

        {
            QWriteLocker lck( &m_lck );
            m_mapIdToPtr.insert( nModuleId, reinterpret_cast< QBaseModule* >( pModule ) );
            m_mapIdToThread.insert( nModuleId, thModule );
        }

        return true;
    }

    template < typename T_MODULE >
    void deregistDependentModule( int nModuleId )
    {
        T_MODULE* pModule = getModuleById<T_MODULE>( nModuleId );

        {
            QWriteLocker lck( &m_lck );
            m_mapIdToPtr.remove( nModuleId );
        }


        if( pModule != nullptr )
        {
            if( pModule->IsStop() == false )
                pModule->stopModule();

            pModule->deleteLater();
            pModule = nullptr;
        }
    }

    template < typename T_MODULE >
    void deregistIndependentModule( int nModuleId )
    {
        T_MODULE* pModule = getModuleById<T_MODULE>( nModuleId );
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

            disconnect( thModule, SIGNAL( started() ), pModule, SLOT( doRun() ) );
            disconnect( thModule, SIGNAL( finished() ), pModule, SLOT( stopModule() ) );

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


    QBaseModule* getBaseModuleById( int nModuleId );
    QBaseModule* getBaseModuleByName( const QString& strName );

    template < typename T_MODULE >
    T_MODULE* getModuleById( int nModuleId )
    {
        {
            QReadLocker lck( &m_lck );
            QBaseModule* pBaseModule = m_mapIdToPtr.value( nModuleId, nullptr );
            if( pBaseModule != nullptr )
                return reinterpret_cast< T_MODULE* >( pBaseModule );
        }

        return nullptr;
    }

    template < typename T_MODULE >
    T_MODULE* getModuleByName( const QString& strName )
    {
        {
            QReadLocker lck( &m_lck );
            QBaseModule* pBaseModule = m_mapIdToPtr.value( (int)QModuleEnum::getIndexFromString( strName ), nullptr );
            if( pBaseModule != nullptr )
                return reinterpret_cast< T_MODULE* >( pBaseModule );
        }

        return nullptr;
    }

    static QModuleManager* GetInstance()
    {
        if( m_isDestroyed )
        {
            m_pModuleManager = new QModuleManager;
            atexit( destroy );
            m_isDestroyed = false;
        }
        else if( m_pModuleManager == nullptr )
        {
            static QModuleManager moduleManager;
            m_pModuleManager = &moduleManager;
        }

        return m_pModuleManager;
    }

private:
    static void destroy()
    {
        if( m_pModuleManager != nullptr )
        {
            m_pModuleManager->~QModuleManager();
            m_pModuleManager = nullptr;
        }
    }

    void printError(int nModuleId, std::string errorMessage );

signals:

public slots:
    void sendRequest(int reqCode, int sender, int reciver, bool response, QVariantMap reqPacket );
    void sendResponse(int resCode, int sender, int reciver, QVariantMap resPacket );
};

#endif // QMODULEMANAGER_H
