/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     Basic Library
 * Purpose:     Class to measure elapsed time, independently from the clock settings
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __BASELIB_OPSYS_UNIX_SYSTEM_TIMEDELAY_H_INCLUDED__
#define __BASELIB_OPSYS_UNIX_SYSTEM_TIMEDELAY_H_INCLUDED__

#include <ostream>
#include <time.h>

namespace SYS
{
    class TimeDelay;
}

std::ostream & operator<<(std::ostream & os, const SYS::TimeDelay & time);

namespace SYS
{
    class TimeDelay
    {
     friend std::ostream & ::operator<<(std::ostream & os, const SYS::TimeDelay & time);

     public:
        inline TimeDelay(void)
        {
        }

        inline TimeDelay & SetNow(void)
        {
#ifdef CLOCK_MONOTONIC_RAW
            static const clockid_t clock_type = CLOCK_MONOTONIC_RAW;
#else
            static const clockid_t clock_type = CLOCK_MONOTONIC;
#endif
            clock_gettime(clock_type, &myTime);
            return *this;
        }

        inline TimeDelay & operator=(const TimeDelay & other)
        {
            myTime = other.myTime;
            return *this;
        }

        TimeDelay & operator-=(const TimeDelay & other);

        inline TimeDelay operator-(const TimeDelay & other) const
        {
            TimeDelay result(*this);
            return result -= other;
        }

        inline int ToSecond(void) const
        {
            return myTime.tv_sec;
        }

        inline int ToMillisecond(void) const
        {
            return myTime.tv_sec * 1000 + myTime.tv_nsec / 1000000L;
        }

        inline int ToMicrosecond(void) const
        {
            return myTime.tv_sec * 1000000 + myTime.tv_nsec / 1000L;
        }

        void AddMillisecond(int delta);
        void AddMicrosecond(int delta);

     private:
        struct timespec myTime;

    }; // class TimeDelay

} // namespace SYS

#endif /* __BASELIB_OPSYS_UNIX_SYSTEM_TIMEDELAY_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
