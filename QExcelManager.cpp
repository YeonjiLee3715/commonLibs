#include "QExcelManager.h"

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

#include <atlconv.h>

#include <CLogger.h>
#include <define/cmnDef.h>

#include <xlsxcellformula.h>
#include <xlsxutility_p.h>

#define EXCEL_SUF_XLS   ".xls"
#define EXCEL_SUF_XLSX  ".xlsx"

namespace nsQExcelManager{
    const char* TAG = "QExcelManager";
}

using namespace nsQExcelManager;

QExcelManager::QExcelManager( QObject *parent)
    : QObject(parent), m_isLoaded(false), m_nFormat(EXCEL_NONE)
    , m_docQXlsx(nullptr), m_docBE(nullptr)
{

}

QExcelManager::~QExcelManager()
{

}

void QExcelManager::setFilePath(const QString& strFilePath)
{
    if( m_isLoaded )
        close();

    m_strFilePath.clear();
    m_nFormat = EXCEL_NONE;

    if( strFilePath.isEmpty() )
        return;

    if( strFilePath.endsWith( QString(EXCEL_SUF_XLS), Qt::CaseInsensitive ) )
        m_nFormat = EXCEL_XLS;
    else if( strFilePath.endsWith( QString(EXCEL_SUF_XLSX), Qt::CaseInsensitive ) )
        m_nFormat = EXCEL_XLSX;

    if( m_nFormat != EXCEL_NONE )
        m_strFilePath = strFilePath;
}

QString QExcelManager::getFilePath()
{
    return m_strFilePath;
}

int QExcelManager::getFileFormat()
{
    return m_nFormat;
}

bool QExcelManager::load()
{
    bool isSuccess = false;

    if( m_nFormat == EXCEL_XLS )
        isSuccess = loadXls();
    else if( m_nFormat == EXCEL_XLSX )
        isSuccess = loadXlsx();
    else
        LOGE(TAG, "Wrong document format. format: %d", m_nFormat);

    return isSuccess;
}

bool QExcelManager::save()
{
    bool isSuccess = false;

    if( m_isLoaded == false )
    {
        LOGE(TAG, "Document not loaded. format: %d", m_nFormat);
        return isSuccess;
    }

    if( m_nFormat == EXCEL_XLS )
        isSuccess = saveXls();
    else if( m_nFormat == EXCEL_XLSX )
        isSuccess = saveXlsx();
    else
        LOGE(TAG, "Wrong document format. format: %d", m_nFormat);

    return isSuccess;
}

bool QExcelManager::saveAs(const QString& strFilePath)
{
    bool isSuccess = false;

    if( m_isLoaded == false )
    {
        LOGE(TAG, "Document not loaded. format: %d", m_nFormat);
        return isSuccess;
    }

    if( m_nFormat == EXCEL_XLS )
        isSuccess = saveXlsAs( strFilePath );
    else if( m_nFormat == EXCEL_XLSX )
        isSuccess = saveXlsxAs( strFilePath );
    else
        LOGE(TAG, "Wrong document format. format: %d", m_nFormat);

    return isSuccess;
}

void QExcelManager::close()
{
    if( m_nFormat == EXCEL_XLS )
        closeXls();
    else if( m_nFormat == EXCEL_XLSX )
        closeXlsx();
    else
        LOGE(TAG, "Wrong document format. format: %d", m_nFormat);
}

QStringList QExcelManager::getSheetList()
{
    if( m_isLoaded == false )
    {
        LOGE(TAG, "Document not loaded. format: %d", m_nFormat);
        return QStringList();
    }

    if( m_nFormat == EXCEL_XLS )
        return getXlsSheetList();
    else if( m_nFormat == EXCEL_XLSX )
        return getXlsxSheetList();
    else
        LOGE(TAG, "Wrong document format. format: %d", m_nFormat);

    return QStringList();
}

int QExcelManager::getTotalSheetCount()
{
    int count = 0;

    if( m_isLoaded == false )
    {
        LOGE(TAG, "Document not loaded. format: %d", m_nFormat);
        return count;
    }

    if( m_nFormat == EXCEL_XLS )
        count = getXlsTotalSheetCount();
    else if( m_nFormat == EXCEL_XLSX )
        count = getXlsxTotalSheetCount();
    else
        LOGE(TAG, "Wrong document format. format: %d", m_nFormat);

    return count;
}

void* QExcelManager::getWorksheet(int idxSheet)
{
    if( m_isLoaded == false )
    {
        LOGE(TAG, "Document not loaded. format: %d", m_nFormat);
        return nullptr;
    }

    if( m_nFormat == EXCEL_XLS )
        return (void*)getXlsWorksheet( idxSheet );
    else if( m_nFormat == EXCEL_XLSX )
        return (void*)getXlsxWorksheet( idxSheet );
    else
        LOGE(TAG, "Wrong document format. format: %d", m_nFormat);

    return nullptr;
}

