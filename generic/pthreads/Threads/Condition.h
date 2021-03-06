/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Condition handler classes for my thread control
 * Author:      György Kövesdi (kgy@etiner.hu)
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
            if (pthread_cond_destroy(&myCond)) {
                // Cannot throw here
                std::cerr << "ERROR: could not destroy condition" << std::endl;
            }
        }

        /// Signals the Condition
        /*! This function can be called if the corresponding mutex has already been locked. */
        inline void Signal(void)
        {
            ASSERT_THREAD_STD(pthread_cond_signal(&myCond));
        }

        /// Signals the Condition
        /*! This function can be called at any time: it locks the corresponding mutex. */
        inline void Signal(Threads::Mutex & mutex)
        {
            Threads::Lock _l(mutex);
            Signal();
        }

        /// Waits for the Signal
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
