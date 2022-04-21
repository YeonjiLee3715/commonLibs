#ifndef QIPCSERVERMANAGER_H
#define QIPCSERVERMANAGER_H

#include <QtNetwork>
#include <QReadWriteLock>

#include <qlocalserver.h>
#include <qlocalsocket.h>

class QIPCServerManager : public QObject
{
    Q_OBJECT
public:
    explicit QIPCServerManager(QObject *parent = nullptr);
    ~QIPCServerManager();

    bool initServer( const QString &serverName );
//    bool set
    void closeServer();

signals:

public slots:
    void sendRequest(int reqCode, int& messageId, QHash<QString, QString> mapParams, int idxSocket = -1 );

    void disconnectByIdx( int idxSocket );
    void disconnectByConnection( QLocalSocket* socketClient );
    void disconnectAll( bool isReListning = false );

private slots:
    void notifyNewConnection();
    void notifyReadyRead();
    void notifyDisconnect();

private:
    QLocalServer* m_localSVR;
    QHash< int, QLocalSocket* > m_mapIdxToSocket;
    QString m_serverName;

    //If the client has a specific ID(ex. QT_UI, MFC_UI, UI_3, ... ), It can be called by ID. But it isn't unique.
    QHash< int, QString > m_mapSocketToName;

    QReadWriteLock m_lck;
};

#endif // QIPCSERVERMANAGER_H