void* QExcelManager::getWorksheet(const QString& strSheetName)
{
    if( m_isLoaded == false )
    {
        LOGE(TAG, "Document not loaded. format: %d", m_nFormat);
        return nullptr;
    }

    if( m_nFormat == EXCEL_XLS )
        return (void*)getXlsWorksheet( strSheetName );
    else if( m_nFormat == EXCEL_XLSX )
        return (void*)getXlsxWorksheet( strSheetName );
    else
        LOGE(TAG, "Wrong document format. format: %d", m_nFormat);

    return nullptr;
}

void* QExcelManager::addWorksheet(int idxSheet)
{
    if( m_isLoaded == false )
    {
        LOGE(TAG, "Document not loaded. format: %d", m_nFormat);
        return nullptr;
    }

    if( m_nFormat == EXCEL_XLS )
        return (void*)addXlsWorksheet( idxSheet );
    else if( m_nFormat == EXCEL_XLSX )
        return (void*)addXlsxWorksheet( idxSheet );
    else
        LOGE(TAG, "Wrong document format. format: %d", m_nFormat);

    return nullptr;
}

void* QExcelManager::addWorksheet(const QString& strSheetName, int idxSheet)
{
    if( m_isLoaded == false )
    {
        LOGE(TAG, "Document not loaded. format: %d", m_nFormat);
        return nullptr;
    }

    if( m_nFormat == EXCEL_XLS )
        return (void*)addXlsWorksheet( strSheetName, idxSheet );
    else if( m_nFormat == EXCEL_XLSX )
        return (void*)addXlsxWorksheet( strSheetName, idxSheet );
    else
        LOGE(TAG, "Wrong document format. format: %d", m_nFormat);

    return nullptr;
}

bool QExcelManager::deleteWorksheet(int idxSheet)
{
    bool isSuccess = false;

    if( m_isLoaded == false )
    {
        LOGE(TAG, "Document not loaded. format: %d", m_nFormat);
        return isSuccess;
    }

    if( m_nFormat == EXCEL_XLS )
        isSuccess = deleteXlsWorksheet( idxSheet );
    else if( m_nFormat == EXCEL_XLSX )
        isSuccess = deleteXlsxWorksheet( idxSheet );
    else
        LOGE(TAG, "Wrong document format. format: %d", m_nFormat);

    return isSuccess;
}

bool QExcelManager::deleteWorksheet(const QString& strSheetName)
{
    bool isSuccess = false;

    if( m_isLoaded == false )
    {
        LOGE(TAG, "Document not loaded. format: %d", m_nFormat);
        return isSuccess;
    }

    if( m_nFormat == EXCEL_XLS )
        isSuccess = deleteXlsWorksheet( strSheetName );
    else if( m_nFormat == EXCEL_XLSX )
        isSuccess = deleteXlsxWorksheet( strSheetName );
    else
        LOGE(TAG, "Wrong document format. format: %d", m_nFormat);

    return isSuccess;
}

int QExcelManager::getSheetIdx(const QString& sheetName)
{
    int idxSheet = -1;
    if( m_isLoaded == false )
    {
        LOGE(TAG, "Document not loaded. format: %d", m_nFormat);
        return idxSheet;
    }

    if( m_nFormat == EXCEL_XLS )
        return getXlsSheetIdx( sheetName );
    else if( m_nFormat == EXCEL_XLSX )
        return getXlsxSheetIdx( sheetName );
    else
        LOGE(TAG, "Wrong document format. format: %d", m_nFormat);

    return idxSheet;
}

QString QExcelManager::getSheetName(int idxSheet)
{
    if( m_isLoaded == false )
    {
        LOGE(TAG, "Document not loaded. format: %d", m_nFormat);
        return QString();
    }

    if( m_nFormat == EXCEL_XLS )
        return getXlsSheetName( idxSheet );
    else if( m_nFormat == EXCEL_XLSX )
        return getXlsxSheetName( idxSheet );
    else
        LOGE(TAG, "Wrong document format. format: %d", m_nFormat);

    return QString();
}

bool QExcelManager::renameWorksheet(int idxSheet, const QString& toSheetName)
{
    bool isSuccess = false;

    if( m_nFormat == EXCEL_XLS )
        isSuccess = renameXlsWorksheet( idxSheet, toSheetName );
    else if( m_nFormat == EXCEL_XLSX )
        isSuccess = renameXlsxWorksheet( idxSheet, toSheetName );
    else
        LOGE(TAG, "Wrong document format. format: %d", m_nFormat);

    return isSuccess;
}

bool QExcelManager::renameWorksheet(const QString& fromSheetName, const QString& toSheetName)
{
    bool isSuccess = false;

    if( m_nFormat == EXCEL_XLS )
        isSuccess = renameXlsWorksheet( fromSheetName, toSheetName );
    else if( m_nFormat == EXCEL_XLSX )
        isSuccess = renameXlsxWorksheet( fromSheetName, toSheetName );
    else
        LOGE(TAG, "Wrong document format. format: %d", m_nFormat);

    return isSuccess;
}

bool QExcelManager::isLoaded()
{
    return m_isLoaded;
}

