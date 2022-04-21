//
// Created by 이연지 on 2017-06-01.
//

#ifndef CBASEMODULE_H
#define CBASEMODULE_H


class CBaseClass
{
public:
    CBaseClass();
    virtual ~CBaseClass(){};

    virtual bool init(){ return true; };

    void setStatus( bool isSet ){ m_isSet = isSet; };
    bool IsSet(){ return m_isSet; };

private:
    bool m_isSet;
};


#endif //CBASEMODULE_H


#ifndef CBASEMODULE_H
#define CBASEMODULE_H

#include <stdlib.h>
#include <QObject>
#include <QReadWriteLock>
#include <QHash>

#include "proto/module.pb.h"
Q_DECLARE_METATYPE( module::requestPacket )

class QBaseModule : public QObject
{
    Q_OBJECT
public:
    QBaseModule(QObject *parent = nullptr);
    ~QBaseModule();

    void setIsSet( bool isSet ){ m_isSet = isSet; }
    bool IsSet(){ return m_isSet; }

    void setStop( bool isStop ){ m_isStop = isStop; }
    bool IsStop(){ return m_isStop; }

    void setIndependentModule( bool isIndependentModule ){ m_isIndependentModule = isIndependentModule; }
    bool IsIndependentModule(){ return m_isIndependentModule; }

    void insertFunctionToReqCode(int reqCode, const QString& functionName );
    QString getFunctionNameFromReqCode( int reqCode );

    void insertFunctionToResCode(int resCode, const QString& functionName );
    QString getFunctionNameFromResCode( int resCode );

signals:

public:
    virtual void init() = 0;

public slots:
    virtual void doRun() = 0;
    virtual void stop() = 0;

public slots:
    void getRequest( int reqCode, int sender, bool response, module::requestPacket reqPacket );
    void getResponse( int resCode, int sender, module::responsePacket resPacket );

private:
    bool m_isSet;
    bool m_isStop;

    bool m_isIndependentModule;
    QString m_moduleName;

    QHash< int, QString > m_mapReqCodeToFuncName;
    QHash< int, QString > m_mapResCodeToFuncName;
};

#endif // CBASEMODULE_H
