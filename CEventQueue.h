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

#ifndef CACHE_ALIGN
// Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define CACHE_ALIGN 64
#else
#define CACHE_ALIGN 32
#endif //end _WIN64
#endif //end _WIN32 || _WIN64

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#define CACHE_ALIGN 64
#else
#define CACHE_ALIGN 32
#endif //end __GNUC__
#endif //end __x86_64__ || __ppc64__
#endif //end ifndef CACHE_ALIGN

typedef struct CEventItem{
    unsigned int event; //< Event ID
    unsigned int id;    //< A unique number generated when registering in the event queue
    void* params;

    int senderId;       //< ID?

    CEventItem():event(0), id(0), params(nullptr), senderId(0){}
    CEventItem(unsigned int _event, unsigned int _id, void* _params = nullptr, int _senderId = 0)
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

    unsigned int push(unsigned int event, void* params = nullptr, unsigned int senderId = 0);
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
    unsigned int push(const F& writer){
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
