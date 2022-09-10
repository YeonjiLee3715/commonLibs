#ifndef QVARIANTCONVERTER_H
#define QVARIANTCONVERTER_H

#include <QtGlobal>
#include <QObject>
#include <string>
#include <QVariant>
#include <QVector>
#ifdef QT_DBUS_LIB
#include <QDBusArgument>
#endif

#define SET_TO_MAP( MAPPARAMS, KEY, VAR ) \
    QVariantConverter::SetToMap( MAPPARAMS, KEY, VAR )

#define GET_FROM_MAP( MAPPARAMS, KEY, VAR, ... ) \
    QVariantConverter::GetFromMap( MAPPARAMS, KEY, VAR, ##__VA_ARGS__ )

class QVariantConverter
{
public:
    QVariantConverter();

    static void                         SetToMap(QVariantMap& mapParams, const std::string& key, QVariant value );
    static void                         SetToMap(QVariantMap& mapParams, const std::string& key, const int& nValue );
    static void                         SetToMap(QVariantMap& mapParams, const std::string& key, const long& nValue );
    static void                         SetToMap(QVariantMap& mapParams, const std::string& key, const std::string& strValue );
    static void                         SetToMap(QVariantMap& mapParams, const std::string& key, const QString& strValue );
    static void                         SetToMap(QVariantMap& mapParams, const std::string& key, const bool& bValue );
    static void                         SetToMap(QVariantMap& mapParams, const std::string& key, const long long& lldValue );
    static void                         SetToMap(QVariantMap& mapParams, const std::string& key, const unsigned long long& ulldValue );
    static void                         SetToMap(QVariantMap& mapParams, const std::string& key, const double& fValue );
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
    static void                         SetToMap(QVariantMap& mapParams, const std::string& key, const QList<int>& vecData );
#endif
    static void                         SetToMap(QVariantMap& mapParams, const std::string& key, const QVector<int>& vecData );
    static void                         SetToMap(QVariantMap& mapParams, const std::string& key, const std::vector<int>& vecData );
    static void                         SetToMap(QVariantMap& mapParams, const std::string& key, const std::vector<std::string>& vecData );
    static void                         SetToMap(QVariantMap& mapParams, const std::string& key, const std::map<std::string, std::string>& mapData );
    static void                         SetToMap(QVariantMap& mapParams, const std::string& key, const std::map<std::string, int>& mapData );
    static void                         SetToMap(QVariantMap& mapParams, const std::string& key, const std::map<int, int>& mapData );
    static void                         SetToMap(QVariantMap& mapParams, const std::string& key, const std::map<int, std::string>& mapData );

    static bool                         GetFromMap( const QVariantMap& mapParams, const std::string& key, QVariant& var
                                                    , QVariant defaultValue = QVariant() );
    static bool                         GetFromMap( const QVariantMap& mapParams, const std::string& key, int& var
                                                    , int defaultValue = 0 );
    static bool                         GetFromMap( const QVariantMap& mapParams, const std::string& key, long& var
                                                    , long defaultValue = 0 );
    static bool                         GetFromMap( const QVariantMap& mapParams, const std::string& key, std::string& var
                                                    , std::string defaultValue = "" );
    static bool                         GetFromMap( const QVariantMap& mapParams, const std::string& key, QString& var
                                                    , QString defaultValue = "" );
    static bool                         GetFromMap( const QVariantMap& mapParams, const std::string& key, bool& var
                                                    , bool defaultValue = false );
    static bool                         GetFromMap( const QVariantMap& mapParams, const std::string& key, long long& var
                                                    , qlonglong defaultValue = 0 );
    static bool                         GetFromMap( const QVariantMap& mapParams, const std::string& key, unsigned long long& var
                                                    , qulonglong defaultValue = 0 );
    static bool                         GetFromMap( const QVariantMap& mapParams, const std::string& key, double& var
                                                    , double defaultValue = 0 );
#ifdef QT_DBUS_LIB
    static bool                         GetFromMap( const QVariantMap& mapParams, const std::string& key, QDBusArgument& var
                                                    , const QDBusArgument& defaultValue = QDBusArgument() );
#endif
    static bool                         GetFromMap( const QVariantMap& mapParams, const std::string& key, QVariantMap& var
                                                    , const QVariantMap& defaultValue = QVariantMap() );
    static bool                         GetFromMap( const QVariantMap& mapParams, const std::string& key, QVariantHash& var
                                                    , const QVariantHash& defaultValue = QVariantHash() );
    static bool                         GetFromMap( const QVariantMap& mapParams, const std::string& key, std::vector<int>& var
                                                    , const std::vector<int>& defaultValue = std::vector<int>() );
    static bool                         GetFromMap( const QVariantMap& mapParams, const std::string& key, std::vector<std::string>& var
                                                    , const std::vector<std::string>& defaultValue = std::vector<std::string>() );
    static bool                         GetFromMap( const QVariantMap& mapParams, const std::string& key, QVariantList& var
                                                    , const QVariantList& defaultValue = QVariantList() );

    static bool                         compareVariant( const QVariant & arg1, const QVariant & arg2 );

#ifdef QT_DBUS_LIB
    static QVariantMap                  ConvertVarDBusArgToQVarMap( const QVariant& var );
    static QVariantMap                  ConvertDBusArgToQVarMap( const QDBusArgument& dbusArg );
#endif
};

#endif // QVARIANTCONVERTER_H
