/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __BASELIB_SRC_MEMORY_DUMP_H_INCLUDED__
#define __BASELIB_SRC_MEMORY_DUMP_H_INCLUDED__

#include <iostream>

namespace Memory
{
    class Dump
    {
     public:
        inline Dump(const void * p_address, int p_size = 256):
            myStartAddress(reinterpret_cast<const unsigned char *>(p_address)),
            mySize(p_size)
        {
        }

        void ToStream(std::ostream & os);

     private:
        static void PrintHex(std::ostream & os, uint64_t p_value, int p_size);

        template <typename T>
        static inline void Print(std::ostream & os, T & p_data)
        {
            PrintHex(os, (uint64_t)p_data, sizeof(T));
        }

        const unsigned char * myStartAddress;

        int mySize;

    }; // class Dump
}; // namespace Memory

static inline std::ostream & operator<<(std::ostream & os, Memory::Dump & mem)
{
 mem.ToStream(os);
 return os;
}

#endif /* __BASELIB_SRC_MEMORY_DUMP_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
