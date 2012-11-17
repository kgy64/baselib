#include "TimeOfDay.h"

using namespace SYS;

TimeOfDay::TimeOfDay(void)
{
 gettimeofday(&tv, NULL);
}

TimeOfDay & TimeOfDay::operator-=(const TimeOfDay & other)
{
 tv.tv_sec -= other.tv.tv_sec;
 tv.tv_usec -= other.tv.tv_usec;
 if (tv.tv_usec < 0) {
    tv.tv_usec += 1000000;
    tv.tv_sec -= 1;
 }
 return *this;
}

void TimeOfDay::AddMillisecond(int delta)
{
 tv.tv_usec += delta * 1000;
 if (tv.tv_usec > 1000000) {
    tv.tv_usec -= 1000000;
    tv.tv_sec += 1;
 } else if (tv.tv_usec < 0) {
    tv.tv_usec += 1000000;
    tv.tv_sec -= 1;
 }
}

std::ostream & operator<<(std::ostream & os, const SYS::TimeOfDay & time)
{
 int days = time.tv.tv_sec / (24*60*60);
 int seconds = time.tv.tv_sec % (24*60*60);
 float ftime = seconds + time.tv.tv_usec / 1e6;
 os << days << "_" << ftime;
 return os;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
