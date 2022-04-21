/*
 * CWorker.h
 *
 *  Created on: 2020. 01. 29.
 *      Author: 이연지
 */

#ifndef CWORKER_H_
#define CWORKER_H_

#include <iostream>
#include <thread>

class CWorker {

public:
    CWorker();
    ~CWorker();

    template <class _Fp, class ... Args >
    bool Start(_Fp&& fnc, Args&&... args)
    {
        std::unique_lock<std::mutex> lock(m_mtxTh);

        if( IsStop() == false )
            return false;

        m_bStopThread = false;

        if( m_thTask == nullptr )
            m_thTask = new std::thread( fnc, args ... );

        return true;
    }

    void Stop();
    bool IsStop();
    bool IsStopped();
    bool IsSleep();

    void sleepThead();
    void wakeupThead();

private:
    std::mutex                  m_mtxTh;
    std::condition_variable     m_cv;
    std::thread*                m_thTask;
    bool                        m_bWakeup;
    bool                        m_bSleepThead;
    bool                        m_bStopThread;
};

#endif /* CWORKER_H_ */
