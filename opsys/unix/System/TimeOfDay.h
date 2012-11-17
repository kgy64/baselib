#ifndef __TIMEFODAY_H__
#define __TIMEFODAY_H__

#include <Debug/Debug.h>

#include <sys/time.h>

namespace SYS
{
    class TimeOfDay;
}

std::ostream & operator<<(std::ostream & os, const SYS::TimeOfDay & time);

namespace SYS
{
    class TimeOfDay
    {
     friend std::ostream & ::operator<<(std::ostream & os, const SYS::TimeOfDay & time);

     public:
        TimeOfDay(void);

        virtual ~TimeOfDay()
        {
        }

        TimeOfDay & operator=(const TimeOfDay & other)
        {
            tv = other.tv;
            return *this;
        }

        TimeOfDay & operator-=(const TimeOfDay & other);

        TimeOfDay operator-(const TimeOfDay & other) const
        {
            TimeOfDay result(*this);
            return result -= other;
        }

        int ToSecond(void) const
        {
            return tv.tv_sec;
        }

        int ToMillisecond(void) const
        {
            return tv.tv_sec * 1000 + tv.tv_usec / 1000;
        }

        int ToMicrosecond(void) const
        {
            return tv.tv_sec * 1000000 + tv.tv_usec;
        }

        void AddMillisecond(int delta);

     protected:
        SYS_DEFINE_CLASS_NAME("GPS::TimeOfDay");

        struct timeval tv;
    };

}

#endif /* __TIMEFODAY_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
