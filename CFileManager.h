#ifndef CFILEMANAGER_H
#define CFILEMANAGER_H

#include "commonLibsDef.h"

#include <iostream>
#include <fcntl.h>
#include <vector>

#if defined( __linux__ ) || defined( __APPLE__ )
#elif defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#if defined(_WIN32)
#include <io.h>

typedef int mode_t;

/// @Note If STRICT_UGO_PERMISSIONS is not defined, then setting Read for any
///       of User, Group, or Other will set Read for User and setting Write
///       will set Write for User.  Otherwise, Read and Write for Group and
///       Other are ignored.
///
/// @Note For the POSIX modes that do not have a Windows equivalent, the modes
///       defined here use the POSIX values left shifted 16 bits.

static const mode_t S_ISUID = 0x08000000;           ///< does nothing
static const mode_t S_ISGID = 0x04000000;           ///< does nothing
static const mode_t S_ISVTX = 0x02000000;           ///< does nothing
static const mode_t S_IRUSR = mode_t( _S_IREAD );   ///< read by user
static const mode_t S_IWUSR = mode_t( _S_IWRITE );  ///< write by user
static const mode_t S_IXUSR = mode_t( _S_IEXEC );   ///< Execute/search by user
//static const mode_t S_IXUSR = 0x00400000;           ///< does nothing
static const mode_t S_IRWXU = mode_t( S_IRUSR | S_IWUSR | S_IXUSR ); ///<  read/writ/Execute/search by user
#ifndef STRICT_UGO_PERMISSIONS
static const mode_t S_IRGRP = mode_t( _S_IREAD );   ///< read by *USER*
static const mode_t S_IWGRP = mode_t( _S_IWRITE );  ///< write by *USER*
static const mode_t S_IXGRP = mode_t( _S_IEXEC );   ///< Execute/search by *USER*
//static const mode_t S_IXGRP = 0x00080000;           ///< does nothing
static const mode_t S_IRWXG = mode_t( S_IRGRP | S_IWGRP | S_IXGRP ); ///<  read/writ/Execute/search by *USER*

static const mode_t S_IROTH = mode_t( _S_IREAD );     ///< read by *USER*
static const mode_t S_IWOTH = mode_t( _S_IWRITE );    ///< write by *USER*
static const mode_t S_IXOTH = mode_t( _S_IEXEC );   ///< Execute/search by *USER*
//static const mode_t S_IXOTH = 0x00010000;           ///< does nothing
static const mode_t S_IRWXO = mode_t( S_IROTH | S_IWOTH | S_IXOTH ); ///<  read/writ/Execute/search by *USER*
#else
static const mode_t S_IRGRP = 0x00200000;           ///< does nothing
static const mode_t S_IWGRP = 0x00100000;           ///< does nothing
static const mode_t S_IXGRP = 0x00080000;           ///< does nothing
static const mode_t S_IROTH = 0x00040000;           ///< does nothing
static const mode_t S_IWOTH = 0x00020000;           ///< does nothing
static const mode_t S_IXOTH = 0x00010000;           ///< does nothing
#endif
static const mode_t MS_MODE_MASK = 0x0000ffff;           ///< low word
#endif
#endif

class CFileManager
{
public:
    CFileManager();

    static bool isExistFile( const std::string& path );
    static bool getFileSize( const std::string& path, size_t& size );

    static bool readFile( int fd, char *data, size_t size, ssize_t&szRead );
    static bool writeFile( int fd, const char* data, size_t size, ssize_t& szWrite );
    static bool rename( const std::string& pathOld, const std::string& pathNew );
    static bool removeFile( const std::string& path );      ///< On Linux systems, call rmdir(2) for directories. However, this is not the case with Windows systems.
    static bool removeDirectory( const std::string& path );
    static bool closeFile( int fd );

    static std::vector<std::string> getFileListOfPath( const std::string& path, bool isIncludeDir = false, bool isRecursive = false );
    static size_t getFolderSize( const std::string& path, bool isRecursive = false );

