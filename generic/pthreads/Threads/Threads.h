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
#include <Memory/Memory.h>
#include <Debug/Debug.h>

#include <exception>
#include <pthread.h>
#include <sched.h>
#include <errno.h>

SYS_DECLARE_MODULE(DM_THREAD);

struct ThreadInfo;

namespace Threads
{
    pid_t getTid(void);

} // namespace Threads

namespace PTHREAD
{
    class Thread;
}

using ThreadPtr = MEM::shared_ptr<PTHREAD::Thread>;
using ThreadWeak = MEM::weak_ptr<PTHREAD::Thread>;

namespace PTHREAD
{
    class Thread
    {
     public:
        Thread(const char * name = "unknown");
        virtual ~Thread();

        template <typename T>
        inline static void Start(MEM::shared_ptr<T> & thread, size_t stack = 1024*1024)
        {
            SYS_DEBUG_STATIC(DM_THREAD);
            ThreadPtr p = MEM::static_pointer_cast<Thread>(thread);
            StartInternal(p, stack);
        }

        void Kill(void);
        bool SetPriority(int prio);
        int GetPriority(void) const;

        /*! This function can be called by the main function to get the exit request.
         *  \note   The virtual function \ref Thread::KillSignal() is also called when this flag
         *          is set to signal the thread if necessary. That may be a better solution. */
        inline bool ToBeFinished(void) const
        {
            SYS_DEBUG_MEMBER(DM_THREAD);
            return toBeFinished;
        }

        /*! This function returns true if the thread has already exited (the main has returned), or
         *  not started yet. */
        inline bool IsFinished(void) const
        {
            return isFinished;
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

        inline bool setThreadName(const char * name)
        {
            if (pthread_setname_np(myThread, name) != 0) {
                return false;
            }
            myThreadName = name;
            return true;
        }

        inline bool setThreadName(const std::string & name)
        {
            if (pthread_setname_np(myThread, name.c_str()) != 0) {
                return false;
            }
            myThreadName = name;
            return true;
        }

        /// Returns the smart pointer of itself
        template <class C = Thread>
        inline auto self(void) -> MEM::shared_ptr<C>
        {
            MEM::shared_ptr<C> me = MEM::static_pointer_cast<C>(mySelf.lock());
            ASSERT(me, "function Thread::self() is called before Thread::Start()");
            return me;
        }

     protected:
        inline bool setThreadName(void)
        {
            return pthread_setname_np(myThread, myThreadName.c_str()) == 0;
        }

        class Attribute
        {
         public:
            inline Attribute(void)
            {
                ASSERT_THREAD(pthread_attr_init(&myAttrib)==0, "pthread_attr_init() failed");
            }

            inline ~Attribute()
            {
                if (pthread_attr_destroy(&myAttrib)) {
                    std::cerr << "ERROR: pthread_attr_destroy() failed" << std::endl;
                }
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

     protected:
        /// A function called before \ref Thread::main()
        virtual void before_main(void)
        {
        }

        /// This function is the startpoint of this thread
        virtual int main(void) =0;

        /// Called when the thread has exited successfully
        /*! \param status   The return value of \ref Threads::main()
         *  \warning    This function is not called if the thread has exited with exception, see \ref Threads::error() */
        virtual void exited(int)
        {
        }

        /// Called when the thread has exited with exception
        /*! \param  ex  Pointer to the exception, or nullptr if it has an unknown type (other than std::exception). */
        virtual void error(std::exception *)
        {
        }

        /// Called to signal the thread to exit
        /*! This default implementation does nothing, see the reimplementations for details. */
        virtual void KillSignal(void)
        {
        }

        void StartInternal(ThreadInfo & info, size_t stack);
        static void StartInternal(ThreadPtr & thread, size_t stack = 1024*1024);

        static void * _main(void * info);

     private:
        bool toBeFinished;

        bool isFinished;

        Attribute myAttr;

        ThreadWeak mySelf;

    }; // class PTHREAD::Thread

} // namespace PTHREAD

#endif // __SRC_THREADS_THREADS_H_INCLUDED__

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
