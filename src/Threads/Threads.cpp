#include "Threads.h"

using namespace Threads;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *     class Thread:                                                                     *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Thread::Thread(void):
    toBeFinished(true),
    myThread(NULL)
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
 if (Glib::Thread::self() == myThread)
    return;

 myThread->join();      // Wait until exited
 myThread = NULL;       // The thread is already exited
}

/// Start the thread
/*! \warning    Do <b>not</b> call it from the constructor (even in the derivative classes), because
                virtual function is used here.
 */
void Thread::Start(void)
{
 // If it has already been started, do nothing:
 if (myThread)
    return;

 // It is in running state:
 toBeFinished = false;

 // Create the thread:
 myThread = Glib::Thread::create(sigc::mem_fun(*this, &Thread::_main), true);
}

/// The physical start of the thread
/*! This is just a helper function, calling the real main() function.
 */
void Thread::_main(void)
{
 atExit(main());
}

/// Called after main()
void Thread::atExit(int p_exitCode)
{
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