QVector<ExcelCellInfo> QExcelManager::getSheetDatas(int idxSheet, const QString& fromCell, const QString& toCell)
{
    QVector<ExcelCellInfo> vecCellInfos;

    if( m_nFormat == EXCEL_XLS )
        vecCellInfos = getXlsSheetDatas( idxSheet, fromCell, toCell );
    else if( m_nFormat == EXCEL_XLSX )
        vecCellInfos = getXlsxSheetDatas( idxSheet, fromCell, toCell );
    else
        LOGE(TAG, "Wrong document format. format: %d", m_nFormat);

    return vecCellInfos;
}

bool QExcelManager::loadXls()
{
    bool isSuccess = false;

    if( m_nFormat != EXCEL_XLS || m_strFilePath.isEmpty() )
        return isSuccess;

    if( m_isLoaded )
    {
        isSuccess = true;
        return isSuccess;
    }

    if( m_docBE == nullptr )
        m_docBE = new YExcel::BasicExcel;

    wchar_t fullFilePath[MAX_PATH];
    int len = m_strFilePath.toWCharArray(fullFilePath);

    if( m_docBE->Load( fullFilePath ) )
    {
        m_isLoaded = true;
        isSuccess = true;
    }

    return isSuccess;
}

bool QExcelManager::saveXls()
{
    bool isSuccess = false;

    if( m_nFormat != EXCEL_XLS || m_strFilePath.isEmpty() )
        return isSuccess;

    if( m_isLoaded == false )
        return isSuccess;

    if( m_docBE == nullptr )
        return isSuccess;

    isSuccess = m_docBE->Save();

    return isSuccess;
}

bool QExcelManager::saveXlsAs(const QString& strFilePath)
{
    bool isSuccess = false;

    if( m_nFormat != EXCEL_XLS || m_strFilePath.isEmpty() || strFilePath.isEmpty() )
        return isSuccess;

    if( m_isLoaded == false )
        return isSuccess;

    if( m_docBE == nullptr )
        return isSuccess;

    isSuccess = m_docBE->SaveAs( strFilePath.toLatin1().data() );

    return isSuccess;
}

void QExcelManager::closeXls()
{
    if( m_nFormat != EXCEL_XLS || m_strFilePath.isEmpty() )
        return;

    if( m_isLoaded == false )
        return;

    if( m_docBE == nullptr )
        return;

    m_docBE->Close();

    delete m_docBE;
    m_docBE = nullptr;
}

QStringList QExcelManager::getXlsSheetList()
{
    QStringList lstSheet;

    if( m_nFormat != EXCEL_XLS || m_docBE == nullptr || m_isLoaded == false )
        return lstSheet;

    int nMaxCount = m_docBE->GetTotalWorkSheets();
    for( int idx = 0; idx < nMaxCount; ++idx )
    {
        if( m_docBE->IsUnicodeSheetName( idx ) )
            lstSheet.append( QString::fromWCharArray(m_docBE->GetUnicodeSheetName(idx)) ); /*locale 적용*/
        else
            lstSheet.append( QString::fromStdString(m_docBE->GetAnsiSheetName(idx)) ); /*locale 적용*/
    }

    return lstSheet;
}

int QExcelManager::getXlsTotalSheetCount()
{
    if( m_nFormat != EXCEL_XLS || m_docBE == nullptr || m_isLoaded == false )
        return 0;

    return m_docBE->GetTotalWorkSheets();
}

YExcel::BasicExcelWorksheet* QExcelManager::getXlsWorksheet(int idxSheet)
{
    if( m_nFormat != EXCEL_XLS || m_docBE == nullptr || m_isLoaded == false )
        return nullptr;

    if( idxSheet < 0 || idxSheet >= m_docBE->GetTotalWorkSheets() )
    {
        LOGE(TAG, "Wrong index. idx: %d", idxSheet);
        return nullptr;
    }

    return m_docBE->GetWorksheet(idxSheet);
}

YExcel::BasicExcelWorksheet* QExcelManager::getXlsWorksheet(const QString& strSheetName)
{
    if( m_nFormat != EXCEL_XLS || m_docBE == nullptr || m_isLoaded == false )
        return nullptr;

    int idxSheet = getXlsSheetIdx( strSheetName );

    return getXlsWorksheet( idxSheet );
}

YExcel::BasicExcelWorksheet* QExcelManager::addXlsWorksheet(int idxSheet)
{
    if( m_nFormat != EXCEL_XLS || m_docBE == nullptr || m_isLoaded == false )
        return nullptr;

    if( idxSheet < 0 || idxSheet >= m_docBE->GetTotalWorkSheets() )
    {
        LOGE(TAG, "Wrong index. idx: %d", idxSheet);
        return nullptr;
    }

    return m_docBE->AddWorksheet(idxSheet);
}

YExcel::BasicExcelWorksheet* QExcelManager::addXlsWorksheet(const QString& strSheetName, bool isUnicode, int idxSheet)
{
    if( m_nFormat != EXCEL_XLS || m_docBE == nullptr || m_isLoaded == false )
        return nullptr;

    if( idxSheet < 0 || idxSheet >= m_docBE->GetTotalWorkSheets() )
    {
        LOGE(TAG, "Wrong index. idx: %d", idxSheet);
        return nullptr;
    }

    if( isUnicode )
        return m_docBE->AddWorksheet(strSheetName.toStdWString().c_str(), idxSheet);
    else
        return m_docBE->AddWorksheet(strSheetName.toStdString().c_str(), idxSheet);

    return nullptr;
}

