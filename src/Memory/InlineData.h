/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     Baselib - Basic System Access Library
 * Purpose:     
 * Author:      György Kövesdi <kgy@etiner.hu>
 * License:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __BASELIB_SRC_MEMORY_INLINEDATA_H_INCLUDED__
#define __BASELIB_SRC_MEMORY_INLINEDATA_H_INCLUDED__

#include <stdint.h>

template <size_t SIZE>
class InlineData
{
 protected:
    void * GetData(void)
    {
        return myData;
    }

    const void * GetData(void) const
    {
        return myData;
    }

    size_t GetSize(void) const
    {
        return SIZE;
    }

    uint8_t myData[SIZE];

}; // class InlineData

#endif /* __BASELIB_SRC_MEMORY_INLINEDATA_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
