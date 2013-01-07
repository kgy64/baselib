#include "Threads.h"

using namespace Threads;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *     class Thread:                                                                     *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Thread::Thread(void):
    toBeFinished(false),
    myThread(NULL)
{
}

Thread::~Thread(void)
{
}

void Thread::Kill(void)
{
 if (!myThread)
    return;
 toBeFinished = true;
 myThread->join();
 myThread = NULL;
}

void Thread::Start(void)
{
 myThread = Glib::Thread::create(sigc::mem_fun(*this, &Thread::_main), true);
}

void Thread::_main(void)
{
 atExit(main());
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
