#include "QVariantConverter.h"

QVariantConverter::QVariantConverter()
{

}

void QVariantConverter::SetToMap( QVariantMap& mapParams, const std::string& key, QVariant value )
{
    mapParams[ QString::fromStdString( key ) ] = value;
}

void QVariantConverter::SetToMap(QVariantMap& mapParams, const std::string& key, const int& nValue)
{
    mapParams[ QString::fromStdString( key ) ] = nValue;
}

void QVariantConverter::SetToMap(QVariantMap& mapParams, const std::string& key, const long& nValue)
{
    mapParams[ QString::fromStdString( key ) ] = static_cast<int>( nValue );
}

void QVariantConverter::SetToMap(QVariantMap& mapParams, const std::string& key, const std::string& strValue)
{
    mapParams[ QString::fromStdString( key ) ] = QString::fromStdString( strValue );
}

void QVariantConverter::SetToMap(QVariantMap& mapParams, const std::string& key, const QString& strValue)
{
    mapParams[ QString::fromStdString( key ) ] = strValue;
}

void QVariantConverter::SetToMap(QVariantMap& mapParams, const std::string& key, const bool& bValue)
{
    mapParams[ QString::fromStdString( key ) ] = bValue;
}

void QVariantConverter::SetToMap(QVariantMap& mapParams, const std::string& key, const long long& lldValue)
{
    mapParams[ QString::fromStdString( key ) ] = static_cast<qlonglong>( lldValue );
}

void QVariantConverter::SetToMap(QVariantMap& mapParams, const std::string& key, const unsigned long long& ulldValue)
{
    mapParams[ QString::fromStdString( key ) ] = static_cast<qulonglong>( ulldValue );
}

void QVariantConverter::SetToMap(QVariantMap& mapParams, const std::string& key, const double& fValue)
{
    mapParams[ QString::fromStdString( key ) ] = fValue;
}

#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
void QVariantConverter::SetToMap(QVariantMap& mapParams, const std::string& key, const QList<int>& lstData)
{
    QVariantList lstVar;

    for( QList<int>::const_iterator it = lstData.cbegin(); it != lstData.cend(); ++it )
        lstVar << (*it);

    mapParams[ QString::fromStdString( key ) ] = lstVar;
}
#endif

void QVariantConverter::SetToMap(QVariantMap& mapParams, const std::string& key, const QVector<int>& vecData)
{
    QVariantList lstVar;

    for( QVector<int>::const_iterator it = vecData.cbegin(); it != vecData.cend(); ++it )
        lstVar << (*it);

    mapParams[ QString::fromStdString( key ) ] = lstVar;
}

void QVariantConverter::SetToMap(QVariantMap& mapParams, const std::string& key, const std::vector<int>& vecData)
{
    QVariantList lstVar;

    for( std::vector<int>::const_iterator it = vecData.cbegin(); it != vecData.cend(); ++it )
        lstVar << (*it);

    mapParams[ QString::fromStdString( key ) ] = lstVar;
}

void QVariantConverter::SetToMap(QVariantMap& mapParams, const std::string& key, const std::vector<std::string>& vecData)
{
    QVariantList lstVar;

    for( std::vector<std::string>::const_iterator it = vecData.cbegin(); it != vecData.cend(); ++it )
        lstVar << QString::fromStdString( *it );

    mapParams[ QString::fromStdString( key ) ] = lstVar;
}

void QVariantConverter::SetToMap(QVariantMap& mapParams, const std::string& key, const std::map<std::string, std::string>& mapData)
{
    QVariantMap mapVar;

    for( std::map<std::string, std::string>::const_iterator it = mapData.cbegin(); it != mapData.cend(); ++it )
        mapVar[ QString::fromStdString( it->first ) ] = QString::fromStdString( it->second );

    mapParams[ QString::fromStdString( key ) ] = mapVar;
}

void QVariantConverter::SetToMap(QVariantMap& mapParams, const std::string& key, const std::map<std::string, int>& mapData)
{
    QVariantMap mapVar;

    for( std::map<std::string, int>::const_iterator it = mapData.cbegin(); it != mapData.cend(); ++it )
        mapVar[ QString::fromStdString( it->first ) ] = QString::number( it->second );

    mapParams[ QString::fromStdString( key ) ] = mapVar;
}

