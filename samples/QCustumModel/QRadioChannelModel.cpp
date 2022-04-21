#include "QRadioChannelModel.h"

#include <QMetaEnum>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  RadioChannelData

namespace nsQRadioChannelModel {
    const char* TAG = "QRadioChannelModel";
}
using namespace nsQRadioChannelModel;

RadioChannelData::RadioChannelData(QObject *parent)
    :QObject(parent)
{

}

RadioChannelData::RadioChannelData(const RadioChannelData& data, QObject *parent)
    :QObject(parent)
{
    m_title = data.getTitle();
    m_imgSource = data.getImgSource();
    m_linkUrl = data.getLinkUrl();
}

RadioChannelData::~RadioChannelData()
{
}

void RadioChannelData::setData(const QString& fieldName, QVariant value )
{
    if( value.isNull() || value.isValid() == false )
    {
        QLOGE( TAG, ("Wropng data, "+fieldName) );
        return;
    }

    int field = QMetaEnum::fromType<eDataField>().keysToValue(fieldName.toStdString().c_str());

    switch (field) {
    case eDataField::FIELD_TITLE:
        m_title = value.value<QString>();
        break;
    case eDataField::FIELD_IMAGESOURCE:
        m_imgSource = value.value<QString>();
        break;
    case eDataField::FIELD_LINKURL:
        m_linkUrl = value.value<QString>();
        break;
    default:
        break;
    }
}

QVariant RadioChannelData::getData(int field ) const
{
    QVariant value;

    switch (field) {
    case eDataField::FIELD_TITLE:
        value.setValue<QString>( m_title );
        break;
    case eDataField::FIELD_IMAGESOURCE:
        value.setValue<QString>( m_imgSource );
        break;
    case eDataField::FIELD_LINKURL:
        value.setValue<QString>( m_linkUrl );
        break;
    default:
        break;
    }

    return value;
}

bool RadioChannelData::isEmpty() const
{
    bool isEmpty = true;

    do
    {
        if(m_title.isEmpty() == false)
        {
            isEmpty = false;
            break;
        }

        if(m_imgSource.isEmpty() == false)
        {
            isEmpty = false;
            break;
        }

        if(m_linkUrl.isEmpty() == false)
        {
            isEmpty = false;
            break;
        }
    }while(false);

    return isEmpty;
}

QString RadioChannelData::getTitle() const
{
    return m_title;
}

QString RadioChannelData::getImgSource() const
{
    return m_imgSource;
}

QString RadioChannelData::getLinkUrl() const
{
    return m_linkUrl;
}

void RadioChannelData::setTitle(const QString &_title)
{
    m_title = _title;
}

void RadioChannelData::setImgSource(const QString &_imgSource)
{
    m_imgSource = _imgSource;
}

void RadioChannelData::setLinkUrl(const QString &_linkUrl)
{
    m_linkUrl = _linkUrl;
}

RadioChannelData& RadioChannelData::operator=(const RadioChannelData &data)
{
    m_title = data.getTitle();
    m_imgSource = data.getImgSource();
    m_linkUrl = data.getLinkUrl();

    return *this;
}

bool RadioChannelData::operator==(const RadioChannelData &data) const
{
    if( m_title.compare( data.getTitle(), Qt::CaseInsensitive ) != 0 )
        return false;
    if( m_imgSource.compare( data.getImgSource(), Qt::CaseInsensitive ) != 0 )
        return false;
    if( m_linkUrl.compare( data.getLinkUrl(), Qt::CaseInsensitive ) != 0 )
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  QRadioChannelModel

QRadioChannelModel::QRadioChannelModel(QObject *parent)
: QAbstractListModel(parent), m_currentSelectedIndex(-1)
{
    m_finished = false;
}
