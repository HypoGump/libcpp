#ifndef LABIP_MUTEX_H_
#define LABIP_MUTEX_H_

#include <pthread.h>
#include <assert.h>

#include "Thread.h"

namespace labip
{

class Mutex
{
public:
    Mutex() : holder_(0) {
        pthread_mutex_init(&mutex_, NULL);
    }
    ~Mutex() {
        assert( holder_ == 0 );
        pthread_mutex_destroy(&mutex_);
    }
    
    
    void lock() {
        pthread_mutex_lock(&mutex_);
        holder_ = CurrentThread::tid();
    }
    void unlock() {
        holder_ = 0;
        pthread_mutex_unlock(&mutex_);
    }
    
    /* check func */
    bool isLockedByThisThread() const {
        return holder_ == CurrentThread::tid();
    }
    void assertLocked() {
        assert(isLockedByThisThread());
    }
    
    pthread_mutex_t* getPthreadMutex() {
        return &mutex_;
    }

private:
    pthread_mutex_t mutex_;
    pid_t holder_;
};


class MutexLockGuard
{
public:
    explicit MutexLockGuard(Mutex& mutex) : mutex_(mutex) {
        mutex_.lock();
    }
    
    ~MutexLockGuard() {
        mutex_.unlock();
    }

private:
    Mutex& mutex_;
};

}

#endif