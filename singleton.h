#ifndef SINGLETON_H
#define SINGLETON_H

#include <QtGlobal>

template <class T>
class CSingleton
{
public:
    static T *instance()
    {
        return sInstance;
    }

protected:
    CSingleton()
    {
        //Q_ASSERT(!sInstance);
        sInstance = static_cast<T *>(this);
    }

    virtual ~CSingleton()
    {
        //Q_ASSERT(sInstance);
        sInstance = nullptr;
    }

private:
    static T *sInstance;
};

template <class T> T *CSingleton<T>::sInstance = nullptr;


#endif // SINGLETON_H
