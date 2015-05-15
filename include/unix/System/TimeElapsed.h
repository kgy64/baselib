/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Measuring short elapsed time using TimeDelay
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __BASELIB_INCLUDE_PUBLIC_SYSTEM_TIMEELAPSED_H_INCLUDED__
#define __BASELIB_INCLUDE_PUBLIC_SYSTEM_TIMEELAPSED_H_INCLUDED__

#include <System/TimeDelay.h>

#include <ostream>

namespace SYS
{
    class TimeElapsed;
}

std::ostream & operator<<(std::ostream & os, const SYS::TimeElapsed & time);

namespace SYS
{
    class TimeElapsed
    {
        friend std::ostream & ::operator<<(std::ostream & os, const SYS::TimeElapsed & time);

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

        void toStream(std::ostream & os);

     private:
        inline void toStream(std::ostream & os) const
        {
            os << myTime;
        }

        SYS::TimeDelay myTime;

    }; // class TimeElapsed;

} // namespace SYS

inline std::ostream & operator<<(std::ostream & os, const SYS::TimeElapsed & time)
{
 time.toStream(os);
 return os;
}

#endif /* __BASELIB_INCLUDE_PUBLIC_SYSTEM_TIMEELAPSED_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
