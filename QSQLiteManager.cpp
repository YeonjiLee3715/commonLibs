#include "QSQLiteManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QJsonArray>
#include <QFile>
#include <QDateTime>
#include <QVariant>
#include <QResource>

#include <sys/types.h>
#if __linux
#include <sys/syscall.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>       // Or something like it.
#endif

#include <CLogger.h>
#include <define/cmnDef.h>

#define DB_SET_DATA_PATH ":/db/DB.json"

#define QUERY_UPDATE_REVISION QString( "INSERT OR REPLACE INTO TB_DB_INFO VALUES(\'REVISION\', \'%1\')" )
#define QUERY_UPDATE_LASTUPDATE QString( "INSERT OR REPLACE INTO TB_DB_INFO VALUES(\'LASTUPDATE\', \'%1\')" )

#define QUERY_SELECT_REVISION QString( "SELECT INFO_VALUE FROM TB_DB_INFO WHERE INFO_KEY = \'REVISION\'" )

#define QUERY_PRAGMA_KEY QString( "pragma key=\'%1\'" )

namespace nsQSQLiteManager{
    const char* TAG = "QSQLiteManager";
}

using namespace nsQSQLiteManager;

QSQLiteManager::QSQLiteManager( QObject *parent) : QObject(parent)
  , m_currentRevision(-1), m_isPassword( false )
{
//    Q_ASSERT(QSqlDatabase::isDriverAvailable("QSQLITE")); // from Qt
//    Q_ASSERT(QSqlDatabase::isDriverAvailable("QSQLCIPHER")); // from QSQLCIPHER plugin
}

QSQLiteManager::~QSQLiteManager()
{
    for( QVector<QString>::iterator it = m_vecConnections.begin();
         it != m_vecConnections.end(); )
    {
        if( QSqlDatabase::contains( (*it) ) )
        {
            bool isOpen = false;

            {
                QSqlDatabase db = QSqlDatabase::database( (*it), false );
                isOpen = isOpenDB( db );

                if( isOpen )
                    db.close();
            }

            if( isOpen )
                QSqlDatabase::removeDatabase( (*it) );
        }

        it = m_vecConnections.erase( it );
    }
}

void QSQLiteManager::setDBInfo(const QString &path)
{
    m_strDBName = path;
    m_strDBPass.clear();
    m_isPassword = false;
}

void QSQLiteManager::setDBInfo(const QString &path, const QString &pass)
{
    m_strDBName = path;
    m_strDBPass = pass;
    m_isPassword = true;
}

bool QSQLiteManager::openDB( QSqlDatabase& db )
{
    db.setDatabaseName( m_strDBName );

    if( db.open() == false )
        LOGE( TAG, "Failed to open database. error(%d): %s", (int)db.lastError().type(), db.lastError().text().toStdString().c_str() );

    return isOpenDB( db );
}

QString QSQLiteManager::getConnectionName( const QString& connectionName )
{
    QString strConnectinName = connectionName;

#include <sys/types.h>
#if __linux
    if( strConnectinName.isEmpty() )
         strConnectinName = "connection" + QString::number( syscall(SYS_gettid) );
#elif defined(_WIN32) || defined(_WIN64)
    if( strConnectinName.isEmpty() )
         strConnectinName = "connection" + QString::number( static_cast<long int>(GetCurrentThreadId()) );
#endif
    return strConnectinName;
}

bool QSQLiteManager::isOpenDB( const QSqlDatabase& db )
{
    return db.isOpen() && db.isValid();
}

bool QSQLiteManager::updateDB( QSqlDatabase &db, QJsonObject& jsObjDBSetData, int updateRevision )
{
    bool isSuccess = false;

    QSqlQuery sqlQuery( db );

    const QJsonArray& jsArrUpdate = jsObjDBSetData["UPDATE"].toArray();
    if( jsArrUpdate.isEmpty() )
        return isSuccess;

    for( int rev = m_currentRevision; rev < updateRevision; ++rev )
    {
        const QJsonArray& jsArrRev = jsArrUpdate.at(rev+1).toArray();
        if( jsArrRev.isEmpty() )
            return isSuccess;

        for( int idx = 0; idx < jsArrRev.size(); ++idx )
        {
            const QJsonObject& jsObjQuery = jsArrRev.at(idx).toObject();
            if( jsObjQuery.isEmpty() )
                return isSuccess;

            const QString& strQuery = jsObjQuery["QUERY"].toString();
            if( strQuery.isEmpty() )
                return isSuccess;

            if( sqlQuery.exec( strQuery ) == false )
            {
                sqlQuery.finish();
                return isSuccess;
            }

            sqlQuery.finish();
        }
    }

    isSuccess = true;
    return isSuccess;
}

bool QSQLiteManager::createDB( QSqlDatabase& db, QJsonObject& jsObjDBSetData )
{
    bool isSuccess = false;

    QSqlQuery sqlQuery( db );
    const QJsonArray& jsArrCreate = jsObjDBSetData["CREATE"].toArray();
    if( jsArrCreate.isEmpty() )
    {
        LOGE( TAG, "Failed to get create object array" );
        return isSuccess;
    }

    for( int idx = 0; idx < jsArrCreate.size(); ++idx )
    {
        const QJsonObject& jsObjQuery = jsArrCreate.at(idx).toObject();
        if( jsObjQuery.isEmpty() )
        {
            LOGE( TAG, "Failed to get create object" );
            return isSuccess;
        }

        const QString& strQuery = jsObjQuery["QUERY"].toString();
        if( strQuery.isEmpty() )
        {
            LOGE( TAG, "Failed to get query object" );
            return isSuccess;
        }

        if( sqlQuery.exec( strQuery ) == false )
        {
            sqlQuery.finish();
            LOGE( TAG, "Failed to get execute create query: %s\n\terror(%d): %s"
                  , sqlQuery.executedQuery().toStdString().c_str()
                  , (int)sqlQuery.lastError().type()
                  , sqlQuery.lastError().text().toStdString().c_str() );
            return isSuccess;
        }

        sqlQuery.finish();
    }

    isSuccess = true;
    return isSuccess;
}

