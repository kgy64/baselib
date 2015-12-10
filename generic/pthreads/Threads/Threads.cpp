/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Thread handler classes
 * Author:      György Kövesdi (kgy@teledigit.eu)
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

    inline ThreadPtr getThread(void)
    {
        ThreadPtr p = thread;       // Must be copied before Signal()
        startCondition.Signal();    // Smart pointer has been instantiated, caller may continue
        return p;
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
    toBeFinished(true)
{
 SYS_DEBUG_MEMBER(DM_THREAD);

 SYS_DEBUG(DL_INFO1, "Thread '" << getThreadName() << "' has been created");
}

Thread::~Thread(void)
{
 SYS_DEBUG_MEMBER(DM_THREAD);
 Kill();
}

/// Stop the thread
/*! This function initiates the exit procedure.<br>
 *  \note   This is a collaborative exit, the thread must poll the function \ref Threads::Finished() and/or
 *          the function \ref Threads::KillSignal() must be used.<br>
 *          If this function is called from another thread, then it does not return while the thread is running.
 *          It is recommended to call it only from the parent thread, because it returns immediately if called
 *          from more threads.
 */
void Thread::Kill(void)
{
 SYS_DEBUG_MEMBER(DM_THREAD);

 // If it has already been stopped, do nothing:
 if (toBeFinished || !myThread) {
    return;
 }

 toBeFinished = true;           // Flag it to be stopped

 KillSignal();                  // Send the signal to the thread

 // If it is called from the thread itself, then it is not necessary at all. Calling
 // the join() is a bad idea in this case.
 if (!pthread_equal(myThread, pthread_self())) {
    return;
 }

 pthread_join(myThread, NULL);  // Wait until exited
}

/// (re)start the thread (public version, static)
/*! \note   Smart pointer <b>must</b> be created to prevent race between threads. It can be deleted when this
 *          function has returned, or can be stored for later inspection. */
void Thread::Start(ThreadPtr & thread, size_t stack)
{
 SYS_DEBUG_STATIC(DM_THREAD);

 // If it has already been started, do nothing:
 if (!thread->IsFinished()) {
    return;
 }

 ThreadInfo info(thread);       // For inter-thread communication
 thread->Start(info, stack);   // Start the thread
}

/// Start the thread (internal version)
void Thread::Start(ThreadInfo & info, size_t stack)
{
 SYS_DEBUG_MEMBER(DM_THREAD);

 // It is in running state:
 toBeFinished = false;

 // Set the attributes
 myAttr.SetStackSize(stack);
 myAttr.SetJoinable(true);

 //
 Threads::Lock _l(info.startMutex);

 // Create the thread:
 ASSERT_THREAD(pthread_create(&myThread, myAttr.get(), &Thread::_main, &info)==0, "pthread_create() failed");

 // Wait until the thread copies its smart pointer
 info.Wait();

 SYS_DEBUG(DL_INFO1, "Thread '" << getThreadName() << "' has been started");
}

/// The physical start of the thread
/*! This is just a helper function, calling the real main() function.
 */
void * Thread::_main(void * info)
{
 SYS_DEBUG_STATIC(DM_THREAD);

 // Keep itself alive while main() is running:
 ThreadPtr self = reinterpret_cast<ThreadInfo *>(info)->getThread();

 self->mySelf = self;

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
    self->myThread = 0;

 } catch (std::exception & ex) {
    DEBUG_OUT("Thread Execution Error in " << self->getThreadName() << "/main(): " << ex.what());
    try {
        self->error(&ex);
    } catch (...) {
        DEBUG_OUT("Thread '" << self->getThreadName() << "': error() got exception!");
    }
    self->myThread = 0;
    return (void*)0;

 } catch (...) {
    DEBUG_OUT("Thread Execution Error in " << self->getThreadName() << "/main() (unknown exception)");
    try {
        self->error(nullptr);
    } catch (...) {
        DEBUG_OUT("Thread '" << self->getThreadName() << "': error() got exception!");
    }
    self->myThread = 0;
    return (void*)0;
 }

 SYS_DEBUG(DL_INFO1, "Thread '" << self->getThreadName() << "' has exited normally, status=" << status);

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
