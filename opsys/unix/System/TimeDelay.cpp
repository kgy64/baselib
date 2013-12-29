/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Class to measure elapsed time, independently from the clock settings
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "TimeDelay.h"

using namespace SYS;

TimeDelay & TimeDelay::operator-=(const TimeDelay & other)
{
 myTime.tv_sec -= other.myTime.tv_sec;
 myTime.tv_nsec -= other.myTime.tv_nsec;
 if (myTime.tv_nsec < 0) {
    myTime.tv_nsec += 1000000000;
    myTime.tv_sec -= 1;
 }
 return *this;
}

void TimeDelay::AddMillisecond(int delta)
{
 myTime.tv_sec += delta / 1000;
 myTime.tv_nsec += (delta % 1000) * 1000000;
 if (myTime.tv_nsec >= 1000000000) {
    myTime.tv_nsec -= 1000000000;
    myTime.tv_sec += 1;
 } else if (myTime.tv_nsec < 0) {
    myTime.tv_nsec += 1000000000;
    myTime.tv_sec -= 1;
 }
}

void TimeDelay::AddMicrosecond(int delta)
{
 myTime.tv_sec += delta / 1000000;
 myTime.tv_nsec += (delta % 1000000) * 1000;
 if (myTime.tv_nsec >= 1000000000) {
    myTime.tv_nsec -= 1000000000;
    myTime.tv_sec += 1;
 } else if (myTime.tv_nsec < 0) {
    myTime.tv_nsec += 1000000000;
    myTime.tv_sec -= 1;
 }
}

std::ostream & operator<<(std::ostream & os, const SYS::TimeDelay & time)
{
 int days = time.myTime.tv_sec / (24*60*60);
 int seconds = time.myTime.tv_sec % (24*60*60);
 float ftime = seconds + time.myTime.tv_nsec / 1e9;
 os << days << "_" << ftime;
 return os;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
