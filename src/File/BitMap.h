/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     Ducktor Navi: The Navigation Project
 * Purpose:     
 * Author:      György Kövesdi <kgy@teledigit.eu>
 * License:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __BASELIB_SRC_FILE_BITMAP_H_INCLUDED__
#define __BASELIB_SRC_FILE_BITMAP_H_INCLUDED__

#include <File/Base.h>
#include <File/FileMap.h>
#include <Memory/InlineData.h>
#include <Exceptions/Exceptions.h>

#include <string.h>
#include <stdint.h>
#include <boost/scoped_array.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *     class BitMapBase:                                                                 *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <size_t BITS, class ALLOC>
class BitMapBase: public ALLOC, public FILES::Writeable
{
 public:
    static constexpr size_t BYTES   =   (BITS+7)/8;

 protected:
    union bitMask
    {
        bitMask(size_t data):
            idx(data)
        {
        }

        size_t  idx;

        struct {
            size_t  flag:3;
            size_t  offset:(sizeof(size_t)*8-3);
        };

        inline uint8_t mask(void) const {
            return 1 << flag;
        }

    }; // union BitMapBase::bitMask

    inline uint8_t * getMyData(void)
    {
        return reinterpret_cast<uint8_t *>(ALLOC::GetData());
    }

    inline const uint8_t * getMyData(void) const
    {
        return reinterpret_cast<const uint8_t *>(ALLOC::GetData());
    }

 public:
    inline BitMapBase(void)
    {
    }

    template <typename T>
    inline BitMapBase(T param1):
        ALLOC(param1)
    {
    }

    template <typename T1, typename T2>
    inline BitMapBase(T1 param1, T2 param2):
        ALLOC(param1, param2)
    {
    }

    virtual ~BitMapBase()
    {
    }

    inline bool operator[](size_t index) const
    {
        ASSERT(index <= BITS, "index overflow in BitMapBase::operator[] (index=" << index << ", allocated=" << BITS << " bits)");
        bitMask m(index);
        return getMyData()[m.offset] & m.mask();
    }

    inline void set(size_t index)
    {
        ASSERT(index <= BITS, "index overflow in BitMapBase::set() (index=" << index << ", allocated=" << BITS << " bits)");
        bitMask m(index);
        getMyData()[m.offset] |= m.mask();
    }

    inline void clear(size_t index)
    {
        ASSERT(index <= BITS, "index overflow in BitMapBase::clear() (index=" << index << ", allocated=" << BITS << " bits)");
        bitMask m(index);
        getMyData()[m.offset] &= ~m.mask();
    }

    virtual const void * GetData(void) const override
    {
        return ALLOC::GetData();
    }

    virtual size_t GetSize(void) const override
    {
        return ALLOC::GetSize();
    }

}; // class BitMapBase<>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *     class BitMapFile_interface:                                                       *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <size_t BYTES>
class BitMapFile_interface: public FILES::FileMap
{
 public:
    BitMapFile_interface(const char * filename, FILES::FileMap::OpenMode mode = FILES::FileMap::Read_Only):
        FILES::FileMap(filename, mode, BYTES)
    {
    }

}; // class BitMapFile_interface<>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *     class BitMapMem:                                                                  *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <size_t BITS>
class BitMapMem: public BitMapBase<BITS, InlineData<(BITS+7)/8> >
{
}; // BitMapMem<>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *     class BitMapFile:                                                                 *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <size_t BITS>
class BitMapFile: public BitMapBase<BITS, BitMapFile_interface<(BITS+7)/8> >
{
 public:
    inline BitMapFile(const char * filename, FILES::FileMap::OpenMode mode = FILES::FileMap::Read_Only):
        BitMapBase<BITS, BitMapFile_interface<(BITS+7)/8> >(filename, mode)
    {
    }

}; // BitMapFile<>

#endif /* __BASELIB_SRC_FILE_BITMAP_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