    static std::string getAbsolutePath( const std::string& path );

    static bool sepPathAndNameFromFullPath( const std::string& fullPath, std::string& path, std::string& name );
    static bool unionPathAndNameToFullPath( const std::string& path, const std::string& name, std::string& fullPath );

    static bool isDir( const std::string& path );
    static std::string getSuffix( const std::string& path );

#if defined(__linux__)
    static int createFile( const std::string& path, __mode_t __mode = S_IRWXU );
    static int openFile( const std::string& path, int flag = O_RDWR|O_CREAT, __mode_t __mode = S_IRWXU );

    static bool resetPermission( const std::string& path, __mode_t __mode = S_IRWXU );
    static bool getOwnership(const std::string& path, const std::string& userName, __mode_t __mode = S_IRWXU);
    static bool makeDir( const std::string& path, __mode_t __mode = S_IRWXU );

    static bool changeAccessRight( const std::string& path, __mode_t __mode = S_IRWXU );
#elif defined(__APPLE__)
    static int createFile( const std::string& path, mode_t __mode = S_IRWXU );
    static int openFile( const std::string& path, int flag = O_RDWR | O_CREAT, mode_t __mode = S_IRWXU );

    static bool resetPermission( const std::string& path, mode_t __mode = S_IRWXU );
    static bool getOwnership( const std::string& path, const std::string& userName, mode_t __mode = S_IRWXU );
    static bool makeDir( const std::string& path, mode_t __mode = S_IRWXU );

    static bool changeAccessRight( const std::string& path, mode_t __mode = S_IRWXU );

#elif defined(_WIN32) || defined(_WIN64)
    static int createFile( const std::string& path, mode_t __mode = S_IRWXU );
    static HANDLE winCreateFile( const std::string& path, DWORD dwDesiredAccess = GENERIC_READ|GENERIC_WRITE, DWORD dwShareMode = 0
                               , LPSECURITY_ATTRIBUTES lpSecurityAttributes = nullptr, DWORD dwCreationDisposition = CREATE_ALWAYS
                               , DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL, HANDLE hTemplateFile = nullptr );

    static int openFile( const std::string& path, int flag = O_RDWR | O_CREAT, mode_t __mode = S_IRWXU );

    static HANDLE winOpenFile( const std::string& path, DWORD dwDesiredAccess = GENERIC_READ|GENERIC_WRITE, DWORD dwShareMode = 0
                               , LPSECURITY_ATTRIBUTES lpSecurityAttributes = nullptr, DWORD dwCreationDisposition = OPEN_EXISTING
                               , DWORD dwFlagsAndAttributes = 0, HANDLE hTemplateFile = nullptr );

    static bool winIsExistFile( const std::string& path );
    static bool winGetFileSize( const std::string& path, size_t& size );

    static bool resetPermission( const std::string& path, mode_t __mode = S_IRWXU );

    static bool winLookupOwnership( const std::string& path, std::string& userName );
    static bool winLookupOwnership( HANDLE hFile, std::string& userName );

    static bool winGetOwnership( const std::string& path, const std::string& userName, mode_t __mode = S_IRWXU );
    static bool winSetPrivilege( HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege );

    static bool makeDir( const std::string& path, mode_t __mode = S_IRWXU );
    static bool winMakeDir( const std::string& path, LPSECURITY_ATTRIBUTES lpSecurityAttributes );

    static bool winReadFile( HANDLE hFile, char* data, size_t size, size_t &szRead);
    static bool winWriteFile( HANDLE hFile, const char* data, size_t size ,size_t& szWrite );

    static bool changeAccessRight( const std::string& path, mode_t __mode = S_IRWXU );

    static bool winCloseFile( HANDLE hFile );

    static std::string GetLastErrorAsString();
#endif
};

#endif // CFILEMANAGER_H
