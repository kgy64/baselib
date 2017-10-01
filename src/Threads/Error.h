/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Error handling classes for my thread control
 * Author:      György Kövesdi (kgy@etiner.hu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_THREADS_ERROR_H_INCLUDED__
#define __SRC_THREADS_ERROR_H_INCLUDED__

#include <Exceptions/Exceptions.h>

#define ASSERT_THREAD(cond, message)    ASSERT_FATAL(cond, message)

#define ASSERT_THREAD_STD(cond)         ASSERT_STD_ZERO(cond)

#endif /* __SRC_THREADS_ERROR_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
