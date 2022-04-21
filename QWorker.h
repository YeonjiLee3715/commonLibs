#ifndef QWORKER_H
#define QWORKER_H

#include <QObject>
#include <QWaitCondition>
#include <QMutex>
#include <QThread>

class QWorker : public QObject
{
    Q_OBJECT
public:
    explicit QWorker();
#ifdef Q_CLANG_QDOC
    template <typename Function, typename... Args>
    QWorker( Function&& f, Args&&... args );
    template <typename Function>
    QWorker( Function&& f );

    template <typename Function, typename... Args>
    bool SetTask( Function&& f, Args&&... args );
    template <typename Function>
    bool SetTask( Function&& f );
#else
#  if QT_CONFIG(cxx11_future)
#    ifdef QTHREAD_HAS_VARIADIC_CREATE
    template <typename Function, typename... Args>
    QWorker( Function&& f, Args&&... args );

    template <typename Function, typename... Args>
    bool SetTask( Function&& f, Args&&... args );
#    else
    template <typename Function>
    QWorker( Function&& f );

    template <typename Function>
    bool SetTask( Function&& f );
#    endif // QTHREAD_HAS_VARIADIC_CREATE
#  endif // QT_CONFIG(cxx11_future)
#endif // Q_CLANG_QDOC

    virtual ~QWorker();

    bool IsStop();
    bool IsStopped();
    bool IsSleep();

    void SleepThead();

public slots:
    void WakeupThead();

public:
    // default argument causes thread to block indefinetely
    bool Start( Qt::ConnectionType connection = Qt::DirectConnection, QThread::Priority priority = QThread::InheritPriority );
    bool Wait( unsigned long time = ULONG_MAX );
    void Stop( Qt::ConnectionType connection = Qt::DirectConnection );

signals:
    void started();
    void finished();

private slots:
    void threadStarted();
    void threadStopped();

private:
    QMutex          m_mtxTh;
    QWaitCondition  m_cv;
    QThread*        m_thTask;
    bool            m_bWakeup;
    bool            m_bSleepThead;
    bool            m_bStopThread;
};

#if QT_CONFIG(cxx11_future)

#if defined(QTHREAD_HAS_VARIADIC_CREATE) || defined(Q_CLANG_QDOC)
// C++17: std::thread's constructor complying call
template <typename Function, typename... Args>
QWorker::QWorker( Function&& f, Args&&... args )
    : QObject( nullptr ), m_thTask( nullptr ), m_bWakeup( true ), m_bSleepThead( false ), m_bStopThread( true )
{
    m_thTask = QThread::create( f, args ... );
}

template <typename Function, typename... Args>
bool QWorker::SetTask( Function&& f, Args&&... args )
{
    if( m_thTask != nullptr )
    {
        if( m_thTask->isRunning() )
            return false;

        delete m_thTask;
        m_thTask = nullptr;
    }

    m_thTask = QThread::create( f, args ... );
}

#elif defined(__cpp_init_captures) && __cpp_init_captures >= 201304
// C++14: implementation for just one callable
template <typename Function>
QWorker::QWorker( Function&& f )
    : QObject( nullptr ), m_thTask( nullptr ), m_bWakeup( true ), m_bSleepThead( false ), m_bStopThread( true )
{
    m_thTask =  QThread::create( f );
}

// C++14: implementation for just one callable
template <typename Function>
bool QWorker::SetTask( Function&& f )
{
    if( m_thTask != nullptr )
    {
        if( m_thTask->isRunning() )
            return false;

        delete m_thTask;
        m_thTask = nullptr;
    }

    m_thTask =  QThread::create( f );
}
#else
// C++11: same as C++14, but with a workaround for not having generalized lambda captures
template <typename Function>
QWorker::QWorker( Function&& f )
    : QObject( nullptr ), m_thTask( nullptr ), m_bWakeup( true ), m_bSleepThead( false ), m_bStopThread( true )
{
    m_thTask = QThread::create( f );
}

// C++11: same as C++14, but with a workaround for not having generalized lambda captures
template <typename Function>
bool QWorker::SetTask( Function&& f )
{
    if( m_thTask != nullptr )
    {
        if( m_thTask->isRunning() )
            return false;

        delete m_thTask;
        m_thTask = nullptr;
    }

    m_thTask = QThread::create( f );
}
#endif // QTHREAD_HAS_VARIADIC_CREATE

#endif // QT_CONFIG(cxx11_future)

#endif // QWORKER_H
