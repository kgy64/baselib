/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Condition handler classes for my thread control
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_THREADS_CONDITION_H_INCLUDED__
#define __SRC_THREADS_CONDITION_H_INCLUDED__

#include <Threads/Mutex.h>

namespace Threads
{
    class Condition
    {
     public:
        inline Condition(void)
        {
            ASSERT_THREAD_STD(pthread_cond_init(&myCond, NULL));
        }

        inline ~Condition(void)
        {
            ASSERT_THREAD_STD(pthread_cond_destroy(&myCond));
        }

        inline void Signal(void)
        {
            ASSERT_THREAD_STD(pthread_cond_signal(&myCond));
        }

        inline void Wait(Threads::Mutex & mutex)
        {
            ASSERT_THREAD_STD(pthread_cond_wait(&myCond, &mutex.myMutex));
        }

     private:
        pthread_cond_t myCond;

    }; // class Condition

} // namespace Threads

#endif /* __SRC_THREADS_CONDITION_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
