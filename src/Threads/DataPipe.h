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
#include <Threads/Condition.h>
#include <Threads/Mutex.h>

namespace Threads
{
    template <typename T>
    class DataPipe
    {
     public:
        typedef boost::shared_ptr<T> DataType;

        inline void push(const DataType & p_data)
        {
            Threads::Lock _l(myDataMutex);
            isFinished = false;
            if (myData.get()) {
                myFreeCondition.Wait(myDataMutex);
            }
            ASSERT(myData.get() == NULL, "Data is not handled yet");
            myData = p_data;
            myUseCondition.Signal();
        }

        inline DataType pop(void)
        {
            Threads::Lock _l(myDataMutex);
            while (!isFinished && !myData.get()) {
                myUseCondition.Wait(myDataMutex);
            }
            DataType result;
            result.swap(myData);
            myFreeCondition.Signal();
            return result;
        }

        inline void finish(void)
        {
            Threads::Lock _l(myDataMutex);
            isFinished = true;
            myUseCondition.Signal();
        }

     protected:
        bool isFinished;

        DataType myData;

        Threads::Mutex myDataMutex;

        Threads::Condition myUseCondition;

        Threads::Condition myFreeCondition;

    }; // class DataPipe

} // namespace Threads

#endif /* __SRC_THREADS_DATAPIPE_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
