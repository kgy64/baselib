/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Thread handler classes
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_THREADS_THREADS_H_INCLUDED__
#define __SRC_THREADS_THREADS_H_INCLUDED__

#include <pthread.h>
#include <sched.h>

#include <Threads/Error.h>
#include <Debug/Debug.h>

SYS_DECLARE_MODULE(DM_THREAD);

namespace Threads
{
    class Thread
    {
     public:
        Thread(void);
        virtual ~Thread();

        virtual void Start(size_t stack = 1024*1024);
        virtual void Kill(void);

        /*! This function can be called by the main function to perform a graceful
            exit if necessary. */
        bool Finished(void) const
        {
            return toBeFinished;
        }

        static inline void Yield(void)
        {
            ASSERT_STD(sched_yield()==0);
        }

        int GetPriority(void) const;

        bool SetPriority(int prio);

     protected:
        class Attribute
        {
         public:
            inline Attribute(void)
            {
                ASSERT_THREAD(pthread_attr_init(&myAttrib)==0, "pthread_attr_init() failed");
            }

            inline ~Attribute()
            {
                ASSERT_THREAD(pthread_attr_destroy(&myAttrib)==0, "pthread_attr_destroy() failed");
            }

            inline void SetJoinable(bool joinable)
            {
                ASSERT_THREAD(pthread_attr_setdetachstate(&myAttrib, joinable ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED)==0, "pthread_attr_setdetachstate() failed");
            }

            inline void SetStackSize(size_t stack)
            {
                ASSERT_THREAD(pthread_attr_setstacksize(&myAttrib, stack)==0, "pthread_attr_setstacksize() failed");
            }

            inline pthread_attr_t * get()
            {
                return &myAttrib;
            }

         private:
            pthread_attr_t myAttrib;

        }; // class Attribute

        pthread_t myThread;

     private:
        SYS_DEFINE_CLASS_NAME("Threads::Thread");

        /// This function is the startpoint of this thread
        virtual int main(void) =0;

        virtual void atExit(int p_exitCode);

        static void * _main(void * thread_pointer);

        bool toBeFinished;

        Attribute myAttr;
    };

} // namespace Threads

#endif // __SRC_THREADS_THREADS_H_INCLUDED__

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
