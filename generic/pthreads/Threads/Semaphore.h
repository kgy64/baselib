/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * License:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __OPSYS_UNIX_THREADS_SEMAPHORE_H_INCLUDED__
#define __OPSYS_UNIX_THREADS_SEMAPHORE_H_INCLUDED__

#include <Threads/Error.h>

#include <stdint.h>
#include <semaphore.h>

namespace Threads
{
    class Semaphore
    {
     public:
        inline Semaphore(int value = 0)
        {
            sem_init(&mySemaphore, 0, value);
        }

        inline ~Semaphore()
        {
            sem_destroy(&mySemaphore);
        }

        inline void Wait(void)
        {
            sem_wait(&mySemaphore);
        }

        inline void Wait(uint32_t milliseconds)
        {
            struct timespec ts;
            ts.tv_sec = milliseconds / 1000U;
            ts.tv_nsec = (milliseconds % 1000U) * 1000000U;
            sem_timedwait(&mySemaphore, &ts);
        }

        inline bool TryWait(void)
        {
            return sem_trywait(&mySemaphore) == 0;
        }

        inline void Post(void)
        {
            sem_post(&mySemaphore);
        }

        inline int GetValue(void)
        {
            int result;
            sem_getvalue(&mySemaphore, &result);
            return result;
        }

     protected:
        sem_t mySemaphore;

    }; // class Threads::Semaphore

} // namespace Threads

#endif /* __OPSYS_UNIX_THREADS_SEMAPHORE_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
