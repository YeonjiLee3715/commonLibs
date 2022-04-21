#ifndef QAPPLICATIONMUTEX_H
#define QAPPLICATIONMUTEX_H

#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>

class QApplicationMutex
{
public:
    QApplicationMutex( const QString& key );
    ~QApplicationMutex();

    bool isAnotherRunning();
    bool tryToRun();
    void release();

private:
    const QString key;
    const QString memLockKey;
    const QString sharedmemKey;

    QSharedMemory sharedMem;
    QSystemSemaphore memLock;

    Q_DISABLE_COPY( QApplicationMutex )
};

#endif // QAPPLICATIONMUTEX_H
