/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Thread handler classes
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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
}

/// Stop the thread
void Thread::Kill(void)
{
 // If it has already been stopped, do nothing:
 if (toBeFinished)
    return;

 toBeFinished = true;   // Flag it to be stopped

 // If it is called from the thread itself, then it is not necessary at all. Calling
 // the join() is a bad idea in this case.
 if (pthread_equal(myThread, pthread_self()))
    return;

 pthread_join(myThread, NULL);  // Wait until exited
 myThread = 0;                  // The thread is already exited
}

/// Start the thread
/*! \warning    Do <b>not</b> call it from the constructor (even in the derivative classes), because
                virtual function is used when it has started.
 */
void Thread::Start(void)
{
 // If it has already been started, do nothing:
 if (myThread)
    return;

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

/// Called after main()
void Thread::atExit(int p_exitCode)
{
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
