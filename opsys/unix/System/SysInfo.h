/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * License:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __OPSYS_UNIX_SYSTEM_SYSINFO_H_INCLUDED__
#define __OPSYS_UNIX_SYSTEM_SYSINFO_H_INCLUDED__

#include <stdint.h>
#include <unistd.h>

#include <string>
#include <vector>

#include <Debug/Debug.h>

namespace SYS
{
    class MemInfo
    {
     public:
        inline MemInfo(void)
        {
        }

        inline VIRTUAL_IF_DEBUG ~MemInfo()
        {
        }

        inline static uint64_t getMemoryPageSize(void)
        {
            return getpagesize();
        }

        inline uint64_t getMemoryHugePageSize(void)
        {
            updateInfo();
            return getGenericInfo("Hugepagesize");
        }

        inline uint64_t getPhisicalMemorySize(void)
        {
            updateInfo();
            return getGenericInfo("MemTotal");
        }

        inline double kbGetPhisicalMemorySize(void)
        {
            return (double)getPhisicalMemorySize() / 1024.0;
        }

        inline double mbGetPhisicalMemorySize(void)
        {
            return kbGetPhisicalMemorySize() / 1024.0;
        }

        inline double gbGetPhisicalMemorySize(void)
        {
            return mbGetPhisicalMemorySize() / 1024.0;
        }

     private:
        SYS_DEFINE_CLASS_NAME("SYS::MemInfo");

        void updateInfo();
        uint64_t getGenericInfo(const char * keyword);

        std::vector<std::string> info;

    }; // class SYS::MemInfo

} // namespace SYS

#endif /* __OPSYS_UNIX_SYSTEM_SYSINFO_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
