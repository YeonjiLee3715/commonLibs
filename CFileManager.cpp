#include "CFileManager.h"
#include <stdlib.h>
#include <iostream>
#include <sys/stat.h>

#ifdef USE_CPP17 //C++17
#include <filesystem>
#elif USE_CPP14 //C++14
#include <experimental/filesystem>
#endif

#if defined( __linux__ ) || defined( __APPLE__ )
#include <pwd.h>
#include <dirent.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <atlbase.h>
#include <atlconv.h>
#include <aclapi.h>
#include <direct.h>
#endif

#include <errno.h>

#include "CStrManager.h"
#include "CLogger.h"

#ifdef USE_CPP17
namespace fs = std::filesystem;
#elif USE_CPP14
namespace fs = std::experimental::filesystem;
#endif

namespace nsCFileManager{
    const char* TAG = "CFileManager";
}

using namespace nsCFileManager;

CFileManager::CFileManager()
{

}

bool CFileManager::isExistFile( const std::string &path )
{
    struct stat buffer;
    return ( stat( path.c_str(), &buffer ) == 0 );
}

bool CFileManager::getFileSize( const std::string &path, size_t& size )
{
    size = 0;

    struct stat buffer;
    if( stat( path.c_str(), &buffer ) == -1 )
    {
        LOGE( TAG, "Failed to get stat file: %s, %s", path.c_str(), strerror( errno ) );
        return false;
    }

    size = static_cast<size_t>( buffer.st_size );

    return true;
}

bool CFileManager::readFile( int fd, char *data, size_t size, ssize_t& szRead )
{
    szRead = read( fd, data, size );
    if( szRead == -1 )
    {
        LOGE( TAG, "Failed to read file: %d, dataSize: %d, %s", fd, (int)size, strerror( errno ) );
        return false;
    }

    return true;
}

bool CFileManager::writeFile( int fd, const char *data, size_t size, ssize_t& szWrite )
{
    szWrite = write( fd, data, size );
    if( szWrite == -1 )
    {
        LOGE( TAG, "Failed to write file: %d, dataSize: %d, %s", fd, (int)size, strerror( errno ) );
        return false;
    }

    return true;
}

bool CFileManager::rename( const std::string & pathOld, const std::string & pathNew )
{
    LOGI( TAG, "from %s to %s", pathOld.c_str(), pathNew.c_str() );

    int result = ::rename( pathOld.c_str(), pathNew.c_str() );
    if( result == -1 && errno != ENOENT )
    {
        LOGE( TAG, "Failed to rename file or directory: from %s to %s, %s"
            , pathOld.c_str(), pathNew.c_str(), strerror( errno ) );
        return false;
    }

    return true;
}

bool CFileManager::removeFile( const std::string& path )
{
    int fd = open( path.c_str(), O_RDONLY );
    if( fd <= 0 )
    {
        if( errno == ENOENT )
            return true;
        LOGE( TAG, "Failed to remove file: %s, %s", path.c_str(), strerror( errno ) );
    }
    else
        close( fd );

    LOGI( TAG, "%s", path.c_str() );

    int result = ENOENT;

    //On Linux systems, it automatically calls unlink (2) on the file.
#if defined(_WIN32) || defined(_WIN64)
    result = _unlink( path.c_str() );
    if( result == -1 && errno != ENOENT )
    {
        LOGE( TAG, "Failed to unlink file: %s, %s", path.c_str(), strerror( errno ) );
        return false;
    }
#endif
    result = remove( path.c_str() );
    if( result == -1 && errno != ENOENT )
    {
        LOGE( TAG, "Failed to remove file: %s, %s", path.c_str(), strerror( errno ) );
        return false;
    }

    return true;
}

bool CFileManager::removeDirectory( const std::string & path )
{
    LOGI( TAG, "%s", path.c_str() );

    int result = rmdir( path.c_str() );
    if( result == -1 && errno != ENOENT )
    {
        LOGE( TAG, "Failed to remove directory: %s, %s", path.c_str(), strerror( errno ) );
        return false;
    }

    return true;
}

bool CFileManager::closeFile( int fd )
{
    int result = close( fd );
    if( result == -1 && errno != ENOENT )
    {
        LOGE( TAG, "Failed to close file. %s", strerror( errno ) );
        return false;
    }

    return true;
}

#if defined( USE_CPP17 ) || defined( USE_CPP14 )
std::vector<std::string> CFileManager::getFileListOfPath( const std::string &path, bool isIncludeDir, bool isRecursive )
{
    std::vector<std::string> vecFileList;

    std::string abPath = getAbsolutePath( path );
    if( fs::exists( abPath.c_str() ) == false )
        return vecFileList;

    fs::directory_iterator end_itr; // default construction yields past-the-end

    for( fs::directory_iterator itr( abPath.c_str() ); itr != end_itr; ++itr )
    {
        if( fs::is_directory( itr->status() ) )
        {
            if( isIncludeDir )
                vecFileList.push_back( itr->path().string() );

            if( isRecursive )
            {
                const std::vector<std::string>& vecChildFileList = getFileListOfPath( itr->path().string(), isRecursive );
                vecFileList.insert( vecFileList.end(), vecChildFileList.begin(), vecChildFileList.end() );
            }
        }
        else
            vecFileList.push_back( itr->path().string() );
    }

    return vecFileList;
}

