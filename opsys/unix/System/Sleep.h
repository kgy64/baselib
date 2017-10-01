/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Sleep functions
 * Author:      Kövesdi György <kgy@etiner.hu>
 * License:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __BASE_OPSYS_UNIX_SYSTEM_SLEEP_H_INCLUDED__
#define __BASE_OPSYS_UNIX_SYSTEM_SLEEP_H_INCLUDED__

#include <unistd.h>

namespace TIME {
    inline void Sleep_seconds(float time)
    {
        usleep(time * 1e6);
    }

    inline void Sleep_seconds(double time)
    {
        usleep(time * 1e6);
    }

    inline void Sleep_milliseconds(float time)
    {
        usleep(time * 1e3);
    }

    inline void Sleep_milliseconds(double time)
    {
        usleep(time * 1e3);
    }

    inline void Sleep_microseconds(float time)
    {
        usleep(time);
    }

    inline void Sleep_microseconds(double time)
    {
        usleep(time);
    }

} // namespace TIME

#endif /* __BASE_OPSYS_UNIX_SYSTEM_SLEEP_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
