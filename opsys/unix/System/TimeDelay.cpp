/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Class to measure elapsed time, independently from the clock settings
 * Author:      György Kövesdi (kgy@etiner.hu)
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

void SYS::TimeDelay::toStream(std::ostream & os) const
{
 uint32_t seconds = myTime.tv_sec;
 uint32_t minutes = seconds / 60;
 seconds %= 60;
 uint32_t hours = minutes / 60;
 minutes %= 60;
 uint32_t days = hours / 24;
 hours %= 24;
 double ftime = (double)seconds + myTime.tv_nsec / 1e9;

 char result[100];

 if (days > 0) {
    snprintf(result, sizeof result, "%d:%02d:%02d:%02.6f", days, hours, minutes, ftime);
 } else if (hours > 0) {
    snprintf(result, sizeof result, "%02d:%02d:%02.6f", hours, minutes, ftime);
 } else {
    snprintf(result, sizeof result, "%02d:%02.6f", minutes, ftime);
 }

 os << result;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
