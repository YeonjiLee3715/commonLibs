#ifndef QEXCELMANAGER_H
#define QEXCELMANAGER_H

#include <QObject>

#include <xlsxdocument.h> // for xlsx
#include <BasicExcel.hpp> // for xls

// 1,048,576 rows by 16,384 columns
class ExcelCellInfo
{
public:
    explicit ExcelCellInfo();
    ExcelCellInfo( const QString& row, const QString& column );
    ExcelCellInfo( const QString& location );
    virtual ~ExcelCellInfo() {}

public:
    void SetRow( const QString& row );
    void SetRowIndex( int row );
    QString GetRow() const { return m_row; }
    int GetRowIndex() const;

    void SetColumn( const QString& column );
    void SetColumnIndex( int column );
    QString GetColumn() const { return m_column; }
    int GetColumnIndex() const;

    void SetData( const QString& data );
    QString GetData() const { return m_data; }

    void SetLocation( const QString& location );
    QString GetLocation() const { return QString( m_row+m_column ); }

public:
    static int ConvertRowToIdx( const QString& row );
    static QString ConvertIdxToRow( int row );

    static int ConvertColumnToIdx( const QString& column );
    static QString ConvertIdxToColumn( int column );

public:
    int     m_rowIdx;
    QString m_row;
    int     m_columnIdx;
    QString m_column;

    QString m_data;
};

class QExcelManager : public QObject
{
    Q_OBJECT

public:
    typedef enum eOpenMode{
        MODE_WRITE = 0x01,
        MODE_READ = 0x02,
        MODE_ATE = 0x04,
        MODE_APPEND = 0x08,
        MODE_TRUNC = 0x10
    }eOpenMode;

    typedef enum eExcelFormat{
        EXCEL_NONE = 0,
        EXCEL_XLS,
        EXCEL_XLSX
    }eExcelFormat;

public:
    explicit QExcelManager(QObject *parent = nullptr);
    ~QExcelManager();

public:
    void                setFilePath( const QString& strFilePath );
    QString             getFilePath();

    int                 getFileFormat();

    bool                load();
    bool                save();
    bool                saveAs( const QString& strFilePath );
    void                close();

    QStringList         getSheetList();
    int                 getTotalSheetCount();

    void*               getWorksheet(int idxSheet);	///< Get a pointer to an Excel worksheet at the given index. Index starts from 0. Returns 0 if index is invalid.
    void*               getWorksheet(const QString& strSheetName); ///< Get a pointer to an Excel worksheet that has given Unicode name. Returns 0 if there is no Excel worksheet with the given name.

    void*               addWorksheet(int idxSheet=-1);	///< Add a new Excel worksheet to the given index. Name given to worksheet is SheetX, where X is a number which starts from 1. Index starts from 0. Worksheet is added to the last position if sheetIndex == -1. Returns a pointer to the worksheet if successful, 0 if otherwise.
    void*               addWorksheet(const QString& strSheetName, int idxSheet=-1); ///< Add a new Excel worksheet with given ANSI name to the given index. Index starts from 0. Worksheet is added to the last position if sheetIndex == -1. Returns a pointer to the worksheet if successful, 0 if otherwise.

    bool                deleteWorksheet(int idxSheet);		///< Delete an Excel worksheet at the given index. Index starts from 0. Returns true if successful, false if otherwise.
    bool                deleteWorksheet(const QString& strSheetName); 	///< Delete an Excel worksheet that has given ANSI name. Returns true if successful, false if otherwise.

    int                 getSheetIdx( const QString& sheetName );
    QString             getSheetName(int idxSheet);		///< Get the worksheet name at the given index. Index starts from 0. Returns false if name is in Unicode format.

    bool                renameWorksheet(int idxSheet, const QString& toSheetName);			///< Rename an Excel worksheet at the given index to the given ANSI name. Index starts from 0. Returns true if successful, false if otherwise.
    bool                renameWorksheet(const QString& fromSheetName, const QString& toSheetName); 		///< Rename an Excel worksheet that has given ANSI name to another ANSI name. Returns true if successful, false if otherwise.

    bool                isLoaded();

    QVector<ExcelCellInfo>  getSheetDatas( int idxSheet, const QString& fromCell=QString(), const QString& toCell=QString() );

    // xls
private:
    bool                loadXls();
    bool                saveXls();
    bool                saveXlsAs( const QString& strFilePath );
    void                closeXls();

    QStringList         getXlsSheetList();
    int                 getXlsTotalSheetCount();

    YExcel::BasicExcelWorksheet*    getXlsWorksheet(int idxSheet);	///< Get a pointer to an Excel worksheet at the given index. Index starts from 0. Returns 0 if index is invalid.
    YExcel::BasicExcelWorksheet*    getXlsWorksheet(const QString& strSheetName); ///< Get a pointer to an Excel worksheet that has given Unicode name. Returns 0 if there is no Excel worksheet with the given name.

