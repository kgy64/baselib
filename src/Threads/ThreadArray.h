/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_THREADS_THREADARRAY_H_INCLUDED__
#define __SRC_THREADS_THREADARRAY_H_INCLUDED__

#include <boost/shared_ptr.hpp>
#include <boost/intrusive/list.hpp>

#include <Threads/Threads.h>
#include <Threads/Mutex.h>

SYS_DECLARE_MODULE(DM_THREAD_ARRAY);

namespace Threads
{
    /// Runs a job on many threads
    /*! Doc: TODO
     *  \param      T       This is the identifier of the threads. Can be e.g. int or string.
     *  \param      U       The parameter passed to each thread. */
    template <typename T, class U>
    class ThreadArray
    {
        typedef boost::intrusive::list_base_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink> > auto_unlink_hook;

     public:
        class Job;
        friend class Job;

        typedef boost::shared_ptr<Job> JobPtr;

        typedef boost::intrusive::list<Job, boost::intrusive::constant_time_size<false> > TaskList;

        class Job: protected Threads::Thread, public auto_unlink_hook
        {
            friend class ThreadArray;

         public:
            /// The main function of the job
            virtual void Work(U task) =0;

            inline const T & GetIndex(void) const
            {
                return myIndex;
            }

            inline ThreadArray & GetParent(void) const
            {
                return myParent;
            }

            /// Move this job to the top
            void Advance(void) const
            {
                GetParent().Advance(*this);
            }

            /// Signals the Thread Server not to use this thread any more and stops the thread
            /*! \warning    This function must be called instead of \ref Threads::Thread::Kill() to force this
                            thread to exit. Need not be used anyway. */
            inline void Kill(void)
            {
                GetParent().Deleted(*this);
                Threads::Thread::Kill();
            }

         protected:
            inline Job(ThreadArray & parent):
                myParent(parent)
            {
                SYS_DEBUG_MEMBER(DM_THREAD_ARRAY);
                GetParent().Created(*this);
            }

            virtual ~Job()
            {
                SYS_DEBUG_MEMBER(DM_THREAD_ARRAY);
                GetParent().Deleted(*this);
            }

            /// Sets new index
            inline void Reindex(const T & new_index)
            {
                SYS_DEBUG_MEMBER(DM_THREAD_ARRAY);
                myIndex = new_index;
            }

         private:
            SYS_DEFINE_CLASS_NAME("Threads::ThreadArray::Job");

            ThreadArray & myParent;

            T myIndex;

            /// This virtual function is called before the thread is accessed by \ref Threads::ThreadArray
            /*! Optional function, not necessary to be reimplemented.
                \note   If the thread is just created, or recycled for other target, then the function
                        \ref ThreadArray::Job::Initialize() is called before being used (instead of this function). */
            virtual void BeforeUse(void)
            {
            }

            /// This virtual function initializes a new or re-used thread
            /*! \param  index   This is the new index to be used. Note that it differs from the old one
                                (see \ref ThreadArray::Job::index) for sure when this function is called.
                \note           The function \ref ThreadArray::Job::Reindex() will be called after this function, so
                                the index has the old value (or uninitialized) during this function.
                \note           It is also called just after the constructor.
                \warning        We must also wait for finish here to prevent creating more threads for the same target. */
            virtual void Initialize(const T & index) =0;

        }; // class ThreadArray::Job

        /// Gets the job at the given index
        JobPtr operator[](const T & index)
        {
            SYS_DEBUG_MEMBER(DM_THREAD_ARRAY);
            JobPtr * th; // Just a reference, but must be initialized later
            bool must_be_inited = false;
            {
                Threads::Lock _l(myThreadMutex);
                th = &myThreads[index];
                if (th->get()) {
                    SYS_DEBUG(DL_INFO2, "Using existing '" << task_order.back().GetIndex() << "' for '" << index << "' [" << no_of_threads << "]");
                } else {
                    if (no_of_threads < max_no_of_threads) {
                        // Create a new thread:
                        SYS_DEBUG(DL_INFO2, "Creating '" << index << "' [" << no_of_threads << "]");
                        *th = CreateJob();
                        (*th)->Start(myStack);
                    } else {
                        SYS_DEBUG(DL_INFO2, "Re-using '" << task_order.back().GetIndex() << "' for '" << index << "' [" << no_of_threads << "]");
                        // Remove and re-use the oldest existing thread:
                        *th = Remove(task_order.back().GetIndex());
                    }
                    must_be_inited = true; // Must be initialized after unlock
                }
                AdvanceLocked(**th);
            }
            if (must_be_inited) {
                (*th)->Initialize(index);
                (*th)->Reindex(index);
            } else {
                (*th)->BeforeUse();
            }
            return *th;
        }

     protected:
        ThreadArray(uint32_t max_threads, size_t stack = 1024*1024):
            myStack(stack),
            no_of_threads(0),
            max_no_of_threads(max_threads)
        {
            SYS_DEBUG_MEMBER(DM_THREAD_ARRAY);
        }

        inline size_t Size(void) const
        {
            Threads::Lock _l(myThreadMutex);
            return myThreads.size();
        }

        typedef std::map<T, JobPtr> ThreadsType;

        inline void AdvanceLocked(Job & job)
        {
            SYS_DEBUG_MEMBER(DM_THREAD_ARRAY);
            const_cast<Job&>(job).unlink();
            task_order.push_front(job);
        }

        inline void Advance(Job & job)
        {
            Threads::Lock _l(myThreadMutex);
            AdvanceLocked(job);
        }

        ThreadsType myThreads;

        TaskList task_order;

     private:
        SYS_DEFINE_CLASS_NAME("Threads/ThreadArray");

        /// Locks the access to \ref ThreadArray::myThreads
        mutable Threads::Mutex myThreadMutex;

        /// Size of the stack for threads
        size_t myStack;

        uint32_t no_of_threads;

        uint32_t max_no_of_threads;

        /// This virtual function creates a new thread
        virtual JobPtr CreateJob(void) =0;

        /// Removes and returns the requested entry
        /*! \warning    It must be called in locked state! (see the caller function(s) for details) */
        JobPtr Remove(const T & index)
        {
            SYS_DEBUG_MEMBER(DM_THREAD_ARRAY);
            typename ThreadsType::iterator i = myThreads.find(index);
            ASSERT (i != myThreads.end(), "Thread index '" << index << "' is missing from thread list");
            JobPtr retval = i->second;
            myThreads.erase(i); // Invalidates 'i'
            return retval;      // The smart pointer holds the old thread yet
        }

        /*! \warning    The constructor is called in locked state (from function \ref ThreadArray::CreateJob()) */
        void Created(Job & job)
        {
            SYS_DEBUG_MEMBER(DM_THREAD_ARRAY);
            ++no_of_threads;
        }

        /*! \warning    It is called from the destructor, in non-locked state. */
        void Deleted(const Job & job)
        {
            SYS_DEBUG_MEMBER(DM_THREAD_ARRAY);
            Threads::Lock _l(myThreadMutex);
            if (job.is_linked()) {
                --no_of_threads;
            }
            // However, the container's destructor calls unlink(), it must be
            // called here to prevent unwanted re-use of the deleted class and
            // other possible race conditions
            const_cast<Job&>(job).unlink();
        }

    }; // class ThreadArray

} // namespace Threads

#endif /* __SRC_THREADS_THREADARRAY_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