bool QExcelManager::deleteXlsWorksheet(int idxSheet)
{
    if( m_nFormat != EXCEL_XLS || m_docBE == nullptr || m_isLoaded == false )
        return false;

    if( idxSheet < 0 || idxSheet >= m_docBE->GetTotalWorkSheets() )
    {
        LOGE(TAG, "Wrong index. idx: %d", idxSheet);
        return false;
    }

    return m_docBE->DeleteWorksheet(idxSheet);
}

bool QExcelManager::deleteXlsWorksheet(const QString& strSheetName)
{
    bool isSuccess = false;

    if( m_nFormat != EXCEL_XLS || m_docBE == nullptr || m_isLoaded == false )
        return false;

    int idxSheet = getXlsSheetIdx( strSheetName );

    return deleteXlsWorksheet( idxSheet );
}

int QExcelManager::getXlsSheetIdx(const QString& strSheetName)
{
    int idxSheet = -1;

    if( m_nFormat != EXCEL_XLS || m_docBE == nullptr || m_isLoaded == false )
        return idxSheet;

    int nMaxCount = m_docBE->GetTotalWorkSheets();
    for( int idx = 0; idx < nMaxCount; ++idx )
    {
        if( m_docBE->IsUnicodeSheetName( idx ) )
        {
            const QString strName = QString::fromWCharArray(m_docBE->GetUnicodeSheetName(idx));
            if( strName.compare( strSheetName.toStdWString().c_str(), Qt::CaseInsensitive ) != 0 )
                continue;
        }
        else
        {
            const QString strName = QLatin1String(m_docBE->GetAnsiSheetName(idx));
            if( strName.compare( strSheetName.toStdString().c_str(), Qt::CaseInsensitive ) != 0 )
                continue;
        }

        idxSheet = idx;
        break;
    }

    return idxSheet;
}

QString QExcelManager::getXlsSheetName(int idxSheet)
{
    if( m_nFormat != EXCEL_XLS || m_docBE == nullptr || m_isLoaded == false )
        return QString();

    if( idxSheet < 0 || idxSheet >= m_docBE->GetTotalWorkSheets() )
    {
        LOGE(TAG, "Wrong index. idx: %d", idxSheet);
        return QString();
    }

    if( m_docBE->IsUnicodeSheetName(idxSheet) )
        return QString::fromWCharArray(m_docBE->GetUnicodeSheetName(idxSheet));

    return QLatin1String(m_docBE->GetAnsiSheetName(idxSheet));
}

bool QExcelManager::renameXlsWorksheet(int idxSheet, const QString& toSheetName)
{
    if( m_nFormat != EXCEL_XLS || m_docBE == nullptr || m_isLoaded == false )
        return false;

    if( idxSheet < 0 || idxSheet >= m_docBE->GetTotalWorkSheets() )
    {
        LOGE(TAG, "Wrong index. idx: %d", idxSheet);
        return false;
    }

    if( m_docBE->IsUnicodeSheetName(idxSheet) )
        return m_docBE->RenameWorksheet(idxSheet, toSheetName.toStdWString().c_str());

    return m_docBE->RenameWorksheet(idxSheet, toSheetName.toLatin1().data());
}

bool QExcelManager::renameXlsWorksheet(const QString& fromSheetName, const QString& toSheetName)
{
    if( m_nFormat != EXCEL_XLS || m_docBE == nullptr || m_isLoaded == false )
        return false;

    int idxSheet = getXlsSheetIdx( fromSheetName );

    return renameXlsWorksheet( idxSheet, toSheetName );
}

QVector<ExcelCellInfo> QExcelManager::getXlsSheetDatas(int idxSheet, const QString& fromCell, const QString& toCell)
{
    QVector<ExcelCellInfo> vecCellInfos;

    YExcel::BasicExcelWorksheet* pWst = getXlsWorksheet(idxSheet);

    if( pWst == nullptr )
    {
        LOGE( TAG, "Failed to get worksheet %d", idxSheet );
        return vecCellInfos;
    }

    ExcelCellInfo startCell(fromCell), endCell(toCell);

    int nMaxRow = -1;
    int nMaxCol = -1;

    int fromRow = startCell.GetRowIndex();
    int fromCol = startCell.GetColumnIndex();
    int toRow = endCell.GetRowIndex();
    int toCol = endCell.GetColumnIndex();

    if( fromCol < 0 )
        fromCol = 0;

    if( fromRow < 0 )
        fromRow = 0;

    if( toCol < 0 || (toCol > pWst->GetTotalCols()) )
        toCol = pWst->GetTotalCols();

    if( toRow < 0 || toRow > pWst->GetTotalRows() )
        toRow = pWst->GetTotalRows();

    for( int row = fromRow; row < toRow; ++row )
    {
        for( int column = fromCol; column < toCol; ++column )
        {
            if ( row > nMaxRow )
                nMaxRow = row;

            if ( column > nMaxCol )
                nMaxCol = column;

            YExcel::BasicExcelCell* pCell = pWst->Cell( row, column );
            vecCellInfos.push_back( getCellInfoFromXlsCell( pWst, row, column, pCell/*value*/ ) );
        }
    }

    return vecCellInfos;
}

