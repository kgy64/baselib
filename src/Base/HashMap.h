/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     General Purpose Library
 * Purpose:     Associative unsorted map with hash
 * Author:      György Kövesdi <kgy@teledigit.eu>
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    This needs C++11
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __INCLUDE_PUBLIC_BASE_HASHMAP_H_INCLUDED__
#define __INCLUDE_PUBLIC_BASE_HASHMAP_H_INCLUDED__

#include <unordered_map>

namespace Base
{
    template <class K, class T>
    using HashMap = std::unordered_map<K,T>;
}

#endif /* __INCLUDE_PUBLIC_BASE_HASHMAP_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
