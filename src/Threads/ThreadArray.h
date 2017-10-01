/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     
 * Author:      György Kövesdi (kgy@etiner.hu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_THREADS_THREADARRAY_H_INCLUDED__
#define __SRC_THREADS_THREADARRAY_H_INCLUDED__

#include <Threads/Threads.h>
#include <Threads/Mutex.h>
#include <Memory/Memory.h>

#include <map>
#include <boost/intrusive/list.hpp>

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

        typedef MEM::shared_ptr<Job> JobPtr;

        virtual ~ThreadArray() throw()
        {
            SYS_DEBUG_MEMBER(DM_THREAD_ARRAY);
            Finish();
            SYS_DEBUG(DL_INFO1, "Threads have been stopped.");
        }

        inline void Finish(void)
        {
            SYS_DEBUG_MEMBER(DM_THREAD_ARRAY);
            for (auto i = myThreads.begin(); i != myThreads.end(); ++i) {
                SYS_DEBUG(DL_INFO2, "Stopping thread " << i->first);
                i->second->Kill();
            }
        }

        class Job: public Threads::Thread, public auto_unlink_hook
        {
            friend class ThreadArray;

         public:
            virtual ~Job()
            {
                SYS_DEBUG_MEMBER(DM_THREAD_ARRAY);
                GetParent().Deleted(*this);
            }

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

         protected:
            inline Job(ThreadArray & parent, const char * thread_name):
                Threads::Thread(thread_name),
                myParent(parent)
            {
                SYS_DEBUG_MEMBER(DM_THREAD_ARRAY);
                GetParent().Created(*this);
            }

            /// Signals the Thread Server not to use this thread any more and stops the thread
            virtual void KillSignal(void) override
            {
                GetParent().Deleted(*this);
            }

         private:
            SYS_DEFINE_CLASS_NAME("Threads::ThreadArray::Job");

            ThreadArray & myParent;

            T myIndex;

            /// Sets new index
            inline void Reindex(const T & new_index)
            {
                SYS_DEBUG_MEMBER(DM_THREAD_ARRAY);
                myIndex = new_index;
            }

            /// This virtual function initializes a new or re-used thread
            /*! \param  index   This is the new index to be used. Note that it differs from the old one
                                (see \ref ThreadArray::Job::index) for sure when this function is called.
                \note           The function \ref ThreadArray::Job::Reindex() will be called after this function, so
                                the index has the old value (or uninitialized) during this function.
                \note           It is also called just after the constructor.
                \warning        We must also wait for finish here to prevent creating more threads for the same target. */
            virtual void Initialize(const T & index)
            {
            }

        }; // class ThreadArray::Job

        /// Gets the job at the given index
        JobPtr operator[](const T & index)
        {
            SYS_DEBUG_MEMBER(DM_THREAD_ARRAY);
            JobPtr jp;
            {
                Threads::Lock _l(myThreadMutex);
                ThreadPtr & th = myThreads[index];
                if (th) {
                    jp = th->self<Job>();
                    SYS_DEBUG(DL_INFO2, "Using existing " << index << " [" << no_of_threads << " threads]");
                    jp->unlink();
                } else {
                    if (no_of_threads < max_no_of_threads) {
                        // Create a new thread:
                        SYS_DEBUG(DL_INFO2, "Creating new thread for " << index << " [" << no_of_threads << " threads]");
                        jp = CreateJob();
                        Threads::Thread::Start(jp, myStack);
                    } else {
                        // Get the oldest index:
                        const T & idx = static_cast<Job &>(task_order.back()).GetIndex();
                        SYS_DEBUG(DL_INFO2, "Re-using '" << idx << "' for '" << index << "' [" << no_of_threads << "]");
                        // Remove and re-use the oldest existing thread:
                        jp = RemoveLocked(idx);
                    }
                    // Store it back:
                    th = jp;
                    jp->Initialize(index);
                    jp->Reindex(index);
                }
                task_order.push_front(*jp);
            }
            return jp;
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

        typedef std::map<T, ThreadPtr> ThreadsType;

        inline void Advance(Job & job)
        {
            Threads::Lock _l(myThreadMutex);
            job.unlink();
            task_order.push_front(job);
        }

        /// Locks the access to \ref ThreadArray::myThreads and \ref ThreadArray::task_order
        mutable Threads::Mutex myThreadMutex;

        ThreadsType myThreads;

        typedef boost::intrusive::list<Job, boost::intrusive::constant_time_size<false> > TaskList;

        /*! \warning    Its destructor uses \ref myThreadMutex, so the mutex must be defined earlier. */
        TaskList task_order;

     private:
        SYS_DEFINE_CLASS_NAME("Threads/ThreadArray");

        /// Size of the stack for threads
        size_t myStack;

        uint32_t no_of_threads;

        uint32_t max_no_of_threads;

        /// This virtual function creates a new thread
        virtual JobPtr CreateJob(void) =0;

        /// Removes and returns the requested entry
        /*! \warning    It must be called in locked state! (see the caller function(s) for details) */
        JobPtr RemoveLocked(const T & index)
        {
            SYS_DEBUG_MEMBER(DM_THREAD_ARRAY);
            auto i = myThreads.find(index);
            ASSERT (i != myThreads.end(), "cannot remove thread index '" << index << "' from thread list, it is missing");
            ThreadPtr & p = i->second;
            JobPtr retval = p->self<Job>();
            myThreads.erase(i); // Invalidates 'i'
            retval->unlink();
            return retval;      // The smart pointer holds the old thread yet
        }

        /*! \warning    The constructor Job::Job() is called in locked state (from function \ref ThreadArray::CreateJob()) */
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
