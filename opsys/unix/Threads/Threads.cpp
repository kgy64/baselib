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

#include <sys/time.h>
#include <sys/resource.h> // for setpriority()

#include "Threads.h"

using namespace Threads;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *     class Thread:                                                                     *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Thread::Thread(void):
    myThread(0),
    toBeFinished(true)
{
}

Thread::~Thread(void)
{
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
 // If it has already been started, do nothing:
 if (myThread)
    return;

 myAttr.SetStackSize(stack);

 // It is in running state:
 toBeFinished = false;

 // Create the thread:
 myAttr.SetJoinable(true);

 ASSERT_THREAD(pthread_create(&myThread, myAttr.get(), &Thread::_main, this)==0, "pthread_create() failed");
}

/// The physical start of the thread
/*! This is just a helper function, calling the real main() function.
 */
void * Thread::_main(void * thread_pointer)
{
 Thread * th = reinterpret_cast<Thread*>(thread_pointer);
 th->atExit(th->main());
 return (void*)0;
}

int Thread::GetPriority(void) const
{
 return getpriority(PRIO_PROCESS, 0);
}

bool Thread::SetPriority(int prio)
{
 return setpriority(PRIO_PROCESS, 0, prio) == 0;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
