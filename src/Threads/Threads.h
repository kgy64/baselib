#ifndef __THREADS_H__
#define __THREADS_H__

//#include <clutter/clutter.h>
#include <glibmm/thread.h>

class ICThread
{
 public:
    ICThread(void);
    ~ICThread();

    void Start(void);
    void Kill(void);

    /*! This function can be called by the main function to perform a graceful
        exit if necessary. */
    bool Finished(void)
    {
        return toBeFinished;
    }

    static void yield(void)
    {
        Glib::Thread::yield();
    }

 private:
    /// This function is the startpoint of this thread
    virtual int main(void) =0;

    virtual void atExit(int)
    {
    }

    void _main(void);

    bool toBeFinished;

    Glib::Thread* myThread;
};

#endif /* __THREADS_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