QJsonDocument QSQLiteManager::getDBSetData()
{
    QFile file( DB_SET_DATA_PATH );
    if( file.open( QIODevice::ReadOnly ) == false )
        return QJsonDocument();

    QByteArray buffer = file.readAll();
    QJsonDocument jsDocDBSetData = QJsonDocument::fromJson( QByteArray( reinterpret_cast<const char*>( buffer.data() ), buffer.size() ) );

    return jsDocDBSetData;
}

bool QSQLiteManager::getConnection( QSqlDatabase& db, const QString &connectionName)
{
    bool isSuccess = false;

    QString strConnectinName = getConnectionName( connectionName );
    if( QSqlDatabase::contains( strConnectinName ) )
    {
        db = QSqlDatabase::database( strConnectinName, true );
        isSuccess = isOpenDB( db );
        if( isSuccess == false )
        {
            LOGE( TAG, "Failed to open database. error(%d): %s"
                  , (int)db.lastError().type()
                  , db.lastError().text().toStdString().c_str() );
        }
    }
    else
    {
        if( isPassword() )
        {
            db = QSqlDatabase::addDatabase("QSQLCIPHER", strConnectinName);
        }
        else
            db = QSqlDatabase::addDatabase("QSQLITE", strConnectinName);

        isSuccess = openDB( db );
        if( isSuccess == false )
        {
            LOGE( TAG, "Failed to open database." );
        }
        else
        {
            if( isPassword() )
            {
                QSqlQuery sqlQuery( db );
                isSuccess = sqlQuery.exec( QUERY_PRAGMA_KEY.arg( m_strDBPass ) );
                if( isSuccess == false )
                {
                    LOGE( TAG, "Failed to get execute create query: %s\n\terror(%d): %s"
                          ,  sqlQuery.executedQuery().toStdString().c_str()
                          , (int)sqlQuery.lastError().type()
                          , sqlQuery.lastError().text().toStdString().c_str() );
                    removeConnection( strConnectinName );
                }

                sqlQuery.finish();
            }
        }

        if( isSuccess && QSqlDatabase::contains( strConnectinName )
                && m_vecConnections.contains( strConnectinName ) == false )
        {
            m_vecConnections.push_back( strConnectinName );
        }
    }

    return isSuccess;
}

void QSQLiteManager::removeConnection(const QString &connectionName)
{
    QString strConnectinName = getConnectionName( connectionName );

    if( QSqlDatabase::contains( strConnectinName ) )
    {
        bool isOpen = false;

        {
            QSqlDatabase db = QSqlDatabase::database( strConnectinName, false );
            isOpen = isOpenDB( db );

            if( isOpen )
                db.close();
        }

        if( isOpen )
            QSqlDatabase::removeDatabase( strConnectinName );
    }

    int idx = m_vecConnections.indexOf( strConnectinName );
    if( idx >= 0 )
        m_vecConnections.remove( idx );
}

bool QSQLiteManager::isPassword()
{
    return m_isPassword;
}

bool QSQLiteManager::checkRevision()
{
    bool isSuccess = false;

    QSqlDatabase db;

    if( getConnection( db ) == false )
    {
        LOGE( TAG, "Failed to get DB connection" );
        return isSuccess;
    }

    if( db.transaction() == false )
    {
        LOGE( TAG, "Failed to get DB transaction" );
        return isSuccess;
    }

    QJsonDocument jsDocDBSetData = getDBSetData();
    if( jsDocDBSetData.isNull() || jsDocDBSetData.isEmpty() )
    {
        LOGE( TAG, "Failed to get DB set data" );
        return isSuccess;
    }

    QJsonObject jsonObject = jsDocDBSetData.object();
    int updateRevision = jsonObject["REVISION"].toInt();

    QSqlQuery sqlQuery( db );
    if( sqlQuery.exec( QUERY_SELECT_REVISION ) )
    {
        if( sqlQuery.next() )
            m_currentRevision = sqlQuery.value( QString( "INFO_VALUE" ) ).toString().toInt();

        sqlQuery.finish();
    }

    if( m_currentRevision < 0 )
        isSuccess = createDB( db, jsonObject );
    else if( m_currentRevision == updateRevision )
        isSuccess = true;
    else
        isSuccess = updateDB( db, jsonObject, updateRevision);

    if( isSuccess && m_currentRevision != updateRevision )
    {
        LOGI( TAG, "m_currentRevision: %d, updateRevision: %d", m_currentRevision, updateRevision );
        isSuccess = sqlQuery.exec( QUERY_UPDATE_REVISION.arg( QString::number( updateRevision ) ) );
        isSuccess &= sqlQuery.exec( QUERY_UPDATE_LASTUPDATE.arg( QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss" ) ) );
        sqlQuery.finish();
    }

    if( isSuccess )
    {
        m_currentRevision = updateRevision;
        db.commit();
    }
    else
        db.rollback();

    return isSuccess;
}
