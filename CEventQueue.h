/*
 * CEventLoop.h
 *
 *  Created on: 2022. 11. 29.
 *      Author: Yeonji Lee
 */

#ifndef CEVENTQUEUE_H_
#define CEVENTQUEUE_H_

#include <stdio.h>
#include <iostream>
#include <shared_mutex>
#include <cassert>
#include <limits>

typedef struct CEventItem{
    uint32_t event;     //< Event ID
    uint32_t id;        //< A unique number generated when registering in the event queue
    void* params;

    uint32_t senderId;  //< ID?

    CEventItem():event(0), id(0), params(nullptr), senderId(0){}
    CEventItem(uint32_t _event, uint32_t _id, void* _params = nullptr, uint32_t _senderId = 0)
        : event(_event), id(_id), params(_params), senderId(_senderId){}
}CEventItem;

typedef struct CEventQueueItem{
    CEventItem evtItem;

    CEventQueueItem* pPre;
    CEventQueueItem* pNext;

    CEventQueueItem():pPre(nullptr), pNext(nullptr){}
    CEventQueueItem(CEventItem _evtItem, CEventQueueItem* _pPre = nullptr, CEventQueueItem* _pNext = nullptr)
        : evtItem(_evtItem), pPre(_pPre), pNext(_pNext){}
}CEventQueueItem;

// Using a mutex will remove atomic
class CEventQueue {

public:
    explicit CEventQueue();
    ~CEventQueue();

    uint32_t push(uint32_t event, void* params = nullptr, uint32_t senderId = 0);
    bool front(CEventItem& out_item) const;
    bool back(CEventItem& out_item) const;
    bool at(size_t pos, CEventItem& out_item) const;
    void removeAt(size_t pos);
    void pop();
    void popAll();

    size_t size() const {return m_size;}
    bool empty() const {return (m_size==0);}

private:
    /* writer() can return false, which indicates that the caller
       of push() changed its mind while writing (e.g. ran out of bytes)*/
    template <typename F>
    uint32_t push(const F& writer){
        CEventQueueItem* newItem = nullptr;

        if( writer(newItem) ){
            std::lock_guard<std::shared_mutex> lck(m_mtx);

            if( m_pRear != nullptr ) // avoid in case of empty queue
                m_pRear->pNext = newItem;
            if( m_pFront == nullptr ) // empty queue
                m_pFront = newItem;

            m_pRear = newItem;
            ++m_size;
            assert(m_size < std::numeric_limits<size_t>::max());
        }

        if( newItem == nullptr )
            return 0;

        return newItem->evtItem.id;
    }

private:
    CEventQueueItem*                m_pFront;
    CEventQueueItem*                m_pRear;
    size_t                          m_size;

    mutable std::shared_mutex       m_mtx;
};

#endif /* CEVENTQUEUE_H_ */
