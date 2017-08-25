/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Generic interfaces for file I/O
 * Author:      Kövesdi György <kgy@etiner.hu>
 * License:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __INCLUDE_UNIX_FILE_OSTREAM_H_INCLUDED__
#define __INCLUDE_UNIX_FILE_OSTREAM_H_INCLUDED__

#include <fstream>

#include <Debug/Debug.h>

SYS_DECLARE_MODULE(DM_FILE);

namespace FILES
{
    class Ostream: public std::ofstream
    {
     public:
        Ostream(const char * filename);

     private:
        SYS_DEFINE_CLASS_NAME("FILES::Ostream");

    }; // class FILES::Ostream

} // namespace FILES

#endif /* __INCLUDE_UNIX_FILE_OSTREAM_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
