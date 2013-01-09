#ifndef __THREADS_H__
#define __THREADS_H__

#include <glibmm/thread.h>

#include <Memory/SPtr.h>

namespace Threads
{
    class Thread
    {
     public:
        Thread(void);
        virtual ~Thread();

        virtual void Start(void);
        virtual void Kill(void);

        /*! This function can be called by the main function to perform a graceful
            exit if necessary. */
        bool Finished(void)
        {
            return toBeFinished;
        }

        static inline void yield(void)
        {
            Glib::Thread::yield();
        }

     private:
        /// This function is the startpoint of this thread
        virtual int main(void) =0;

        virtual void atExit(int p_exitCode);

        void _main(void);

        bool toBeFinished;

        Glib::Thread* myThread;
    };

    template <typename T>
    class DataPipe
    {
     public:
        inline void push(const SPtr<T> & p_data)
        {
            Glib::Mutex::Lock _l(myDataMutex);
            myData = p_data;
            myCond.signal();
        }

        inline SPtr<T> pop(void)
        {
            Glib::Mutex::Lock _l(myDataMutex);
            SPtr<T> result;
            while (!isFinished && !myData.get()) {
                myCond.wait(myDataMutex);
            }
            result = myData;
            myData.reset();
            return result;
        }

        inline void finish(void)
        {
            Glib::Mutex::Lock _l(myDataMutex);
            isFinished = true;
            myCond.signal();
        }

     protected:
        bool isFinished;

        SPtr<T> myData;

        Glib::Mutex myDataMutex;

        Glib::Cond myCond;
    };
} // namespace Threads

#endif /* __THREADS_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