void QVariantConverter::SetToMap(QVariantMap& mapParams, const std::string& key, const std::map<int, int>& mapData)
{
    QVariantMap mapVar;

    for( std::map<int, int>::const_iterator it = mapData.cbegin(); it != mapData.cend(); ++it )
        mapVar[ QString::number( it->first ) ] = QString::number( it->second );

    mapParams[ QString::fromStdString( key ) ] = mapVar;
}

void QVariantConverter::SetToMap(QVariantMap& mapParams, const std::string& key, const std::map<int, std::string>& mapData)
{
    QVariantMap mapVar;

    for( std::map<int, std::string>::const_iterator it = mapData.cbegin(); it != mapData.cend(); ++it )
        mapVar[ QString::number( it->first ) ] = QString::fromStdString( it->second );

    mapParams[ QString::fromStdString( key ) ] = mapVar;
}

bool QVariantConverter::GetFromMap( const QVariantMap& mapParams, const std::string& key, QVariant& var, QVariant defaultValue )
{
    bool isSucccess = false;

    if( mapParams.contains( QString::fromStdString( key ) ) )
    {
        var = mapParams[ QString::fromStdString( key ) ];
        isSucccess = true;
    }
    else
        var = defaultValue;

    return isSucccess;
}

bool QVariantConverter::GetFromMap(const QVariantMap& mapParams, const std::string& key, int& var, int defaultValue)
{
    bool isSucccess = false;

    QVariantMap::const_iterator it = mapParams.find( QString::fromStdString( key ) );
    if( it != mapParams.end() && it.value().isNull() == false
        && it.value().isValid() && it.value().type() == QVariant::Int )
    {
        var = it.value().toInt( &isSucccess );
    }

    if( isSucccess == false )
        var = defaultValue;

    return isSucccess;
}

bool QVariantConverter::GetFromMap(const QVariantMap& mapParams, const std::string& key, long& var, long defaultValue)
{
    bool isSucccess = false;

    QVariantMap::const_iterator it = mapParams.find( QString::fromStdString( key ) );
    if( it != mapParams.end() && it.value().isNull() == false
        && it.value().isValid() && it.value().type() == QVariant::Int )
    {
        var = static_cast<long>( it.value().toInt( &isSucccess ) );
    }

    if( isSucccess == false )
        var = defaultValue;

    return isSucccess;
}

bool QVariantConverter::GetFromMap(const QVariantMap& mapParams, const std::string& key, std::string& var, std::string defaultValue)
{
    QString strVal;
    bool isSucccess = GetFromMap( mapParams, key, strVal );
    if( isSucccess == false )
    {
        var = defaultValue;
        return isSucccess;
    }

    var = strVal.toStdString();

    return isSucccess;
}

bool QVariantConverter::GetFromMap(const QVariantMap& mapParams, const std::string& key, QString& var, QString defaultValue)
{
    bool isSucccess = false;

    QVariantMap::const_iterator it = mapParams.find( QString::fromStdString( key ) );
    if( it != mapParams.end() && it.value().isNull() == false
        && it.value().isValid() && it.value().type() == QVariant::String )
    {
        var = it.value().toString();
        isSucccess = true;
    }
    else
        var = defaultValue;

    return isSucccess;
}

bool QVariantConverter::GetFromMap( const QVariantMap& mapParams, const std::string& key, bool& var, bool defaultValue )
{
    bool isSucccess = false;

    QVariantMap::const_iterator it = mapParams.find( QString::fromStdString( key ) );
    if( it != mapParams.end() && it.value().isNull() == false
        && it.value().isValid() && it.value().type() == QVariant::Bool )
    {
        var = it.value().toBool();
        isSucccess = true;
    }
    else
        var = defaultValue;

    return isSucccess;
}

bool QVariantConverter::GetFromMap(const QVariantMap& mapParams, const std::string& key, long long& var, qlonglong defaultValue)
{
    bool isSucccess = false;

    QVariantMap::const_iterator it = mapParams.find( QString::fromStdString( key ) );
    if( it != mapParams.end() && it.value().isNull() == false
        && it.value().isValid() && it.value().type() == QVariant::LongLong )
    {
        var = it.value().toLongLong( &isSucccess );
    }

    if( isSucccess == false )
        var = defaultValue;

    return isSucccess;
}

