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

#include <Debug/Debug.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h> // for setpriority()
#include <unistd.h>
#include <sys/syscall.h>  // For SYS_xxx definitions

SYS_DEFINE_MODULE(DM_THREAD);

using namespace PTHREAD;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *     class Thread:                                                                     *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Thread::Thread(void):
    myThread(0),
    toBeFinished(true)
{
 SYS_DEBUG_MEMBER(DM_THREAD);
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
 */
void Thread::Kill(void)
{
 SYS_DEBUG_MEMBER(DM_THREAD);

 // If it has already been stopped, do nothing:
 if (toBeFinished)
    return;

 toBeFinished = true;           // Flag it to be stopped

 KillSignal();                  // Send the signal to the thread

 // If it is called from the thread itself, then it is not necessary at all. Calling
 // the join() is a bad idea in this case.
 if (pthread_equal(myThread, pthread_self()))
    return;

 pthread_join(myThread, NULL);  // Wait until exited
 myThread = 0;                  // The thread is already exited
}

/// Start the thread
void Thread::Start(size_t stack)
{
 SYS_DEBUG_MEMBER(DM_THREAD);

 // If it has already been started, do nothing:
 if (myThread)
    return;

 // It is in running state:
 toBeFinished = false;

 // Set the attributes
 myAttr.SetStackSize(stack);
 myAttr.SetJoinable(true);

 // Create the thread:
 ASSERT_THREAD(pthread_create(&myThread, myAttr.get(), &Thread::_main, this)==0, "pthread_create() failed");
}

/// The physical start of the thread
/*! This is just a helper function, calling the real main() function.
 */
void * Thread::_main(void * thread_pointer)
{
 int status = 0;
 Thread & th = *reinterpret_cast<Thread*>(thread_pointer);

 try {
    th.before_main();
    status = th.main();
 } catch (std::exception & ex) {
    DEBUG_OUT("Thread Execution Error in main(): " << ex.what());
    return (void*)0;
 }

 try {
    th.atExit(status);
 } catch (std::exception & ex) {
    DEBUG_OUT("Thread Execution Error in atExit(): " << ex.what());
    return (void*)0;
 }

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
