/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Associative sorted map
 * Author:      György Kövesdi <kgy@teledigit.eu>
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
    class Map: public std::map<K,T>
    {
    };
}

#endif /* __INCLUDE_PUBLIC_BASE_MAP_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
