/*
 * CWorker.cpp
 *
 *  Created on: 2020. 01. 29.
 *      Author: 이연지
 */

#include <zconf.h>
#include "CWorker.h"

CWorker::CWorker()
        : m_thTask(nullptr), m_bWakeup(true), m_bSleepThead(false), m_bStopThread(true)
{

}

CWorker::~CWorker()
{
    if( IsStopped() == false )
        Stop();
}

void CWorker::Stop()
{
    if( m_bStopThread )
        return;

    {
        std::unique_lock< std::mutex > lck( m_mtxTh );

        m_bStopThread = true;

        if( m_bSleepThead )
        {
            m_bWakeup = true;
            m_cv.notify_all();
        }
    }

    usleep( 10 * 1000 ); //wait for thread stop

    std::unique_lock< std::mutex > lck( m_mtxTh );

    if( m_thTask != nullptr )
    {
        if( m_thTask->joinable() )
            m_thTask->join();
        delete m_thTask;
        m_thTask = nullptr;
    }
}

bool CWorker::IsStop()
{
    return m_bStopThread;
}

bool CWorker::IsStopped()
{
    return !(m_thTask != nullptr && m_thTask->joinable());
}

bool CWorker::IsSleep()
{
    return (m_bSleepThead && m_bWakeup == false);
}

void CWorker::sleepThead()
{
    if( m_bSleepThead || m_bStopThread )
        return;

    std::unique_lock<std::mutex> lck(m_mtxTh);
    m_bWakeup = false;
    while( m_bStopThread == false && m_bWakeup == false )
    {
        m_bSleepThead = true;
        m_cv.wait( lck );

        if( m_bStopThread || m_bWakeup )
            break;
    }

    m_bSleepThead = false;
}

void CWorker::wakeupThead()
{
    if( m_bSleepThead == false )
        return;

    std::unique_lock<std::mutex> lck(m_mtxTh);
    m_bWakeup = true;
    m_cv.notify_one();
}