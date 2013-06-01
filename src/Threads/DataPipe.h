/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Data Pipe handler classes (pass data between threads)
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_THREADS_DATAPIPE_H_INCLUDED__
#define __SRC_THREADS_DATAPIPE_H_INCLUDED__

#include <boost/shared_ptr.hpp>
#include <pthread.h>

#include <Threads/Error.h>

namespace Threads
{
    template <typename T>
    class DataPipe
    {
     public:
        typedef boost::shared_ptr<T> DataType;

        inline void push(const DataType & p_data)
        {
            Thread::Mutex::Lock _l(myDataMutex);
            myData = p_data;
            myCond.Signal();
        }

        inline DataType pop(void)
        {
            Threads::Mutex::Lock _l(myDataMutex);
            while (!isFinished && !myData.get()) {
                myCond.Wait(myDataMutex);
            }
            DataType result = myData;
            myData.reset();
            return result;
        }

        inline void finish(void)
        {
            Glib::Mutex::Lock _l(myDataMutex);
            isFinished = true;
            myCond.Signal();
        }

     protected:
        bool isFinished;

        DataType myData;

        Threads::Mutex myDataMutex;

        Threads::Condition myCond;

    }; // class DataPipe

} // namespace Threads

#endif /* __SRC_THREADS_DATAPIPE_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
