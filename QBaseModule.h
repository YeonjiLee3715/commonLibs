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

class QThread;
class QModuleManager;
class QBaseModule : public QObject
{
    Q_OBJECT
public:
    QBaseModule(QObject *parent = nullptr);
    virtual ~QBaseModule();

private:
    friend QThread;
    friend QModuleManager;

public:
    typedef enum eInitMethod{INIT_MODULE_MANAGER = 0, INIT_MANUAL} eInitMethod;

private:
    void connectThreadSignals( QThread* pThModule );
    void disconnectThreadSignals( QThread* pThModule );

protected:
    void setIndependentModule( bool isIndependentModule );

    /*! If the initialization of the independent module is done manually,
     * the thread must also be run manually.*/
    void setInitMethod( eInitMethod eInitMethod );
    void setIsSet( bool isSet );
    void setStop( bool isStop );

    void insertFunctionToReqCode(int reqCode, const QString& functionName );
    void insertFunctionToResCode(int resCode, const QString& functionName );

    QString getFunctionNameFromReqCode( int reqCode );
    QString getFunctionNameFromResCode( int resCode );

public:
    bool IsSet();
    eInitMethod InitMethod();
    bool IsIndependentModule();
    bool IsStop();
    bool IsStopped();

signals:

public:
    virtual void init();

private slots:
    void moduleStopped();

public slots:
    virtual void doRun() = 0;
    virtual void stopModule();

public slots:
    void getRequest(int reqCode, int sender, bool response, QVariantMap reqPacket );
    void getResponse( int resCode, int sender, QVariantMap resPacket );

private:
    bool m_isSet;
    bool m_isStop;
    bool m_isStopped;

    eInitMethod m_eInitMethod;
    bool m_isIndependentModule;

    QHash< int, QString > m_mapReqCodeToFuncName;
    QHash< int, QString > m_mapResCodeToFuncName;
};

#endif // QBASEMODULE_H
