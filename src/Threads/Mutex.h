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

#include <pthread.h>
#include <Threads/Error.h>

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

     public:
        inline Mutex(void)
        {
            ASSERT_THREAD_STD(pthread_mutex_init(&myMutex, NULL));
        }

        inline ~Mutex()
        {
            ASSERT_THREAD_STD(pthread_mutex_destroy(&myMutex));
        }

     private:
        pthread_mutex_t myMutex;

    }; // class Mutex

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
