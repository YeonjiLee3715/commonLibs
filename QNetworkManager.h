#ifndef QNETWORKMANAGER_H
#define QNETWORKMANAGER_H

#include <QObject>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "../networkManager/define/NetworkEnum.h"

#define NETWORK_RESPONS_SIGNAL( RECEIVER ) \
    void SendResponsTo##RECEIVER( int messageId, QNetworkReply* netRes );

#define NETWORK_EMIT_RESPONS_SIGNAL( RECEIVER , MESSAGEID, NETRES) \
    emit SendResponsTo##RECEIVER( MESSAGEID, NETRES );

#define NETWORK_RESPONS_SLOT    \
    void ReceiveNetworkRespons( int messageId, QNetworkReply* netRes );

#define NETWORK_DEF_RESPONS_SLOT( RECEIVER )    \
    void RECEIVER::ReceiveNetworkRespons( int messageId, QNetworkReply* netRes )

#define NETWORK_CONNECT_SIGNAL( P_NETWORKMANAGER, RECEIVER, P_RECEIVER ) \
    connect(P_NETWORKMANAGER, SIGNAL(SendResponsTo##RECEIVER(int, QNetworkReply*)),P_RECEIVER, SLOT(ReceiveNetworkRespons(int, QNetworkReply*)), Qt::QueuedConnection);

class QNetworkManager : public QObject
{
    Q_OBJECT

public:
    explicit QNetworkManager( QObject* parent = 0);
    ~QNetworkManager();

    int Get(int nSender, QNetworkRequest& netReq, bool& isSuccess );
    QNetworkReply* GetWait(int nSender, QNetworkRequest& netReq, bool& isSuccess, int msTimeout = 1000*60);

    int Post(int nSender, QNetworkRequest& netReq, const QByteArray& pData , bool &isSuccess);
    QNetworkReply* PostWait(int nSender, QNetworkRequest& netReq, const QByteArray& pData, bool& isSuccess, int msTimeout = 1000*60 );

protected:
    QNetworkAccessManager* getNetworkAccessManager();

    void SetSender(int nSender, QNetworkRequest& netReq );

    bool initNetworkAccessManager();
public slots:
    void ReplyFinished(QNetworkReply* netRes);
    void ReplyError( QNetworkReply::NetworkError err );
#ifndef QT_NO_SSL
    void ReplySslErrors(const QList<QSslError> &lstSslErr );
#endif


private:
    int CreateMessageId();
    int SetMessageId( QNetworkRequest& netReq );
private:
    QNetworkAccessManager*  m_mngNetwork;
};

#endif // QNETWORKMANAGER_H
