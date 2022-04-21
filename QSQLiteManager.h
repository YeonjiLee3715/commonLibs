#ifndef QSQLITEMODULE_H
#define QSQLITEMODULE_H

#include <QObject>
#include <QSqlDatabase>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVector>

//TODO: crypto 가능한 라이브러리 적용

class QSQLiteManager : public QObject
{
    Q_OBJECT
public:
    explicit QSQLiteManager(QObject *parent = nullptr);
    ~QSQLiteManager();

    void setDBInfo( const QString& path );
    void setDBInfo(const QString& path, const QString& pass );

    bool checkRevision();

    bool getConnection(QSqlDatabase &db, const QString& connectionName = "" );
    void removeConnection( const QString& connectionName = "" );

    bool isPassword();

public:
    static bool isOpenDB(const QSqlDatabase &db);

private:

    bool openDB( QSqlDatabase &db );

    bool updateDB( QSqlDatabase &db, QJsonObject& jsObjDBSetData, int updateRevision );
    bool createDB( QSqlDatabase &db, QJsonObject& jsObjDBSetData );

    QString getConnectionName( const QString& connectionName = "" );

    QJsonDocument getDBSetData();

signals:

public slots:

private:
    QString m_strDBName;
    QString m_strDBPass;
    bool m_isPassword;

    QVector<QString> m_vecConnections;

    int m_currentRevision;
};

#endif // QSQLITEMODULE_H
