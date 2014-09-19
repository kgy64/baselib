/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     Ducktor Navi - my Navigation Project
 * Purpose:     
 * Author:      Copyright (c) 2014 - Gyorgy Kovesdi <kgy@teledigit.eu>
 * License:     GPLv2
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_MEMORY_MEMORY_H_INCLUDED__
#define __SRC_MEMORY_MEMORY_H_INCLUDED__

#include <memory>

namespace MEM
{
    template <typename T>
    using shared_ptr = typename std::shared_ptr<T>;

    template <typename T>
    class scoped_ptr: public std::auto_ptr<T>
    {
     public:
        scoped_ptr(void)
        {
        }

        scoped_ptr(T * ptr):
            std::auto_ptr<T>(ptr)
        {
        }

        T * get(void)
        {
            return std::auto_ptr<T>::get();
        }

        const T * get(void) const
        {
            return std::auto_ptr<T>::get();
        }

        operator bool()
        {
            return get();
        }

    }; // class MEM::scoped_ptr

    template <typename T>
    class scoped_array: public std::shared_ptr<T>
    {
     public:
        scoped_array(void)
        {
        }

        scoped_array(T * ptr):
            std::shared_ptr<T>(ptr, std::default_delete<T[]>())
        {
        }

    }; // class MEM::scoped_array

} // namespace MEM

#endif /* __SRC_MEMORY_MEMORY_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
