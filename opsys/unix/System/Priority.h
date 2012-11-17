#ifndef __PRIORITY_H__
#define __PRIORITY_H__

/// Increases the static initialization priority
/*! Such a variable will be initialized before others.
    \note   It is useful only for statically initialized instances. */
#define INITIALIZE_PRIORITY_HIGH    __attribute__((init_priority(500)))

/// Decreases the static initialization priority
/*! Such a variable will be initialized after others.
    \note   It is useful only for statically initialized instances. */
#define INITIALIZE_PRIORITY_LOW     __attribute__((init_priority(1500)))

#endif /* __PRIORITY_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