bool QVariantConverter::GetFromMap(const QVariantMap& mapParams, const std::string& key, unsigned long long& var, qulonglong defaultValue)
{
    bool isSucccess = false;

    QVariantMap::const_iterator it = mapParams.find( QString::fromStdString( key ) );
    if( it != mapParams.end() && it.value().isNull() == false
        && it.value().isValid() && it.value().type() == QVariant::ULongLong )
    {
        var = it.value().toULongLong( &isSucccess );
    }

    if( isSucccess == false )
        var = defaultValue;

    return isSucccess;
}

bool QVariantConverter::GetFromMap(const QVariantMap& mapParams, const std::string& key, double& var, double defaultValue)
{
    bool isSucccess = false;

    QVariantMap::const_iterator it = mapParams.find( QString::fromStdString( key ) );
    if( it != mapParams.end() && it.value().isNull() == false
        && it.value().isValid() && it.value().type() == QVariant::Double )
    {
        var = it.value().toDouble( &isSucccess );
    }

    if( isSucccess == false )
        var = defaultValue;

    return isSucccess;
}

#ifdef QT_DBUS_LIB
bool QVariantConverter::GetFromMap(const QVariantMap& mapParams, const std::string& key, QDBusArgument& var, const QDBusArgument& defaultValue)
{
    bool isSucccess = false;

    QVariantMap::const_iterator it = mapParams.find( QString::fromStdString( key ) );
    if( it != mapParams.end() && it.value().isNull() == false
        && it.value().isValid() )
    {
        var = it.value().value<QDBusArgument>();
        isSucccess = true;
    }
    else
        var = defaultValue;

    return isSucccess;
}
#endif

bool QVariantConverter::GetFromMap(const QVariantMap& mapParams, const std::string& key, QVariantMap& var, const QVariantMap& defaultValue)
{
    bool isSucccess = false;

    QVariantMap::const_iterator it = mapParams.find( QString::fromStdString( key ) );
    if( it != mapParams.end() && it.value().isNull() == false
        && it.value().isValid() )
    {
        if( it.value().type() == QVariant::Map )
        {
            var = it.value().toMap();
            isSucccess = true;
        }
#ifdef QT_DBUS_LIB
        else if( it.value().type() == QVariant::UserType )
        {
            const QVariantMap& mapVarParams = qdbus_cast<QVariantMap>( it.value().value<QDBusArgument>() );
            var = mapVarParams;
            isSucccess = true;
        }
#endif
    }
    else
        var = defaultValue;

    return isSucccess;
}

bool QVariantConverter::GetFromMap(const QVariantMap& mapParams, const std::string& key, QVariantHash& var, const QVariantHash& defaultValue)
{
    bool isSucccess = false;

    QVariantMap::const_iterator it = mapParams.find( QString::fromStdString( key ) );
    if( it != mapParams.end() && it.value().isNull() == false
        && it.value().isValid() )
    {
        if( it.value().type() == QVariant::Map )
        {
            var = it.value().toHash();
            isSucccess = true;
        }
#ifdef QT_DBUS_LIB
        else if( it.value().type() == QVariant::UserType )
        {
            const QVariantHash& mapVarParams = qdbus_cast<QVariantHash>( it.value().value<QDBusArgument>() );
            var = mapVarParams;
            isSucccess = true;
        }
#endif
    }
    else
        var = defaultValue;

    return isSucccess;
}

bool QVariantConverter::GetFromMap(const QVariantMap& mapParams, const std::string& key, std::vector<int>& var, const std::vector<int>& defaultValue)
{
    QVariantList lstVal;
    bool isSucccess = GetFromMap( mapParams, key, lstVal );

    if( isSucccess == false )
    {
        var = defaultValue;
        return isSucccess;
    }

    var.clear();
    for( QVariantList::const_iterator it = lstVal.cbegin(); it != lstVal.cend(); ++it )
    {
        int nValue = 0;

        if( it->isNull() == false && it->isValid() &&
            it->type() == QVariant::Int )
        {
            nValue = it->toInt( &isSucccess );
        }
        else
            isSucccess = false;

        if( isSucccess == false )
            break;

        var.push_back( nValue );
    }

    if( isSucccess == false )
    {
        var = defaultValue;
        return isSucccess;
    }

    return isSucccess;
}

