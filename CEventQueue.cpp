/*
 * CEventQueue.cpp
 *
 *  Created on: 2022. 11. 29.
 *      Author: Yeonji Lee
 */

#include "CEventQueue.h"

#include <time.h>

CEventQueue::CEventQueue()
    : m_pFront(nullptr), m_pRear(nullptr), m_size(0){
}

CEventQueue::~CEventQueue(){

}

uint32_t CEventQueue::push(uint32_t event, void* params, uint32_t senderId){
    return push([&](CEventQueueItem*& newItem)->bool{
                    srand(time(0));
                    newItem = new CEventQueueItem(CEventItem(event
                                                , (uint32_t)rand()+1/*todo: verification*/
                                                , params, senderId));
                    return true;
                });
}

bool CEventQueue::front(CEventItem& out_item) const {
    {
        std::shared_lock<std::shared_mutex> lck(m_mtx);
        if(m_pFront == nullptr)
            return false;

        out_item = m_pFront->evtItem;
    }

    return true;
}

bool CEventQueue::back(CEventItem& out_item) const {
    {
        std::shared_lock<std::shared_mutex> lck(m_mtx);

        if(m_pRear == nullptr)
            return false;

        out_item = m_pRear->evtItem;
    }

    return true;
}

bool CEventQueue::at(size_t pos, CEventItem& out_item) const {
    CEventQueueItem* pCur = nullptr;

    {
        std::shared_lock<std::shared_mutex> lck(m_mtx);
        if( m_size == 0 || pos >= m_size )
            return false;
        size_t count = 0;
        if( pos <= (m_size/2) ){
            pCur = m_pFront;
            while( count < pos && pCur != nullptr ){
                pCur = pCur->pNext;
                ++count;
            }
        }
        else{
            pCur = m_pRear;
            count = m_size-1;
            while( count > pos && pCur != nullptr ){
                pCur = pCur->pPre;
                --count;
            }
        }

        if(pos != count || pCur == nullptr) //something wrong
            return false;
    }

    return true;
}

void CEventQueue::removeAt(size_t pos) {
    CEventQueueItem* pCur = nullptr;

    {
        std::lock_guard<std::shared_mutex> lck(m_mtx);

        if( m_size == 0 || pos >= m_size )
            return;
        size_t count = 0;
        if( pos <= (m_size/2) ){
            pCur = m_pFront;
            while( count < pos && pCur != nullptr ){
                pCur = pCur->pNext;
                ++count;
            }
        }
        else{
            pCur = m_pRear;
            count = m_size-1;
            while( count > pos && pCur != nullptr ){
                pCur = pCur->pPre;
                --count;
            }
        }

        if(pos != count || pCur == nullptr) //something wrong
            return;

        if(pCur->pPre != nullptr)
            pCur->pPre->pNext = pCur->pNext;

        if(pCur->pNext != nullptr)
            pCur->pNext->pPre = pCur->pPre;

        --m_size;
    }

    delete pCur;
    pCur = nullptr;
}

void CEventQueue::pop(){
    CEventQueueItem* pDelete = nullptr;

    {
        std::lock_guard<std::shared_mutex> lck(m_mtx);

        if(m_pFront == nullptr) // empty
            return;

        pDelete = m_pFront;
        if( m_pFront == m_pRear ){
            m_pFront = nullptr;
            m_pRear = nullptr;
            m_size = 0;
        }
        else{
            m_pFront = m_pFront->pNext;
            --m_size;
        }
    }

    if( pDelete != nullptr ){
        delete pDelete;
        pDelete = nullptr;
    }
}

void CEventQueue::popAll()
{
    CEventQueueItem* pCur = nullptr;

    {
        std::lock_guard<std::shared_mutex> lck(m_mtx);

        pCur = m_pFront;
        m_pFront = nullptr;
        m_pRear = nullptr;
        m_size = 0;
    }

    if(pCur == nullptr) // empty
        return;

    CEventQueueItem* pDelete = nullptr;

    while(pCur != nullptr){
        pDelete = pCur;
        pCur = pCur->pNext;

        if( pDelete != nullptr ){
            delete pDelete;
            pDelete = nullptr;
        }
    }
}
