#ifndef LABIP_CONDITION_H_
#define LABIP_CONDITION_H_

#include <pthread.h>
#include "Mutex.h"

namespace labip
{

class Condition
{
public:
    explicit Condition(Mutex& mutex) : mutex_(mutex) {
        pthread_cond_init(&pcond_, NULL);
    }
    ~Condition() {
        pthread_cond_destroy(&pcond_);
    }
    
    void wait() {
        pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
    }
    bool waitUntilTimeout(time_t sec, long nsec = 0) {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += sec;
        abstime.tv_nsec += nsec;
        return ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.getPthreadMutex(), &abstime);
    }
    
    void notify() {
        pthread_cond_signal(&pcond_);
    }
    void notifyAll() {
        pthread_cond_broadcast(&pcond_);
    }
    
private:
    Mutex& mutex_;
    pthread_cond_t pcond_;
};

} // labip

#endif