/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __OPSYS_UNIX_SYSTEM_GENERIC_H_INCLUDED__
#define __OPSYS_UNIX_SYSTEM_GENERIC_H_INCLUDED__

#define PACKED  __attribute__((aligned(1), packed))

/// Forces the object to be linked in
#define MAKE_REFERENCED __attribute__((used))

/// Increases the static initialization priority
/*! Such a variable will be initialized before others.
    \note   It is useful only for statically initialized instances. */
#define INITIALIZE_PRIORITY_HIGH    __attribute__((init_priority(500)))

/// Decreases the static initialization priority
/*! Such a variable will be initialized after others.
    \note   It is useful only for statically initialized instances. */
#define INITIALIZE_PRIORITY_LOW     __attribute__((init_priority(1500)))

#endif /* __OPSYS_UNIX_SYSTEM_GENERIC_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