size_t CFileManager::getFolderSize( const std::string &path, bool isRecursive )
{
    size_t szTotal = 0;

    if( fs::exists( path.c_str() ) == false )
        return 0;

    fs::directory_iterator end_itr; // default construction yields past-the-end

    for( fs::directory_iterator itr( path.c_str() ); itr != end_itr; ++itr )
    {
        if( fs::is_directory( itr->status() ) )
        {

            if( isRecursive )
                szTotal += getFolderSize( itr->path().string(), isRecursive );
        }
        else
            szTotal += fs::file_size( itr->path() );
    }

    return szTotal;
}

std::string CFileManager::getAbsolutePath( const std::string& path )
{
    return fs::canonical( path ).string();
}

#elif defined( __linux__ ) || defined( __APPLE__ )
std::vector<std::string> CFileManager::getFileListOfPath( const std::string& path, bool isIncludeDir, bool isRecursive )
{
    std::vector<std::string> vecFileList;

    DIR* dir_info;

    std::string abPath = getAbsolutePath( path );
    dir_info = opendir( abPath.c_str() );
    if( dir_info == NULL )
    {
        LOGE( TAG, "Failed to get file list of directory: %s, %s", abPath.c_str(), strerror( errno ) );
        return vecFileList;
    }

    std::string fullPath = abPath;
    if( fullPath.back() != '/' )
        fullPath.push_back( '/' );

    struct dirent* dir_entry;
    while( ( dir_entry = readdir( dir_info ) ) )
    {
        std::string d_name( dir_entry->d_name );

        if( dir_entry->d_type == DT_DIR )
        {
            if ( nsCmn::compare( d_name.c_str(), d_name.size(), "." ) || nsCmn::compare( d_name.c_str(), d_name.size(), ".." ) )
                continue;

            if( isIncludeDir ) {
                vecFileList.push_back( fullPath + d_name );
            }

            if( isRecursive )
            {
                const std::vector<std::string>& vecChildFileList = getFileListOfPath( fullPath+d_name, isIncludeDir, isRecursive );
                vecFileList.insert( vecFileList.end(), vecChildFileList.begin(), vecChildFileList.end() );
            }
        }
        else
            vecFileList.push_back( fullPath + d_name );
    }

    closedir( dir_info );

    return vecFileList;
}

size_t CFileManager::getFolderSize( const std::string &path, bool isRecursive )
{
    size_t szTotal = 0;

    DIR* dir_info;

    dir_info = opendir( path.c_str() );
    if( dir_info == NULL )
    {
        LOGE( TAG, "Failed to get file list of directory: %s, %s", path.c_str(), strerror( errno ) );
        return 0;
    }

    std::string fullPath = path;
    if( fullPath.back() != '/' )
        fullPath.push_back( '/' );

    struct dirent* dir_entry;
    while( ( dir_entry = readdir( dir_info ) ) )
    {
        std::string d_name( dir_entry->d_name );

        if( dir_entry->d_type == DT_DIR )
        {
            if( nsCmn::compare( d_name.c_str(), d_name.size(), "." ) || nsCmn::compare( d_name.c_str(), d_name.size(), ".." ) )
                continue;

            if( isRecursive )
                szTotal += getFolderSize( fullPath + d_name, isRecursive );
        }
        else
        {
            size_t fileSize = 0;
            if( getFileSize( fullPath + d_name, fileSize ) )
                szTotal += fileSize;
        }
    }

    closedir( dir_info );

    return szTotal;
}

std::string CFileManager::getAbsolutePath( const std::string & path )
{
    std::string strPath;

    char* real_path = realpath( path.c_str(), nullptr );

    if( real_path == nullptr )
    {
        LOGE( TAG, "Failed to get real path from %s, %s", path.c_str(), strerror( errno ) );
        return strPath;
    }

    strPath = std::string( real_path );

    free( real_path );

    return strPath;
}

#elif defined(_WIN32) || defined(_WIN64)
std::vector<std::string> CFileManager::getFileListOfPath( const std::string& path, bool isIncludeDir, bool isRecursive )
{
    std::vector<std::string> vecFileList;

    if( path.empty() )
    {
        LOGE( TAG, "File path is Empty", "" );
        return vecFileList;
    }

    std::string abPath = getAbsolutePath( path );
    std::string fullPath = abPath;
    if( fullPath.back() != '/' )
        fullPath.push_back( '/' );

#ifdef UNICODE
    CA2W wstPath( abPath.c_str() );
    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW( wstPath, &fd );
#else
    WIN32_FIND_DATAA fd;
    hFind = FindFirstFileA( (LPCSTR)abPath.c_str(), &FindFileData );
#endif // !UNICODE

    if( hFind == INVALID_HANDLE_VALUE )
    {
        LOGE( TAG, "Error in FindFirstFile: %s, %s", abPath.c_str(), GetLastErrorAsString().c_str() );
        return vecFileList;
    }

    do
    {
#ifdef UNICODE
        std::string d_name( CT2A( fd.cFileName ) );
#else
        std::string d_name( fd.cFileName );
#endif // !UNICODE

        if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        {
            if ( nsCmn::compare( d_name.c_str(), d_name.size(), "." ) || nsCmn::compare( d_name.c_str(), d_name.size(), ".." ) )
                continue;

            if( isIncludeDir ) {
                vecFileList.push_back( fullPath + d_name );
            }

            if( isRecursive )
            {
                const std::vector<std::string>& vecChildFileList = getFileListOfPath( fullPath+d_name, isIncludeDir, isRecursive );
                vecFileList.insert( vecFileList.end(), vecChildFileList.begin(), vecChildFileList.end() );
            }
        }
        else
            vecFileList.push_back( std::string( fullPath + d_name ) );

    } while( ::FindNextFile( hFind, &fd ) );

    if( hFind )
        ::FindClose( hFind );

    return vecFileList;
}

