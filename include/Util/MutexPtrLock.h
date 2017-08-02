// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef UTIL_MUTEX_PTR_LOCK_H
#define UTIL_MUTEX_PTR_LOCK_H

#include <ConfigUtil.h>
#include <ThreadException.h>

namespace UtilInternal
{

template<class T>
class MutexPtrLock
{
public:
    
    MutexPtrLock<T>(const T* mutex) :
        _mutex(mutex),
        _acquired(false)
    {
        if(_mutex)
        {
            _mutex->lock();
            _acquired = true;
        }
    }

    ~MutexPtrLock<T>()
    {
        if(_mutex && _acquired)
        {
            _mutex->unlock();
        }
    }

    void acquire() const
    {
        if(_mutex)
        {
            _mutex->lock();
            _acquired = true;
        }
    }

    void release() const
    {
        if(_mutex)
        {
            if(!_acquired)
            {
                throw Util::ThreadLockedException(__FILE__, __LINE__);
            }
            _mutex->unlock();
            _acquired = false;
        }
    }

    bool acquired() const
    {
        return _acquired;
    }
   
private:
    
    // Not implemented; prevents accidental use.
    //
    MutexPtrLock<T>(const MutexPtrLock<T>&);
    MutexPtrLock<T>& operator=(const MutexPtrLock<T>&);

    const T* _mutex;
    mutable bool _acquired;
};

} // End namespace UtilInternal

#endif
