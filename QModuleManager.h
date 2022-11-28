#ifndef QMODULEMANAGER_H
#define QMODULEMANAGER_H

#include <QThread>
#include <QHash>
#include <QObject>
#include <QReadWriteLock>

#include "QBaseModule.h"

#include <ModuleEnum.h>

#define GET_MODULE( CLASS ) \
    QModuleManager::GetInstance()->getModuleById( MDL_ID( CLASS ) )

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

class QModuleManager final : public QObject
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
    bool runIndependentModule( int nModuleId );
    bool stopIndependentModules();
    bool stopIndependentModule( int nModuleId );

    void deregistModule( int nModuleId );

    QBaseModule* getModuleById( int nModuleId, bool bCreate = true );
    QBaseModule* getModuleByName( const QString& strName, bool bCreate = true );

    static QModuleManager* GetInstance()
    {
        if( m_isDestroyed )
        {
            m_pModuleManager = new QModuleManager;
            atexit( destroy );
            m_isDestroyed = false;
        }
        else if( !m_pModuleManager )
        {
            static QModuleManager moduleManager;
            m_pModuleManager = &moduleManager;
        }

        return m_pModuleManager;
    }

private:
    QBaseModule* createModule( int nModuleId );
    bool registModule( QBaseModule* pModule, int nModuleId );

private:
    static void destroy()
    {
        if( !m_pModuleManager )
            m_pModuleManager->~QModuleManager();
    }

    void printError(int nModuleId, std::string errorMessage );

signals:

public slots:
    void sendRequest(int reqCode, int sender, int reciver, bool response, QVariantMap reqPacket );
    void sendResponse(int resCode, int sender, int reciver, QVariantMap resPacket );
};

#endif // QMODULEMANAGER_H
