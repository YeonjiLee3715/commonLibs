#ifndef QBASEMODULE_H
#define QBASEMODULE_H

#include <stdlib.h>
#include <QObject>
#include <QReadWriteLock>
#include <QHash>
#include <QVariant>

//#include "proto/module.pb.h"

//Q_DECLARE_METATYPE( module::requestPacket )
//Q_DECLARE_METATYPE( module::responsePacket )

#define DECLARE_MODULE_REQUEST( FUNCTION_NAME ) \
    void Req##FUNCTION_NAME( int sender, bool response, QVariantMap mapReq );
#define DEFINE_MODULE_REQUEST( CLASS, FUNCTION_NAME ) \
    void CLASS::Req##FUNCTION_NAME( int sender, bool response, QVariantMap mapReq )
#define REGIST_MODULE_REQUEST( REQ_CODE, FUNCTION_NAME ) \
    insertFunctionToReqCode( static_cast<int>( REQ_CODE ), QString("Req")+#FUNCTION_NAME );

#define DECLARE_MODULE_RESPONSE( FUNCTION_NAME ) \
    void Res##FUNCTION_NAME( int sender, QVariantMap mapRes );
#define DEFINE_MODULE_RESPONSE( CLASS, FUNCTION_NAME ) \
    void CLASS::Res##FUNCTION_NAME( int sender, QVariantMap mapRes )
#define REGIST_MODULE_RESPONSE( RES_CODE, FUNCTION_NAME ) \
    insertFunctionToResCode( static_cast<int>( RES_CODE ), QString("Res")+#FUNCTION_NAME );

class QBaseModule : public QObject
{
    Q_OBJECT
public:
    QBaseModule(QObject *parent = nullptr);
    virtual ~QBaseModule();

    void setIsSet( bool isSet );
    bool IsSet();

    void setCancelEvent( bool bCancelEvent );
    bool CancelEvent();

    void setStop( bool isStop );
    bool IsStop();

    void setIndependentModule( bool isIndependentModule );
    bool IsIndependentModule();

    void insertFunctionToReqCode(int reqCode, const QString& functionName );
    QString getFunctionNameFromReqCode( int reqCode );

    void insertFunctionToResCode(int resCode, const QString& functionName );
    QString getFunctionNameFromResCode( int resCode );

signals:

public:
    virtual void init();

public slots:
    virtual void doRun() = 0;
    virtual void stopModule();

public slots:
    void getRequest(int reqCode, int sender, bool response, QVariantMap reqPacket );
    void getResponse( int resCode, int sender, QVariantMap resPacket );

private:
    bool m_isSet;
    bool m_bCancelEvent;
    bool m_isStop;

    bool m_isIndependentModule;

    QHash< int, QString > m_mapReqCodeToFuncName;
    QHash< int, QString > m_mapResCodeToFuncName;
};

#endif // QBASEMODULE_H
