#include "QNetworkManager.h"

#include <QNetworkRequest>

#include "CEventLoop.h"
#include "CLogger.h"
#include "CStrManager.h"
#include "CCompressor.h"

#include <unistd.h>

QNetworkManager::QNetworkManager( QObject* parent) :
    QObject(parent)
{
    initNetworkAccessManager();
}

QNetworkManager::~QNetworkManager()
{
    if( m_mngNetwork!= NULL )
    {
        delete m_mngNetwork;
        m_mngNetwork = NULL;
    }
}

int QNetworkManager::Get(int nSender, QNetworkRequest& netReq, bool &isSuccess)
{
    isSuccess = false;

    if( netReq.url().isEmpty() || netReq.url().isValid() == false )
        return 0;

    int messageId = SetMessageId( netReq );
    SetSender( nSender, netReq );
    QLOGI( QString("messageId: %1, Sender: %2").arg(messageId).arg( nSender ) );

    QNetworkReply* netRes = m_mngNetwork->get(netReq);

    if( netRes->error() == QNetworkReply::NoError )
        isSuccess = true;

    return messageId;
}

int QNetworkManager::Post(int nSender, QNetworkRequest& netReq, const QByteArray& data, bool& isSuccess )
{
    isSuccess = false;

    if( netReq.url().isEmpty() || netReq.url().isValid() == false )
        return 0;

    int messageId = SetMessageId( netReq );
    SetSender( nSender, netReq );
    QLOGI( QString("messageId: %1, Sender: %2").arg(messageId).arg( nSender ) );

    QNetworkReply* netRes = m_mngNetwork->put(netReq, data);

    if( netRes->error() == QNetworkReply::NoError )
        isSuccess = true;

    return messageId;
}

QNetworkReply* QNetworkManager::PostWait( int nSender, QNetworkRequest& netReq, const QByteArray& data, bool& isSuccess, int msTimeout)
{
    isSuccess = false;

    if( netReq.url().isEmpty() || netReq.url().isValid() == false )
        return NULL;

    netReq.setAttribute((QNetworkRequest::Attribute)NETWORK_ATTRIBUTE_MESSAGE_ID, QVariant((int)0));
    SetSender( nSender, netReq );
    QLOGI( QString("messageId: 0, Sender: %2").arg( nSender ) );

    CEventLoop loop;

    QNetworkReply* netRes = m_mngNetwork->put(netReq, data);

    connect(netRes, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec(msTimeout);
    disconnect(netRes, SIGNAL(finished()), &loop, SLOT(quit()));

    if( loop.IsTimeout() == false && netRes->error() == QNetworkReply::NoError )
        isSuccess = true;
    else
        QLOGI( QString("messageId: 0, nSender:%1 Error(%2): %3, Timeout: %4").
               arg( nSender ).arg( netRes->error() ).arg( netRes->errorString() ).arg( nsCmn::boolToText( loop.IsTimeout() ) ) );

    if( netRes->isFinished() == false )
        netRes->abort();

    return netRes;
}

QNetworkAccessManager* QNetworkManager::getNetworkAccessManager()
{
    return m_mngNetwork;
}

QNetworkReply* QNetworkManager::GetWait( int nSender, QNetworkRequest& netReq, bool& isSuccess, int msTimeout )
{
    isSuccess = false;

    if( netReq.url().isEmpty() || netReq.url().isValid() == false )
        return NULL;

    netReq.setAttribute((QNetworkRequest::Attribute)NETWORK_ATTRIBUTE_MESSAGE_ID, QVariant((int)0));
    SetSender( nSender, netReq );
    QLOGI( QString("messageId: 0, Sender: %1").arg( nSender ) );

    CEventLoop loop;

    QNetworkReply* netRes = m_mngNetwork->get(netReq);

    connect(netRes, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec(msTimeout);
    disconnect(netRes, SIGNAL(finished()), &loop, SLOT(quit()));

    if( loop.IsTimeout() == false && netRes->error() == QNetworkReply::NoError )
        isSuccess = true;
    else
        QLOGI( QString("messageId: 0, nSender: %1 Error(%2): %3, Timeout: %4").
               arg( nSender ).arg(netRes->error()).arg(netRes->errorString()).arg( nsCmn::boolToText( loop.IsTimeout() ) ) );

    if( netRes->isFinished() == false )
        netRes->abort();

    return netRes;
}

void QNetworkManager::ReplyFinished(QNetworkReply* netRes)
{
    if( netRes == NULL )
        return;

    int messageId = netRes->request().attribute( (QNetworkRequest::Attribute)NETWORK_ATTRIBUTE_MESSAGE_ID ).toInt();
    int nSender = netRes->request().attribute( (QNetworkRequest::Attribute)NETWORK_ATTRIBUTE_SENDER ).toInt();

    QLOGI( QString("messageId: %1, eSender:%2 Error(%3): %4").arg(messageId).arg(nSender).arg(netRes->error()).arg(netRes->errorString()) );

    netRes->deleteLater();
}

void QNetworkManager::ReplyError(QNetworkReply::NetworkError err)
{
    QNetworkReply* netRes = qobject_cast<QNetworkReply*>(sender());

    int messageId = 0;
    if( netRes != NULL )
       messageId = netRes->request().attribute( (QNetworkRequest::Attribute)NETWORK_ATTRIBUTE_MESSAGE_ID ).toInt();

    QLOGE( QString("messageId: %1, Error(%2): %3").arg(messageId).arg((int)err).arg(netRes->errorString()) );
}

#ifndef QT_NO_SSL
void QNetworkManager::ReplySslErrors(const QList<QSslError>& lstSslErr)
{
    QNetworkReply* netRes = qobject_cast<QNetworkReply*>(sender());
    int messageId = 0;

    if( netRes != NULL )
       netRes->request().attribute( (QNetworkRequest::Attribute)NETWORK_ATTRIBUTE_MESSAGE_ID ).toInt();

    QString strErr = QString("messageId: %1 \n").arg(messageId);
    for( QList<QSslError>::const_iterator it = lstSslErr.cbegin(); it != lstSslErr.cend(); ++it )
        strErr.append( QString( "\tError(%1): %2\n" ).arg( it->error() ).arg( it->errorString() ) );

    QLOGE( strErr );
}
#endif

int QNetworkManager::CreateMessageId()
{
    return qrand();
}

int QNetworkManager::SetMessageId(QNetworkRequest& netReq)
{
    int messageId = CreateMessageId();
    //응답 받은 후 처리를 위한 정보 저장
    netReq.setAttribute((QNetworkRequest::Attribute)NETWORK_ATTRIBUTE_MESSAGE_ID, QVariant(messageId));

    return messageId;
}

void QNetworkManager::SetSender(int nSender, QNetworkRequest& netReq)
{
    netReq.setAttribute((QNetworkRequest::Attribute)NETWORK_ATTRIBUTE_SENDER, QVariant(nSender));
}

bool QNetworkManager::initNetworkAccessManager()
{
    m_mngNetwork = new QNetworkAccessManager(this);
//    connect(m_mngNetwork, SIGNAL(finished(QNetworkReply*)),this, SLOT(ReplyFinished(QNetworkReply*)));

    return true;
}
