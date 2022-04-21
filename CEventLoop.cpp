#include "CEventLoop.h"

CEventLoop::CEventLoop(QObject *parent)
    : QEventLoop(parent), m_bIsTimeout(false)
{
    m_timer.setSingleShot(true);

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(Timeout()));
}

CEventLoop::~CEventLoop()
{

}

bool CEventLoop::IsTimeout()
{
    return m_bIsTimeout;
}

int CEventLoop::exec( int msTimeout, QEventLoop::ProcessEventsFlags flags)
{
    m_timer.start( msTimeout );
    return QEventLoop::exec( flags );
}

void CEventLoop::Timeout()
{
    m_bIsTimeout = true;
    quit();
}

void CEventLoop::quit()
{
    if( m_timer.isActive() )
        m_timer.stop();

    disconnect(&m_timer, SIGNAL(timeout()), this, SLOT(Timeout()));

    QEventLoop::quit();
}
