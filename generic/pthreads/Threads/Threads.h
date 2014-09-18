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

#include <Threads/Error.h>
#include <Debug/Debug.h>

#include <pthread.h>
#include <sched.h>
#include <errno.h>

SYS_DECLARE_MODULE(DM_THREAD);

namespace Threads
{
    pid_t getTid(void);

} // namespace Threads

namespace PTHREAD
{
    class Thread
    {
     public:
        Thread(const char * name);
        virtual ~Thread();

        void Start(size_t stack = 1024*1024);
        void Kill(void);
        bool SetPriority(int prio);
        int GetPriority(void) const;

        /*! This function can be called by the main function to get the exit status. The thread
         *  must exit if it returns true.
         *  \note   The virtual function \ref Thread::KillSignal() is also called when this
         *          flag is just set to signal the thread if necessary. */
        inline bool ToBeFinished(void) const
        {
            SYS_DEBUG_MEMBER(DM_THREAD);
            return toBeFinished;
        }

        /*! This function returns true if the thread has already exited (the main has returned), or
         *  not started yet. */
        inline bool IsFinished(void) const
        {
            return !myThread;
        }

        /// Allows other processes to run
        static inline void Yield(void)
        {
            SYS_DEBUG_STATIC(DM_THREAD);
            ASSERT_STD(sched_yield()==0);
        }

        inline const std::string & getThreadName(void) const
        {
            return myThreadName;
        }

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
                ASSERT_THREAD(pthread_attr_setstacksize(&myAttrib, stack)==0, "pthread_attr_setstacksize(" << stack << ") failed");
            }

            inline pthread_attr_t * get()
            {
                return &myAttrib;
            }

         private:
            pthread_attr_t myAttrib;

        }; // class PTHREAD::Thread::Attribute

        pthread_t myThread;

        /// The thread name
        /*! \note   This is mainly for debug purposes. */
        std::string myThreadName;

     private:
        SYS_DEFINE_CLASS_NAME("PTHREAD::Thread");

        /// A function called before \ref Thread::main()
        virtual void before_main(void)
        {
        }

        /// A function called after \ref Thread::main()
        virtual void after_main(void)
        {
        }

        /// This function is the startpoint of this thread
        virtual int main(void) =0;

        /// Called after main()
        virtual void atExit(int p_exitCode)
        {
        }

        /// Called to signal the thread to exit
        /*! This default implementation does nothing, see the reimplementations for details. */
        virtual void KillSignal(void)
        {
        }

        static void * _main(void * thread_pointer);

        bool toBeFinished;

        Attribute myAttr;

    }; // class PTHREAD::Thread

} // namespace PTHREAD

#endif // __SRC_THREADS_THREADS_H_INCLUDED__

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
