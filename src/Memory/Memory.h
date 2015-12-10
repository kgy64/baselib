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
    template <typename T = void>
    using shared_ptr = typename std::shared_ptr<T>;

    template <typename T = void>
    using weak_ptr = typename std::weak_ptr<T>;

    template <typename T = void>
    class scoped_ptr: public std::auto_ptr<T>
    {
     public:
        inline scoped_ptr(void)
        {
        }

        inline scoped_ptr(T * ptr):
            std::auto_ptr<T>(ptr)
        {
        }

        inline T * get(void)
        {
            return std::auto_ptr<T>::get();
        }

        inline const T * get(void) const
        {
            return std::auto_ptr<T>::get();
        }

        inline operator bool()
        {
            return get();
        }

    }; // class MEM::scoped_ptr

    template <typename T>
    class scoped_array: public std::shared_ptr<T>
    {
     public:
        inline scoped_array(void)
        {
        }

        inline scoped_array(T * ptr):
            std::shared_ptr<T>(ptr, std::default_delete<T[]>())
        {
        }

    }; // class MEM::scoped_array

    template <typename T, typename U>
    inline shared_ptr<T> static_pointer_cast(const shared_ptr<U> & p)
    {
        return std::static_pointer_cast<T>(p);
    }

    namespace noncopyable_
    {
        class noncopyable
        {
         protected:
            inline noncopyable() {}

         private:
            noncopyable(const noncopyable &);
            const noncopyable & operator=(const noncopyable &);
        };
    }

    using noncopyable = noncopyable_::noncopyable;

} // namespace MEM

#endif /* __SRC_MEMORY_MEMORY_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