bool QVariantConverter::GetFromMap(const QVariantMap& mapParams, const std::string& key, std::vector<std::string>& var, const std::vector<std::string>& defaultValue)
{
    QVariantList lstVal;
    bool isSucccess = GetFromMap( mapParams, key, lstVal );

    if( isSucccess == false )
    {
        var = defaultValue;
        return isSucccess;
    }

    var.clear();
    for( QVariantList::const_iterator it = lstVal.cbegin(); it != lstVal.cend(); ++it )
    {
        std::string strValue;

        if( it->isNull() == false && it->isValid() &&
            it->type() == QVariant::String )
        {
            strValue = it->toString().toStdString();
        }
        else
            isSucccess = false;

        if( isSucccess == false )
            break;

        var.push_back( strValue );
    }

    if( isSucccess == false )
    {
        var = defaultValue;
        return isSucccess;
    }

    return isSucccess;
}

bool QVariantConverter::GetFromMap(const QVariantMap& mapParams, const std::string& key, QVariantList& var, const QVariantList& defaultValue)
{
    bool isSucccess = false;

    QVariantMap::const_iterator it = mapParams.find( QString::fromStdString( key ) );
    if( it != mapParams.end() && it.value().isNull() == false
        && it.value().isValid() )
    {
        if( it.value().type() == QVariant::List )
        {
            var = it.value().toList();
            isSucccess = true;
        }
#ifdef QT_DBUS_LIB
        else if( it.value().type() == QVariant::UserType )
        {
            const QVariantList& lstVarParams = qdbus_cast<QVariantList>( it.value().value<QDBusArgument>() );
            var = lstVarParams;
            isSucccess = true;
        }
#endif
    }
    else
        var = defaultValue;

    return isSucccess;
}

bool QVariantConverter::compareVariant( const QVariant & arg1, const QVariant & arg2 )
{
    bool isMatch = false;

    if( arg1.isNull() && arg1.isValid() == false && arg2.isNull() && arg2.isValid() == false )
    {
        isMatch = true;
        return isMatch;
    }

    if( arg1.isNull() || arg1.isValid() == false || arg2.isNull() || arg2.isValid() == false )
        return isMatch;

    if( arg1.type() != arg2.type() )
        return isMatch;

    if( arg1.type() == QVariant::String )
    {
        if( arg1.toString().compare( arg2.toString(), Qt::CaseInsensitive ) == 0 )
            isMatch = true;
    }
    else if( arg1.type() == QVariant::Bool )
    {
        if( arg1.toBool() == arg2.toBool() )
            isMatch = true;
    }
    else if( arg1.type() == QVariant::Int )
    {
        bool isOk1 = false, isOk2 = false;
        if( arg1.toInt( &isOk1 ) == arg2.toInt( &isOk2 ) && isOk1 && isOk2 )
            isMatch = true;
    }
    else if( arg1.type() == QVariant::LongLong )
    {
        bool isOk1 = false, isOk2 = false;
        if( arg1.toLongLong( &isOk1 ) == arg2.toLongLong( &isOk2 ) && isOk1 && isOk2 )
            isMatch = true;
    }
    else if( arg1.type() == QVariant::Double )
    {
        bool isOk1 = false, isOk2 = false;
        if( arg1.toDouble( &isOk1 ) == arg2.toDouble( &isOk2 ) && isOk1 && isOk2 )
            isMatch = true;
    }

    return isMatch;
}

#ifdef QT_DBUS_LIB
QVariantMap QVariantConverter::ConvertVarDBusArgToQVarMap( const QVariant& var )
{
    return qdbus_cast<QVariantMap>( var.value<QDBusArgument>() );
}

QVariantMap QVariantConverter::ConvertDBusArgToQVarMap( const QDBusArgument& dbusArg )
{
    return qdbus_cast<QVariantMap>( dbusArg );
}
#endif
