/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_THREADS_THREADARRAY_H_INCLUDED__
#define __SRC_THREADS_THREADARRAY_H_INCLUDED__

#include <boost/shared_ptr.hpp>

#include <Threads/Threads.h>
#include <Threads/Mutex.h>

namespace Threads
{
    /// Runs a job on many threads
    /*! Doc: TODO
     *  \param      T       This is the identifier of the threads. Can be e.g. int or string.
     *  \param      U       The parameter passed to each thread. */
    template <typename T, class U>
    class ThreadArray
    {
     public:
        class Job: public Threads::Thread
        {
         public:
            /// The main function of the job
            virtual void Work(U & task) =0;

            /// This virtual function is called before the thread is accessed by \ref Threads::ThreadArray
            virtual void BeforeUse(void)
            {
            }

         protected:
            Job(ThreadArray & parent):
                myParent(parent)
            {
            }

            ThreadArray & myParent;

         private:
            SYS_DEFINE_CLASS_NAME("Threads::ThreadArray::Job");

        }; // class ThreadArray::Job

        typedef boost::shared_ptr<Job> JobPtr;

     protected:
        ThreadArray(size_t stack = 1024*1024):
            myStack(stack)
        {
        }

        inline size_t Size(void) const
        {
            Threads::Lock _l(myThreadMutex);
            return myThreads.size();
        }

        JobPtr operator[](const T & index)
        {
            Threads::Lock _l(myThreadMutex);
            JobPtr & th(myThreads[index]);
            if (!th.get()) {
                th = CreateJob(index);
                th->Start(myStack);
            }
            th->BeforeUse();
            return th;
        }

        typedef std::map<T, JobPtr> ThreadsType;

        /// Removes and returns the requested entry
        JobPtr Remove(const T & index)
        {
            typename ThreadsType::iterator i = myThreads.find(index);
            if (i == myThreads.end()) {
                return JobPtr();
            }
            JobPtr retval = i->second;
            myThreads.erase(i);
            return retval;
        }

        ThreadsType myThreads;

     private:
        SYS_DEFINE_CLASS_NAME("Threads/ThreadArray");

        /// Locks the access to \ref ThreadArray::myThreads
        mutable Threads::Mutex myThreadMutex;

        size_t myStack;

        /// This virtual function creates a new thread
        virtual JobPtr CreateJob(const T & index) =0;

    }; // class ThreadArray

} // namespace Threads

#endif /* __SRC_THREADS_THREADARRAY_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