size_t CFileManager::getFolderSize( const std::string &path, bool isRecursive )
{
    size_t szTotal = 0;
 
    if( path.empty() )
    {
        LOGE( TAG, "File path is Empty", "" );
        return szTotal;
    }

    std::string fullPath = path;
    if( fullPath.back() != '/' )
        fullPath.push_back( '/' );

#ifdef UNICODE
    CA2W wstPath( path.c_str() );
    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW( wstPath, &fd );
#else
    WIN32_FIND_DATAA fd;
    hFind = FindFirstFileA( (LPCSTR)path.c_str(), &FindFileData );
#endif // !UNICODE

    if( hFind == INVALID_HANDLE_VALUE )
    {
        LOGE( TAG, "Error in FindFirstFile: %s, %s", path.c_str(), GetLastErrorAsString().c_str() );
        return szTotal;
    }

    do
    {
#ifdef UNICODE
        std::string d_name( CT2A( fd.cFileName ) );
#else
        std::string d_name( fd.cFileName );
#endif // !UNICODE

        if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        {
            if( nsCmn::compare( d_name.c_str(), d_name.size(), "." ) || nsCmn::compare( d_name.c_str(), d_name.size(), ".." ) )
                continue;

            if( isRecursive )
                szTotal += getFolderSize( fullPath + d_name, isRecursive );
        }
        else
        {
            size_t fileSize = ( static_cast<size_t>( fd.nFileSizeHigh ) <<
                sizeof( fd.nFileSizeLow ) * 8 ) | fd.nFileSizeLow;
            szTotal += fileSize;
        }
    } while( ::FindNextFile( hFind, &fd ) );

    if( hFind )
        ::FindClose( hFind );

    return szTotal;
}

std::string CFileManager::getAbsolutePath( const std::string & path )
{
    std::string strPath;

    if( path.empty() )
    {
        LOGE( TAG, "File path is Empty", "" );
        return strPath;
    }

#ifdef UNICODE
    CA2W wstPath( path.c_str() );
    wchar_t fullFilePath[MAX_PATH];

    if( GetFullPathNameW( wstPath, MAX_PATH, fullFilePath, nullptr ) == ERROR_SUCCESS )
    {
        strPath = CT2A( fullFilePath );
    }
#else
    char fullFilePath[MAX_PATH];
    if( GetFullPathNameA( (LPCSTR)path.c_str(), MAX_PATH, fullFilePath, nullptr ) == ERROR_SUCCESS )
    {
        strPath = std::string( fullFilePath );
    }
#endif // !UNICODE
    else
    {
        LOGE( TAG, "Failed to GetFullPathName: %s, %s"
            , path.c_str()
            , GetLastErrorAsString().c_str() );
    }

    return strPath;
}
#endif

bool CFileManager::sepPathAndNameFromFullPath( const std::string& fullPath, std::string& path, std::string& name )
{
    path = "";
    name = fullPath;

    if( fullPath.empty() )
    {
        LOGE( TAG, "fullPath is empty", "" );
        return false;
    }

    nsCmn::replace( name, "\\", "/" );

    std::string::size_type pos = name.find_last_of( "/" );
    if( pos == std::string::npos )
    {
        LOGE( TAG, "%s does not have a path", fullPath.c_str() );
        return false;
    }

    path = name.substr( 0, ( pos - 1 ) );   //skip '/'
    name = name.substr( pos + 1, ( name.size() - ( pos + 1 ) ) );   //skip '/'

    return true;
}
bool CFileManager::unionPathAndNameToFullPath( const std::string& path, const std::string& name, std::string & fullPath )
{
    fullPath = path;

    if( path.empty() || name.empty() )
    {
        LOGE( TAG, "fullPath is empty", "" );
        return false;
    }

    nsCmn::replace( fullPath, "\\", "/" );

    if( fullPath.back() != '/' )
        fullPath.push_back( '/' );

    std::string::const_iterator it = name.cbegin();
    if( name.front() == '/' )
        ++it;

    fullPath.insert( fullPath.end(), it, name.cend() );

    return true;
}

bool CFileManager::isDir( const std::string& path )
{
    struct stat buffer;
    if( stat( path.c_str(), &buffer ) == -1 )
    {
        LOGE( TAG, "Failed to get stat file: %s, %s", path.c_str(), strerror( errno ) );
        return false;
    }

    return buffer.st_mode & S_IFDIR;
}

std::string CFileManager::getSuffix( const std::string & path )
{
    std::string suffix = "";

    int pos = path.find_last_of( '.' );

    if( pos != std::string::npos )
    {
        ++pos; //skip dot
        suffix = path.substr( pos );
    }

    return suffix;
}

#if defined( __linux__ )
int CFileManager::createFile(const std::string &path, __mode_t __mode)
{
    int fd = creat( path.c_str(), __mode );
    if( fd <= 0 )
        LOGE( TAG, "Failed to create file: %s, mode: %d, %s", path.c_str(), (int)__mode, strerror( errno ) );
    return fd;
}

