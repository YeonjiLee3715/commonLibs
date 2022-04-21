#include "QWorker.h"

#include <QMutexLocker>

QWorker::QWorker()
 : QObject(nullptr), m_thTask( nullptr ), m_bWakeup( true ), m_bSleepThead( false ), m_bStopThread( true )
{
}

QWorker::~QWorker()
{
    if( IsStopped() == false )
        Stop();

    delete m_thTask;
    m_thTask = nullptr;
}

bool QWorker::Start( Qt::ConnectionType connection, QThread::Priority priority )
{
    QMutexLocker lck( &m_mtxTh );

    if( IsStop() == false )
        return false;

    m_bStopThread = false;

    if( m_thTask == nullptr )
        return false;

    if( m_thTask->isRunning() )
        return true;

    connect( m_thTask, &QThread::started, this, &QWorker::threadStarted );
    connect( m_thTask, &QThread::finished, this, &QWorker::threadStopped );

    return QMetaObject::invokeMethod( m_thTask, "start", connection, Q_ARG( QThread::Priority, priority ) );
}

bool QWorker::Wait( unsigned long time )
{
    if( m_thTask == nullptr || m_thTask->isRunning() == false )
        return true;

    return m_thTask->wait( time );
}

void QWorker::Stop( Qt::ConnectionType connection )
{
    if( m_bStopThread )
        return;

    {
        QMutexLocker lck( &m_mtxTh );

        m_bStopThread = true;

        if( m_bSleepThead )
        {
            m_bWakeup = true;
            m_cv.wakeAll();
        }
    }

    if( m_thTask == nullptr )
        return;

    if( m_thTask->isRunning() )
        m_thTask->wait( 10 * 1000 );

    QMutexLocker lck( &m_mtxTh );

    if( m_thTask != nullptr && m_thTask->isRunning() )
        QMetaObject::invokeMethod( m_thTask, "quit", connection );
}

void QWorker::threadStarted()
{
    // do something if needed

    emit this->started();
}

void QWorker::threadStopped()
{
    if( m_bStopThread == false )
        m_bStopThread = true;

    // do something if needed

    emit this->finished();
}

bool QWorker::IsStop()
{
    return m_bStopThread;
}

bool QWorker::IsStopped()
{
    return !( m_thTask != nullptr && m_thTask->isRunning() );
}

bool QWorker::IsSleep()
{
    return ( m_bSleepThead && m_bWakeup == false );
}

void QWorker::SleepThead()
{
    if( m_bSleepThead || m_bStopThread )
        return;

    QMutexLocker lck( &m_mtxTh );
    m_bWakeup = false;
    while( m_bStopThread == false && m_bWakeup == false )
    {
        m_bSleepThead = true;
        m_cv.wait( lck.mutex() );

        if( m_bStopThread || m_bWakeup )
            break;
    }

    m_bSleepThead = false;
}

void QWorker::WakeupThead()
{
    if( m_bSleepThead == false )
        return;

    QMutexLocker lck( &m_mtxTh );
    m_bWakeup = true;
    m_cv.wakeOne();
}
