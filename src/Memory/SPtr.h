/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_MEMORY_SPTR_H_INCLUDED__
#define __SRC_MEMORY_SPTR_H_INCLUDED__

#include <boost/shared_ptr.hpp>

// Note: to have a template typedef we need a c++11 compatible compiler. To be
//       compatible with older ones, use inheritance instead:

template <typename T>
class SPtr: public boost::shared_ptr<T>
{
 public:
    inline SPtr(void):
        boost::shared_ptr<T>()
    {
    }

    inline SPtr(T* p_ptr):
        boost::shared_ptr<T>(p_ptr)
    {
    }

    inline SPtr(const boost::shared_ptr<T>& p_other):
        boost::shared_ptr<T>(p_other)
    {
    }

    template <typename U>
    inline SPtr(const boost::shared_ptr<U>& p_other):
        boost::shared_ptr<T>(p_other)
    {
    }
};

#endif /* __SRC_MEMORY_SPTR_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
