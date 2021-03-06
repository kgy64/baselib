/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Thread handler classes
 * Author:      György Kövesdi (kgy@etiner.hu)
 * Modified by: KGy Removed debug code, because debug logger includes it
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Threads.h"

#include <Threads/Condition.h>
#include <Threads/Mutex.h>
#include <Debug/Debug.h>

#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h> // for setpriority()
#include <sys/syscall.h>  // For SYS_xxx definitions

SYS_DEFINE_MODULE(DM_THREAD);

using PTHREAD::Thread;

/// Helper struct to start threads
struct ThreadInfo
{
    inline ThreadInfo(ThreadPtr & thread):
        thread(thread)
    {
    }

    inline void Wait(void)
    {
        startCondition.Wait(startMutex);
    }

    inline void Signal(void)
    {
        Threads::Lock _l(startMutex);
        startCondition.Signal();
    }

    ThreadPtr & thread;

    Threads::Mutex startMutex;

    Threads::Condition startCondition;

}; // struct ThreadInfo

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *     class Thread:                                                                     *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Thread::Thread(const char * name):
    myThread(0),
    myThreadName(name),
    toBeFinished(true),
    isFinished(true)
{
 SYS_DEBUG_MEMBER(DM_THREAD);

 SYS_DEBUG(DL_INFO1, "Thread '" << getThreadName() << "' has been created");
}

Thread::~Thread(void)
{
 SYS_DEBUG_MEMBER(DM_THREAD);
}

/// Stop the thread
/*! This function initiates the exit procedure.<br>
 *  \param  is_join     If it is set to 'false', then this function will not wait for the thread to exit.<br>
 *                      Its default value is 'true'.
 *  \note   This is a collaborative exit, the thread must poll the function \ref Threads::Finished() and/or
 *          the function \ref Threads::KillSignal() must be used.<br>
 *          It is recommended to call it only from the parent thread, because it returns immediately if called
 *          from more threads.
 */
void Thread::Kill(bool is_join)
{
 SYS_DEBUG_MEMBER(DM_THREAD);

 if (myThread) {
    if (!toBeFinished) {
        toBeFinished = true;           // Flag it to be stopped
        KillSignal();                  // Send the signal to the thread
    }
    if (is_join) {
        Join();
    }
 }
}

/// Wait for the thread to exit
void Thread::Join(void)
{
 SYS_DEBUG_MEMBER(DM_THREAD);

 // If it is called from the thread itself, then calling join() is not necessary at all.
 if (pthread_equal(myThread, pthread_self())) {
    SYS_DEBUG(DL_INFO2, "Self thread is not joined");
    return;
 }

 SYS_DEBUG(DL_INFO2, "Joining thread...");
 pthread_join(myThread, NULL);  // Wait until exited
 SYS_DEBUG(DL_INFO2, "Joined, OK.");
}

/// (re)start the thread (public version, static)
/*! \note   Smart pointer <b>must</b> be created to prevent race between threads. It can be deleted when this
 *          function has returned, or can be stored for later inspection. */
void Thread::StartInternal(ThreadPtr & thread, size_t stack)
{
 SYS_DEBUG_STATIC(DM_THREAD);

 ThreadInfo info(thread);               // For inter-thread communication

 Threads::Lock _l(info.startMutex);

 // If it has already been started, do nothing:
 if (!thread->IsFinished()) {
    return;
 }

 thread->StartInternal(info, stack);    // Start the thread
}

/// Start the thread (internal version)
void Thread::StartInternal(ThreadInfo & info, size_t stack)
{
 SYS_DEBUG_MEMBER(DM_THREAD);

 // It is in running state:
 toBeFinished = false;
 isFinished = false;

 // Set the attributes
 myAttr.SetStackSize(stack);
 myAttr.SetJoinable(true);

 SYS_DEBUG(DL_INFO1, "Creating Thread '" << getThreadName() << "' ...");
 // Create the thread:
 ASSERT_THREAD(pthread_create(&myThread, myAttr.get(), &Thread::_main, &info)==0, "pthread_create() failed");

 // Wait until the thread copies its smart pointer
 info.Wait();

 SYS_DEBUG(DL_INFO1, "Thread '" << getThreadName() << "' started");
}

/// The physical start of the thread
/*! This is just a helper function, calling the real main() function.
 */
void * Thread::_main(void * p_info)
{
 SYS_DEBUG_STATIC(DM_THREAD);

 ThreadPtr self;

 {
    ThreadInfo & info = *reinterpret_cast<ThreadInfo *>(p_info);
    // Keep itself alive while main() is running:
    self = info.thread;
    // Also store the self pointer:
    self->mySelf = self;
    // Smart pointers have been updated, caller may continue:
    info.Signal();
    // Note: the content of 'info' is not available from this point.
 }

 int status = 0;

 self->before_main();   // Can change the thread name if necessary - for the log to be consistent

 // Set the thread name:
 if (!self->setThreadName()) {
    DEBUG_OUT("Could not set name of thread " << self->getThreadName());
 }

 SYS_DEBUG(DL_INFO1, "Thread '" << self->getThreadName() << "' started");

 try {
    status = self->main();
    self->exited(status);
    self->toBeFinished = true;

 } catch (std::exception & ex) {
    DEBUG_OUT("Thread Execution Error in " << self->getThreadName() << "/main(): " << ex.what());
    try {
        self->error(&ex);
        self->exited(-1);
    } catch (...) {
        DEBUG_OUT("Thread '" << self->getThreadName() << "': exception in exception handler!");
    }

 } catch (...) {
    DEBUG_OUT("Thread Execution Error in " << self->getThreadName() << "/main() (unknown exception)");
    try {
        self->error(nullptr);
        self->exited(-1);
    } catch (...) {
        DEBUG_OUT("Thread '" << self->getThreadName() << "': exception in exception handler!");
    }
 }

 SYS_DEBUG(DL_INFO1, "Thread '" << self->getThreadName() << "' has exited normally, status=" << status);

 self->isFinished = true;

 return (void*)0;
}

int Thread::GetPriority(void) const
{
 return getpriority(PRIO_PROCESS, 0);
}

bool Thread::SetPriority(int prio)
{
 SYS_DEBUG_MEMBER(DM_THREAD);
 SYS_DEBUG(DL_INFO1, "Priotiry is set to " << prio);

 return setpriority(PRIO_PROCESS, 0, prio) == 0;
}

pid_t Threads::getTid(void)
{
#ifdef SYS_gettid
 // It is suitable for my Linux distributions
 return syscall(SYS_gettid);
#else
 // It is suitable for Android
 return gettid();
#endif
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
