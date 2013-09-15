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

#include <Threads/Error.h>
#include <Threads/Condition.h>
#include <Threads/Mutex.h>

namespace Threads
{
    template <typename T>
    class DataPipe
    {
     public:
        inline DataPipe(void):
            isBusy(false),
            isFinished(false)
        {
        }

        typedef boost::shared_ptr<T> DataType;

        inline void push(const DataType & p_data)
        {
            Threads::Lock _l(myDataMutex);
            if (isBusy) {
                myFreeCondition.Wait(myDataMutex);
            }
            ASSERT(myData.get() == NULL, "Data is not handled yet");
            myData = p_data;
            isBusy = true;
            myUseCondition.Signal();
        }

        inline DataType pop(void)
        {
            Threads::Lock _l(myDataMutex);
            if (!isBusy && !isFinished) {
                myUseCondition.Wait(myDataMutex);
            }
            DataType result;
            result.swap(myData);
            isBusy = false;
            myFreeCondition.Signal();
            return result;
        }

        inline bool busy(void) const
        {
            return isBusy;
        }

        inline void finish(void)
        {
            isFinished = true;
            myUseCondition.Signal(myDataMutex);
        }

     protected:
        bool isBusy;

        bool isFinished;

        DataType myData;

        Threads::Mutex myDataMutex;

        Threads::Condition myUseCondition;

        Threads::Condition myFreeCondition;

    }; // class DataPipe

} // namespace Threads

#endif /* __SRC_THREADS_DATAPIPE_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
