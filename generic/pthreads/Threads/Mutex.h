/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Mutex classes for my thread control
 * Author:      György Kövesdi (kgy@etiner.hu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_THREADS_MUTEX_H_INCLUDED__
#define __SRC_THREADS_MUTEX_H_INCLUDED__

#include <Threads/Error.h>

#include <pthread.h>
#include <Memory/Memory.h>

#include <iostream>

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

        inline ~Mutex() throw()
        {
            int errorcode = pthread_mutex_destroy(&myMutex);
            switch (errorcode) {
                case 0:
                    // Success
                break;

                case EBUSY:
                    std::cerr << "ERROR: Could not destroy locked mutex" << std::endl;
                break;

                case EINVAL:
                    std::cerr << "ERROR: Trying to destroy invalid mutex" << std::endl;
                break;

                default:
                    std::cerr << "ERROR: Could not destroy mutex: " << strerror(errorcode) << std::endl;
                break;
            }
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
            if (pthread_mutex_unlock(&myMutex.myMutex)) {
                // Cannot throw here. Also cannot emit any debug message due to
                // eldless loop possibility.
                std::cerr << "ERROR: could not unlock mutex" << std::endl;
            }
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
                if (pthread_mutex_unlock(&myMutex.myMutex)) {
                    // Cannot throw here. Also cannot emit any debug message due to
                    // eldless loop possibility.
                    std::cerr << "ERROR: could not unlock mutex" << std::endl;
                }
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

    typedef MEM::shared_ptr<Lock> LockPtr;

} // namespace Threads

#endif /* __SRC_THREADS_MUTEX_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