int CFileManager::openFile(const std::string &path, int flag, __mode_t __mode)
{
    int fd = open( path.c_str(), flag, __mode );
    if( fd <= 0 )
        LOGE( TAG, "Failed to open file: %s, flag: %d, mode: %d, %s", path.c_str(), flag, (int)__mode, strerror( errno ) );
    return fd;
}

bool CFileManager::resetPermission(const std::string& path, __mode_t __mode)
{
    int result = chmod( path.c_str(), __mode );
    if( result == -1 )
    {
        LOGE( TAG, "Failed to reset permissions: %s, %s", path.c_str(), strerror( errno ) );
        return false;
    }

    return true;
}

bool CFileManager::getOwnership(const std::string& path, const std::string& userName, __mode_t __mode)
{
    if( getuid() != 0 )
    {
        LOGE( TAG, "Permission denided. now uid: %d", (int)getuid() );
        return false;
    }

    struct passwd* root_pw  = getpwnam( userName.c_str() );

    int result = chown( path.c_str(), root_pw->pw_uid, root_pw->pw_gid );
    if( result == -1 )
    {
        LOGE( TAG, "Failed to get ownership: %s, %s", path.c_str(), strerror( errno ) );
        return false;
    }

    return true;
}

bool CFileManager::makeDir(const std::string &path, __mode_t __mode)
{
    DIR* dirCache;
    int result = mkdir( path.c_str(), __mode );
    if( result == -1 && errno != EEXIST )
    {
        LOGE( TAG, "Failed to make dir: %s, %s", path.c_str(), strerror( errno ) );
        return false;
    }

    dirCache = opendir( path.c_str() );
    if( dirCache == NULL )
    {
        LOGE( TAG, "Failed to make path: %s.", path.c_str() );
        return false;
    }

    closedir( dirCache );

    return true;
}

bool CFileManager::changeAccessRight(const std::string &path, __mode_t __mode)
{
    int result = chmod( path.c_str(), __mode );
    if( result == -1 )
    {
        LOGE( TAG, "Failed to change access right: %s, new right: %d, %s", path.c_str(), (int)__mode, strerror( errno ) );
        return false;
    }

    return true;
}

#elif defined( __APPLE__ )

int CFileManager::createFile( const std::string& path, mode_t __mode )
{
    int fd = creat( path.c_str(), __mode );
    if( fd <= 0 )
        LOGE( TAG, "Failed to create file: %s, mode: %d, %s", path.c_str(), (int)__mode, strerror( errno ) );
    return fd;
}

int CFileManager::openFile( const std::string& path, int flag, mode_t __mode )
{
    int fd = open( path.c_str(), flag, __mode );
    if( fd <= 0 )
        LOGE( TAG, "Failed to open file: %s, flag: %d, mode: %d, %s", path.c_str(), flag, (int)__mode, strerror( errno ) );
    return fd;
}

bool CFileManager::resetPermission( const std::string& path, mode_t __mode )
{
    int result = chmod( path.c_str(), __mode );
    if( result == -1 )
    {
        LOGE( TAG, "Failed to reset permissions: %s, %s", path.c_str(), strerror( errno ) );
        return false;
    }

    return true;
}

bool CFileManager::getOwnership( const std::string& path, const std::string& userName, mode_t __mode )
{
    if( getuid() != 0 )
    {
        LOGE( TAG, "Permission denided. now uid: %d", (int)getuid() );
        return false;
    }

    struct passwd* root_pw = getpwnam( userName.c_str() );

    int result = chown( path.c_str(), root_pw->pw_uid, root_pw->pw_gid );
    if( result == -1 )
    {
        LOGE( TAG, "Failed to get ownership: %s, %s", path.c_str(), strerror( errno ) );
        return false;
    }

    return true;
}

bool CFileManager::makeDir( const std::string& path, mode_t __mode )
{
    DIR* dirCache;
    int result = mkdir( path.c_str(), __mode );
    if( result == -1 && errno != EEXIST )
    {
        LOGE( TAG, "Failed to make dir: %s, %s", path.c_str(), strerror( errno ) );
        return false;
    }

    dirCache = opendir( path.c_str() );
    if( dirCache == NULL )
    {
        LOGE( TAG, "Failed to make path: %s.", path.c_str() );
        return false;
    }

    closedir( dirCache );

    return true;
}

bool CFileManager::changeAccessRight( const std::string& path, mode_t __mode )
{
    int result = chmod( path.c_str(), __mode );
    if( result == -1 )
    {
        LOGE( TAG, "Failed to change access right: %s, new right: %d, %s", path.c_str(), (int)__mode, strerror( errno ) );
        return false;
    }

    return true;
}

#elif defined(_WIN32) || defined(_WIN64)

int CFileManager::createFile( const std::string &path, mode_t __mode )
{
    int fd = creat( path.c_str(), __mode & MS_MODE_MASK );
    if( fd <= 0 )
        LOGE( TAG, "Failed to create file: %s, mode: %d, %s", path.c_str(), (int)__mode, strerror( errno ) );
    return fd;
}

