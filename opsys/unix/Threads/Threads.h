/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     Ducktor Navi - my Navigation Project
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
    }; // class Threads::Thread

} // namespace Threads

#endif /* __OPSYS_UNIX_THREADS_THREADS_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
