#include "QIPCServerManager.h"

#include "CLogger.h"

QIPCServerManager::QIPCServerManager(QObject *parent) : QObject(parent)
{
    m_localSVR = new QLocalServer(this);
}

QIPCServerManager::~QIPCServerManager()
{
    disconnectAll();
    QLocalServer::removeServer( m_serverName );
}

bool QIPCServerManager::initServer( const QString& serverName )
{
    m_serverName.clear();

    QLOGD( QString( "ServerName: " ) + serverName );
    QLocalServer::removeServer( serverName );

    if( !m_localSVR->listen( serverName ) )
    {
        QLOGE( QString( "Listenning failed. Error(%1): %2" ).arg( (int)m_localSVR->serverError() ).arg( m_localSVR->errorString() ) );
        return false;
    }

    m_serverName = serverName;
    connect(m_localSVR, &QLocalServer::newConnection, this, &QIPCServerManager::notifyNewConnection);
    return true;
}

void QIPCServerManager::closeServer()
{
    if( m_localSVR->isListening() )
    {
        m_localSVR->close();
        disconnect(m_localSVR, &QLocalServer::newConnection, this, &QIPCServerManager::notifyNewConnection);
    }
}

void QIPCServerManager::sendRequest(int reqCode, int& messageId, QHash<QString, QString> mapParams, int idxSocket )
{
    ///Todo: modify request


    if( idxSocket > 0 )
    {
        QLocalSocket socketClient;
        if( socketClient.setSocketDescriptor( idxSocket ) && socketClient.state() != QLocalSocket::UnconnectedState )
        {
//          socketClient->write(block);
        }
    }
    else if( idxSocket == -1 )
    {
        m_lck.lockForRead();
        QList< int > lstSocket = m_mapIdxToSocket.keys();
        m_lck.unlock();
        for( QList< int >::const_iterator it = lstSocket.cbegin(); it != lstSocket.cend(); ++it )
        {
            QLocalSocket socketClient;
            if( socketClient.setSocketDescriptor( idxSocket ) && socketClient.state() != QLocalSocket::UnconnectedState )
            {
//              socketClient->write(block);
            }
        }
    }
}

void QIPCServerManager::disconnectByIdx(int idxSocket )
{
    m_lck.lockForRead();
    QLocalSocket* socketClient = m_mapIdxToSocket.value( idxSocket, NULL );
    m_lck.unlock();

    if( socketClient != NULL && socketClient->state() != QLocalSocket::UnconnectedState )
    {
        QLOGI( QString( "Disconnect socket: %1" ).arg( idxSocket ) );
        socketClient->flush();
        socketClient->disconnectFromServer();
    }
}

void QIPCServerManager::disconnectByConnection( QLocalSocket* socketClient )
{
    m_lck.lockForRead();
    int idxSocket = m_mapIdxToSocket.key( socketClient, -1 );
    m_lck.unlock();

    if( socketClient != NULL && socketClient->state() != QLocalSocket::UnconnectedState )
    {
        QLOGI( QString( "Disconnect socket: %1" ).arg( idxSocket ) );
        socketClient->flush();
        socketClient->disconnectFromServer();
    }
}

void QIPCServerManager::disconnectAll( bool isReListning )
{
    closeServer();
    QLOGI("disconnect all connections");

    {
        m_lck.lockForRead();
        QList< int > lstSocket = m_mapIdxToSocket.keys();
        m_lck.unlock();
        for( QList< int >::const_iterator it = lstSocket.cbegin(); it != lstSocket.cend(); ++it )
            disconnectByIdx( *it );
    }

    if( isReListning )
    {
        QString serverName = m_serverName;
        initServer( serverName );
    }
    QLOGI("disconnect all connections");
}

void QIPCServerManager::notifyNewConnection()
{
    QLocalSocket* socketClient = m_localSVR->nextPendingConnection();
    int idxSocket = socketClient->socketDescriptor();
    {
        QWriteLocker lck( &m_lck );
        m_mapIdxToSocket.insert( idxSocket, socketClient );
    }

    connect(socketClient, &QLocalSocket::disconnected, this, &QIPCServerManager::notifyDisconnect );
    connect(socketClient, &QLocalSocket::readyRead, this, &QIPCServerManager::notifyReadyRead );

    QLOGI( QString( "New connection detected. idxSocket: %1. total size: %2" ).arg( idxSocket ).arg( m_mapIdxToSocket.size() ) );
}

void QIPCServerManager::notifyReadyRead()
{
    QLocalSocket* socketClient = qobject_cast<QLocalSocket*>( sender() );
    int idxSocket = socketClient->socketDescriptor();
    if( idxSocket <= 0 )
    {
        QLOGW( QString( "socketClient is not valied. idxSocket: %d" ).arg( idxSocket ) );
        return;
    }
    else
    {
        m_lck.lockForRead();
        bool isExist = m_mapIdxToSocket.contains( idxSocket );
        m_lck.unlock();

        if( isExist == false )
        {
            QLOGW( QString( "unregisted socket. idxSocket: %d" ).arg( idxSocket ) );
            disconnectByConnection( socketClient );
            return;
        }
    }
}

void QIPCServerManager::notifyDisconnect()
{
    QLocalSocket* socketClient = qobject_cast<QLocalSocket*>( sender() );

    int idxSocket = -1;

    {
        QWriteLocker lck( &m_lck );

        idxSocket =  m_mapIdxToSocket.key( socketClient, -1 );
        if( idxSocket > 0 )
            m_mapIdxToSocket.remove( idxSocket );
    }

    disconnect( socketClient, &QLocalSocket::disconnected, this, &QIPCServerManager::notifyDisconnect );
    disconnect( socketClient, &QLocalSocket::readyRead, this, &QIPCServerManager::notifyReadyRead );
    socketClient->deleteLater();

    QLOGI( QString( "Connection disconnected. idxSocket: %1. total size: %2" ).arg( idxSocket ).arg( m_mapIdxToSocket.size() ) );
}