ExcelCellInfo QExcelManager::getXlsSheetDataAt(int idxSheet, const QString& cell)
{
    ExcelCellInfo atCell(cell);

    YExcel::BasicExcelWorksheet* pWst = getXlsWorksheet(idxSheet);

    if( pWst == nullptr )
    {
        LOGE( TAG, "Failed to get worksheet %d", idxSheet );
        return atCell;
    }

    // QString privateName = d->name; // name of sheet (not object type)
    // qDebug() << privateName ;

    int atRow = atCell.GetRowIndex();
    int atCol = atCell.GetColumnIndex();

    if( atRow < 0 || atRow > pWst->GetTotalRows() )
        return atCell;

    if( atCol < 0 || atCol > pWst->GetTotalCols() )
        return atCell;

    YExcel::BasicExcelCell* pCell = pWst->Cell( atRow, atCol );
    return getCellInfoFromXlsCell( pWst, atRow, atCol, pCell/*value*/ );
}

ExcelCellInfo QExcelManager::getCellInfoFromXlsCell(YExcel::BasicExcelWorksheet* pWst, int row, int column, YExcel::BasicExcelCell* pCell)
{
    ExcelCellInfo curCell;

    curCell.SetRowIndex( row );
    curCell.SetColumnIndex( column );

    if( pWst == nullptr )
    {
        LOGE( TAG, "Worksheet is nullptr" );
        return curCell;
    }

    if( pCell == nullptr )
    {
        LOGE( TAG, "Cell is nullptr" );
        return curCell;
    }

    do
    {
        if( pCell->Type() == YExcel::BasicExcelCell::INT )
        {
            curCell.SetData(QString::number( pCell->GetInteger() ));
            break;
        }
        else if( pCell->Type() == YExcel::BasicExcelCell::DOUBLE )
        {
            curCell.SetData(QString::number( pCell->GetDouble() ));
            break;
        }
        else if( pCell->Type() == YExcel::BasicExcelCell::STRING )
        {
            curCell.SetData( QLatin1String( pCell->GetString() ) );
            break;
        }
        else if( pCell->Type() == YExcel::BasicExcelCell::WSTRING  )
        {
            curCell.SetData( QString::fromWCharArray( pCell->GetWString() ) );
            break;
        }
        else if( pCell->Type() == YExcel::BasicExcelCell::FORMULA )
        {
            QString strFormula( QLatin1String( pCell->GetString() ) );
            if( strFormula.isEmpty() )
                strFormula = QString::fromWCharArray( pCell->GetWString() );
            curCell.SetData( strFormula );
            break;
        }

    }while(false);

    return curCell;
}

bool QExcelManager::loadXlsx()
{
    bool isSuccess = false;

    if( m_nFormat != EXCEL_XLSX || m_strFilePath.isEmpty() )
        return isSuccess;

    if( m_isLoaded )
    {
        isSuccess = true;
        return isSuccess;
    }

    if( m_docQXlsx != nullptr )
    {
        delete m_docQXlsx;
        m_docQXlsx = nullptr;
    }

    m_docQXlsx = new QXlsx::Document( m_strFilePath );

    if( m_docQXlsx->load() )
    {
        m_isLoaded = true;
        isSuccess = true;
    }

    return isSuccess;
}

bool QExcelManager::saveXlsx()
{
    bool isSuccess = false;

    if( m_nFormat != EXCEL_XLSX || m_strFilePath.isEmpty() )
        return isSuccess;

    if( m_isLoaded == false )
        return isSuccess;

    if( m_docQXlsx == nullptr )
        return isSuccess;

    isSuccess = m_docQXlsx->save();

    return isSuccess;
}

bool QExcelManager::saveXlsxAs(const QString& strFilePath)
{
    bool isSuccess = false;

    if( m_nFormat != EXCEL_XLSX || m_strFilePath.isEmpty() || strFilePath.isEmpty() )
        return isSuccess;

    if( m_isLoaded == false )
        return isSuccess;

    if( m_docQXlsx == nullptr )
        return isSuccess;

    isSuccess = m_docQXlsx->saveAs( strFilePath );

    return isSuccess;
}

void QExcelManager::closeXlsx()
{
    if( m_nFormat != EXCEL_XLSX || m_strFilePath.isEmpty() )
        return;

    if( m_isLoaded == false )
        return;

    if( m_docQXlsx == nullptr )
        return;

    delete m_docQXlsx;
    m_docQXlsx = nullptr;
}

QStringList QExcelManager::getXlsxSheetList()
{
    if( m_nFormat != EXCEL_XLSX || m_docQXlsx == nullptr || m_isLoaded == false )
        return QStringList();

    return m_docQXlsx->sheetNames();
}

