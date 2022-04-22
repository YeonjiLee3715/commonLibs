#ifndef CFILEWATCHER_H
#define CFILEWATCHER_H

#include <sys/inotify.h>
#include <memory>
#include <thread>
#include <vector>

class IFileWatcherListener
{
public:
    virtual void            OnDirChanged( uint32_t event, const std::string& watchPath, const std::string& fileName ) = 0;
    virtual void            OnFileChanged( uint32_t event, const std::string& watchPath, const std::string& fileName ) = 0;
};

typedef struct tyWatchItem
{
    int watchDescriptor;
    std::string watchPath;
    uint32_t mask;

    tyWatchItem():watchDescriptor(-1), mask(-1) {}

}WATCH_ITEM;

class CFileWatcher
{
public:
    CFileWatcher();
    ~CFileWatcher();

    void AddWatch( const std::string& watchPath, bool isRecursive, uint32_t eventFilter = IN_ALL_EVENTS );
    void RemoveWatch(int watchDescriptor );
    void RemoveWatch( const std::string& watchPath );

    WATCH_ITEM GetWatchItemByWD( int watchDescriptor );
    std::string GetPathByWD( int watchDescriptor );
    int GetWDByPath( const std::string& watchPath );

    bool IsStopWatch();

    void StartWatch();
    void StopWatch();

    void clear();

    void EmitDirChanged( uint32_t event, const std::string& watchPath, const std::string& fileName );
    void EmitFileChanged( uint32_t event, const std::string& watchPath, const std::string& fileName );

    int GetFd();

    void SetListener( IFileWatcherListener* listener );

protected:
    static void threadFileWatch( CFileWatcher* pCFileWatcher );

private:
    bool m_isStopWatch;
    std::shared_ptr< std::thread > m_thWatch;
    int m_fd;

    IFileWatcherListener* m_IFileWatcherListener;

    std::vector< WATCH_ITEM > m_vecWatchItem;
};

#endif // CFILEWATCHER_H
