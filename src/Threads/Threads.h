#ifndef __THREADS_H__
#define __THREADS_H__

#include <clutter/clutter.h>
#include <glibmm/threads.h>

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

    /// Thread-level lock
    /*! This class uses the clutter thread lock (see clutter_threads_enter() and
        clutter_threads_leave() for details) to be able to lock against clutter
        threads too. */
    class Lock
    {
     public:
        Lock(void)
        {
            clutter_threads_enter();
        }

        ~Lock(void)
        {
            clutter_threads_leave();
        }
    };

    static void yield(void)
    {
        Glib::Threads::Thread::yield();
    }

 private:
    /// This function is the startpoint of this thread
    virtual int main(void) =0;

    virtual void atExit(int)
    {
    }

    void _main(void);

    bool toBeFinished;

    Glib::Threads::Thread* myThread;
};

#endif /* __THREADS_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
