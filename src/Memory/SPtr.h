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

template <typename T>
using SPtr = boost::shared_ptr<T>;

#endif /* __SRC_MEMORY_SPTR_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
