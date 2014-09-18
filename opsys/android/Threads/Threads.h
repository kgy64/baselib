/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     Android Interface
 * Purpose:     Android-specific thread handler class
 * Author:      Copyright (c) 2014 - Gyorgy Kovesdi <kgy@teledigit.eu>
 * License:     GPLv2
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_THREADS_ANDROIDTHREAD_H_INCLUDED__
#define __SRC_THREADS_ANDROIDTHREAD_H_INCLUDED__

#include "access/access-base.h"
#include "src-pthreads/Threads/Threads.h"

#include <Debug/Debug.h>

SYS_DECLARE_MODULE(DM_THREAD);

namespace Threads
{
    /// Android-specific thread
    class Thread: public AndroidAccess::ThreadJNIEnv, public PTHREAD::Thread
    {
     public:
        inline Thread(const char * name):
            PTHREAD::Thread(name)
        {
            SYS_DEBUG_MEMBER(DM_THREAD);
        }

        virtual ~Thread()
        {
            SYS_DEBUG_MEMBER(DM_THREAD);
        }

     private:
        SYS_DEFINE_CLASS_NAME("Threads::Thread");

        virtual void before_main(void) override
        {
            attachAndroidThread();
        }

        virtual void after_main(void) override
        {
            detachAndroidThread();
        }

    }; // class AndroidAccess::Thread

} // namespace AndroidAccess

#endif /* __SRC_THREADS_ANDROIDTHREAD_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