HANDLE CFileManager::winCreateFile( const std::string& path, DWORD dwDesiredAccess, DWORD dwShareMode
                                 , LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition
                                 , DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    if( path.empty() )
    {
        LOGE( TAG, "File path is Empty", "" );
        return INVALID_HANDLE_VALUE;
    }

#ifdef UNICODE
    CA2W wstPath( path.c_str() );

    HANDLE hFile = CreateFileW( wstPath
                            , dwDesiredAccess
                            , dwShareMode
                            , lpSecurityAttributes
                            , dwCreationDisposition
                            , dwFlagsAndAttributes
                            , hTemplateFile );
#else
    HANDLE hFile = CreateFileA( (LPCSTR)path.c_str()
                            , dwDesiredAccess
                            , dwShareMode
                            , lpSecurityAttributes
                            , dwCreationDisposition
                            , dwFlagsAndAttributes
                            , hTemplateFile );
#endif // !UNICODE
    if( !hFile )
        LOGE( TAG, "Failed to create file: %s, dwDesiredAccess: %ld, dwShareMode: %ld, dwCreationDisposition: %ld, dwFlagsAndAttributes: %ld, hTemplateFile: %ld, %s"
            , path.c_str()
            , dwDesiredAccess
            , dwShareMode
            , dwCreationDisposition
            , dwFlagsAndAttributes
            , hTemplateFile
            , GetLastErrorAsString().c_str() );

    return hFile;
}

int CFileManager::openFile( const std::string &path, int flag, mode_t __mode )
{
    int fd = open( path.c_str(), flag, __mode  & MS_MODE_MASK );
    if( fd <= 0 )
        LOGE( TAG, "Failed to open file: %s, flag: %d, mode: %d, %s", path.c_str(), flag, (int)__mode, strerror( errno ) );
    return fd;
}

HANDLE CFileManager::winOpenFile( const std::string& path, DWORD dwDesiredAccess, DWORD dwShareMode
    , LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition
    , DWORD dwFlagsAndAttributes, HANDLE hTemplateFile )
{
    if( path.empty() )
    {
        LOGE( TAG, "File path is Empty", "" );
        return INVALID_HANDLE_VALUE;
    }

#ifdef UNICODE
    CA2W wstPath( path.c_str() );

    HANDLE hFile = CreateFileW( wstPath
                            , dwDesiredAccess
                            , dwShareMode
                            , lpSecurityAttributes
                            , dwCreationDisposition
                            , dwFlagsAndAttributes
                            , hTemplateFile );
#else
    HANDLE hFile = CreateFileA( (LPCSTR)path.c_str()
                            , dwDesiredAccess
                            , dwShareMode
                            , lpSecurityAttributes
                            , dwCreationDisposition
                            , dwFlagsAndAttributes
                            , hTemplateFile );

#endif // !UNICODE
    if( !hFile )
        LOGE( TAG, "Failed to open file: %s, dwDesiredAccess: %ld, dwShareMode: %ld, dwCreationDisposition: %ld, dwFlagsAndAttributes: %ld, hTemplateFile: %ld, %s"
            , path.c_str()
            , dwDesiredAccess
            , dwShareMode
            , dwCreationDisposition
            , dwFlagsAndAttributes
            , hTemplateFile
            , GetLastErrorAsString().c_str() );

    return hFile;
}

bool CFileManager::winIsExistFile( const std::string& path )
{
    if( path.empty() )
    {
        LOGE( TAG, "File path is Empty", "" );
        return false;
    }

    CA2W wstPath( path.c_str() );

    DWORD fileAttr = GetFileAttributes( wstPath );
    if( 0xFFFFFFFF == fileAttr && GetLastError() == ERROR_FILE_NOT_FOUND )
        return false;

    return true;
}

bool CFileManager::winGetFileSize( const std::string &path, size_t& size )
{
    size = 0;

    if( path.empty() )
    {
        LOGE( TAG, "File path is Empty", "" );
        return false;
    }

    BOOL bRes = FALSE;

    LOGI( TAG, "open file for get file size: %s", path.c_str() );

    HANDLE hFile = winOpenFile( path, GENERIC_READ );
    if( hFile )
    {
        LARGE_INTEGER lpFileSize;
        bRes = GetFileSizeEx( hFile, &lpFileSize );
        winCloseFile( hFile );

        if( lpFileSize.QuadPart > ( (ULONGLONG)( (size_t)( -1 ) ) ) )
        {
            LOGE( TAG, "File size is too big", "" );
            return false;
        }

        size = (size_t)( lpFileSize.QuadPart );
    }

    return ( bRes == TRUE );
}

bool CFileManager::resetPermission( const std::string & path, mode_t __mode )
{
    int result = chmod( path.c_str(), __mode & MS_MODE_MASK );
    if( result == -1 )
    {
        LOGE( TAG, "Failed to reset permissions: %s, %s", path.c_str(), strerror( errno ) );
        return false;
    }

    return true;
}

