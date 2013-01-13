/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _____KGY_BASE_INCLUDE_PUBLIC_MEMORY_WPTR_H_INCLUDED__
#define _____KGY_BASE_INCLUDE_PUBLIC_MEMORY_WPTR_H_INCLUDED__

#include <boost/weak_ptr.hpp>

#include <Memory/SPtr.h>

template <typename T>
class WPtr: public boost::weak_ptr<T>
{
 public:
    inline WPtr(void):
        boost::weak_ptr<T>()
    {
    }

    inline WPtr(const WPtr<T> & p_other):
        boost::weak_ptr<T>(p_other)
    {
    }

    inline WPtr(const SPtr<T> & p_other):
        boost::weak_ptr<T>(p_other)
    {
    }

    inline WPtr & operator=(const WPtr<T> & p_other)
    {
        boost::weak_ptr<T>::operator=(p_other);
        return *this;
    }

    inline WPtr & operator=(const SPtr<T> & p_other)
    {
        boost::weak_ptr<T>::operator=(p_other);
        return *this;
    }

    inline SPtr<T> lock(void) const
    {
        return SPtr<T>(boost::weak_ptr<T>::lock());
    }

    inline void reset(void)
    {
        boost::weak_ptr<T>::reset();
    }

    template <class U>
    inline bool operator<(const WPtr<U> & p_other) const
    {
        return boost::weak_ptr<T>::operator<(p_other);
    }
};

#endif /* _____KGY_BASE_INCLUDE_PUBLIC_MEMORY_WPTR_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
