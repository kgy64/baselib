/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     Baselib - Basic System Access Library
 * Purpose:     
 * Author:      Copyright (c) 2014 - Gyorgy Kovesdi <kgy@teledigit.eu>
 * License:     GPLv2
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __OPSYS_UNIX_THREADS_THREADS_H_INCLUDED__
#define __OPSYS_UNIX_THREADS_THREADS_H_INCLUDED__

#include "src-pthreads/Threads/Threads.h"

namespace Threads
{
    class Thread: public PTHREAD::Thread
    {
     public:
        inline Thread(const char * name = "unknown"):
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

    }; // class Threads::Thread

} // namespace Threads

#endif /* __OPSYS_UNIX_THREADS_THREADS_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
