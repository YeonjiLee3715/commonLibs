#ifndef QCUSTOMMODEL_H
#define QCUSTOMMODEL_H

#include <QMetaEnum>
#include <QItemSelection>
#include <QReadWriteLock>

/*!
 * QAbstractModel 류의 모델(QAbstractListModel 등)을 상속받은 클래스에서 사용. 해당 기본 함수를 수정해야한다면 상속받아서 사용하거나 직접 정의
 */

// m_currentSelectedIndex가 setDatas 중 자동 초기화 되지만, 형식 상 생성자를 선언 할 때 -1로 초기화 권장
// 리스트에서 사용 할 데이터형, field enum, field max count( last field +1 )
#define DEF_QCUSTOMMODEL( DATA, ENUMFIELD, MAXFIELD )                                                       \
                                                                                                            \
    Q_PROPERTY(QList<DATA*> list READ list NOTIFY listChanged)                                              \
    Q_PROPERTY(int currentSelectedIndex READ getCurrentSelectedIndex WRITE setCurrentSelectedIndex)         \
                                                                                                            \
signals:                                                                                                    \
    void listChanged();                                                                                     \
                                                                                                            \
public:                                                                                                     \
    void addData(DATA* &modelData)                                                                          \
    {                                                                                                       \
        beginInsertRows(QModelIndex(), rowCount(), rowCount());                                             \
        {                                                                                                   \
            QWriteLocker lck(&m_lck);                                                                       \
            m_lstModelData.append( modelData );                                                             \
        }                                                                                                   \
        endInsertRows();                                                                                    \
    }                                                                                                       \
                                                                                                            \
    void setDatas(const QList<DATA*>& lstModelData)                                                         \
    {                                                                                                       \
        int idx = -1;                                                                                       \
                                                                                                            \
        {                                                                                                   \
            QReadLocker lck(&m_lck);                                                                        \
            if( m_currentSelectedIndex >= 0 && m_currentSelectedIndex < m_lstModelData.size()               \
                && lstModelData.isEmpty() == false )                                                        \
            {                                                                                               \
                DATA* currentSelectedItem = m_lstModelData[m_currentSelectedIndex];                         \
                if( currentSelectedItem != NULL )                                                           \
                    idx = lstModelData.indexOf( currentSelectedItem );                                      \
            }                                                                                               \
        }                                                                                                   \
                                                                                                            \
        if(rowCount() > 0)                                                                                  \
        {                                                                                                   \
            beginRemoveRows(QModelIndex(), 0, rowCount()-1);                                                \
            {                                                                                               \
                QWriteLocker lck(&m_lck);                                                                   \
                for( QList<DATA*>::iterator it = m_lstModelData.begin(); it != m_lstModelData.end(); )      \
                {                                                                                           \
                    delete *it;                                                                             \
                    it = m_lstModelData.erase(it);                                                          \
                }                                                                                           \
                m_currentSelectedIndex = -1;                                                                \
            }                                                                                               \
            endRemoveRows();                                                                                \
        }                                                                                                   \
                                                                                                            \
        if(lstModelData.isEmpty() == false)                                                                 \
        {                                                                                                   \
            beginInsertRows(QModelIndex(), 0, lstModelData.count()-1 );                                     \
            {                                                                                               \
                QWriteLocker lck(&m_lck);                                                                   \
                m_lstModelData = lstModelData;                                                              \
                if( idx < lstModelData.count() )                                                            \
                    m_currentSelectedIndex = idx;                                                           \
                else                                                                                        \
                    m_currentSelectedIndex = 0;                                                             \
            }                                                                                               \
            endInsertRows();                                                                                \
        }                                                                                                   \
                                                                                                            \
    }                                                                                                       \
                                                                                                            \
    int rowCount(const QModelIndex & parent = QModelIndex()) const                                          \
    {                                                                                                       \
        Q_UNUSED(parent);                                                                                   \
        QReadLocker lck(&m_lck);                                                                            \
        return m_lstModelData.count();                                                                      \
    }                                                                                                       \
                                                                                                            \
    QVariant data(const QModelIndex& index, int field = Qt::DisplayRole) const                              \
    {                                                                                                       \
        QVariant value;                                                                                     \
                                                                                                            \
        QReadLocker lck(&m_lck);                                                                            \
        if( index.row() >= 0 && index.row() < m_lstModelData.count() )                                      \
        {                                                                                                   \
            const DATA* data = m_lstModelData.at(index.row());                                              \
            if( data != NULL )                                                                              \
                value = data->getData( field );                                                             \
        }                                                                                                   \
                                                                                                            \
        return value;                                                                                       \
    }                                                                                                       \
                                                                                                            \
public slots:                                                                                               \
                                                                                                            \
    QList<DATA*> list(){ return m_lstModelData; }                                                           \
                                                                                                            \
    int getCurrentSelectedIndex(){ return m_currentSelectedIndex; }                                         \
    void setCurrentSelectedIndex( int currentSelectedIndex ){ m_currentSelectedIndex = currentSelectedIndex; }            \
                                                                                                            \
protected:                                                                                                  \
    QHash<int, QByteArray> roleNames() const                                                                \
    {                                                                                                       \
        QHash<int, QByteArray> mapRoles;                                                                    \
                                                                                                            \
        QMetaEnum meField = QMetaEnum::fromType< ENUMFIELD >();                                             \
        for(int idx = Qt::UserRole+1; idx < m_maxFieldIndex; ++idx )                                        \
            mapRoles.insert( (ENUMFIELD)idx, meField.valueToKey((ENUMFIELD)idx) );                          \
        return mapRoles;                                                                                    \
    }                                                                                                       \
                                                                                                            \
private:                                                                                                    \
                                                                                                            \
    QList<DATA*> m_lstModelData;                                                                            \
    const int m_maxFieldIndex = MAXFIELD;                                                                   \
    int m_currentSelectedIndex;                                                                             \
                                                                                                            \
    mutable QReadWriteLock m_lck;

class  QCustomModelItem: public QObject
{
    Q_OBJECT
public:
    explicit    QCustomModelItem( QObject* parent = nullptr ):QObject(parent){}
    virtual     ~QCustomModelItem(){};

    virtual     QVariant    getData( int field ) const = 0;
};

#endif // QCUSTOMMODEL_H
