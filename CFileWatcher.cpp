#include "CFileWatcher.h"
#include <errno.h>

#include <utils/CStrManager.h>
#include <logger/CLogger.h>
#include <define/Defines.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

namespace nsCFileWatcher{
    const char* TAG = "CFileWatcher";
}

using namespace nsCFileWatcher;

CFileWatcher::CFileWatcher()
{
    m_fd = inotify_init();
}

CFileWatcher::~CFileWatcher()
{
    clear();
}

void CFileWatcher::AddWatch( const std::string& watchPath, bool isRecursive, uint32_t eventFilter )
{
    if( m_fd < 0 )
        return;

    int wd = inotify_add_watch( m_fd, watchPath.c_str(), eventFilter );
    if( wd < 0 )
        return;

    WATCH_ITEM watchItem;
    watchItem.watchDescriptor = wd;
    watchItem.watchPath = watchPath;
    watchItem.mask = eventFilter;

    m_vecWatchItem.push_back( watchItem );
}

void CFileWatcher::RemoveWatch( int watchDescriptor )
{
    if( m_fd < 0 )
        return;

    std::vector< WATCH_ITEM >::iterator it = m_vecWatchItem.begin();
    for( ; it != m_vecWatchItem.end(); ++it )
    {
        if( watchDescriptor == it->watchDescriptor )
            break;
    }

    if( it != m_vecWatchItem.end() )
    {
        (void)inotify_rm_watch( m_fd, it->watchDescriptor );
        m_vecWatchItem.erase( it );
    }
}

void CFileWatcher::RemoveWatch( const std::string& watchPath )
{
    if( m_fd < 0 )
        return;

    std::vector< WATCH_ITEM >::iterator it = m_vecWatchItem.begin();
    for( ; it != m_vecWatchItem.end(); ++it )
    {
        if( watchPath == it->watchPath )
            break;
    }

    if( it != m_vecWatchItem.end() )
    {
        (void)inotify_rm_watch( m_fd, it->watchDescriptor );
        m_vecWatchItem.erase( it );
    }
}

WATCH_ITEM CFileWatcher::GetWatchItemByWD(int watchDescriptor)
{
    for( std::vector< WATCH_ITEM >::iterator it = m_vecWatchItem.begin()
         ; it != m_vecWatchItem.end(); ++it )
    {
        if( watchDescriptor == it->watchDescriptor )
            return *it;
    }

    return WATCH_ITEM();
}

std::string CFileWatcher::GetPathByWD(int watchDescriptor)
{
    for( std::vector< WATCH_ITEM >::iterator it = m_vecWatchItem.begin()
         ; it != m_vecWatchItem.end(); ++it )
    {
        if( watchDescriptor == it->watchDescriptor )
            return it->watchPath;
    }

    return std::string();
}

int CFileWatcher::GetWDByPath( const std::string& watchPath )
{
    for( std::vector< WATCH_ITEM >::iterator it = m_vecWatchItem.begin()
         ; it != m_vecWatchItem.end(); ++it )
    {
        if( watchPath == it->watchPath )
            return it->watchDescriptor;
    }

    return -1;
}

bool CFileWatcher::IsStopWatch()
{
    return m_isStopWatch;
}

void CFileWatcher::StartWatch()
{
    m_isStopWatch = false;
    m_thWatch = std::make_shared<std::thread>(std::bind(&CFileWatcher::threadFileWatch, this));
}

void CFileWatcher::StopWatch()
{
    m_isStopWatch = true;
}

void CFileWatcher::clear()
{
    m_isStopWatch = true;
    if( m_thWatch != nullptr )
    {
        m_thWatch->join();
        m_thWatch = nullptr;
    }

    while( m_vecWatchItem.empty() == false )
    {
        std::vector< WATCH_ITEM >::iterator it = m_vecWatchItem.begin();
        RemoveWatch( it->watchDescriptor );
    }

    (void)close( m_fd );
}

void CFileWatcher::EmitDirChanged( uint32_t event, const std::string& watchPath, const std::string& fileName )
{
    if( m_IFileWatcherListener != nullptr )
        m_IFileWatcherListener->OnDirChanged( event, watchPath, fileName );
}

void CFileWatcher::EmitFileChanged( uint32_t event, const std::string& watchPath, const std::string& fileName )
{
    if( m_IFileWatcherListener != nullptr )
        m_IFileWatcherListener->OnDirChanged( event, watchPath, fileName );
}

int CFileWatcher::GetFd()
{
    return m_fd;
}

void CFileWatcher::SetListener(IFileWatcherListener* listener)
{
    if( listener != nullptr )
        m_IFileWatcherListener = listener;
}

void CFileWatcher::threadFileWatch(CFileWatcher* pCFileWatcher)
{
    int length = 0;
    int idx = 0;
    char buffer[BUF_LEN];

    while( pCFileWatcher != nullptr && pCFileWatcher->IsStopWatch() == false )
    {
        struct inotify_event* event;

        length = read( pCFileWatcher->GetFd(), buffer, BUF_LEN );

        if( length < 0 )
        {
            LOGE( TAG, "Failed to read event" );
            break;
            //emit error;
        }

        event = ( struct inotify_event * ) &buffer[ idx ];

        if( event->len <= 0 )
        {
            usleep( 1000 );
            LOGW( TAG, "Invalied len" );
            continue;
        }

        WATCH_ITEM watchItem = pCFileWatcher->GetWatchItemByWD( event->wd );
        if( watchItem.watchDescriptor < 0 )
        {
            usleep( 1000 );
            LOGW( TAG, "Invalied watch descriptor: %d name: %s", event->wd, event->name );
            continue;
        }

        if( event->mask & IN_ISDIR )
        {
            LOGD( TAG, "Received event %u from directory( %s ) of &s", event->mask, event->name, watchItem.watchPath.c_str() );
            pCFileWatcher->EmitDirChanged( event->mask, watchItem.watchPath, event->name );
        }
        else
        {
            LOGD( TAG, "Received event %u from file( %s ) of %s", event->mask, event->name, watchItem.watchPath.c_str() );
            pCFileWatcher->EmitFileChanged( event->mask, watchItem.watchPath, event->name );
        }
    }
}
