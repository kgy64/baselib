/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Generic interfaces for file I/O
 * Author:      Kövesdi György <kgy@etiner.hu>
 * License:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <File/Decode.h>

#include "Ostream.h"

using FILES::Ostream;

Ostream::Ostream(const char * filename):
    std::ofstream(FILES::DecodeName(filename).c_str())
{
 SYS_DEBUG_MEMBER(DM_FILE);
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