int QExcelManager::getXlsxTotalSheetCount()
{
    if( m_nFormat != EXCEL_XLSX || m_docQXlsx == nullptr || m_isLoaded == false )
        return 0;

    return m_docQXlsx->sheetNames().size();
}

QXlsx::Worksheet* QExcelManager::getXlsxWorksheet(int idxSheet)
{
    if( m_nFormat != EXCEL_XLSX || m_docQXlsx == nullptr || m_isLoaded == false )
        return nullptr;

    const QString& strSheetName = getXlsxSheetName(idxSheet);

    QXlsx::AbstractSheet *pSt = m_docQXlsx->sheet(strSheetName);

    if(pSt && pSt->sheetType() == QXlsx::AbstractSheet::ST_WorkSheet)
        return static_cast<QXlsx::Worksheet*>(pSt);

    return nullptr;
}

QXlsx::Worksheet* QExcelManager::getXlsxWorksheet(const QString& strSheetName)
{
    if( m_nFormat != EXCEL_XLSX || m_docQXlsx == nullptr || m_isLoaded == false )
        return nullptr;

    QXlsx::AbstractSheet *pSt = m_docQXlsx->sheet(strSheetName);

    if(pSt && pSt->sheetType() == QXlsx::AbstractSheet::ST_WorkSheet)
        return static_cast<QXlsx::Worksheet*>(pSt);

    return nullptr;
}

QXlsx::Worksheet* QExcelManager::addXlsxWorksheet(int idxSheet)
{
    if( m_nFormat != EXCEL_XLSX || m_docQXlsx == nullptr || m_isLoaded == false )
        return nullptr;

    bool isSuccess = false;

    if( idxSheet < 0 )
    {
        isSuccess = m_docQXlsx->addSheet();
        if(isSuccess)
            idxSheet = getXlsxTotalSheetCount();
    }
    else
        isSuccess = m_docQXlsx->insertSheet(idxSheet);

    if(isSuccess == false)
        return nullptr;

    return getXlsxWorksheet(idxSheet);
}

QXlsx::Worksheet* QExcelManager::addXlsxWorksheet(const QString& strSheetName, int idxSheet)
{
    if( m_nFormat != EXCEL_XLSX || m_docQXlsx == nullptr || m_isLoaded == false )
        return nullptr;

    bool isSuccess = false;

    if( idxSheet < 0 )
    {
        isSuccess = m_docQXlsx->addSheet(strSheetName);
        if(isSuccess)
            idxSheet = getXlsxTotalSheetCount();
    }
    else
        isSuccess = m_docQXlsx->insertSheet(idxSheet, strSheetName);

    if(isSuccess == false)
    {
        LOGE( TAG, "Failed to add xlsx worksheet. idx: %d, name: %s"
              , idxSheet, strSheetName.toLatin1().data() );
        return nullptr;
    }

    return getXlsxWorksheet(idxSheet);
}

bool QExcelManager::deleteXlsxWorksheet(int idxSheet)
{
    if( m_nFormat != EXCEL_XLSX || m_docQXlsx == nullptr || m_isLoaded == false )
        return false;

    if( idxSheet < 0 || idxSheet >= getXlsxTotalSheetCount() )
        return false;

    const QString& strSheetName = getXlsxSheetName(idxSheet);
    return m_docQXlsx->deleteSheet(strSheetName);
}

bool QExcelManager::deleteXlsxWorksheet(const QString& strSheetName)
{
    if( m_nFormat != EXCEL_XLSX || m_docQXlsx == nullptr || m_isLoaded == false )
        return false;

    return m_docQXlsx->deleteSheet(strSheetName);
}

int QExcelManager::getXlsxSheetIdx(const QString& strSheetName)
{
    int idxSheet = -1;

    if( m_nFormat != EXCEL_XLSX || m_docQXlsx == nullptr || m_isLoaded == false )
        return idxSheet;

    const QStringList& lstNames = m_docQXlsx->sheetNames();
    for( int idx = 0; idx < lstNames.size(); ++idx )
    {
        if( strSheetName.compare(lstNames[idx], Qt::CaseInsensitive) != 0 )
            continue;

        idxSheet = idx;
        break;
    }

    return idxSheet;
}

QString QExcelManager::getXlsxSheetName(int idxSheet)
{
    if( m_nFormat != EXCEL_XLSX || m_docQXlsx == nullptr || m_isLoaded == false )
        return QString();

    const QStringList& lstNames = m_docQXlsx->sheetNames();
    if( idxSheet < 0 || idxSheet >= lstNames.size() )
    {
        LOGE(TAG, "Wrong index. idx: %d", idxSheet);
        return QString();
    }

    return lstNames.at( idxSheet );
}

bool QExcelManager::renameXlsxWorksheet(int idxSheet, const QString& toSheetName)
{
    if( m_nFormat != EXCEL_XLSX || m_docQXlsx == nullptr || m_isLoaded == false )
        return false;

    if( idxSheet < 0 || idxSheet >= getXlsxTotalSheetCount() )
        return false;

    const QString& strSheetName = getXlsxSheetName(idxSheet);
    return m_docQXlsx->renameSheet(strSheetName, toSheetName);
}

