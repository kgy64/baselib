/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Mutex classes for my thread control
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_THREADS_MUTEX_H_INCLUDED__
#define __SRC_THREADS_MUTEX_H_INCLUDED__

#include <Threads/Error.h>

#include <pthread.h>

namespace Threads
{
    class Lock;
    class TryLock;
    class Condition;

    class Mutex
    {
        friend class Lock;
        friend class TryLock;
        friend class Condition;

     protected:
        inline Mutex(bool recursive)
        {
            pthread_mutexattr_t attr;
            ASSERT_THREAD_STD(pthread_mutexattr_init(&attr));
            ASSERT_THREAD_STD(pthread_mutexattr_settype(&attr, recursive ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_NORMAL));
            ASSERT_THREAD_STD(pthread_mutex_init(&myMutex, &attr));
        }

     public:
        inline Mutex(void):
            Mutex(false)
        {
        }

        inline ~Mutex()
        {
            ASSERT_THREAD_STD(pthread_mutex_destroy(&myMutex));
        }

     private:
        pthread_mutex_t myMutex;

    }; // class Mutex

    class MutexRecursive: public Mutex
    {
     public:
        MutexRecursive(void):
            Mutex(true)
        {
        }

    }; // MutexRecursive

    class Lock
    {
     public:
        inline Lock(Mutex & mutex):
            myMutex(mutex)
        {
            ASSERT_THREAD_STD(pthread_mutex_lock(&myMutex.myMutex));
        }

        inline ~Lock()
        {
            ASSERT_THREAD_STD(pthread_mutex_unlock(&myMutex.myMutex));
        }

     private:
        Mutex & myMutex;

    }; // class Lock

    class TryLock
    {
     public:
        inline TryLock(Mutex & mutex):
            myMutex(mutex)
        {
            myStatus = pthread_mutex_trylock(&myMutex.myMutex);
        }

        inline ~TryLock()
        {
            if (IsLocked()) {
                ASSERT_THREAD_STD(pthread_mutex_unlock(&myMutex.myMutex));
            }
        }

        inline bool IsLocked(void) const
        {
            return myStatus == 0;
        }

        inline int GetStatus(void) const
        {
            return myStatus;
        }

     private:
        Mutex & myMutex;

        int myStatus;

    }; // class TryLock

} // namespace Threads

#endif /* __SRC_THREADS_MUTEX_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