    YExcel::BasicExcelWorksheet*    addXlsWorksheet(int idxSheet=-1);	///< Add a new Excel worksheet to the given index. Name given to worksheet is SheetX, where X is a number which starts from 1. Index starts from 0. Worksheet is added to the last position if sheetIndex == -1. Returns a pointer to the worksheet if successful, 0 if otherwise.
    YExcel::BasicExcelWorksheet*    addXlsWorksheet(const QString& strSheetName, bool isUnicode=true, int idxSheet=-1); ///< Add a new Excel worksheet with given ANSI name to the given index. Index starts from 0. Worksheet is added to the last position if sheetIndex == -1. Returns a pointer to the worksheet if successful, 0 if otherwise.

    bool                deleteXlsWorksheet(int idxSheet);		///< Delete an Excel worksheet at the given index. Index starts from 0. Returns true if successful, false if otherwise.
    bool                deleteXlsWorksheet(const QString& strSheetName); 	///< Delete an Excel worksheet that has given ANSI name. Returns true if successful, false if otherwise.

    int                 getXlsSheetIdx( const QString& strSheetName );
    QString             getXlsSheetName(int idxSheet);		///< Get the worksheet name at the given index. Index starts from 0. Returns false if name is in Unicode format.

    bool                renameXlsWorksheet(int idxSheet, const QString& toSheetName);			///< Rename an Excel worksheet at the given index to the given ANSI name. Index starts from 0. Returns true if successful, false if otherwise.
    bool                renameXlsWorksheet(const QString& fromSheetName, const QString& toSheetName); 		///< Rename an Excel worksheet that has given ANSI name to another ANSI name. Returns true if successful, false if otherwise.

    QVector<ExcelCellInfo>  getXlsSheetDatas( int idxSheet, const QString& fromCell=QString(), const QString& toCell=QString() ); //empty: parse the data until it faces an empty cell
    ExcelCellInfo       getXlsSheetDataAt( int idxSheet, const QString& cell=QString() );

    ExcelCellInfo       getCellInfoFromXlsCell( YExcel::BasicExcelWorksheet* pWst, int row, int column, YExcel::BasicExcelCell* pCell );

    // xlsx
private:
    bool                loadXlsx();
    bool                saveXlsx();
    bool                saveXlsxAs( const QString& strFilePath );
    void                closeXlsx();

    QStringList         getXlsxSheetList();
    int                 getXlsxTotalSheetCount();

    QXlsx::Worksheet*   getXlsxWorksheet(int idxSheet);	///< Get a pointer to an Excel worksheet at the given index. Index starts from 0. Returns 0 if index is invalid.
    QXlsx::Worksheet*   getXlsxWorksheet(const QString& strSheetName); ///< Get a pointer to an Excel worksheet that has given Unicode name. Returns 0 if there is no Excel worksheet with the given name.

    QXlsx::Worksheet*   addXlsxWorksheet(int idxSheet=-1);	///< Add a new Excel worksheet to the given index. Name given to worksheet is SheetX, where X is a number which starts from 1. Index starts from 0. Worksheet is added to the last position if sheetIndex == -1. Returns a pointer to the worksheet if successful, 0 if otherwise.
    QXlsx::Worksheet*   addXlsxWorksheet(const QString& strSheetName, int idxSheet=-1); ///< Add a new Excel worksheet with given ANSI name to the given index. Index starts from 0. Worksheet is added to the last position if sheetIndex == -1. Returns a pointer to the worksheet if successful, 0 if otherwise.

    bool                deleteXlsxWorksheet(int idxSheet);		///< Delete an Excel worksheet at the given index. Index starts from 0. Returns true if successful, false if otherwise.
    bool                deleteXlsxWorksheet(const QString& strSheetName); 	///< Delete an Excel worksheet that has given ANSI name. Returns true if successful, false if otherwise.

    int                 getXlsxSheetIdx( const QString& strSheetName );
    QString             getXlsxSheetName(int idxSheet);		///< Get the worksheet name at the given index. Index starts from 0. Returns false if name is in Unicode format.

    bool                renameXlsxWorksheet(int idxSheet, const QString& toSheetName);			///< Rename an Excel worksheet at the given index to the given ANSI name. Index starts from 0. Returns true if successful, false if otherwise.
    bool                renameXlsxWorksheet(const QString& fromSheetName, const QString& toSheetName); 		///< Rename an Excel worksheet that has given ANSI name to another ANSI name. Returns true if successful, false if otherwise.

    QVector<ExcelCellInfo>  getXlsxSheetDatas( int idxSheet, const QString& fromCell=QString(), const QString& toCell=QString() ); //empty: parse the data until it faces an empty cell
    ExcelCellInfo       getXlsxSheetDataAt( int idxSheet, const QString& cell=QString() );

    ExcelCellInfo       getCellInfoFromXlsxCell( QXlsx::Worksheet* pWst, int row, int column, QSharedPointer<QXlsx::Cell> pCell );

signals:

public slots:


private:
    bool                    m_isLoaded;
    QString                 m_strFilePath;
    int                     m_nFormat;
    int                     m_nModeFlag;
    QXlsx::Document*        m_docQXlsx; // for xlsx
    YExcel::BasicExcel*     m_docBE;    // for xls
};

#endif // QEXCELMANAGER_H