bool QExcelManager::renameXlsxWorksheet(const QString& fromSheetName, const QString& toSheetName)
{
    if( m_nFormat != EXCEL_XLSX || m_docQXlsx == nullptr || m_isLoaded == false )
        return false;

    return m_docQXlsx->renameSheet(fromSheetName, toSheetName);
}

QVector<ExcelCellInfo> QExcelManager::getXlsxSheetDatas(int idxSheet, const QString& fromCell, const QString& toCell)
{
    QVector<ExcelCellInfo> vecCellInfos;

    QXlsx::Worksheet* pWst = getXlsxWorksheet(idxSheet);

    if( pWst == nullptr )
    {
        LOGE( TAG, "Failed to get worksheet %d", idxSheet );
        return vecCellInfos;
    }

    // QString privateName = d->name; // name of sheet (not object type)
    // qDebug() << privateName ;

    if( pWst->sheetType() != QXlsx::AbstractSheet::ST_WorkSheet  )
    {
        if ( pWst->sheetType() == QXlsx::AbstractSheet::ST_ChartSheet )
        {
            return vecCellInfos;
        }
        else
        {
            LOGW(TAG, "unsupported sheet type: %d", (int)pWst->sheetType());
            return vecCellInfos;
        }
    }

    QMapIterator< int, QMap< int, QSharedPointer<QXlsx::Cell> > > _it = pWst->getCellTableIterator();

    ExcelCellInfo startCell(fromCell), endCell(toCell);

    int nMaxRow = -1;
    int nMaxCol = -1;

    int fromRow = startCell.GetRowIndex();
    int fromCol = startCell.GetColumnIndex();
    int toRow = endCell.GetRowIndex();
    int toCol = endCell.GetColumnIndex();

    while ( _it.hasNext() )
    {
        _it.next();

        int keyI = _it.key()-1; // key (cell row)
        if( toRow >= 0 && keyI > toRow )
            break;

        if( fromRow >= 0 && keyI < fromRow )
            continue;

        QMapIterator<int, QSharedPointer<QXlsx::Cell> > _iit( _it.value() ); // value

        while ( _iit.hasNext() )
        {
            _iit.next();

            int keyII = _iit.key()-1; // key (cell column)

            if( toCol >= 0 && keyII > toCol )
                break;

            if( fromCol >= 0 && keyII < fromCol )
                continue;

            if ( keyI > nMaxRow )
                nMaxRow = keyI;

            if ( keyII > nMaxCol )
                nMaxCol = keyII;

            vecCellInfos.push_back( getCellInfoFromXlsxCell( pWst, keyI, keyII, _iit.value()/*value*/ ) );
        }
    }

    return vecCellInfos;
}

ExcelCellInfo QExcelManager::getXlsxSheetDataAt(int idxSheet, const QString& cell)
{
    ExcelCellInfo atCell(cell);

    QXlsx::Worksheet* pWst = getXlsxWorksheet(idxSheet);

    if( pWst == nullptr )
    {
        LOGE( TAG, "Failed to get worksheet %d", idxSheet );
        return atCell;
    }

    // QString privateName = d->name; // name of sheet (not object type)
    // qDebug() << privateName ;

    if( pWst->sheetType() != QXlsx::AbstractSheet::ST_WorkSheet  )
    {
        if ( pWst->sheetType() == QXlsx::AbstractSheet::ST_ChartSheet )
        {
            return atCell;
        }
        else
        {
            LOGW(TAG, "unsupported sheet type: %d", (int)pWst->sheetType());
            return atCell;
        }
    }

    int atRow = atCell.GetRowIndex();
    int atCol = atCell.GetColumnIndex();

    return getCellInfoFromXlsxCell( pWst, atRow, atCol, QSharedPointer<QXlsx::Cell>(pWst->cellAt( atRow, atCol ) ) );
}

ExcelCellInfo QExcelManager::getCellInfoFromXlsxCell(QXlsx::Worksheet* pWst, int row, int column, QSharedPointer<QXlsx::Cell> pCell )
{
    ExcelCellInfo curCell;

    curCell.SetRowIndex( row );
    curCell.SetColumnIndex( column );

    if( pWst == nullptr )
    {
        LOGE( TAG, "Worksheet is nullptr" );
        return curCell;
    }

    if( pCell == nullptr )
    {
        LOGE( TAG, "Cell is nullptr" );
        return curCell;
    }

    do
    {
        if (pCell->hasFormula())
        {
            if (pCell->formula().formulaType() == QXlsx::CellFormula::NormalType)
            {
                curCell.SetData( QLatin1String("=")+pCell->formula().formulaText());
                break;
            }
            else if (pCell->formula().formulaType() == QXlsx::CellFormula::SharedType)
            {
                if (!pCell->formula().formulaText().isEmpty())
                {
                    curCell.SetData(QLatin1String("=")+pCell->formula().formulaText());
                    break;
                }
                else
                {
                    int si = pCell->formula().sharedIndex();
                    const QXlsx::CellFormula &rootFormula = pWst->getCellFormula( si );
                    QXlsx::CellReference rootCellRef = rootFormula.reference().topLeft();
                    QString rootFormulaText = rootFormula.formulaText();
                    QString newFormulaText = QXlsx::convertSharedFormula(rootFormulaText, rootCellRef, QXlsx::CellReference(row, column));
                    curCell.SetData(QLatin1String("=")+newFormulaText);
                    break;
                }
            }
        }

        if (pCell->isDateTime())
        {
            curCell.SetData( pCell->dateTime().toString() );
            break;
        }

        curCell.SetData( pCell->value().toString() );

    }while(false);

    return curCell;
}

