#include "Threads.h"

ICThread::ICThread(void):
    toBeFinished(false),
    myThread(NULL)
{
}

ICThread::~ICThread(void)
{
 Kill();
}

void ICThread::Kill(void)
{
 if (!myThread)
    return;
 toBeFinished = true;
 myThread->join();
 myThread = NULL;
}

void ICThread::Start(void)
{
 myThread = Glib::Thread::create(sigc::mem_fun(*this, &ICThread::_main), true);
}

void ICThread::_main(void)
{
 atExit(main());
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
