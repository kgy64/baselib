/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     Glesly: my GLES-based rendering library
 * Purpose:     Measuring short elapsed time using TimeDelay
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __BASELIB_INCLUDE_PUBLIC_SYSTEM_TIMEELAPSED_H_INCLUDED__
#define __BASELIB_INCLUDE_PUBLIC_SYSTEM_TIMEELAPSED_H_INCLUDED__

#include <System/TimeDelay.h>

namespace SYS
{
    class TimeElapsed
    {
     public:
        inline TimeElapsed(const SYS::TimeDelay & start):
            myTime(TimeDelay().SetNow() - start)
        {
        }

        inline int ToSecond(void) const
        {
            return myTime.ToSecond();
        }

        inline int ToMillisecond(void) const
        {
            return myTime.ToMillisecond();
        }

        inline int ToMicrosecond(void) const
        {
            return myTime.ToMicrosecond();
        }

     private:
        SYS::TimeDelay myTime;

    }; // class TimeElapsed;

} // namespace SYS

#endif /* __BASELIB_INCLUDE_PUBLIC_SYSTEM_TIMEELAPSED_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