ExcelCellInfo::ExcelCellInfo()
    :m_rowIdx(-1), m_columnIdx(-1)
{

}

ExcelCellInfo::ExcelCellInfo(const QString& row, const QString& column)
    :m_rowIdx(-1), m_columnIdx(-1)
{
    SetRow( row );
    SetColumn( column );
}

ExcelCellInfo::ExcelCellInfo(const QString& location)
    :m_rowIdx(-1), m_columnIdx(-1)
{
    SetLocation( location );
}

void ExcelCellInfo::SetRow(const QString& row)
{
    m_row.clear();
    m_rowIdx = -1;

    QRegExp regex("(^\\d+$)");
    bool isOk = false;

    if( regex.indexIn( row ) != 0 || (row.toInt(&isOk) == 0 && isOk == false) ) // integer digit only
    {
        LOGE( TAG, "Wrong row: %s", row.toStdString().c_str() );
        return;
    }

    m_rowIdx = ConvertRowToIdx( row );
    m_row = row;
}

void ExcelCellInfo::SetRowIndex(int row)
{
    m_row.clear();
    m_rowIdx = -1;

    if( row < 0 )
        return;

    m_rowIdx = row;
    m_row = ConvertIdxToRow(row);
}

int ExcelCellInfo::GetRowIndex() const
{
    return m_rowIdx;
}

void ExcelCellInfo::SetColumn(const QString& column)
{
    m_column.clear();
    m_columnIdx = -1;

    QRegExp regex("(^[a-zA-Z]+$)"); // alphabet only

    if( regex.indexIn( column ) < 0 )
    {
        LOGE( TAG, "Wrong column: %s", column.toStdString().c_str() );
        return;
    }

    m_columnIdx = ConvertColumnToIdx(column);
    m_column = column;
}

void ExcelCellInfo::SetColumnIndex(int column)
{
    m_column.clear();
    m_columnIdx = -1;

    if( column < 0 )
        return;

    m_columnIdx = column;
    m_column = ConvertIdxToColumn(column);
}

int ExcelCellInfo::GetColumnIndex() const
{
    return m_columnIdx;
}

void ExcelCellInfo::SetData(const QString& data)
{
    m_data = data;
}

void ExcelCellInfo::SetLocation(const QString& location)
{
    m_column.clear();
    m_row.clear();

    m_columnIdx = -1;
    m_rowIdx = -1;

    QRegExp regex("(^[a-zA-Z]+)(\\d+$)"); // alphabet only
    if( regex.indexIn( location ) < 0 )
    {
        LOGE( TAG, "Wrong location: %s", location.toStdString().c_str() );
        return;
    }

    const QStringList& caps = regex.capturedTexts();
    if( caps.size() < 2 )
    {
        LOGE( TAG, "Wrong location: %s", location.toStdString().c_str() );
        return;
    }

    m_columnIdx = ConvertColumnToIdx(caps[0]);
    m_rowIdx = ConvertRowToIdx(caps[1]);

    m_column = caps[0];
    m_row = caps[1];
}

int ExcelCellInfo::ConvertRowToIdx(const QString& row)
{
    int idx = -1;

    if( row.isEmpty() == false )
    {
        bool isOk = false;
        idx = row.toInt(&isOk);

        if( isOk )
            --idx;  // row start from 1
        else
            idx = -1;
    }

    return idx;
}

QString ExcelCellInfo::ConvertIdxToRow(int row)
{
    if( row < 0 )
        return QString();

    return QString::number( row );
}

int ExcelCellInfo::ConvertColumnToIdx(const QString& column)
{
    int idx = -1;

    if( column.isEmpty() == false )
    {
        QByteArray arrColumn = column.toUpper().toUtf8();
        int offset = ((int)'A')-1;

        for( int pos = column.size()-1, sq = 0; pos >= 0; --pos, ++sq )
        {
            int diff = (int)column[pos].unicode()-offset;
            if( diff < 0 )
            {
                idx = -1;
                break;
            }

            idx += (diff*pow(26, sq));
        }
    }

    return idx;
}

QString ExcelCellInfo::ConvertIdxToColumn(int column)
{
    if( column < 0 )
        return QString();

    QByteArray arrColumn;
    int offset = ((int)'A')-1;
    int quot = column+1;
    int remain = 0;

    while( quot > 26 )
    {
        remain = quot%26;
        quot /= 26;

        arrColumn.push_front( (char)(offset+remain) );
    }

    arrColumn.push_front( (char)(offset+quot) );

    return QString::fromUtf8( arrColumn );
}
