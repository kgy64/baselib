/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     OSM Database converter
 * Purpose:     Create my own binary database for Ducktor Navi Project out of OSM Database
 * Author:      Copyright (c) 2015 - Gyorgy Kovesdi <kgy@teledigit.eu>
 * License:     GPLv2
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_MEMORY_ALLOCATED_H_INCLUDED__
#define __SRC_MEMORY_ALLOCATED_H_INCLUDED__

#include <Memory/Memory.h>

#include <stdint.h>

namespace MEM
{
    class Allocated: public MEM::scoped_ptr<char>
    {
     public:
        inline Allocated(uint64_t size):
            MEM::scoped_ptr<char>(new char[size]),
            size(size)
        {
        }

        inline uint64_t getSize(void) const
        {
            return size;
        }

     private:
        uint64_t size;

    }; // class MEM::Allocated

} // namespace MEM

#endif /* __SRC_MEMORY_ALLOCATED_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
