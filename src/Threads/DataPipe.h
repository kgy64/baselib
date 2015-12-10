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

#include <list>

#include <Threads/Error.h>
#include <Threads/Condition.h>
#include <Threads/Mutex.h>

namespace Threads
{
    template <typename T, size_t maxSize = 1>
    class DataPipe
    {
     public:
        inline DataPipe(void):
            currentSize(0),
            isFinished(false)
        {
        }

        typedef T DataType;

        inline void push(const DataType & p_data)
        {
            Threads::Lock _l(myDataMutex);
            while (currentSize >= maxSize) {
                myFreeCondition.Wait(myDataMutex);
            }
            myData.push_back(p_data);
            ++currentSize;
            myUseCondition.Signal();
        }

        inline void push_drop(const DataType & p_data)
        {
            Threads::Lock _l(myDataMutex);
            while (currentSize >= maxSize) {
                myData.pop_front();
                --currentSize;
            }
            myData.push_back(p_data);
            ++currentSize;
            myUseCondition.Signal();
        }

        inline DataType pop(void)
        {
            Threads::Lock _l(myDataMutex);
            while (true) {
                if (isFinished) {
                    return DataType();
                }
                if (!myData.empty()) {
                    DataType result = myData.front();
                    myData.pop_front();
                    --currentSize;
                    myFreeCondition.Signal();
                    return result;
                }
                myUseCondition.Wait(myDataMutex);
            }
        }

        inline bool busy(void) const
        {
            return currentSize > 0; // assuming it is atomic
        }

        inline void finish(void)
        {
            isFinished = true;
            myUseCondition.Signal(myDataMutex);
        }

     protected:
        size_t currentSize;

        bool isFinished;

        std::list<DataType> myData;

        Threads::Mutex myDataMutex;

        Threads::Condition myUseCondition;

        Threads::Condition myFreeCondition;

    }; // class Threads::DataPipe

} // namespace Threads

#endif /* __SRC_THREADS_DATAPIPE_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
