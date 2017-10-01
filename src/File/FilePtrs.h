/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     
 * Author:      György Kövesdi (kgy@etiner.hu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __OPSYS_UNIX_FILE_FILEPTRS_H_INCLUDED__
#define __OPSYS_UNIX_FILE_FILEPTRS_H_INCLUDED__

#include <Memory/Memory.h>

namespace FILES
{
    class FileHandler;
    class DirHandler;
    class FileMap;

    typedef MEM::shared_ptr<FileHandler> FilePtr;
    typedef MEM::shared_ptr<FileMap> MapPtr;
    typedef MEM::shared_ptr<DirHandler> DirPtr;
}

#endif /* __OPSYS_UNIX_FILE_FILEPTRS_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
