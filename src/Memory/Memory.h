/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     Baselib - Basic System Access Library
 * Purpose:     
 * Author:      Copyright (c) 2014 - Gyorgy Kovesdi <kgy@etiner.hu>
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
    using weak_ptr = typename std::weak_ptr<T>;

    /// Smart pointer similar to std::shared_ptr
    /*! This class is intended to be more thread-safe than std::shared_ptr.<br>
     *  The only difference is that this pointer is in a well defined state when the desctuctor
     *  of the referenced object is called. However, this problem of std::shared_ptr can appear
     *  in single-theraded cases too. */
    template <typename T>
    class shared_ptr: public std::shared_ptr<T>
    {
     private:
        using super = std::shared_ptr<T>;

     public:
        inline shared_ptr(void)
        {
        }

        inline shared_ptr(T * ptr):
            super(ptr)
        {
        }

        inline shared_ptr(const super & other):
            super(other)
        {
        }

        inline shared_ptr(super && other):
            super(other)
        {
        }

        template <typename U>
        inline shared_ptr(const U & other):
            super(other)
        {
        }

        template <typename U>
        inline shared_ptr(U && other):
            super(other)
        {
        }

        inline ~shared_ptr()
        {
            std::atomic_store(this, super());
        }

        inline void reset(T * ptr = nullptr)
        {
            std::atomic_store(this, super(ptr));
        }

    }; // class MEM::shared_ptr

    template <typename T>
    class scoped_ptr: public std::unique_ptr<T>
    {
     public:
        inline scoped_ptr(void)
        {
        }

        inline scoped_ptr(T * ptr):
            std::unique_ptr<T>(ptr)
        {
        }

        inline T * get(void)
        {
            return std::unique_ptr<T>::get();
        }

        inline const T * get(void) const
        {
            return std::unique_ptr<T>::get();
        }

        inline operator bool()
        {
            return get();
        }

    }; // class MEM::scoped_ptr

    template <typename T>
    class scoped_array: public MEM::scoped_ptr<T>
    {
     public:
        inline scoped_array(void)
        {
        }

        inline scoped_array(T * ptr):
            MEM::scoped_ptr<T>(ptr)
        {
        }

        inline T & operator[](size_t index)
        {
            return get()[index];
        }

        inline const T & operator[](size_t index) const
        {
            return get()[index];
        }

        inline T * get(void)
        {
            return MEM::scoped_ptr<T>::get();
        }

        inline const T * get(void) const
        {
            return MEM::scoped_ptr<T>::get();
        }

        inline operator bool()
        {
            return get();
        }

    }; // class MEM::scoped_array

    template <typename T>
    class shared_array: public MEM::shared_ptr<T>
    {
     public:
        inline shared_array(void)
        {
        }

        inline shared_array(T * ptr):
            MEM::shared_ptr<T>(ptr)
        {
        }

        inline T & operator[](size_t index)
        {
            return get()[index];
        }

        inline const T & operator[](size_t index) const
        {
            return get()[index];
        }

        inline T * get(void)
        {
            return MEM::shared_ptr<T>::get();
        }

        inline const T * get(void) const
        {
            return MEM::shared_ptr<T>::get();
        }

        inline operator bool()
        {
            return get();
        }

    }; // class MEM::shared_array

    template <typename T, typename U>
    inline shared_ptr<T> static_pointer_cast(const std::shared_ptr<U> & p)
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
