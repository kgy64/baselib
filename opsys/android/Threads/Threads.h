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

namespace Threads
{
    /// Android-specific thread
    class Thread: public PTHREAD::Thread, public AndroidAccess::ThreadJNIEnv
    {
    }; // class AndroidAccess::Thread

} // namespace AndroidAccess

#endif /* __SRC_THREADS_ANDROIDTHREAD_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