bool CFileManager::winLookupOwnership( const std::string& path, std::string& userName )
{
    bool isSuccess = false;

    if( path.empty() )
    {
        LOGE( TAG, "File path is Empty", "" );
        return false;
    }

    LOGI( TAG, "open file for change ownership: %s", path.c_str() );

#ifdef UNICODE
    CA2W wstPath( path.c_str() );

    HANDLE hFile = CreateFileW( wstPath
                            , GENERIC_READ
                            , FILE_SHARE_READ
                            , nullptr
                            , OPEN_EXISTING
                            , FILE_ATTRIBUTE_NORMAL
                            , nullptr );
#else
    HANDLE hFile = CreateFileA( (LPCSTR)path.c_str()
                            , GENERIC_READ
                            , FILE_SHARE_READ
                            , nullptr
                            , OPEN_EXISTING
                            , FILE_ATTRIBUTE_NORMAL
                            , nullptr );
#endif // !UNICODE

    if( hFile == INVALID_HANDLE_VALUE )
    {
        LOGE( TAG, "Failed to open file: %s, %s"
            , path.c_str()
            , GetLastErrorAsString().c_str() );
        return isSuccess;
    }

    isSuccess = winLookupOwnership( hFile, userName );

    winCloseFile( hFile );

    return isSuccess;
}

