/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __OPSYS_UNIX_FILE_FILEPTRS_H_INCLUDED__
#define __OPSYS_UNIX_FILE_FILEPTRS_H_INCLUDED__

#include <Memory/SPtr.h>

namespace FILES
{
    class FileHandler;
    class DirHandler;
    class FileMap;

    typedef SPtr<FileHandler> FilePtr;
    typedef SPtr<FileMap> MapPtr;
    typedef SPtr<DirHandler> DirPtr;
}

#endif /* __OPSYS_UNIX_FILE_FILEPTRS_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
