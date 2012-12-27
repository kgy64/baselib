/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     General Purpose Library
 * Purpose:     Associative sorted map
 * Author:      György Kövesdi <kgy@teledigitleu>
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __INCLUDE_PUBLIC_BASE_MAP_H_INCLUDED__
#define __INCLUDE_PUBLIC_BASE_MAP_H_INCLUDED__

#include <map>

namespace Base
{
    template <class K, class T>
    using Map = std::map<K,T>;
}

#endif /* __INCLUDE_PUBLIC_BASE_MAP_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