bool CFileManager::winLookupOwnership( HANDLE hFile, std::string& userName )
{
    bool isSuccess = false;

    if( !hFile )
    {
        LOGE( TAG, "Invalied file hadle", "" );
        return false;
    }

    PSID pSidOwner = nullptr;
    PSECURITY_DESCRIPTOR pSD = nullptr;

    // Get the owner SID of the file.
    DWORD dwRes = GetSecurityInfo( hFile
                                , SE_FILE_OBJECT
                                , OWNER_SECURITY_INFORMATION
                                , &pSidOwner
                                , nullptr
                                , nullptr
                                , nullptr
                                , &pSD );

    // Check GetLastError for GetSecurityInfo error condition.
    if( dwRes != ERROR_SUCCESS )
    {
        LOGE( TAG, "Failed to GetSecurityInfo: %d, %s"
            , hFile
            , GetLastErrorAsString().c_str() );
        return isSuccess;
    }

    DWORD dwAcctName = 1;
    DWORD dwDomainName = 1;
    SID_NAME_USE eUse = SidTypeUnknown;

#ifdef UNICODE
    LPTSTR lpAcctName = nullptr;
    LPTSTR lpDomainName = nullptr;

    do
    {
        // First call to LookupAccountSid to get the buffer sizes.
        BOOL bRes = LookupAccountSidW( nullptr           // local computer
                                    , pSidOwner
                                    , lpAcctName
                                    , (LPDWORD)&dwAcctName
                                    , lpDomainName
                                    , (LPDWORD)&dwDomainName
                                    , &eUse );

        // Reallocate memory for the buffers.
        lpAcctName = (LPTSTR)GlobalAlloc( GMEM_FIXED, dwAcctName );

        // Check GetLastError for GlobalAlloc error condition.
        if( lpAcctName == nullptr )
        {
            LOGE( TAG, "Failed to AcctName GlobalAlloc: %d, %s"
                , hFile
                , GetLastErrorAsString().c_str() );
            break;
        }

        lpDomainName = (LPTSTR)GlobalAlloc( GMEM_FIXED, dwDomainName );

        // Check GetLastError for GlobalAlloc error condition.
        if( lpDomainName == nullptr )
        {
            LOGE( TAG, "Failed to DomainName GlobalAlloc: %d, %s"
                , hFile
                , GetLastErrorAsString().c_str() );
            break;

        }

        // Second call to LookupAccountSid to get the account name.
        bRes = LookupAccountSidW( nullptr                // name of local or remote computer
                                , pSidOwner             // security identifier
                                , lpAcctName            // account name buffer
                                , (LPDWORD)&dwAcctName  // size of account name buffer 
                                , lpDomainName          // domain name
                                , (LPDWORD)&dwDomainName// size of domain name buffer
                                , &eUse );              // SID type

        // Check GetLastError for LookupAccountSid error condition.
        if( bRes == FALSE )
        {
            DWORD dwErrorCode = GetLastError();
            if( dwErrorCode == ERROR_NONE_MAPPED )
                LOGE( TAG, "Account owner not found for specified SID: %d, %s", hFile, GetLastErrorAsString().c_str() );
            else
                LOGE( TAG, "Error in LookupAccountSid: %d, %s", hFile, GetLastErrorAsString().c_str() );
            break;
        }

        userName = CT2A( lpAcctName );
#else
    LPSTR lpAcctName = nullptr;
    LPSTR lpDomainName = nullptr;

    do
    {
        // First call to LookupAccountSid to get the buffer sizes.
        BOOL bRes = LookupAccountSidA( nullptr           // local computer
                                    , pSidOwner
                                    , lpAcctName
                                    , (LPDWORD)&dwAcctName
                                    , lpDomainName
                                    , (LPDWORD)&dwDomainName
                                    , &eUse );

        // Reallocate memory for the buffers.
        lpAcctName = (LPSTR)GlobalAlloc( GMEM_FIXED, dwAcctName );

        // Check GetLastError for GlobalAlloc error condition.
        if( lpAcctName == nullptr )
        {
            LOGE( TAG, "Failed to AcctName GlobalAlloc: %d, %s"
                , hFile
                , GetLastErrorAsString().c_str() );
            break;
        }

        lpDomainName = (LPCSTR)GlobalAlloc( GMEM_FIXED, dwDomainName );

        // Check GetLastError for GlobalAlloc error condition.
        if( lpDomainName == nullptr )
        {
            LOGE( TAG, "Failed to DomainName GlobalAlloc: %d, %s"
                , hFile
                , GetLastErrorAsString().c_str() );
            break;

        }

        // Second call to LookupAccountSid to get the account name.
        bRes = LookupAccountSidA( nullptr                // name of local or remote computer
                                , pSidOwner             // security identifier
                                , lpAcctName            // account name buffer
                                , (LPDWORD)&dwAcctName  // size of account name buffer 
                                , lpDomainName          // domain name
                                , (LPDWORD)&dwDomainName// size of domain name buffer
                                , &eUse );              // SID type

        // Check GetLastError for LookupAccountSid error condition.
        if( bRes == FALSE )
        {
            DWORD dwErrorCode = GetLastError();
            if( dwErrorCode == ERROR_NONE_MAPPED )
                LOGE( TAG, "Account owner not found for specified SID: %d, %s", hFile, GetLastErrorAsString().c_str() );
            else
                LOGE( TAG, "Error in LookupAccountSid: %d, %s", hFile, GetLastErrorAsString().c_str() );
            break;
        }

        userName = lpAcctName;

#endif // !UNICODE

        // Print the account name.
        LOGD( TAG, "Account owner = %s", userName.c_str() );

        isSuccess = true;

    } while( false );

    if( pSD != nullptr )
        LocalFree( pSD );

    if( lpAcctName != nullptr )
        GlobalFree( lpAcctName );

    if( lpDomainName != nullptr )
        GlobalFree( lpDomainName );

    return isSuccess;
}

bool CFileManager::winGetOwnership( const std::string& path, const std::string& userName, mode_t __mode )
{
    bool isSuccess = false;
    PSID pSidOwner = nullptr;
    PSECURITY_DESCRIPTOR pSD = nullptr;

    do 
    {
        // #includes omitted for the sake of sanity
        HANDLE token;
        int retValue = 1;

        // Get the privileges you need
        if( OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token ) )
        {
            if( winSetPrivilege( token, SE_TAKE_OWNERSHIP_NAME, 1 ) == false || winSetPrivilege( token, SE_SECURITY_NAME, 1 ) == false
                || winSetPrivilege( token, SE_BACKUP_NAME, 1 ) == false || winSetPrivilege( token, SE_RESTORE_NAME, 1 ) == false )
            {
                LOGE( TAG, "Error in SetPrivilege: %s, userNamer: %s, %s"
                    , path.c_str()
                    , userName.c_str()
                    , GetLastErrorAsString().c_str() );
                break;
            }
        }
        else
        {
            LOGE( TAG, "Error in OpenProcessToken: %s, userNamer: %s, %s"
                , path.c_str()
                , userName.c_str()
                , GetLastErrorAsString().c_str() );
            break;
        }

        // Create the security descriptor
        DWORD dwSdSize = 0;
#ifdef UNICODE
        CA2W wstPath( path.c_str() );
        GetFileSecurityW( wstPath, OWNER_SECURITY_INFORMATION, pSD, 0, &dwSdSize );
#else
        GetFileSecurityA( (LPCSTR)path.c_str(), OWNER_SECURITY_INFORMATION, pSD, 0, &dwSdSize );
#endif // !UNICODE
        pSD = (PSECURITY_DESCRIPTOR)malloc( dwSdSize );
        if( !InitializeSecurityDescriptor( pSD, SECURITY_DESCRIPTOR_REVISION ) )
        {
            LOGE( TAG, "Error in InitializeSecurityDescriptor: %s, userNamer: %s, %s"
                , path.c_str()
                , userName.c_str()
                , GetLastErrorAsString().c_str() );
            break;
        }

        // Get the sid for the username
        DWORD dwDomainName = 4096;
        DWORD sidSize = 0;
        SID_NAME_USE eUse = SidTypeUnknown;

#ifdef UNICODE
        wchar_t domainbuf[4096];

        CA2W wstUserName( userName.c_str() );
        LookupAccountNameW( NULL, wstUserName, pSidOwner, &sidSize, domainbuf, &dwDomainName, &eUse );
        pSidOwner = (PSID)malloc( sidSize );
        if( !LookupAccountNameW( NULL, wstUserName, (PSID)pSidOwner, &sidSize, domainbuf, &dwDomainName, &eUse ) )
#else
        char domainbuf[4096];

        LookupAccountNameA( NULL, (LPCSTR)userName.c_str(), pSidOwner, &sidSize, domainbuf, &dwDomainName, &eUse );
        pSidOwner = (PSID)malloc( sidSize );
        if( !LookupAccountNameA( NULL, (LPCSTR)userName.c_str(), (PSID)pSidOwner, &sidSize, domainbuf, &dwDomainName, &eUse ) )
#endif // !UNICODE
        {
            LOGE( TAG, "Error in LookupAccountName: %s, userNamer: %s, %s"
                , path.c_str()
                , userName.c_str()
                , GetLastErrorAsString().c_str() );
            break;
        }

        // Set the sid to be the new owner
        if( !SetSecurityDescriptorOwner( pSD, pSidOwner, 0 ) )
        {
            LOGE( TAG, "Error in SetSecurityDescriptorOwner: %s, userNamer: %s, %s"
                , path.c_str()
                , userName.c_str()
                , GetLastErrorAsString().c_str() );
            break;
        }

        // Save the security descriptor
#ifdef UNICODE
        if( !SetFileSecurityW( wstPath, OWNER_SECURITY_INFORMATION, pSD ) )
#else
        if( !SetFileSecurityA( (LPCSTR)path.c_str(), OWNER_SECURITY_INFORMATION, pSD ) )
#endif // !UNICODE
        {
            LOGE( TAG, "Error in SetFileSecurity: %s, userNamer: %s, %s"
                , path.c_str()
                , userName.c_str()
                , GetLastErrorAsString().c_str() );
            break;
        }

        isSuccess = true;

    } while( false );

    if( pSD )
        free( pSD );

    if( pSidOwner )
        free( pSidOwner );

    if( isSuccess )
        isSuccess = resetPermission( path, __mode );

    return isSuccess;
}

