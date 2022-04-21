#ifndef QRADIOCHANNELMODEL_H
#define QRADIOCHANNELMODEL_H

#include <QAbstractListModel>

#include "QCustomModel.h"
#include "CLogger.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  RadioChannelData

///*!
// * RadioChannelData : QRadioChannelModel 사용 될 데이터 모델의 데이터형식 정의
// */

class RadioChannelData : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString FIELD_TITLE READ getTitle CONSTANT)
    Q_PROPERTY(QString FIELD_IMAGESOURCE READ getImgSource CONSTANT)
    Q_PROPERTY(QString FIELD_LINKURL READ getLinkUrl CONSTANT)

public:

    enum eDataField {
        FIELD_TITLE     = Qt::UserRole + 1,   ///< 제목
        FIELD_IMAGESOURCE,                    ///< 이미지소스
        FIELD_LINKURL
    };
    Q_ENUM(eDataField)

    explicit RadioChannelData( QObject* parent = 0);
    RadioChannelData(const RadioChannelData& data, QObject* parent = 0);
    ~RadioChannelData();

    void        setData(const QString& fieldName, QVariant value );
    QVariant    getData( int field ) const;
    bool        isEmpty() const;

    QString getTitle() const;
    QString getImgSource() const;
    QString getLinkUrl() const;

    void setTitle(const QString& _title);
    void setImgSource(const QString& _imgSource);
    void setLinkUrl(const QString& _linkUrl);

    RadioChannelData& operator= (const RadioChannelData& data);
    bool operator== (const RadioChannelData& data) const;

private:
    QString m_title;
    QString m_imgSource;
    QString m_linkUrl;
};
Q_DECLARE_METATYPE( RadioChannelData )


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  QRadioChannelModel

///*!
// * QRadioChannelModel 데이터 모델 정의
// */
class QRadioChannelModel: public QAbstractListModel
{
    Q_OBJECT
    DEF_QCUSTOMMODEL( RadioChannelData, RadioChannelData::eDataField, (RadioChannelData::eDataField::FIELD_LINKURL+1) )

public:
    explicit QRadioChannelModel(QObject *parent = 0);
    ~QRadioChannelModel(){ m_finished = true; }

    bool m_finished;
};

#endif // QRADIOCHANNELMODEL_H