bool CFileManager::winSetPrivilege( HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege )
{
    TOKEN_PRIVILEGES tp;
    LUID luid;

    if( !LookupPrivilegeValue( nullptr          // lookup privilege on local system
                            , lpszPrivilege     // privilege to lookup 
                            , &luid ) )         // receives LUID of privilege
    {
        LOGE( TAG, "Error in LookupPrivilegeValue: %s, bEnablePrivilege: %d, %s"
            , lpszPrivilege
            , (int)bEnablePrivilege
            , GetLastErrorAsString().c_str() );
        return false;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if( bEnablePrivilege )
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;

    // Enable the privilege or disable all privileges.

    if( !AdjustTokenPrivileges( hToken
                            , FALSE
                            , &tp
                            , sizeof( TOKEN_PRIVILEGES )
                            , (PTOKEN_PRIVILEGES)nullptr
                            , (PDWORD)nullptr ) )
    {
        LOGE( TAG, "Error in AdjustTokenPrivileges: %s, bEnablePrivilege: %d, %s"
            , lpszPrivilege
            , (int)bEnablePrivilege
            , GetLastErrorAsString().c_str() );
        return false;
    }

    if( GetLastError() == ERROR_NOT_ALL_ASSIGNED )
    {        
        LOGE( TAG, "The token does not have the specified privilege: %d, %s"
            , lpszPrivilege
            , (int)bEnablePrivilege
            , GetLastErrorAsString().c_str() );
        return false;
    }

    return true;
}

bool CFileManager::makeDir( const std::string& path, mode_t __mode )
{
    int result = mkdir( path.c_str() );
    if( result == -1 && errno != EEXIST )
    {
        LOGE( TAG, "Failed to make cache dir: %s, %s", path.c_str(), strerror( errno ) );
        return false;
    }

    return resetPermission( path, __mode );
}

bool CFileManager::winMakeDir( const std::string & path, LPSECURITY_ATTRIBUTES lpSecurityAttributes )
{
    if( path.empty() )
    {
        LOGE( TAG, "File path is Empty", "" );
        return INVALID_HANDLE_VALUE;
    }

    BOOL bRes = FALSE;

#ifdef UNICODE
    CA2W wstPath( path.c_str() );

    bRes = CreateDirectoryW( wstPath
                        , lpSecurityAttributes );
#else
    bRes = CreateDirectoryA( (LPCSTR)path.c_str()
                        , lpSecurityAttributes );
#endif // !UNICODE

    if( bRes == FALSE )
    {
        if( GetLastError() != ERROR_ALREADY_EXISTS )
        {
            LOGE( TAG, "Failed to create directory: %s, %s"
                , path.c_str(), GetLastErrorAsString().c_str() );
        }
        else
            bRes = TRUE;
    }

    return ( bRes == TRUE );
}

bool CFileManager::winReadFile( HANDLE hFile, char* data, size_t size, size_t& szRead )
{
    if( !hFile )
    {
        LOGE( TAG, "Invalied file hadle", "" );
        return false;
    }

    DWORD dwRead = 0;
    szRead = -1;

    if( ReadFile( hFile, (LPVOID)data, static_cast<DWORD>( size ), &dwRead, nullptr ) == FALSE
        && GetLastError() != ERROR_IO_PENDING )
    {
        LOGE( TAG, "Failed to read file: %s", GetLastErrorAsString().c_str() );
        return false;
    }

    szRead = static_cast<DWORD>( dwRead );

    return true;
}

bool CFileManager::winWriteFile( HANDLE hFile, const char* data, size_t size, size_t& szWrite )
{
    if( !hFile )
    {
        LOGE( TAG, "Invalied file hadle", "" );
        return false;
    }

    DWORD dwWrite = 0;
    szWrite = -1;

    if( WriteFile( hFile, (LPVOID)data, static_cast<DWORD>(size), &dwWrite, nullptr ) == FALSE
        && GetLastError() != ERROR_IO_PENDING )
    {
        LOGE( TAG, "Failed to write file: %s", GetLastErrorAsString().c_str() );
        return false;
    }

    szWrite = static_cast<DWORD>( dwWrite );

    return true;
}

bool CFileManager::changeAccessRight( const std::string& path, mode_t __mode )
{
    int result = chmod( path.c_str(), __mode & MS_MODE_MASK );
    if( result == -1 )
    {
        LOGE( TAG, "Failed to reset permissions: %s, %s", path.c_str(), strerror( errno ) );
        return false;
    }

    return true;
}

bool CFileManager::winCloseFile( HANDLE hFile )
{
    BOOL bRes = FALSE;

    bRes = CloseHandle( hFile );
    if( bRes == FALSE )
        LOGE( TAG, "Failed to close handle: %s", GetLastErrorAsString().c_str() );

    return ( bRes == TRUE );
}

std::string CFileManager::GetLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0)
        return std::string(); //No error message has been recorded

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}

#endif
